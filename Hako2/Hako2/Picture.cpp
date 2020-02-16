#include "all.h"

// Pic_ >

int Pic_FileData2SoftImage(autoList<uchar> *fileData) // fileData: unbind
{
	int hdl = LoadSoftImageToMem(fileData->ElementAt(0), fileData->GetCount());

	errorCase(hdl == -1); // ? é∏îs

	int w;
	int h;

	errorCase(GetSoftImageSize(hdl, &w, &h)); // ? é∏îs

	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);

	// RGB -> RGBA
	{
		int h2 = MakeARGB8ColorSoftImage(w, h);

		errorCase(h2 == -1); // ? é∏îs
		errorCase(BltSoftImage(0, 0, w, h, hdl, 0, 0, h2)); // ? é∏îs
		errorCase(DeleteSoftImage(hdl) == -1); // ? é∏îs

		hdl = h2;
	}
	return hdl;
}
int Pic_SoftImage2GraphicHandle(int si_h) // si_h: bind
{
	int h = CreateGraphFromSoftImage(si_h);

	errorCase(h == -1); // ? é∏îs
	errorCase(DeleteSoftImage(si_h) == -1); // ? é∏îs

	return h;
}
PicInfo_t *Pic_GraphicHandle2PicInfo(int handle) // handle: bind
{
	PicInfo_t *i = nb_(PicInfo_t);
	int w;
	int h;

	Pic_GetGraphicHandleSize(handle, w, h);

	i->Handle = handle;
	i->W = w;
	i->H = h;

	return i;
}
void Pic_ReleasePicInfo(PicInfo_t *i)
{
	errorCase(DeleteGraph(i->Handle)); // ? é∏îs
	memFree(i);
}

void Pic_GetSoftImageSize(int si_h, int &w, int &h)
{
	errorCase(GetSoftImageSize(si_h, &w, &h)); // ? é∏îs

	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);
}
void Pic_GetGraphicHandleSize(int handle, int &w, int &h)
{
	errorCase(GetGraphSize(handle, &w, &h)); // ? é∏îs

	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);
}

int SI_R;
int SI_G;
int SI_B;
int SI_A;

void Pic_GetSIPixel(int si_h, int x, int y)
{
	errorCase(GetPixelSoftImage(si_h, x, y, &SI_R, &SI_G, &SI_B, &SI_A)); // ? é∏îs

	errorCase(SI_R < 0 || 255 < SI_R);
	errorCase(SI_G < 0 || 255 < SI_G);
	errorCase(SI_B < 0 || 255 < SI_B);
	errorCase(SI_A < 0 || 255 < SI_A);
}
void Pic_SetSIPixel(int si_h, int x, int y)
{
	m_range(SI_R, 0, 255);
	m_range(SI_G, 0, 255);
	m_range(SI_B, 0, 255);
	m_range(SI_A, 0, 255);

	errorCase(DrawPixelSoftImage(si_h, x, y, SI_R, SI_G, SI_B, SI_A)); // ? é∏îs
}

// < Pic_

static oneObject(autoList<resCluster<PicInfo_t *> *>, new autoList<resCluster<PicInfo_t *> *>(), GetPicResList);

resCluster<PicInfo_t *> *CreatePicRes(PicInfo_t *(*picLoader)(autoList<uchar> *), void (*picUnloader)(PicInfo_t *))
{
	resCluster<PicInfo_t *> *res = new resCluster<PicInfo_t *>("Picture.dat", "..\\..\\Picture.txt", P_MAX, 110000000, picLoader, picUnloader);

	GetPicResList()->AddElement(res);
	return res;
}
void UnloadAllPicResHandle(void) // ÉXÉNÉäÅ[ÉìÉÇÅ[ÉhêÿÇËë÷Ç¶íºëOÇ…åƒÇ‘Ç±Ç∆ÅB
{
	for(int index = 0; index < GetPicResList()->GetCount(); index++)
	{
		GetPicResList()->GetElement(index)->UnloadAllHandle();
	}
}

static resCluster<PicInfo_t *> *CurrPicRes;

void SetPicRes(resCluster<PicInfo_t *> *resclu) // resclu: NULL == reset
{
	CurrPicRes = resclu;
}
resCluster<PicInfo_t *> *GetPicRes(void)
{
	if(!CurrPicRes)
		return GetStdPicRes(); // app <- âΩåÃapp?

	return CurrPicRes;
}
void ResetPicRes(void)
{
	CurrPicRes = NULL;
}

int Pic(int picId)
{
	if(picId & DTP)
		return Der(GetPicRes(), picId & ~DTP);

	return GetPicRes()->GetHandle(picId)->Handle;
}
int Pic_W(int picId)
{
	if(picId & DTP)
		return Der_W(picId & ~DTP);

	return GetPicRes()->GetHandle(picId)->W;
}
int Pic_H(int picId)
{
	if(picId & DTP)
		return Der_H(picId & ~DTP);

	return GetPicRes()->GetHandle(picId)->H;
}
