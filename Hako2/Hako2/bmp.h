autoTable<uint> *readBmpFile(autoList<uchar> *fileData);
autoTable<uint> *readBmpFile_x(autoList<uchar> *fileData);

void writeBmpFile(autoList<uchar> *fileData, autoTable<uint> *bmp);
void writeBmpFile_x(autoList<uchar> *fileData, autoTable<uint> *bmp);
void writeBmpFile(char *file, autoTable<uint> *bmp);
void writeBmpFile_cx(char *file, autoTable<uint> *bmp);
void writeBmpFile_xx(char *file, autoTable<uint> *bmp);

#define m_bmpColor(r, g, b) \
	((r) * 65536 + (g) * 256 + (b))
