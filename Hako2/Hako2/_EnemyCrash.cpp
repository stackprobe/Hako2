#include "all.h"

typedef struct Crash_st
{
	fPInt_t X;
	fPInt_t Y;
	fPInt_t W;
	fPInt_t H;
}
Crash_t;

static oneObject(autoList<Crash_t *>, new autoList<Crash_t *>(), GetCrashList);

void ResetEnemyCrash(void)
{
	GetCrashList()->Clear((void (*)(Crash_t *))memFree);
}
void AddEnemyCrash(fPInt_t x, fPInt_t y, fPInt_t w, fPInt_t h)
{
	errorCase(x < -IMAX || IMAX < x);
	errorCase(y < -IMAX || IMAX < y);
	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);

	Crash_t *i = nb_(Crash_t);

	i->X = x;
	i->Y = y;
	i->W = w;
	i->H = h;

	GetCrashList()->AddElement(i);
}
static int IsCrashedLinear(int s1, int w1, int s2, int w2)
{
	return
		s1 < s2 + w2 &&
		s2 < s1 + w1;
}
int IsEnemyCrashed(fPInt_t x, fPInt_t y, fPInt_t w, fPInt_t h)
{
	errorCase(x < -IMAX || IMAX < x);
	errorCase(y < -IMAX || IMAX < y);
	errorCase(w < 1 || IMAX < w);
	errorCase(h < 1 || IMAX < h);

	for(int index = 0; index < GetCrashList()->GetCount(); index++)
	{
		Crash_t *i = GetCrashList()->GetElement(index);

		if(
			IsCrashedLinear(x, w, i->X, i->W) &&
			IsCrashedLinear(y, h, i->Y, i->H)
			)
		{
			return 1;
		}
	}
	return 0;
}
