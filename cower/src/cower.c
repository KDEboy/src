/* Copyright (c) 2010-2014 Dave Reisner
 *
 * cower.c
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/* glibc */
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <fnmatch.h>
#include <getopt.h>
#include <locale.h>
#include <pthread.h>
#include <pwd.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <wchar.h>
#include <wordexp.h>

/* external libs */
#include <alpm.h>
#include <archive.h>
#include <archive_entry.h>
#include <curl/curl.h>
#include <yajl/yajl_parse.h>

#include "aur.h"
#include "macro.h"
#include "package.h"

/* macros */
#define UNUSED                __attribute__((unused))
#define NCFLAG(val, flag)     (!cfg.color && (val)) ? (flag) : ""

#define _cleanup_(x) __attribute__((cleanup(x)))
#define _cleanup_free_ _cleanup_(freep)
static inline void freep(void *p) { free(*(void**) p); }

#define _cleanup_fclose_ _cleanup_(fclosep)
static inline void fclosep(FILE **f) { if (*f) fclose(*f); }

#define FALLTHROUGH
#ifdef __GNUC__
#ifndef __clang__
#undef FALLTHROUGH
#define FALLTHROUGH __attribute__((fallthrough))
#endif
#endif

#ifndef PACMAN_ROOT
  #define PACMAN_ROOT         "/"
#endif
#ifndef PACMAN_DBPATH
  #define PACMAN_DBPATH       "/var/lib/pacman"
#endif
#ifndef PACMAN_CONFIG
  #define PACMAN_CONFIG       "/etc/pacman.conf"
#endif

#define NC                    "\033[0m"
#define BOLD                  "\033[1m"

#define BLACK                 "\033[0;30m"
#define RED                   "\033[0;31m"
#define GREEN                 "\033[0;32m"
#define YELLOW                "\033[0;33m"
#define BLUE                  "\033[0;34m"
#define MAGENTA               "\033[0;35m"
#define CYAN                  "\033[0;36m"
#define WHITE                 "\033[0;37m"
#define BOLDBLACK             "\033[1;30m"
#define BOLDRED               "\033[1;31m"
#define BOLDGREEN             "\033[1;32m"
#define BOLDYELLOW            "\033[1;33m"
#define BOLDBLUE              "\033[1;34m"
#define BOLDMAGENTA           "\033[1;35m"
#define BOLDCYAN              "\033[1;36m"
#define BOLDWHITE             "\033[1;37m"

/* typedefs and objects */
typedef enum __loglevel_t {
  LOG_INFO    = 1,
  LOG_ERROR   = (1 << 1),
  LOG_WARN    = (1 << 2),
  LOG_DEBUG   = (1 << 3),
  LOG_VERBOSE = (1 << 4),
} loglevel_t;

typedef enum __operation_t {
  OP_SEARCH   = 1,
  OP_INFO     = (1 << 1),
  OP_DOWNLOAD = (1 << 2),
  OP_UPDATE   = (1 << 3),
} operation_t;

enum {
  OP_DEBUG = 1000,
  OP_FORMAT,
  OP_SORT,
  OP_RSORT,
  OP_IGNOREPKG,
  OP_IGNOREREPO,
  OP_LISTDELIM,
  OP_LITERAL,
  OP_THREADS,
  OP_TIMEOUT,
  OP_NOIGNOREOOD,
  OP_AURDOMAIN,
  OP_SEARCHBY,
};

enum {
  SORT_FORWARD = 1,
  SORT_REVERSE = -1
};

struct buffer_t {
  char *data;
  size_t size;
  size_t capacity;
};

struct task_t {
  struct aur_t *aur;
  CURL *curl;
  aurpkg_t **(*threadfn)(struct task_t*, const char*);
};

/* function prototypes */
static int allow_regex(void);
static inline int streq(const char *, const char *);
static inline int startswith(const char *, const char *);
static alpm_list_t *alpm_find_foreign_pkgs(void);
static alpm_handle_t *alpm_init(void);
static int alpm_pkg_is_foreign(alpm_pkg_t*);
static const char *alpm_provides_pkg(const char*);
static int archive_extract_file(char *, size_t);
static int aurpkg_cmpver(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmpmaint(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmpvotes(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmppopularity(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmpood(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmplastmod(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmpfirstsub(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmpname(const aurpkg_t *pkg1, const aurpkg_t *pkg2);
static int aurpkg_cmp(const void*, const void*);
static aurpkg_t **cower_perform(struct task_t *task, int num_threads);
static size_t curl_buffer_response(void*, size_t, size_t, void*);
static int cwr_fprintf(FILE*, loglevel_t, const char*, ...) __attribute__((format(printf,3,4)));
static int cwr_printf(loglevel_t, const char*, ...) __attribute__((format(printf,2,3)));
static int cwr_vfprintf(FILE*, loglevel_t, const char*, va_list) __attribute__((format(printf,3,0)));
static aurpkg_t **dedupe_results(aurpkg_t **list);
static aurpkg_t **download(struct task_t *task, const char*);
static aurpkg_t **filter_results(aurpkg_t **);
static int find_search_fragment(const char *, char **);
static char *get_file_as_buffer(const char*);
static int getcols(void);
static int get_config_path(char *config_path, size_t pathlen);
static int globcompare(const void *a, const void *b);
static int have_unignored_results(aurpkg_t **packages);
static void indentprint(const char*, int);
static alpm_list_t *load_targets_from_files(alpm_list_t *files);
static alpm_list_t *parse_bash_array(alpm_list_t*, char*);
static int parse_configfile(void);
static int parse_options(int, char*[]);
static int parse_keyname(char*);
static int pkg_is_binary(const char *pkg);
static void pkgbuild_get_depends(char*, alpm_list_t**);
static int print_escaped(const char*);
static void print_extinfo_list(char **, const char*, const char*, int);
static void print_colored(const char *fieldname, const char *color, const char *value);
static void print_time(const char *fieldname, time_t* timestamp);
static void print_pkg_formatted(aurpkg_t*);
static void print_pkg_info(aurpkg_t*);
static void print_pkg_installed_tag(aurpkg_t*);
static void print_pkg_search(aurpkg_t*);
static void print_results(aurpkg_t **, void (*)(aurpkg_t*));
static int read_targets_from_file(FILE *in, alpm_list_t **targets);
static void resolve_one_dep(struct task_t *task, const char *depend);
static void resolve_pkg_dependencies(struct task_t *task, aurpkg_t *package);
static rpc_type rpc_op_from_opmask(int opmask);
static aurpkg_t **rpc_do(struct task_t *task, rpc_type type, const char *arg);
static int ch_working_dir(void);
static int should_ignore_package(const aurpkg_t *package, regex_t *pattern);
static void strings_init(void);
static size_t strtrim(char*);
static int task_http_execute(struct task_t *, const char *, const char *);
static void task_reset(struct task_t *, const char *, void *);
static void task_reset_for_download(struct task_t *, const char *, void *);
static void task_reset_for_rpc(struct task_t *, const char *, void *);
static aurpkg_t **task_download(struct task_t*, const char*);
static aurpkg_t **task_query(struct task_t*, const char*);
static aurpkg_t **task_update(struct task_t*, const char*);
static void *thread_pool(void*);
static void usage(void);
static void version(void);

/* globals */
static alpm_handle_t *pmhandle;
static alpm_db_t *db_local;
static alpm_list_t *workq;
static pthread_mutex_t listlock = PTHREAD_MUTEX_INITIALIZER;

static const int kInfoIndent = 17;
static const int kSearchIndent = 4;
static const int kRegexOpts = REG_ICASE|REG_EXTENDED|REG_NOSUB|REG_NEWLINE;
static const char kListDelim[] = "  ";
static const char kCowerUserAgent[] = "cower/" COWER_VERSION;
static const char kRegexChars[] = "^.+*?$[](){}|\\";
static const char kDigits[] = "0123456789";
static const char kPrintfFlags[] = "'-+ #0I";

static struct {
  const char *error;
  const char *warn;
  const char *info;
  const char *pkg;
  const char *repo;
  const char *url;
  const char *ood;
  const char *utd;
  const char *nc;
} colstr = {
  .error = "error:",
  .warn = "warning:",
  .info = "::",
  .pkg = "",
  .repo = "",
  .url = "",
  .ood = "",
  .utd = "",
  .nc = ""
};

/* runtime configuration */
static struct {
  const char *aur_domain;
  rpc_by search_by;

  char *working_dir;
  const char *delim;
  const char *format;

  operation_t opmask;
  loglevel_t logmask;

  short color;
  short ignoreood;
  short sortorder;
  int force:1;
  int getdeps:1;
  int literal:1;
  int quiet:1;
  int skiprepos:1;
  int frompkgbuild:1;
  int maxthreads;
  long timeout;

  int (*sort_fn)(const aurpkg_t*, const aurpkg_t*);

  alpm_list_t *targets;
  struct {
    alpm_list_t *pkgs;
    alpm_list_t *repos;
  } ignore;
} cfg = {
  .aur_domain = "aur.archlinux.org",
  .search_by = SEARCHBY_NAME_DESC,
  .sortorder = SORT_FORWARD,
  .timeout = 10L,
  .delim = kListDelim,
  .maxthreads = 10,
  .logmask = LOG_ERROR|LOG_WARN|LOG_INFO,
  .sort_fn = aurpkg_cmpname,
};

int allow_regex() {
  return cfg.opmask & OP_SEARCH && !cfg.literal && cfg.search_by != SEARCHBY_MAINTAINER;
}

int streq(const char *s1, const char *s2) {
  return strcmp(s1, s2) == 0;
}

int startswith(const char *s1, const char *s2) {
  return strncmp(s1, s2, strlen(s2)) == 0;
}

/* TODO: handle includes. maybe use pkgfile's parser as a starting point. */
alpm_handle_t *alpm_init(void) {
  _cleanup_fclose_ FILE *fp = NULL;
  char line[PATH_MAX];
  char *ptr, *section = NULL;
  alpm_errno_t err;

  cwr_printf(LOG_DEBUG, "initializing alpm\n");
  pmhandle = alpm_initialize(PACMAN_ROOT, PACMAN_DBPATH, &err);
  if (!pmhandle) {
    fprintf(stderr, "failed to initialize alpm: %s\n", alpm_strerror(err));
    return NULL;
  }

  fp = fopen(PACMAN_CONFIG, "r");
  if (!fp) {
    return pmhandle;
  }

  while (fgets(line, PATH_MAX, fp)) {
    size_t linelen;

    if ((ptr = strchr(line, '#'))) {
      *ptr = '\0';
    }
    if (!(linelen = strtrim(line))) {
      continue;
    }

    if (line[0] == '[' && line[linelen - 1] == ']') {
      free(section);
      section = strndup(&line[1], linelen - 2);

      if (!streq(section, "options")) {
        if (!cfg.skiprepos && !alpm_list_find_str(cfg.ignore.repos, section)) {
          alpm_register_syncdb(pmhandle, section, 0);
          cwr_printf(LOG_DEBUG, "registering alpm db: %s\n", section);
        }
      }
    } else {
      char *key, *token;

      key = ptr = line;
      strsep(&ptr, "=");
      strtrim(key);
      strtrim(ptr);
      if (streq(key, "IgnorePkg")) {
        for (token = strtok(ptr, "\t\n "); token; token = strtok(NULL, "\t\n ")) {
          cwr_printf(LOG_DEBUG, "ignoring package: %s\n", token);
          cfg.ignore.pkgs = alpm_list_add(cfg.ignore.pkgs, strdup(token));
        }
      }
    }
  }
  free(section);

  db_local = alpm_get_localdb(pmhandle);

  /* hack: prepopulate the package cache to avoid potentially doing it after
   * thread creation. */
  alpm_db_get_pkgcache(db_local);

  return pmhandle;
}

alpm_list_t *alpm_find_foreign_pkgs(void) {
  const alpm_list_t *i;
  alpm_list_t *ret = NULL;

  for (i = alpm_db_get_pkgcache(db_local); i; i = i->next) {
    alpm_pkg_t *pkg = i->data;

    if (alpm_pkg_is_foreign(pkg)) {
      ret = alpm_list_add(ret, strdup(alpm_pkg_get_name(pkg)));
    }
  }

  return ret;
}

int alpm_pkg_is_foreign(alpm_pkg_t *pkg) {
  const alpm_list_t *i;
  const char *pkgname;

  pkgname = alpm_pkg_get_name(pkg);

  for (i = alpm_get_syncdbs(pmhandle); i; i = i->next) {
    if (alpm_db_get_pkg(i->data, pkgname)) {
      return 0;
    }
  }

  return 1;
}

const char *alpm_provides_pkg(const char *pkgname) {
  const alpm_list_t *i;
  const char *dbname = NULL;
  static pthread_mutex_t alpmlock = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&alpmlock);
  for (i = alpm_get_syncdbs(pmhandle); i; i = i->next) {
    alpm_db_t *db = i->data;
    if (alpm_find_satisfier(alpm_db_get_pkgcache(db), pkgname)) {
      dbname = alpm_db_get_name(db);
      break;
    }
  }
  pthread_mutex_unlock(&alpmlock);

  return dbname;
}

int archive_extract_file(char *data, size_t size) {
  struct archive *archive;
  struct archive_entry *entry;
  const int archive_flags = ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME;
  int r = 0;

  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  if (archive_read_open_memory(archive, data, size) != ARCHIVE_OK) {
    return archive_errno(archive);
  }

  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    const char *entryname = archive_entry_pathname(entry);

    cwr_printf(LOG_DEBUG, "extracting file: %s\n", entryname);

    r = archive_read_extract(archive, entry, archive_flags);
    /* NOOP ON ARCHIVE_{OK,WARN,RETRY} */
    if (r == ARCHIVE_FATAL || r == ARCHIVE_WARN) {
      r = archive_errno(archive);
      break;
    } else if (r == ARCHIVE_EOF) {
      r = 0;
      break;
    }
  }

  archive_read_close(archive);
  archive_read_free(archive);

  return r;
}

int aurpkg_cmp(const void *a, const void *b) {
  const aurpkg_t * const *pkg1 = a;
  const aurpkg_t * const *pkg2 = b;

  return cfg.sortorder * cfg.sort_fn(*pkg1, *pkg2);
}

int aurpkg_cmpname(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return strcmp(pkg1->name, pkg2->name);
}

int aurpkg_cmpver(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return alpm_pkg_vercmp(pkg1->version, pkg2->version);
}

int aurpkg_cmpmaint(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return strcmp(pkg1->maintainer, pkg2->maintainer);
}

int aurpkg_cmpvotes(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return pkg1->votes - pkg2->votes;
}

int aurpkg_cmppopularity(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  const double diff = pkg1->popularity - pkg2->popularity;

  if (diff > DBL_EPSILON) {
    return 1;
  } else if (diff < -DBL_EPSILON) {
    return -1;
  } else {
    return 0;
  }
}

int aurpkg_cmpood(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return pkg1->out_of_date - pkg2->out_of_date;
}

int aurpkg_cmplastmod(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return difftime(pkg1->modified_s, pkg2->modified_s);
}

int aurpkg_cmpfirstsub(const aurpkg_t *pkg1, const aurpkg_t *pkg2) {
  return difftime(pkg1->submitted_s, pkg2->submitted_s);
}

int globcompare(const void *a, const void *b) {
  return fnmatch(a, b, 0);
}

int have_unignored_results(aurpkg_t **packages) {
  aurpkg_t **p;

  if (packages == NULL) {
    return 0;
  }

  for (p = packages; *p; p++) {
    if (!(*p)->ignored) {
      return 1;
    }
  }

  return 0;
}

int cwr_fprintf(FILE *stream, loglevel_t level, const char *format, ...) {
  int ret;
  va_list args;

  va_start(args, format);
  ret = cwr_vfprintf(stream, level, format, args);
  va_end(args);

  return ret;
}

int cwr_printf(loglevel_t level, const char *format, ...) {
  int ret;
  va_list args;

  va_start(args, format);
  ret = cwr_vfprintf(stdout, level, format, args);
  va_end(args);

  return ret;
}

int cwr_vfprintf(FILE *stream, loglevel_t level, const char *format, va_list args) {
  const char *prefix;
  char bufout[128];

  if (!(cfg.logmask & level)) {
    return 0;
  }

  switch(level) {
    case LOG_VERBOSE:
    case LOG_INFO:
      prefix = colstr.info;
      break;
    case LOG_ERROR:
      prefix = colstr.error;
      break;
    case LOG_WARN:
      prefix = colstr.warn;
      break;
    case LOG_DEBUG:
      prefix = "debug:";
      break;
    default:
      prefix = "";
      break;
  }

  /* f.l.w.: 128 should be big enough... */
  snprintf(bufout, 128, "%s %s", prefix, format);

  return vfprintf(stream, bufout, args);
}

void task_reset(struct task_t *task, const char *url, void *writedata) {
  curl_easy_reset(task->curl);

  curl_easy_setopt(task->curl, CURLOPT_URL, url);
  curl_easy_setopt(task->curl, CURLOPT_WRITEFUNCTION, curl_buffer_response);
  curl_easy_setopt(task->curl, CURLOPT_WRITEDATA, writedata);
  curl_easy_setopt(task->curl, CURLOPT_USERAGENT, kCowerUserAgent);
  curl_easy_setopt(task->curl, CURLOPT_CONNECTTIMEOUT, cfg.timeout);
  curl_easy_setopt(task->curl, CURLOPT_FOLLOWLOCATION, 1L);

  /* Required for multi-threaded apps using timeouts. See
   * CURLOPT_NOSIGNAL(3) */
  if (cfg.timeout > 0L) {
    curl_easy_setopt(task->curl, CURLOPT_NOSIGNAL, 1L);
  }
}

void task_reset_for_rpc(struct task_t *task, const char *url, void *writedata) {
  task_reset(task, url, writedata);

  /* The empty string indicates that we should accept any supported encoding. */
  curl_easy_setopt(task->curl, CURLOPT_ACCEPT_ENCODING, "");
}

void task_reset_for_download(struct task_t *task, const char *url, void *writedata) {
  task_reset(task, url, writedata);

  /* disable compression, since downloads are compressed tarballs */
  curl_easy_setopt(task->curl, CURLOPT_ACCEPT_ENCODING, "identity");
}

size_t curl_buffer_response(void *ptr, size_t size, size_t nmemb, void *userdata) {
  const size_t realsize = size * nmemb;
  struct buffer_t *mem = userdata;

  if (mem->data == NULL || mem->capacity - realsize <= mem->size) {
    char *newdata;
    const size_t newcap = (mem->capacity + realsize) * 2.5;

    newdata = realloc(mem->data, newcap);
    if (newdata == NULL) {
      cwr_fprintf(stderr, LOG_ERROR, "failed to reallocate %zd bytes\n",
          mem->size + realsize);
      return 0;
    }

    mem->capacity = newcap;
    mem->data = newdata;
  }

  memcpy(mem->data + mem->size, ptr, realsize);
  mem->size += realsize;
  mem->data[mem->size] = '\0';

  return realsize;
}

int task_http_execute(struct task_t *task, const char *url, const char *arg) {
  CURLcode r;
  long response_code;

  cwr_printf(LOG_DEBUG, "[%s]: curl_easy_perform %s\n", arg, url);

  r = curl_easy_perform(task->curl);
  if (r != CURLE_OK) {
    cwr_fprintf(stderr, LOG_ERROR, "[%s]: %s\n", arg, curl_easy_strerror(r));
    return 1;
  }

  curl_easy_getinfo(task->curl, CURLINFO_RESPONSE_CODE, &response_code);
  cwr_printf(LOG_DEBUG, "[%s]: server responded with %ld\n", arg, response_code);

  if (response_code != 200) {
    cwr_fprintf(stderr, LOG_ERROR, "[%s]: server responded with HTTP %ld\n",
        arg, response_code);
    return 1;
  }

  return 0;
}

aurpkg_t **download(struct task_t *task, const char *package) {
  aurpkg_t **result;
  _cleanup_free_ char *url = NULL;
  int ret;
  struct buffer_t response = { NULL, 0, 0 };

  result = rpc_do(task, RPC_INFO, package);
  if (!result) {
    cwr_fprintf(stderr, LOG_ERROR, "no results found for %s\n", package);
    return NULL;
  }

  cwr_printf(LOG_DEBUG, "package %s is part of pkgbase %s\n", package, result[0]->pkgbase);

  if (access(result[0]->pkgbase, F_OK) == 0 && !cfg.force) {
    cwr_fprintf(stderr, LOG_ERROR, "`%s/%s' already exists. Use -f to overwrite.\n",
        cfg.working_dir, result[0]->pkgbase);
    aur_packages_free(result);
    return NULL;
  }

  url = aur_build_url(task->aur, result[0]->aur_urlpath);
  if (url == NULL) {
    return NULL;
  }

  task_reset_for_download(task, url, &response);
  if (task_http_execute(task, url, package) != 0) {
    goto finish;
  }

  ret = archive_extract_file(response.data, response.size);
  if (ret != 0) {
    cwr_fprintf(stderr, LOG_ERROR, "[%s]: failed to extract tarball: %s\n",
        package, strerror(ret));
    goto finish;
  }

  cwr_printf(LOG_INFO, "%s%s%s downloaded to %s\n",
      colstr.pkg, result[0]->name, colstr.nc, cfg.working_dir);

  if (cfg.getdeps) {
    resolve_pkg_dependencies(task, result[0]);
  }

finish:
  free(response.data);

  return result;
}

/* TODO: rewrite comparators to avoid this duplication */
static int aurpkgp_cmpname(const void *a, const void *b) {
  const aurpkg_t *const *p1 = a;
  const aurpkg_t *const *p2 = b;

  return aurpkg_cmpname(*p1, *p2);
}

aurpkg_t **dedupe_results(aurpkg_t **packages) {
  aurpkg_t *prev = NULL;
  int i, len;

  if (packages == NULL) {
    return NULL;
  }

  len = aur_packages_count(packages);

  qsort(packages, len, sizeof(*packages), aurpkgp_cmpname);

  for (i = 0; i < len; i++) {
    if (prev == NULL || aurpkg_cmpname(packages[i], prev) != 0) {
      prev = packages[i];
    } else {
      packages[i]->ignored = 1;
    }
  }

  return packages;
}

int should_ignore_package(const aurpkg_t *package, regex_t *pattern) {
  if ((cfg.search_by == SEARCHBY_NAME || cfg.search_by == SEARCHBY_NAME_DESC) &&
      regexec(pattern, package->name, 0, 0, 0) != REG_NOMATCH) {
    return 0;
  }

  if (cfg.search_by == SEARCHBY_NAME_DESC && package->description &&
      regexec(pattern, package->description, 0, 0, 0) != REG_NOMATCH) {
    return 0;
  }

  return 1;
}

aurpkg_t **filter_results(aurpkg_t **packages) {
  if (packages == NULL) {
    return NULL;
  }

  dedupe_results(packages);

  if (allow_regex()) {
    const alpm_list_t *i;

    for (i = cfg.targets; i; i = i->next) {
      regex_t regex;
      aurpkg_t **p;

      /* this should succeed since we validated the regex up front */
      regcomp(&regex, i->data, kRegexOpts);

      for (p = packages; *p; p++) {
        aurpkg_t *pkg = *p;

        if (pkg->ignored || (pkg->ignored = should_ignore_package(pkg, &regex))) {
          continue;
        }
      }
      regfree(&regex);
    }
  }

  return packages;
}

int getcols(void) {
  int c;
  struct winsize ws;
  static int cached_columns = -1;

  if (cached_columns >= 0) {
    return cached_columns;
  }

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
    c = ws.ws_col;
  } else {
    c = isatty(STDOUT_FILENO) ? 80 : 0;
  }

  cached_columns = c;
  return cached_columns;
}

char *get_file_as_buffer(const char *path) {
  _cleanup_fclose_ FILE *fp = NULL;
  char *buf;
  long fsize, nread;

  fp = fopen(path, "r");
  if (!fp) {
    cwr_fprintf(stderr, LOG_ERROR, "error: failed to open %s: %s\n",
        path, strerror(errno));
    return NULL;
  }

  fseek(fp, 0L, SEEK_END);
  fsize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  buf = calloc(1, (size_t)fsize + 1);

  nread = fread(buf, 1, fsize, fp);

  if (nread < fsize) {
    cwr_fprintf(stderr, LOG_ERROR, "Failed to read full PKGBUILD\n");
    free(buf);
    return NULL;
  }

  return buf;
}

int get_config_path(char *config_path, size_t pathlen) {
  char *var;
  struct passwd *pwd;

  var = getenv("XDG_CONFIG_HOME");
  if (var != NULL) {
    snprintf(config_path, pathlen, "%s/cower/config", var);
    return 0;
  }

  var = getenv("HOME");
  if (var != NULL) {
    snprintf(config_path, pathlen, "%s/.config/cower/config", var);
    return 0;
  }

  pwd = getpwuid(getuid());
  if (pwd != NULL && pwd->pw_dir != NULL) {
    snprintf(config_path, pathlen, "%s/.config/cower/config", pwd->pw_dir);
    return 0;
  }

  return 1;
}

void indentprint(const char *str, int indent) {
  wchar_t *wcstr;
  const wchar_t *p;
  int len, cidx, cols;

  if (!str) {
    return;
  }

  cols = getcols();

  /* if we're not a tty, print without indenting */
  if (cols == 0) {
    fputs(str, stdout);
    return;
  }

  len = strlen(str) + 1;
  wcstr = calloc(len, sizeof(wchar_t));
  len = mbstowcs(wcstr, str, len);
  p = wcstr;
  cidx = indent;

  if (!p || !len) {
    return;
  }

  while (*p) {
    if (*p == L' ') {
      const wchar_t *q, *next;
      p++;
      if (!p || *p == L' ') {
        continue;
      }
      next = wcschr(p, L' ');
      if (!next) {
        next = p + wcslen(p);
      }

      /* len captures # cols */
      len = 0;
      q = p;

      while (q < next) {
        len += wcwidth(*q++);
      }

      if (len > (cols - cidx - 1)) {
        /* wrap to a newline and reindent */
        printf("\n%-*s", indent, "");
        cidx = indent;
      } else {
        fputc(' ', stdout);
        cidx++;
      }
      continue;
    }
#ifdef __clang__
    printf("%lc", *p);
#else /* assume GCC */
    printf("%lc", (wint_t)*p);
#endif
    cidx += wcwidth(*p);
    p++;
  }
  free(wcstr);
}

alpm_list_t *load_targets_from_files(alpm_list_t *files) {
  alpm_list_t *i, *targets = NULL, *results = NULL;

  for (i = files; i; i = i->next) {
    _cleanup_free_ char *pkgbuild = get_file_as_buffer(i->data);

    /* TODO: parse .SRCINFO instead of PKGBUILD */
    pkgbuild_get_depends(pkgbuild, &results);
  }

  /* sanitize and dedupe */
  for (i = results; i; i = i->next) {
    char *sanitized = strdup(i->data);

    sanitized[strcspn(sanitized, "<>=")] = '\0';
    if (!alpm_list_find_str(targets, sanitized)) {
      targets = alpm_list_add(targets, sanitized);
    }
  }

  return targets;
}

alpm_list_t *parse_bash_array(alpm_list_t *deplist, char *array) {
  char *ptr, *token, *saveptr;

  if (!array) {
    return NULL;
  }

  for (token = strtok_r(array, " \t\n", &saveptr); token;
      token = strtok_r(NULL, " \t\n", &saveptr)) {
    /* found an embedded comment. skip to the next line */
    if (*token == '#') {
      strtok_r(NULL, "\n", &saveptr);
      continue;
    }

    /* unquote the element */
    if (*token == '\'' || *token == '\"') {
      ptr = strrchr(token + 1, *token);
      if (ptr) {
        token++;
        *ptr = '\0';
      }
    }

    /* some people feel compelled to do insane things in PKGBUILDs. these people suck */
    if (!*token || strlen(token) < 2 || *token == '$') {
      continue;
    }

    cwr_printf(LOG_DEBUG, "adding depend: %s\n", token);
    if (!alpm_list_find_str(deplist, token)) {
      deplist = alpm_list_add(deplist, strdup(token));
    }
  }

  return deplist;
}

int parse_configfile(void) {
  char line[BUFSIZ], config_path[PATH_MAX];
  int r = 0;
  _cleanup_fclose_ FILE *fp = NULL;

  if (get_config_path(config_path, sizeof(config_path)) != 0) {
    return 0;
  }

  fp = fopen(config_path, "r");
  if (!fp) {
    cwr_printf(LOG_DEBUG, "config file not found. skipping parsing\n");
    return 0; /* not an error, just nothing to do here */
  }

  while (fgets(line, sizeof(line), fp)) {
    char *key, *val;
    size_t linelen;

    linelen = strtrim(line);
    if (!linelen || line[0] == '#') {
      continue;
    }

    if ((val = strchr(line, '#'))) {
      *val = '\0';
    }

    key = val = line;
    strsep(&val, "=");
    strtrim(key);
    strtrim(val);

    if (val && !*val) {
      val = NULL;
    }

    cwr_printf(LOG_DEBUG, "found config option: %s => %s\n", key, val);

    /* colors are not initialized in this section, so usage of cwr_printf
     * functions is verboten unless we're using loglevel_t LOG_DEBUG */
    if (streq(key, "IgnoreRepo")) {
      for (key = strtok(val, " "); key; key = strtok(NULL, " ")) {
        cwr_printf(LOG_DEBUG, "ignoring repo: %s\n", key);
        cfg.ignore.repos = alpm_list_add(cfg.ignore.repos, strdup(key));
      }
    } else if (streq(key, "IgnorePkg")) {
      for (key = strtok(val, " "); key; key = strtok(NULL, " ")) {
        cwr_printf(LOG_DEBUG, "ignoring package: %s\n", key);
        cfg.ignore.pkgs = alpm_list_add(cfg.ignore.pkgs, strdup(key));
      }
    } else if (streq(key, "IgnoreOOD")) {
      cfg.ignoreood = 1;
    } else if (streq(key, "TargetDir")) {
      if (val) {
        wordexp_t p;
        if (wordexp(val, &p, 0) == 0) {
          if (p.we_wordc == 1) {
            cfg.working_dir = strdup(p.we_wordv[0]);
          }
          wordfree(&p);
          /* error on relative paths */
          if (*cfg.working_dir != '/') {
            fprintf(stderr, "error: TargetDir cannot be a relative path\n");
            r = 1;
          }
        } else {
          fprintf(stderr, "error: failed to resolve option to TargetDir\n");
          r = 1;
        }
      }
    } else if (streq(key, "MaxThreads")) {
      if (val) {
        cfg.maxthreads = strtol(val, &key, 10);
        if (*key != '\0' || cfg.maxthreads <= 0) {
          fprintf(stderr, "error: invalid option to MaxThreads: %s\n", val);
          r = 1;
        }
      }
    } else if (streq(key, "ConnectTimeout")) {
      if (val) {
        cfg.timeout = strtol(val, &key, 10);
        if (*key != '\0' || cfg.timeout < 0) {
          fprintf(stderr, "error: invalid option to ConnectTimeout: %s\n", val);
          r = 1;
        }
      }
    } else if (streq(key, "Color")) {
      if (!val || streq(val, "auto")) {
        cfg.color = isatty(fileno(stdout));
      } else if (streq(val, "always")) {
        cfg.color = 1;
      } else if (streq(val, "never")) {
        cfg.color = 0;
      } else {
        fprintf(stderr, "error: invalid option to Color: %s\n", val);
        return 1;
      }
    } else {
      fprintf(stderr, "ignoring unknown option: %s\n", key);
    }
  }

  return r;
}

int parse_options(int argc, char *argv[]) {
  int opt, option_index = 0;

  static const struct option opts[] = {
    /* operations */
    {"download",      no_argument,        0, 'd'},
    {"info",          no_argument,        0, 'i'},
    {"msearch",       no_argument,        0, 'm'},
    {"search",        no_argument,        0, 's'},
    {"update",        no_argument,        0, 'u'},

    /* options */
    {"by",            required_argument,  0, OP_SEARCHBY},
    {"color",         optional_argument,  0, 'c'},
    {"debug",         no_argument,        0, OP_DEBUG},
    {"domain",        required_argument,  0, OP_AURDOMAIN},
    {"force",         no_argument,        0, 'f'},
    {"format",        required_argument,  0, OP_FORMAT},
    {"sort",          required_argument,  0, OP_SORT},
    {"rsort",         required_argument,  0, OP_RSORT},
    {"from-pkgbuild", no_argument,        0, 'p'},
    {"help",          no_argument,        0, 'h'},
    {"ignore",        required_argument,  0, OP_IGNOREPKG},
    {"ignore-ood",    no_argument,        0, 'o'},
    {"no-ignore-ood", no_argument,        0, OP_NOIGNOREOOD},
    {"ignorerepo",    optional_argument,  0, OP_IGNOREREPO},
    {"listdelim",     required_argument,  0, OP_LISTDELIM},
    {"literal",       no_argument,        0, OP_LITERAL},
    {"quiet",         no_argument,        0, 'q'},
    {"target",        required_argument,  0, 't'},
    {"threads",       required_argument,  0, OP_THREADS},
    {"timeout",       required_argument,  0, OP_TIMEOUT},
    {"verbose",       no_argument,        0, 'v'},
    {"version",       no_argument,        0, 'V'},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "bc::dfhimopqst:uvV", opts, &option_index)) != -1) {
    char *token;

    switch(opt) {
      /* operations */
      case 's':
        cfg.opmask |= OP_SEARCH;
        break;
      case 'u':
        cfg.opmask |= OP_UPDATE;
        break;
      case 'i':
        cfg.opmask |= OP_INFO;
        break;
      case 'd':
        if (!(cfg.opmask & OP_DOWNLOAD)) {
          cfg.opmask |= OP_DOWNLOAD;
        } else {
          cfg.getdeps |= 1;
        }
        break;
      case 'm':
        cfg.opmask |= OP_SEARCH;
        cfg.search_by = SEARCHBY_MAINTAINER;
        break;

      /* options */
      case 'c':
        if (!optarg || streq(optarg, "auto")) {
          if (isatty(fileno(stdout))) {
            cfg.color = 1;
          } else {
            cfg.color = 0;
          }
        } else if (streq(optarg, "always")) {
          cfg.color = 1;
        } else if (streq(optarg, "never")) {
          cfg.color = 0;
        } else {
          fprintf(stderr, "invalid argument to --color\n");
          return 1;
        }
        break;
      case 'f':
        cfg.force |= 1;
        break;
      case 'h':
        usage();
        exit(0);
      case 'q':
        cfg.quiet |= 1;
        break;
      case 't':
        cfg.working_dir = strdup(optarg);
        break;
      case 'v':
        cfg.logmask |= LOG_VERBOSE;
        break;
      case 'V':
        version();
        exit(0);
      case OP_DEBUG:
        cfg.logmask |= LOG_DEBUG;
        break;
      case OP_FORMAT:
        cfg.format = optarg;
        break;
      case OP_RSORT:
        cfg.sortorder = SORT_REVERSE;
        FALLTHROUGH;
      case OP_SORT:
        if (parse_keyname(optarg)) {
          fprintf(stderr, "error: invalid argument to --%s\n", opts[option_index].name);
          return 1;
        }
        break;
      case 'o':
        cfg.ignoreood = 1;
        break;
      case 'p':
        cfg.frompkgbuild |= 1;
        break;
      case OP_IGNOREPKG:
        for (token = strtok(optarg, ","); token; token = strtok(NULL, ",")) {
          cwr_printf(LOG_DEBUG, "ignoring package: %s\n", token);
          cfg.ignore.pkgs = alpm_list_add(cfg.ignore.pkgs, strdup(token));
        }
        break;
      case OP_IGNOREREPO:
        if (!optarg) {
          cfg.skiprepos |= 1;
        } else {
          for (token = strtok(optarg, ","); token; token = strtok(NULL, ",")) {
            cwr_printf(LOG_DEBUG, "ignoring repos: %s\n", token);
            cfg.ignore.repos = alpm_list_add(cfg.ignore.repos, strdup(token));
          }
        }
        break;
      case OP_NOIGNOREOOD:
        cfg.ignoreood = 0;
        break;
      case OP_AURDOMAIN:
        cfg.aur_domain = optarg;
        break;
      case OP_LISTDELIM:
        cfg.delim = optarg;
        break;
      case OP_LITERAL:
        cfg.literal |= 1;
        break;
      case OP_THREADS:
        cfg.maxthreads = strtol(optarg, &token, 10);
        if (*token != '\0' || cfg.maxthreads <= 0) {
          fprintf(stderr, "error: invalid argument to --threads: %s\n", optarg);
          return 1;
        }
        break;
      case OP_TIMEOUT:
        cfg.timeout = strtol(optarg, &token, 10);
        if (*token != '\0' || cfg.timeout < 0) {
          fprintf(stderr, "error: invalid argument to --timeout: %s\n", optarg);
          return 1;
        }
        break;
      case OP_SEARCHBY:
        if (streq(optarg, "maintainer")) {
          cfg.search_by = SEARCHBY_MAINTAINER;
        } else if (streq(optarg, "name-desc")) {
          cfg.search_by = SEARCHBY_NAME_DESC;
        } else if (streq(optarg, "name")) {
          cfg.search_by = SEARCHBY_NAME;
        } else {
          fprintf(stderr, "error: invalid argument to --by: %s\n", optarg);
        }
        break;
      case '?':
      default:
        return 1;
    }
  }

  if (!cfg.opmask) {
    fprintf(stderr, "error: no operation specified (use -h for help)\n");
    return 3;
  }

#define NOT_EXCL(val) (cfg.opmask & (val) && (cfg.opmask & ~(val)))
  /* check for invalid operation combos */
  if (NOT_EXCL(OP_INFO) || NOT_EXCL(OP_SEARCH) ||
      NOT_EXCL(OP_UPDATE|OP_DOWNLOAD)) {
    fprintf(stderr, "error: invalid operation\n");
    return 1;
  }

  if (allow_regex()) {
    int i;

    for (i = optind; i < argc; i++) {
      regex_t regex;
      int r;

      r = regcomp(&regex, argv[i], kRegexOpts);
      if (r != 0) {
        char error_buffer[100];

        regerror(r, &regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "error: invalid regex: %s: %s\n", argv[i], error_buffer);

        return 1;
      }
      regfree(&regex);
    }
  }

  while (optind < argc) {
    if (!alpm_list_find_str(cfg.targets, argv[optind])) {
      cwr_printf(LOG_DEBUG, "adding target: %s\n", argv[optind]);
      cfg.targets = alpm_list_add(cfg.targets, strdup(argv[optind]));
    }
    optind++;
  }

  strings_init();

  return ch_working_dir();
}

int parse_keyname(char* keyname) {
  if (streq("name", keyname)) {
    cfg.sort_fn = aurpkg_cmpname;
    return 0;
  } else if (streq("version", keyname)) {
    cfg.sort_fn = aurpkg_cmpver;
    return 0;
  } else if (streq("maintainer", keyname)) {
    cfg.sort_fn = aurpkg_cmpmaint;
    return 0;
  } else if (streq("votes", keyname)) {
    cfg.sort_fn = aurpkg_cmpvotes;
    return 0;
  } else if (streq("popularity", keyname)) {
    cfg.sort_fn = aurpkg_cmppopularity;
    return 0;
  } else if (streq("outofdate", keyname)) {
    cfg.sort_fn = aurpkg_cmpood;
    return 0;
  } else if (streq("lastmodified", keyname)) {
    cfg.sort_fn = aurpkg_cmplastmod;
    return 0;
  } else if (streq("firstsubmitted", keyname)) {
    cfg.sort_fn = aurpkg_cmpfirstsub;
    return 0;
  }
  return 1;
}

int pkg_is_binary(const char *pkg) {
  const char *db = alpm_provides_pkg(pkg);

  if (db) {
    cwr_fprintf(stderr, LOG_WARN, "%s%s%s is available in %s%s%s "
        "(ignore this with --ignorerepo=%s)\n",
        colstr.pkg, pkg, colstr.nc,
        colstr.repo, db, colstr.nc,
        db);
    return 1;
  }

  return 0;
}

void pkgbuild_get_depends(char *pkgbuild, alpm_list_t **deplist) {
  char *lineptr;

  for (lineptr = pkgbuild; lineptr; lineptr = strchr(lineptr, '\n')) {
    char *arrayend, *arrayptr;
    int depth = 1;
    size_t linelen;

    linelen = strtrim(++lineptr);
    if (!linelen || *lineptr == '#') {
      continue;
    }

    if (!startswith(lineptr, "depends=(") &&
      !startswith(lineptr, "checkdepends=(") &&
      !startswith(lineptr, "makedepends=(")) {
      continue;
    }

    arrayptr = (char*)memchr(lineptr, '(', linelen) + 1;
    for (arrayend = arrayptr; depth; arrayend++) {
      switch(*arrayend) {
        case ')':
          depth--;
          break;
        case '(':
          depth++;
          break;
      }
    }
    *(arrayend - 1) = '\0';
    *deplist = parse_bash_array(*deplist, arrayptr);
    lineptr = arrayend;
  }
}

int print_escaped(const char *delim) {
  const char *f;
  int out = 0;

  for (f = delim; *f != '\0'; f++) {
    if (*f == '\\') {
      switch(*++f) {
        case '\\':
          fputc('\\', stdout);
          break;
        case '"':
          fputc('\"', stdout);
          break;
        case 'a':
          fputc('\a', stdout);
          break;
        case 'b':
          fputc('\b', stdout);
          break;
        case 'e': /* \e is nonstandard */
          fputc('\033', stdout);
          break;
        case 'n':
          fputc('\n', stdout);
          break;
        case 'r':
          fputc('\r', stdout);
          break;
        case 't':
          fputc('\t', stdout);
          break;
        case 'v':
          fputc('\v', stdout);
          break;
      }
      ++out;
    } else {
      fputc(*f, stdout);
      ++out;
    }
  }

  return out;
}

void print_colored(const char *fieldname, const char *color, const char *value) {
  printf("%-*s: %s%s%s\n", kInfoIndent - 2, fieldname, color ? color : "", value, color ? colstr.nc : "");
}

void print_time(const char *fieldname, time_t *timestamp) {
  char str[42];
  struct tm t;

  strftime(str, 42, "%c", localtime_r(timestamp, &t));
  printf("%-*s: %s\n", kInfoIndent - 2, fieldname, str);
}

void print_extinfo_list(char **list, const char *fieldname, const char *delim, int wrap) {
  char **i, **next;
  size_t cols, count = 0;

  if (!list) {
    return;
  }

  cols = wrap ? getcols() : 0;

  if (fieldname) {
    count += printf("%-*s: ", kInfoIndent - 2, fieldname);
  }

  for (i = list; *i; i = next) {
    size_t data_len = strlen(*i);
    next = i + 1;
    if (wrap && cols > 0 && count + data_len >= cols) {
      printf("%-*c", kInfoIndent + 1, '\n');
      count = kInfoIndent;
    }
    count += data_len;
    fputs(*i, stdout);
    if (next) {
      count += print_escaped(delim);
    }
  }
  if (wrap) {
    fputc('\n', stdout);
  }
}

void print_pkg_formatted(aurpkg_t *pkg) {
  const char *p;
  char fmt[32], buf[64];
  int len;

  if (pkg->ignored) {
    return;
  }

  for (p = cfg.format; *p; p++) {
    len = 0;
    if (*p == '%') {
      len = strspn(p + 1 + len, kPrintfFlags);
      len += strspn(p + 1 + len, kDigits);
      snprintf(fmt, len + 3, "%ss", p);
      fmt[len + 1] = 's';
      p += len + 1;
      switch(*p) {
        /* simple attributes */
        case 'a':
          snprintf(buf, 64, "%ld", pkg->modified_s);
          printf(fmt, buf);
          break;
        case 'b':
          printf(fmt, pkg->pkgbase);
          break;
        case 'd':
          printf(fmt, pkg->description ? pkg->description : "");
          break;
        case 'i':
          snprintf(buf, 64, "%d", pkg->package_id);
          printf(fmt, buf);
          break;
        case 'm':
          printf(fmt, pkg->maintainer ? pkg->maintainer : "(orphan)");
          break;
        case 'n':
          printf(fmt, pkg->name);
          break;
        case 'o':
          snprintf(buf, 64, "%d", pkg->votes);
          printf(fmt, buf);
          break;
        case 'p':
          snprintf(buf, 64, "https://%s/packages/%s", cfg.aur_domain, pkg->name);
          printf(fmt, buf);
          break;
        case 'r':
          snprintf(buf, 64, "%.2f", pkg->popularity);
          printf(fmt, buf);
          break;
        case 's':
          snprintf(buf, 64, "%ld", pkg->submitted_s);
          printf(fmt, buf);
          break;
        case 't':
          printf(fmt, pkg->out_of_date ? "yes" : "no");
          break;
        case 'u':
          printf(fmt, pkg->upstream_url);
          break;
        case 'v':
          printf(fmt, pkg->version);
          break;
        case 'w':
          snprintf(buf, 64, "%ld", pkg->out_of_date);
          printf(fmt, buf);
          break;
        /* list based attributes */
        case 'C':
          print_extinfo_list(pkg->conflicts, NULL, cfg.delim, 0);
          break;
        case 'K':
          print_extinfo_list(pkg->checkdepends, NULL, cfg.delim, 0);
          break;
        case 'D':
          print_extinfo_list(pkg->depends, NULL, cfg.delim, 0);
          break;
        case 'M':
          print_extinfo_list(pkg->makedepends, NULL, cfg.delim, 0);
          break;
        case 'O':
          print_extinfo_list(pkg->optdepends, NULL, cfg.delim, 0);
          break;
        case 'P':
          print_extinfo_list(pkg->provides, NULL, cfg.delim, 0);
          break;
        case 'R':
          print_extinfo_list(pkg->replaces, NULL, cfg.delim, 0);
          break;
        case 'W':
          print_extinfo_list(pkg->keywords, NULL, cfg.delim, 0);
          break;
        case 'G':
          print_extinfo_list(pkg->groups, NULL, cfg.delim, 0);
          break;
        case 'L':
          print_extinfo_list(pkg->licenses, NULL, cfg.delim, 0);
          break;
        case '%':
          fputc('%', stdout);
          break;
        default:
          fputc('?', stdout);
          break;
      }
    } else if (*p == '\\') {
      char ebuf[3];
      ebuf[0] = *p;
      ebuf[1] = *++p;
      ebuf[2] = '\0';
      print_escaped(ebuf);
    } else {
      fputc(*p, stdout);
    }
  }

  return;
}

void print_pkg_installed_tag(aurpkg_t *pkg) {
  alpm_pkg_t *local_pkg;
  const char *instcolor;

  local_pkg = alpm_db_get_pkg(db_local, pkg->name);
  if (local_pkg == NULL) {
    return;
  }

  instcolor = alpm_pkg_vercmp(pkg->version, alpm_pkg_get_version(local_pkg)) > 0
    ? colstr.ood
    : colstr.utd;

  if (streq(pkg->version, alpm_pkg_get_version(local_pkg))) {
    printf(" %s[%sinstalled%s]%s", colstr.url, instcolor, colstr.url, colstr.nc);
  } else {
    printf(" %s[%sinstalled: %s%s]%s", colstr.url, instcolor,
        alpm_pkg_get_version(local_pkg), colstr.url, colstr.nc);
  }
}

void print_pkg_info(aurpkg_t *pkg) {
  if (pkg->ignored) {
    return;
  }

  print_colored("Repository", colstr.repo, "aur");
  printf("Name           : %s%s%s", colstr.pkg, pkg->name, colstr.nc);
  print_pkg_installed_tag(pkg);
  fputc('\n', stdout);

  if (!streq(pkg->name, pkg->pkgbase)) {
    print_colored("PackageBase", colstr.pkg, pkg->pkgbase);
  }

  print_colored("Version", pkg->out_of_date ? colstr.ood : colstr.utd, pkg->version);
  print_colored("URL", colstr.url, pkg->upstream_url);
  printf("AUR Page       : %shttps://%s/packages/%s%s\n",
      colstr.url, cfg.aur_domain, pkg->name, colstr.nc);
  if (pkg->keywords) {
    print_extinfo_list(pkg->keywords, "Keywords", kListDelim, 1);
  }
  print_extinfo_list(pkg->groups, "Groups", kListDelim, 1);

  print_extinfo_list(pkg->depends, "Depends On", kListDelim, 1);
  print_extinfo_list(pkg->makedepends, "Makedepends", kListDelim, 1);
  print_extinfo_list(pkg->checkdepends, "Checkdepends", kListDelim, 1);
  print_extinfo_list(pkg->provides, "Provides", kListDelim, 1);
  print_extinfo_list(pkg->conflicts, "Conflicts With", kListDelim, 1);

  if (pkg->optdepends) {
    char **i = pkg->optdepends;
    printf("Optional Deps  : %s\n", *i);
    while (*++i) {
      printf("%-*s%s\n", kInfoIndent, "", *i);
    }
  }

  print_extinfo_list(pkg->replaces, "Replaces", kListDelim, 1);
  print_extinfo_list(pkg->licenses, "License", kListDelim, 1);

  printf("Votes          : %d\n", pkg->votes);
  printf("Popularity     : %.2f\n", pkg->popularity);

  printf("Out of Date    : ");
  if (pkg->out_of_date) {
    char str[42];
    struct tm t;

    strftime(str, 42, "%c", localtime_r(&pkg->out_of_date, &t));

    printf("%sYes%s [%s]\n", colstr.ood, colstr.nc, str);
  } else {
    printf("%sNo%s\n", colstr.utd, colstr.nc);
  }

  print_colored("Maintainer", NULL, pkg->maintainer ? pkg->maintainer : "(orphan)");
  print_time("Submitted", &pkg->submitted_s);
  print_time("Last Modified", &pkg->modified_s);

  printf("Description    : ");
  indentprint(pkg->description, kInfoIndent);
  fputs("\n\n", stdout);
}

void print_pkg_search(aurpkg_t *pkg) {
  if (pkg->ignored) {
    return;
  }

  if (cfg.quiet) {
    printf("%s%s%s\n", colstr.pkg, pkg->name, colstr.nc);
  } else {
    printf("%saur/%s%s%s %s%s%s%s (%d, %.2f)", colstr.repo, colstr.nc, colstr.pkg,
        pkg->name, pkg->out_of_date ? colstr.ood : colstr.utd, pkg->version,
        NCFLAG(pkg->out_of_date, " <!>"), colstr.nc, pkg->votes, pkg->popularity);
    print_pkg_installed_tag(pkg);
    printf("\n    ");
    indentprint(pkg->description, kSearchIndent);
    fputc('\n', stdout);
  }
}

void print_results(aurpkg_t **packages, void (*printfn)(aurpkg_t*)) {
  aurpkg_t **r;

  if (printfn == NULL || packages == NULL) {
    return;
  }

  qsort(packages, aur_packages_count(packages), sizeof(*packages), aurpkg_cmp);
  for (r = packages; *r; r++) {
    printfn(*r);
  }
}

void resolve_one_dep(struct task_t *task, const char *depend) {
  char *sanitized = strdup(depend);

  sanitized[strcspn(sanitized, "<>=")] = '\0';

  if (!alpm_list_find_str(cfg.targets, sanitized)) {
    pthread_mutex_lock(&listlock);
    cfg.targets = alpm_list_add(cfg.targets, sanitized);
    pthread_mutex_unlock(&listlock);
  } else {
    free(sanitized);
    return;
  }

  if (alpm_find_satisfier(alpm_db_get_pkgcache(db_local), depend)) {
    cwr_printf(LOG_DEBUG, "%s is already satisified\n", depend);
  } else {
    if (!pkg_is_binary(depend)) {
      aur_packages_free(task_download(task, sanitized));
    }
  }

  return;
}

void resolve_pkg_dependencies(struct task_t *task, aurpkg_t *package) {
  struct deparray_t {
    char **array;
    const char *name;
  } deparrays[] = {
    { package->depends, "depends" },
    { package->makedepends, "makedepends" },
    { package->checkdepends, "checkdepends" },
  };
  unsigned i;

  for (i = 0; i < ARRAYSIZE(deparrays); ++i) {
    const struct deparray_t *d = &deparrays[i];

    if (d->array) {
      char **p;

      cwr_printf(LOG_DEBUG, "resolving %s for %s\n", d->name, package->name);
      for (p = d->array; *p; ++p) {
        resolve_one_dep(task, *p);
      }
    }
  }
}

int ch_working_dir(void) {
  if (!(cfg.opmask & OP_DOWNLOAD)) {
    return 0;
  }

  if (access(cfg.working_dir ? cfg.working_dir : ".", W_OK) != 0) {
    fprintf(stderr, "error: cannot write to %s: %s\n",
        cfg.working_dir ? cfg.working_dir : "current directory", strerror(errno));
    return 1;
  }

  if (cfg.working_dir && chdir(cfg.working_dir) != 0) {
      fprintf(stderr, "error: failed to chdir to %s: %s\n", cfg.working_dir,
          strerror(errno));
      return 1;
  }

  free(cfg.working_dir);
  cfg.working_dir = getcwd(NULL, 0);

  return 0;
}

void strings_init(void) {
  if (cfg.color > 0) {
    colstr.error = BOLDRED "::" NC;
    colstr.warn = BOLDYELLOW "::" NC;
    colstr.info = BOLDBLUE "::" NC;
    colstr.pkg = BOLD;
    colstr.repo = BOLDMAGENTA;
    colstr.url = BOLDCYAN;
    colstr.ood = BOLDRED;
    colstr.utd = BOLDGREEN;
    colstr.nc = NC;
  }

  /* guard against delim being something other than kListDelim if extinfo
   * and format aren't provided */
  cfg.delim = cfg.format ? cfg.delim : kListDelim;
}

size_t strtrim(char *str) {
  char *left = str, *right;

  if (!str || *str == '\0') {
    return 0;
  }

  while (isspace((unsigned char)*left)) {
    left++;
  }
  if (left != str) {
    memmove(str, left, (strlen(left) + 1));
  }

  if (*str == '\0') {
    return 0;
  }

  right = (char*)rawmemchr(str, '\0') - 1;
  while (isspace((unsigned char)*right)) {
    right--;
  }
  *++right = '\0';

  return right - left;
}

aurpkg_t **task_download(struct task_t *task, const char *arg) {
  if (pkg_is_binary(arg)) {
    return NULL;
  } else {
    return download(task, arg);
  }
}

aurpkg_t **rpc_do(struct task_t *task, rpc_type type, const char *arg) {
  struct buffer_t response = { NULL, 0, 0 };
  _cleanup_free_ char *url = NULL;
  aurpkg_t **packages = NULL;
  int r, packagecount;

  url = aur_build_rpc_url(task->aur, type, cfg.search_by, arg);
  if (url == NULL) {
    return NULL;
  }

  task_reset_for_rpc(task, url, &response);
  if (task_http_execute(task, url, arg) != 0) {
    return NULL;
  }

  r = aur_packages_from_json(response.data, &packages, &packagecount);
  if (r < 0) {
    cwr_fprintf(stderr, LOG_ERROR, "[%s]: json parsing failed: %s\n", arg, strerror(-r));
    return NULL;
  }

  cwr_printf(LOG_DEBUG, "rpc %d request for %s returned %d results\n",
      type, arg, packagecount);

  free(response.data);

  return packages;
}

int find_search_fragment(const char *arg, char **fragment) {
  int span = 0;
  const char *argstr;

  for (argstr = arg; *argstr; argstr++) {
    span = strcspn(argstr, kRegexChars);

    /* given 'cow?', we can't include w in the search */
    if (argstr[span] == '?' || argstr[span] == '*') {
      span--;
    }

    /* a string inside [] or {} cannot be a valid span */
    if (strchr("[{", *argstr)) {
      argstr = strpbrk(argstr + span, "]}");
      if (!argstr) {
        cwr_fprintf(stderr, LOG_ERROR, "invalid regular expression: %s\n", arg);
        return -EINVAL;
      }
      continue;
    }

    if (span >= 2) {
      break;
    }
  }

  if (span < 2) {
    cwr_fprintf(stderr, LOG_ERROR, "search string '%s' too short\n", arg);
    return -ENOMSG;
  }

  *fragment = strndup(argstr, span);
  return 0;
}

rpc_type rpc_op_from_opmask(int opmask) {
  if (opmask & OP_SEARCH) {
    return RPC_SEARCH;
  } else {
    return RPC_INFO;
  }
}

aurpkg_t **task_query(struct task_t *task, const char *arg) {
  _cleanup_free_ char *fragment = NULL;

  if (allow_regex()) {
    if (find_search_fragment(arg, &fragment) < 0) {
      return NULL;
    }

    cwr_printf(LOG_DEBUG, "searching with fragment '%s' from '%s'\n", fragment, arg);
    arg = fragment;
  }

  return rpc_do(task, rpc_op_from_opmask(cfg.opmask), arg);
}

aurpkg_t **task_update(struct task_t *task, const char *arg) {
  aurpkg_t **packages;
  alpm_pkg_t *pmpkg;

  cwr_printf(LOG_VERBOSE, "Checking %s%s%s for updates...\n",
      colstr.pkg, arg, colstr.nc);

  packages = rpc_do(task, RPC_INFO, arg);
  if (packages == NULL) {
    return NULL;
  }

  pmpkg = alpm_db_get_pkg(db_local, arg);
  if (!pmpkg) {
    cwr_fprintf(stderr, LOG_WARN, "skipping uninstalled package %s\n", arg);
    goto finish;
  }

  if (alpm_pkg_vercmp(packages[0]->version, alpm_pkg_get_version(pmpkg)) > 0) {
    if (alpm_list_find(cfg.ignore.pkgs, arg, globcompare)) {
      if (!cfg.quiet) {
        cwr_fprintf(stderr, LOG_WARN, "%s%s%s [ignored] %s%s%s -> %s%s%s\n",
            colstr.pkg, arg, colstr.nc,
            colstr.ood, alpm_pkg_get_version(pmpkg), colstr.nc,
            colstr.utd, packages[0]->version, colstr.nc);
      }
      goto finish;
    }

    if (cfg.opmask & OP_DOWNLOAD) {
      aur_packages_free(task_download(task, packages[0]->name));
    } else {
      if (cfg.quiet) {
        printf("%s%s%s\n", colstr.pkg, arg, colstr.nc);
      } else {
        cwr_printf(LOG_INFO, "%s%s %s%s%s -> %s%s%s\n",
            colstr.pkg, arg,
            colstr.ood, alpm_pkg_get_version(pmpkg), colstr.nc,
            colstr.utd, packages[0]->version, colstr.nc);
      }
    }

    return packages;
  }

finish:
  aur_packages_free(packages);
  return NULL;
}

void *thread_pool(void *arg) {
  aurpkg_t **packages = NULL;
  struct task_t task = *(struct task_t *)arg;

  task.curl = curl_easy_init();
  if (!task.curl) {
    cwr_fprintf(stderr, LOG_ERROR, "curl: failed to initialize handle\n");
    return NULL;
  }

  while (1) {
    char *job = NULL;
    aurpkg_t **ret;

    pthread_mutex_lock(&listlock);
    if (workq) {
      job = workq->data;
      workq = workq->next;
    }
    pthread_mutex_unlock(&listlock);

    if (!job) {
      break;
    }

    ret = task.threadfn(&task, job);
    if (ret != NULL) {
      int r;

      r = aur_packages_append(&packages, ret);
      if (r < 0) {
        cwr_fprintf(stderr, LOG_ERROR, "failed to append task return to package list: %s\n",
            strerror(-r));
      }
    }
  }

  curl_easy_cleanup(task.curl);

  return packages;
}

void usage(void) {
  fprintf(stderr, "cower %s\n"
      "Usage: cower <operations> [options] target...\n\n", COWER_VERSION);
  fprintf(stderr,
      " Operations:\n"
      "  -d, --download            download target(s) -- pass twice to "
                                     "download AUR dependencies\n"
      "  -i, --info                show info for target(s)\n"
      "  -m, --msearch             show packages maintained by target(s)\n"
      "  -s, --search              search for target(s)\n"
      "  -u, --update              check for updates against AUR -- can be combined "
                                     "with the -d flag\n\n");
  fprintf(stderr, " General options:\n"
      "      --by <search-by>      search by one of 'name', 'name-desc', or 'maintainer'\n"
      "      --domain <fqdn>       point cower at a different AUR (default: aur.archlinux.org)\n"
      "  -f, --force               overwrite existing files when downloading\n"
      "  -h, --help                display this help and exit\n"
      "      --ignore <pkg>        ignore a package upgrade (can be used more than once)\n"
      "      --ignorerepo[=repo]   ignore some or all binary repos\n"
      "  -t, --target <dir>        specify an alternate download directory\n"
      "      --threads <num>       limit number of threads created\n"
      "      --timeout <num>       specify connection timeout in seconds\n"
      "  -V, --version             display version\n\n");
  fprintf(stderr, " Output options:\n"
      "  -c[WHEN], --color[=WHEN]  use colored output. WHEN is `never', `always', or `auto'\n"
      "      --debug               show debug output\n"
      "      --format <string>     print package output according to format string\n"
      "  -o, --ignore-ood          skip displaying out of date packages\n"
      "      --no-ignore-ood       the opposite of --ignore-ood\n"
      "      --sort <key>          sort results in ascending order by key\n"
      "      --rsort <key>         sort results in descending order by key\n"
      "      --listdelim <delim>   change list format delimeter\n"
      "      --literal             disable regex search, interpret target as a literal string\n"
      "  -q, --quiet               output less\n"
      "  -v, --verbose             output more\n\n");
}

void version(void) {
  fputs("\n  " COWER_VERSION "\n", stdout);
  fputs("     \\\n"
        "      \\\n"
        "        ,__, |    |\n"
        "        (oo)\\|    |___\n"
        "        (__)\\|    |   )\\_\n"
        "          U  |    |_w |  \\\n"
        "             |    |  ||   *\n"
        "\n"
        "             Cower....\n\n", stdout);
}

int read_targets_from_file(FILE *in, alpm_list_t **targets) {
  char line[BUFSIZ];
  int i = 0, c = 0, end = 0;
  while (!end) {
    c = fgetc(in);

    if (c == EOF) {
      end = 1;
    }

    if (end || isspace(c)) {
      line[i] = '\0';
      /* avoid adding zero length arg, if multiple spaces separate args */
      if (i > 0) {
        if (!alpm_list_find_str(*targets, line)) {
          cwr_printf(LOG_DEBUG, "adding target: %s\n", line);
          *targets = alpm_list_add(*targets, strdup(line));
        }
        i = 0;
      }
    } else {
      line[i] = c;
      ++i;
      if (i >= BUFSIZ) {
        cwr_fprintf(stderr, LOG_ERROR, "buffer overflow detected in stdin\n");
        return -1;
      }
    }
  }

  return 0;
}

aurpkg_t **cower_perform(struct task_t *task, int num_threads) {
  aurpkg_t **results = NULL;
  _cleanup_free_ pthread_t *threads = NULL;
  int i;

  threads = malloc(num_threads * sizeof(*threads));
  if (threads == NULL) {
    return NULL;
  }

  for (i = 0; i < num_threads; i++) {
    int r;

    r = pthread_create(&threads[i], NULL, thread_pool, task);
    if (r != 0) {
      cwr_fprintf(stderr, LOG_ERROR, "failed to spawn new thread: %s\n",
          strerror(r));
      return NULL;
    }
  }

  for (i = 0; i < num_threads; i++) {
    aurpkg_t **thread_return;

    pthread_join(threads[i], (void**)&thread_return);
    if (thread_return != NULL) {
      int r;

      r = aur_packages_append(&results, thread_return);
      if (r < 0) {
        cwr_fprintf(stderr, LOG_ERROR,
            "failed to append thread result to package list: %s\n", strerror(-r));
      }
    }
  }

  return filter_results(results);
}

int main(int argc, char *argv[]) {
  int num_threads, ret;
  aurpkg_t **results;
  void (*printfn)(aurpkg_t*) = NULL;
  struct task_t task;

  setlocale(LC_ALL, "");

  /* Override LC_NUMERIC with a C locale for now. This matches what the AUR
   * responds with in the RPC interface. Once we're done gathering data, we'll
   * reset to the user's chosen LC_NUMERIC locale. */
  setlocale(LC_NUMERIC, "C");

  ret = parse_configfile();
  if (ret != 0) {
    return ret;
  }

  ret = parse_options(argc, argv);
  if (ret != 0) {
    return ret;
  }

  ret = aur_new("https", cfg.aur_domain, &task.aur);
  if (ret < 0) {
    fprintf(stderr, "error: aur_new failed: %s\n", strerror(-ret));
    return 1;
  }

  if (cfg.frompkgbuild) {
    /* treat arguments as filenames to load/extract */
    cfg.targets = load_targets_from_files(cfg.targets);
  } else if (alpm_list_find_str(cfg.targets, "-")) {
    char *vdata;
    cfg.targets = alpm_list_remove_str(cfg.targets, "-", &vdata);
    free(vdata);
    cwr_printf(LOG_DEBUG, "reading targets from stdin\n");
    ret = read_targets_from_file(stdin, &cfg.targets);
    if (ret != 0) {
      goto finish;
    }
    if (!freopen(ctermid(NULL), "r", stdin)) {
      cwr_printf(LOG_DEBUG, "failed to reopen stdin for reading\n");
    }
  }

  pmhandle = alpm_init();
  if (!pmhandle) {
    cwr_fprintf(stderr, LOG_ERROR, "failed to initialize alpm library\n");
    goto finish;
  }

  /* allow specific updates to be provided instead of examining all foreign pkgs */
  if ((cfg.opmask & OP_UPDATE) && !cfg.targets) {
    cfg.targets = alpm_find_foreign_pkgs();
    if (cfg.targets == NULL) {
      /* no foreign packages found, just exit */
      goto finish;
    }
  }

  /* override task behavior */
  if (cfg.opmask & OP_UPDATE) {
    task.threadfn = task_update;
  } else if (cfg.opmask & OP_INFO) {
    task.threadfn = task_query;
    printfn = cfg.format ? print_pkg_formatted : print_pkg_info;
  } else if (cfg.opmask & OP_SEARCH) {
    task.threadfn = task_query;
    printfn = cfg.format ? print_pkg_formatted : print_pkg_search;
  } else if (cfg.opmask & OP_DOWNLOAD) {
    task.threadfn = task_download;
  }

  workq = cfg.targets;

  num_threads = alpm_list_count(cfg.targets);
  if (num_threads == 0) {
    fprintf(stderr, "error: no targets specified (use -h for help)\n");
    goto finish;
  } else if (num_threads > cfg.maxthreads) {
    num_threads = cfg.maxthreads;
  }

  results = cower_perform(&task, num_threads);

  /* we need to exit with a non-zero value when:
   * a) search/info/download returns nothing
   * b) update (without download) returns something
   * this is opposing behavior, so just XOR the result on a pure update */
  ret = (!have_unignored_results(results) ^ !(cfg.opmask & ~OP_UPDATE));

  /* Restore the user-defined LC_NUMERIC locale before printing results. */
  setlocale(LC_NUMERIC, "");
  print_results(results, printfn);

  aur_packages_free(results);

finish:
  free(cfg.working_dir);
  FREELIST(cfg.targets);
  FREELIST(cfg.ignore.pkgs);
  FREELIST(cfg.ignore.repos);

  cwr_printf(LOG_DEBUG, "releasing curl\n");

  aur_free(task.aur);

  cwr_printf(LOG_DEBUG, "releasing alpm\n");
  alpm_release(pmhandle);

  return ret;
}
