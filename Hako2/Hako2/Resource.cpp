#include "all.h"

int RC_ResId;

static int Res_ReadInt(FILE *fp)
{
	int c1 = fgetc(fp);
	int c2 = fgetc(fp);
	int c3 = fgetc(fp);
	int c4 = fgetc(fp);

	errorCase(
		c1 == EOF ||
		c2 == EOF ||
		c3 == EOF ||
		c4 == EOF
		);

	int value =
		c1 << 8 * 0 |
		c2 << 8 * 1 |
		c3 << 8 * 2 |
		c4 << 8 * 3;

	errorCase(!m_isRange(value, 0, IMAX));
	return value;
}
static void Res_FSeek(FILE *fp, __int64 pos)
{
	errorCase(_fseeki64(fp, pos, SEEK_SET) != 0); // ? Ž¸”s
}
static void *Res_FRead(FILE *fp, int size)
{
	void *buff = memAlloc(size);
	errorCase(fread(buff, 1, size, fp) != size); // ? Ž¸”s
	return buff;
}
autoList<uchar> *LoadFileDataFromCluster(char *clusterFile, int resId, int tweek)
{
	errorCase(m_isEmpty(clusterFile));
	errorCase(!m_isRange(resId, 0, IMAX));
	errorCase(!m_isRange(tweek, 0, IMAX));

	FILE *fp = fileOpen(clusterFile, "rb");

	int num = Res_ReadInt(fp);
	errorCase(num <= resId);
	__int64 pos = (num + 1) * 4;

	for(int count = 0; count < resId; count++)
	{
		pos += Res_ReadInt(fp);
	}
	int size = Res_ReadInt(fp);

	Res_FSeek(fp, pos);
	void *fileData = Res_FRead(fp, size);

	fileClose(fp);

	errorCase(!camellia_decrypt_extend(fileData, size, tweek));
	return new autoList<uchar>((uchar *)fileData, size);
}
