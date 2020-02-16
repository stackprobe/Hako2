#include "all.h"

static autoList<SubScreen_t *> *LogoChars;
static d2D_t *LCPosList;
static double *LCRotList;
static double *LCZoomList;
static Random *Rnd;
static __int64 StartTime;

void Logo(void)
{
	LogoChars = new autoList<SubScreen_t *>();

	for(char *p = GetDatString(DATSTR_AUTHOR); *p; p++) // init LogoChars
	{
		SubScreen_t *ss = CreateSubScreen(20, 26, 1);

		ChangeDrawScreen(ss);

		SetPrint(5, 5);
		PE_Border(GetColor(0, 128, 64)); // == charlotte
//		PE_Border(GetColor(0, 0, 128)); // == navy blue
//		PE_Border(GetColor(41, 86, 178)); // == cerulean blue
		Print_x(xcout("%c", *p));
		PE_Reset();

		RestoreDrawScreen();

		LogoChars->AddElement(ss);
	}
	LCPosList = na_(d2D_t, LogoChars->GetCount());
	LCRotList = na_(double, LogoChars->GetCount());
	LCZoomList = na_(double, LogoChars->GetCount());
	Rnd = new Random((int)time(NULL));

	for(int index = 0; index < LogoChars->GetCount(); index++)
	{
		d2D_t *pos = LCPosList + index;

		const int RANGE = 1000;
		const int MARGIN = 900;

		do
		{
			pos->X = (double)Rnd->BRnd(-RANGE, RANGE + SCREEN_W);
			pos->Y = (double)Rnd->BRnd(-RANGE, RANGE + SCREEN_H);
		}
		while(m_isRange(pos->X, -MARGIN, MARGIN + SCREEN_W) || m_isRange(pos->Y, -MARGIN,  MARGIN + SCREEN_H));

		LCRotList[index] = (Rnd->DRnd() * 50.0 + 50.0) * Rnd->SRnd();
		LCZoomList[index] = Rnd->DRnd() * 50.0 + 50.0;
	}

	// touch...
	{
		MusicTouch(MUS_TITLE);
		MusicTouch(MUS_FLOOR_01);
		MusicTouch(MUS_ENDING);
	}

	for(int frame = 0; ; frame++)
	{
		if(frame == 1)
		{
			StartTime = FrameStartTime;
		}
		if(80 < frame && StartTime + 2000 < FrameStartTime)
		{
			m_maxim(frame, 120);
		}
		if(frame == 160)
		{
			SetCurtain(30, -1.0);
		}
		if(frame == 200)
		{
			break;
		}
		double frmRate = frame / 200.0;

		DPE_SetAlpha(frmRate * frmRate);
//		DPE_SetAlpha(frame < 60 ? 0.0 : 0.1);
		DPE_SetBright(0, 0, 0);
		DrawRect(P_WHITEBOX, 0, 0, SCREEN_W, SCREEN_H);
		DPE_Reset();

		for(int index = 0; index < LogoChars->GetCount(); index++)
		{
			const int LOGO_L = 135;
			const int LOGO_T = 284;
			const int LOGO_SPAN = 30;

			int x = LOGO_L + index * LOGO_SPAN;
			int y = LOGO_T;

			d2D_t *pos = LCPosList + index;

			if(index < frame / 2)
			{
				double mvRt = 0.9;

				if(120 < frame)
					mvRt = 0.45;

				m_approach(pos->X, x, mvRt);
				m_approach(pos->Y, y, mvRt);
				m_approach(LCRotList[index], 0.0, mvRt);
				m_approach(LCZoomList[index], 1.0, mvRt);

				x = m_d2i(pos->X);
				y = m_d2i(pos->Y);

				DPE.GraphicHandleFlag = 1;
				DPE.GraphicSize = makeI2D(40, 52);
				DPE.MosaicOn = 1;
				DrawBegin(GetHandle(LogoChars->GetElement(index)), x + 10, y + 16);
				DrawZoom(LCZoomList[index]);
				DrawRotate(LCRotList[index]);
				DrawEnd();
				DPE_Reset();
			}
		}
		EachFrame();
	}
	releaseList(LogoChars, ReleaseSubScreen);
	memFree(LCPosList);
	memFree(LCRotList);
	memFree(LCZoomList);
	delete Rnd;
}
