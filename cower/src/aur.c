#include "aur.h"

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <curl/curl.h>

struct rpc_method_t {
  const char *type;
  const char *argkey;
};

static const struct rpc_method_t method_table[] = {
  [RPC_SEARCH] = {
    .type = "type=search",
    .argkey = "arg",
  },
  [RPC_INFO] = {
    .type = "type=info",
    .argkey = "arg[]",
  },
};

static const char *search_by_to_string[] = {
  [SEARCHBY_NAME]       = "name",
  [SEARCHBY_NAME_DESC]  = "name-desc",
  [SEARCHBY_MAINTAINER] = "maintainer",
};

static char *aur_vurlf(aur_t *aur, const char *urlpath_format, va_list ap) {
  int len;
  va_list aq;
  char *out;

  va_copy(aq, ap);
  len = strlen(aur->urlprefix) + vsnprintf(NULL, 0, urlpath_format, aq) + 1;
  va_end(aq);

  out = malloc(len);
  if (out == NULL) {
    return NULL;
  }

  vsprintf(stpcpy(out, aur->urlprefix), urlpath_format, ap);

  return out;
}

static char *aur_urlf(aur_t *aur, const char *urlpath_format, ...) {
  va_list ap;
  char *out;

  va_start(ap, urlpath_format);
  out = aur_vurlf(aur, urlpath_format, ap);
  va_end(ap);

  return out;
}

char *aur_build_rpc_url(aur_t *aur, rpc_type type, rpc_by by, const char *arg) {
  char *escaped, *url;
  const struct rpc_method_t *method = &method_table[type];

  escaped = curl_easy_escape(NULL, arg, 0);
  if (escaped == NULL) {
    return NULL;
  }

  switch (type) {
  case RPC_SEARCH:
    url = aur_urlf(aur, "/rpc.php?v=%d&%s&%s=%s&by=%s", aur->rpc_version,
        method->type, method->argkey, escaped, search_by_to_string[by]);
    break;
  default:
    url = aur_urlf(aur, "/rpc.php?v=%d&%s&%s=%s", aur->rpc_version,
        method->type, method->argkey, escaped);
    break;
  }

  free(escaped);

  return url;
}

char *aur_build_url(aur_t *aur, const char *urlpath) {
  return aur_urlf(aur, urlpath);
}

int aur_new(const char *proto, const char *domain, aur_t **aur) {
  aur_t *a;

  if (proto == NULL || domain == NULL || aur == NULL) {
    return -EINVAL;
  }

  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
    return -ENOSYS;
  }

  a = calloc(1, sizeof(*a));
  if (a == NULL) {
    return -ENOMEM;
  }

  if (asprintf(&a->urlprefix, "%s://%s", proto, domain) < 0) {
    free(a);
    return -ENOMEM;
  }

  a->rpc_version = 5;

  *aur = a;
  return 0;
}

void aur_free(aur_t *aur) {
  if (aur == NULL) {
    return;
  }

  curl_global_cleanup();

  free(aur->urlprefix);
  free(aur);
}
