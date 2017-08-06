#include "all.h"

static PicInfo_t *LoadPic(autoList<uchar> *fileData)
{
	return Pic_GraphicHandle2PicInfo(Pic_SoftImage2GraphicHandle(Pic_FileData2SoftImage(fileData)));
}
static void UnloadPic(PicInfo_t *i)
{
	Pic_ReleasePicInfo(i);
}
oneObject(resCluster<PicInfo_t *>, CreatePicRes(LoadPic, UnloadPic), GetStdPicRes);

static PicInfo_t *LoadGSPic(autoList<uchar> *fileData)
{
	int si_h = Pic_FileData2SoftImage(fileData);
	int w;
	int h;

	Pic_GetSoftImageSize(si_h, w, h);

	for(int x = 0; x < w; x++)
	for(int y = 0; y < h; y++)
	{
		Pic_GetSIPixel(si_h, x, y);

		int lv = (SI_R + SI_G + SI_B) / 3;

		SI_R = lv;
		SI_G = lv;
		SI_B = lv;

		Pic_SetSIPixel(si_h, x, y);
	}
	return Pic_GraphicHandle2PicInfo(Pic_SoftImage2GraphicHandle(si_h));
}
static void UnloadGSPic(PicInfo_t *i)
{
	Pic_ReleasePicInfo(i);
}
oneObject(resCluster<PicInfo_t *>, CreatePicRes(LoadGSPic, UnloadGSPic), GetGSPicRes);
