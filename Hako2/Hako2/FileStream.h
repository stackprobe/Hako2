FILE *fileOpen(char *file, char *mode);
void fileClose(FILE *fp);

autoList<uchar> *readFile(char *file);
autoList<uchar> *readFile_x(char *file);
char *readLine(autoList<uchar> *fileData, int &rIndex);
char *anReadLine(autoList<uchar> *fileData, int &rIndex);
char *nnReadLine(autoList<uchar> *fileData, int &rIndex, char *defaultLine = "");
autoList<char *> *readLines(autoList<uchar> *fileData);
autoList<char *> *readLines_x(autoList<uchar> *fileData);

void writeFile(char *file, autoList<uchar> *fileData);
void writeFile_cx(char *file, autoList<uchar> *fileData);
void writeToken(autoList<uchar> *fileData, char *token);
void writeLine(autoList<uchar> *fileData, char *line);
void writeLine_x(autoList<uchar> *fileData, char *line);

uint64 readUI64(autoList<uchar> *fileData, int &rIndex, int width = 8);
uint readUI32(autoList<uchar> *fileData, int &rIndex, int width = 4);

void writeUI64(autoList<uchar> *fileData, uint64 value, int width = 8);
void writeUI32(autoList<uchar> *fileData, uint value, int width = 4);

autoList<uchar> *readBlock(autoList<uchar> *fileData, int &rIndex, int width);
