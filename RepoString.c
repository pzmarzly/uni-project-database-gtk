#include "RepoString.h"
#include "Utils.h"
#include <stdlib.h>
#include <string.h>

void repo_string_get(Repo *repo, ID id, char **dest) {
  StringMetadata metadata;
  repo_get(repo, TableStringMetadata, id, &metadata);
  if (metadata.len == 0) error("Błąd odczytu - tekst usunięty");

  *dest = malloc(metadata.len * STRING_FRAGMENT_MAX);
  for (ID i = 0; i < metadata.len; i++) {
    StringFragment fragment;
    repo_get(repo, TableStringFragment, metadata.start + i, &fragment);
    memcpy(*dest + i * STRING_FRAGMENT_MAX, fragment.data, STRING_FRAGMENT_MAX);
  }
}

void repo_string_set(Repo *repo, ID id, char **src) {
  repo_string_del(repo, id);

  StringMetadata metadata;
  metadata.start = repo_len(repo, TableStringFragment);
  unsigned data = strlen(*src) + 1;
  metadata.len = data / STRING_FRAGMENT_MAX;
  if (metadata.len * STRING_FRAGMENT_MAX < data)
    metadata.len++;

  for (ID i = 0; i < metadata.len; i++) {
    StringFragment fragment;
    int to_copy = data;
    if (to_copy > STRING_FRAGMENT_MAX)
      to_copy = STRING_FRAGMENT_MAX;
    memcpy(fragment.data, *src + i * STRING_FRAGMENT_MAX, to_copy);
    repo_set(repo, TableStringFragment, metadata.start + i, &fragment);
    data -= to_copy;
  }

  repo_set(repo, TableStringMetadata, id, &metadata);
}

void repo_string_del(Repo *repo, ID id) {
  if (id < repo_string_len(repo)) {
    // Remove string content.
    StringMetadata metadata;
    repo_get(repo, TableStringMetadata, id, &metadata);
    if (metadata.len == 0) error("Błąd usuwania - tekst już usunięty.");
    repo_del_n(repo, TableStringFragment, metadata.start, metadata.len);

    // Fix references to the moved area.
    for (ID i = 0; i < repo_string_len(repo); i++) {
      StringMetadata other;
      repo_get(repo, TableStringMetadata, i, &other);
      if (other.start > metadata.start) {
        other.start -= metadata.len;
        repo_set(repo, TableStringMetadata, i, &other);
      }
    }

    // Leave a tombstone so that IDs don't change.
    metadata.len = 0;
    repo_set(repo, TableStringMetadata, id, &metadata);
  }
}

ID repo_string_len(Repo *repo) { return repo_len(repo, TableStringMetadata); }
