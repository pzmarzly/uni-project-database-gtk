#include "RepoString.h"
#include <stdlib.h>
#include <string.h>
#include "Utils.h"

// Returns the ID of the first part of the string.
static bool string_to_id(Repo *repo, String str, ID *output) {
    StringFragment fragment;
    unsigned max = repo_len(repo, TableStringFragment);
    for (ID i = str; i < max;) {
        if (!repo_get(repo, TableStringFragment, i, &fragment)) return false;
        if (fragment.string > str) return false;
        if (fragment.string == str) {
            *output = i;
            return true;
        }
        i += str - fragment.string;
    }
    return false;
}

bool repo_string_load(Repo *repo, String str, char **dest) {
    StringFragment fragment;
    ID first, id;
    if (!string_to_id(repo, str, &first)) return false;

    // Count the parts.
    id = first;
    int parts = 0;
    while (true) {
        if (!repo_get(repo, TableStringFragment, id, &fragment)) break;
        if (fragment.string != str) break;
        if (fragment.len == STRING_FRAGMENT_TOMBSTONE) return false;
        parts++;
        if (fragment.len != STRING_FRAGMENT_MORE) break;
    }

    *dest = calloc(parts * STRING_FRAGMENT_MAX + 1, sizeof(char));
    id = first;
    for (int i = 0; i < parts; i++) {
        if (!repo_get(repo, TableStringFragment, id, &fragment)) return false;
        strcat(*dest, fragment.data);
    }
    return true;
}

String repo_string_save(Repo *repo, char **src) {
    // Get the last String and add 1.
    StringFragment fragment;
    unsigned table_size = repo_len(repo, TableStringFragment);
    String str = 0;
    ID first = table_size;
    if (table_size > 0) {
        if (!repo_get(repo, TableStringFragment, table_size - 1, &fragment))
            bug("Nie można wczytać ostatniego elementu StringFragment.");
        str = fragment.string + 1;
    }

    ID id = first;
    int data = strlen(*src);
    for (int written = 0; written <= data;) {
        int to_copy = data - written;
        fragment.len = to_copy;
        if (fragment.len > STRING_FRAGMENT_MAX) {
            to_copy = STRING_FRAGMENT_MAX;
            fragment.len = STRING_FRAGMENT_MORE;
        }

        fragment.string = str;
        memcpy(fragment.data, *src + written, to_copy);
        fragment.data[to_copy] = '\0';
        repo_set(repo, TableStringFragment, id, &fragment);

        written += fragment.len;
        id++;

        if (written == data && fragment.len != STRING_FRAGMENT_MORE) break;
    }
    return str;
}

void repo_string_remove(Repo *repo, String str) {
    StringFragment fragment;

    // First part becomes a tombstone (to keep String keys constant).
    ID first;
    if (!string_to_id(repo, str, &first)) return;
    if (!repo_get(repo, TableStringFragment, first, &fragment)) return;
    fragment.len = STRING_FRAGMENT_TOMBSTONE;
    repo_set(repo, TableStringFragment, first, &fragment);

    // Other parts can be deleted.
    while (true) {
        if (!repo_get(repo, TableStringFragment, first + 1, &fragment)) return;
        if (fragment.string != str) return;
        repo_del(repo, TableStringFragment, first + 1);
    }
}
