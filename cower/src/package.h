#ifndef PACKAGE_H
#define PACKAGE_H

#include <sys/types.h>

struct aurpkg_t {
  char *name;
  char *description;
  char *maintainer;
  char *pkgbase;
  char *upstream_url;
  char *aur_urlpath;
  char *version;

  int category_id;
  int package_id;
  int pkgbaseid;
  int votes;
  double popularity;
  time_t out_of_date;
  time_t submitted_s;
  time_t modified_s;

  char **licenses;
  char **conflicts;
  char **depends;
  char **groups;
  char **makedepends;
  char **optdepends;
  char **checkdepends;
  char **provides;
  char **replaces;
  char **keywords;

  int ignored;
};
typedef struct aurpkg_t aurpkg_t;

int aur_packages_from_json(const char *json, aurpkg_t ***packages, int *count);

void aur_package_free(aurpkg_t *package);
void aur_packages_free(aurpkg_t **packages);

int aur_packages_count(aurpkg_t **l);
int aur_packages_append(aurpkg_t ***dest, aurpkg_t **src);

#endif  /* PACKAGE_H */
