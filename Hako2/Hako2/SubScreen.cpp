#include "all.h"

static oneObject(autoList<SubScreen_t *>, new autoList<SubScreen_t *>(), GetSubScreenList);

SubScreen_t *CreateSubScreen(int w, int h, int aFlag)
{
	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);

	SubScreen_t *i = nb(SubScreen_t);

	i->W = w;
	i->H = h;
	i->AFlag = aFlag;
	i->Handle = -1;

	GetSubScreenList()->AddElement(i);
	return i;
}
void ReleaseSubScreen(SubScreen_t *i)
{
	if(i == NULL)
		return;

	for(int index = 0; index < GetSubScreenList()->GetCount(); index++)
	{
		if(GetSubScreenList()->GetElement(index) == i)
		{
			GetSubScreenList()->FastDesertElement(index);
			goto endDesert;
		}
	}
	error(); // not found
endDesert:

	if(i->Handle != -1)
		errorCase(DeleteGraph(i->Handle)); // ? 失敗

	memFree(i);
}
int GetHandle(SubScreen_t *i)
{
	errorCase(i == NULL);

	if(i->Handle == -1)
	{
		i->Handle = MakeScreen(i->W, i->H, i->AFlag ? TRUE : FALSE);
		errorCase(i->Handle == -1); // ? 失敗
	}
	return i->Handle;
}
int Get_W(SubScreen_t *i)
{
	return i->W;
}
int Get_H(SubScreen_t *i)
{
	return i->H;
}

int CurrDrawScreenHandle = DX_SCREEN_BACK;

void ChangeDrawScreen(int handle)
{
	errorCase(SetDrawScreen(handle)); // ? 失敗
	CurrDrawScreenHandle = handle;
}
void ChangeDrawScreen(SubScreen_t *screen)
{
	ChangeDrawScreen(GetHandle(screen));
}
void RestoreDrawScreen(void)
{
	ChangeDrawScreen(Gnd.MainScreen ? GetHandle(Gnd.MainScreen) : DX_SCREEN_BACK);
}

i2D_t GetSubScreenSize(SubScreen_t *screen)
{
	i2D_t size;

	size.X = screen->W;
	size.Y = screen->H;

	return size;
}
i2D_t GetDrawScreenSize(void) // 描画領域のサイズ？
{
	int w;
	int h;
	int cbd;

	errorCase(GetScreenState(&w, &h, &cbd));
	errorCase(!m_isRange(w, 1, IMAX));
	errorCase(!m_isRange(h, 1, IMAX));

	i2D_t size;

	size.X = w;
	size.Y = h;

	return size;
}
void UnloadAllSubScreenHandle(void)
{
	ChangeDrawScreen(DX_SCREEN_BACK); // これから開放するハンドルであるとマズいので...

	for(int index = 0; index < GetSubScreenList()->GetCount(); index++)
	{
		SubScreen_t *i = GetSubScreenList()->GetElement(index);

		if(i->Handle != -1)
		{
			errorCase(DeleteGraph(i->Handle)); // ? 失敗
			i->Handle = -1;
		}
	}
}
