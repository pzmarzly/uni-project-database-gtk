typedef struct RepoSelect RepoSelect;
RepoSelect repo_select_new();
void repo_select_set_quit_on_destroy(RepoSelect *rs, bool quit_on_destroy);
int repo_select_run(RepoSelect *rs);
