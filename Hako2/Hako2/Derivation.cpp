#include "all.h"

static autoList<DerInfo_t *> *DerList;

static void AddDer(int parentPicId, int x, int y, int w, int h)
{
	DerInfo_t *i = nb_(DerInfo_t);

	i->ParentPicId = parentPicId;
	i->X = x;
	i->Y = y;
	i->W = w;
	i->H = h;

	DerList->AddElement(i);
}
static void AddAnimation(int parentPicId, int x, int y, int w, int h, int xNum, int yNum, int xStep = -1, int yStep = -1, int xyMode = 0)
{
	if(xStep == -1) xStep = w;
	if(yStep == -1) yStep = h;

	if(xyMode)
	{
		for(int xc = 0; xc < xNum; xc++)
		for(int yc = 0; yc < yNum; yc++)
		{
			AddDer(parentPicId, x + xc * xStep, y + yc * yStep, w, h);
		}
	}
	else
	{
		for(int yc = 0; yc < yNum; yc++)
		for(int xc = 0; xc < xNum; xc++)
		{
			AddDer(parentPicId, x + xc * xStep, y + yc * yStep, w, h);
		}
	}
}
static void CreateDerList(void)
{
	DerList = new autoList<DerInfo_t *>();

	AddAnimation(P_DUMMY, 0, 0, 25, 25, 2, 2);

	// app >

	for(int color = 0; color < 10; color++)
	{
		AddAnimation(P_STAR_00 + color, 0, 0, 120, 120, 5, 3);
	}
	for(int color = 0; color < 9; color++)
	{
		AddAnimation(P_FIRE_00 + color, 0, 0, 120, 120, 10, 1);
	}

	AddAnimation(P_KIGOU_Y, 0, 0, 60, 60, 10, 2);
	AddAnimation(P_KIGOU_W, 0, 0, 60, 60, 10, 2);
	AddAnimation(P_KIGOU_DW, 0, 0, 60, 60, 10, 2);

	AddAnimation(P_MAHOUJIN, 0, 0, 120, 120, 14, 1);

	AddAnimation(P_WINDOW_Y, 64, 0, 16, 16, 4, 4);
	AddAnimation(P_WINDOW_B, 64, 0, 16, 16, 4, 4);
	AddAnimation(P_WINDOW_G, 64, 0, 16, 16, 4, 4);

	AddAnimation(P_BLUEEXPLOSION, 0, 0, 640, 240, 1, 7);

	AddAnimation(P_MAPCHIP_A1, 0, 0, 16, 16, 32, 24);
	AddAnimation(P_MAPCHIP_A4, 0, 0, 16, 16, 32, 32);
	AddAnimation(P_MAPCHIP_A5, 0, 0, 32, 32, 8, 16);
	AddAnimation(P_MAPCHIP_B, 0, 0, 32, 32, 16, 16);
	AddAnimation(P_MAPCHIP_D, 0, 0, 32, 32, 16, 16);
	AddAnimation(P_MAPCHIP_E, 0, 0, 32, 32, 16, 16);

	for(int color = 0; color < METEOR_P_MAX; color++)
	{
		AddAnimation(P_METEOR_00 + color, 0, 0, 120, 120, 5, 4);
	}
//	AddAnimation(P_RESPAWN_POINT, 0, 0, 640, 480, 3, 10); // old
//	AddAnimation(P_RESPAWN_POINT, 0, 0, 640, 480, 3, 10, -1, -1, 1); // 違うぽい
	AddAnimation(P_RESPAWN_POINT, 0, 0, 200, 200, 5, 2);

	// < app

	errorCase(DerList->GetCount() != D_MAX);
}
static autoList<DerInfo_t *> *GetDerList(void)
{
	if(!DerList)
		CreateDerList();

	return DerList;
}

static int LoadDer(PicInfo_t *parent, int x, int y, int w, int h)
{
	int der_h;

	// check rect {
	errorCase(x < 0 || IMAX < x);
	errorCase(y < 0 || IMAX < y);
	errorCase(w < 1 || IMAX - x < w);
	errorCase(h < 1 || IMAX - y < h);
	// }

	if(
		parent->W < x + w ||
		parent->H < y + h
		)
		// ? 範囲外
	{
		m_range(w, 1, 50);
		m_range(h, 1, 50);

		der_h = DerivationGraph(0, 0, w, h, GetPicRes()->GetHandle(P_DUMMY)->Handle); // ダミー画像
	}
	else
		der_h = DerivationGraph(x, y, w, h, parent->Handle);

	errorCase(der_h == -1); // ? 失敗
	return der_h;
}
static void UnloadDer(int h)
{
	if(h == -1) // ? 未オープン
		return;

	errorCase(DeleteGraph(h)); // ? 失敗
}

int Der(resCluster<PicInfo_t *> *resclu, int derId)
{
	errorCase(derId < 0 || D_MAX <= derId);

	int h = resclu->GetDerHandleList()->RefElement(derId, -1);

	if(h == -1)
	{
		DerInfo_t *i = GetDerList()->GetElement(derId);

		h = LoadDer(
			resclu->GetHandle(i->ParentPicId),
			i->X,
			i->Y,
			i->W,
			i->H
			);

		resclu->GetDerHandleList()->PutElement(derId, h, -1);
	}
	return h;
}
int Der_W(int derId)
{
	return GetDerList()->GetElement(derId)->W;
}
int Der_H(int derId)
{
	return GetDerList()->GetElement(derId)->H;
}

void UnloadAllDer(resCluster<PicInfo_t *> *resclu)
{
	for(int derId = 0; derId < resclu->GetDerHandleList()->GetCount(); derId++)
	{
		UnloadDer(resclu->GetDerHandleList()->GetElement(derId));
	}
	resclu->GetDerHandleList()->Clear();
}
