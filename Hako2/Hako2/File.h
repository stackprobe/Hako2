extern struct _finddata_t lastFindData;

void getFileList(char *dir, autoList<char *> *subDirs, autoList<char *> *files);
void updateFindData(char *path);

// ----

int accessible(char *path);
char *refLocalPath(char *path);
char *refExt(char *path);
