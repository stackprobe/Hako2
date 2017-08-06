void LoadMap(autoTable<uint> *bmp);
void LoadMap_x(autoTable<uint> *bmp);
void SaveMap(autoTable<uint> *bmp);

void SaveMap(autoList<uchar> *bmpImage);
void SaveMap(char *bmpFile);
void SaveMap_x(char *bmpFile);

void RestoreOriginalMapsIfNeed(void);
void MakeDummyMapsIfNeed(void);
