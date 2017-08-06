#include "all.h"

#define DEF_BORDER_COLOR GetColor(100, 50, 100)

static Random *Rnd;

// --- Wall ----

typedef struct WallBox_st
{
	double X;
	double Y;
	double XAdd;
	double YAdd;
	double Br_R;
	double Br_G;
	double Br_B;
	double BrD_R;
	double BrD_G;
	double BrD_B;
	double Zoom;
	double DZoom;
}
WallBox_t;

static autoList<WallBox_t *> *WallBoxes;
static double Wall_RotRate;
static double Wall_WallX;
static double Wall_WallY;
static double Wall_WallDX;
static double Wall_WallDY;
static taskList *Wall_DLLogo;

static void InitWall(void)
{
	WallBoxes = new autoList<WallBox_t *>();

	for(int c = 0; c < 100; c++)
	{
		WallBox_t *wb = nb(WallBox_t);

		wb->X = Rnd->DRnd() * SCREEN_W;
		wb->Y = Rnd->DRnd() * SCREEN_H;
		wb->XAdd = Rnd->ERnd() * 2.0;
		wb->YAdd = Rnd->ERnd() * 2.0;
		wb->Br_R = Rnd->DRnd();
		wb->Br_G = Rnd->DRnd();
		wb->Br_B = Rnd->DRnd();
		wb->BrD_R = Rnd->DRnd();
		wb->BrD_G = Rnd->DRnd();
		wb->BrD_B = Rnd->DRnd();
		wb->Zoom = 10.0;
		wb->DZoom = 1.0 + Rnd->DRnd() * 3.0;

		WallBoxes->AddElement(wb);
	}
	Wall_RotRate = 0.01;
	Wall_WallX = 0.0;
	Wall_WallY = 0.0;
	Wall_WallDX = SCREEN_W - Pic_W(P_TITLE_WALL);
	Wall_WallDY = SCREEN_H - Pic_H(P_TITLE_WALL);
	Wall_DLLogo = new taskList();
}
static void FnlzWall(void)
{
	for(int index = 0; index < WallBoxes->GetCount(); index++)
	{
		memFree(WallBoxes->GetElement(index));
	}
	delete WallBoxes;
	WallBoxes = NULL;
	delete Wall_DLLogo;
}
static void DrawWall(void)
{
//	DrawCurtain();
	DrawSimple(P_TITLE_WALL, Wall_WallX, Wall_WallY);

	m_approach(Wall_WallX, Wall_WallDX, 0.9999);
	m_approach(Wall_WallY, Wall_WallDY, 0.9999);

	for(int index = 0; index < WallBoxes->GetCount(); index++)
	{
		WallBox_t *wb = WallBoxes->GetElement(index);

		wb->X += wb->XAdd;
		wb->Y += wb->YAdd;

		{
			const double MARGIN = 200.0;

			if(wb->X < 0.0 - MARGIN) wb->X += SCREEN_W + MARGIN * 2.0;
			if(wb->Y < 0.0 - MARGIN) wb->Y += SCREEN_H + MARGIN * 2.0;
			if(SCREEN_W + MARGIN < wb->X) wb->X -= SCREEN_W + MARGIN * 2.0;
			if(SCREEN_H + MARGIN < wb->Y) wb->Y -= SCREEN_H + MARGIN * 2.0;
		}

		wb->XAdd += Rnd->ERnd() * 0.01;
		wb->YAdd += Rnd->ERnd() * 0.01;

		m_range(wb->XAdd, -2.0, 2.0);
		m_range(wb->YAdd, -2.0, 2.0);

		m_approach(wb->Br_R, wb->BrD_R, 0.97);
		m_approach(wb->Br_G, wb->BrD_G, 0.97);
		m_approach(wb->Br_B, wb->BrD_B, 0.97);
		m_approach(wb->Zoom, wb->DZoom, 0.97);

		if(Rnd->DRnd() < 0.01) wb->BrD_R = Rnd->DRnd();
		if(Rnd->DRnd() < 0.01) wb->BrD_G = Rnd->DRnd();
		if(Rnd->DRnd() < 0.01) wb->BrD_B = Rnd->DRnd();
		if(Rnd->DRnd() < 0.01) wb->DZoom = 1.0 + Rnd->DRnd() * 4.0;

		DPE_SetBright(wb->Br_R, wb->Br_G, wb->Br_B);
		DrawBegin(P_WBOX32, wb->X, wb->Y);
		DrawZoom(wb->Zoom);
		DrawRotate((wb->X + wb->Y) * Wall_RotRate);
		DrawEnd();
		DPE_Reset();
	}

	m_approach(Wall_RotRate, 0.001, 0.9999);
}

// ----

static void DrawTitleLogo(void)
{
	const char *logoMap =
		"# # ### # # ### # $$$$$"
		"# # # # # # # # #  $ $ "
		"# # # # # # # # #  $ $ "
		"### ### ##  # # #  $ $ "
		"# # # # # # # # #  $ $ "
		"# # # # # # # #    $ $ "
		"# # # # # # ### # $$$$$";
	const int LM_W = 23;
	const int LM_H = 7;

	errorCase(strlen(logoMap) != LM_W * LM_H); // 2bs

	const int DR_L = 32;
	const int DR_T = 32;
	const int cellSpan = 32;

	for(int xz = -1; xz <= 1; xz++)
	for(int yz = -1; yz <= 1; yz++)
	for(int x = 0; x < LM_W; x++)
	for(int y = 0; y < LM_H; y++)
	{
		int lmIndex = x + LM_W * y;

		if(logoMap[lmIndex] != ' ')
		{
			double bright = 1.0;

			if((x + y + ProcFrame / 30) % 2)
				bright = 0.9;

			if(logoMap[lmIndex] == '$')
				bright = 1.0 - ((x + y + ProcFrame / 10) % 10) * 0.02;

			int drX = DR_L + x * cellSpan;
			int drY = DR_T + y * cellSpan;

			if(xz != 0 || xz != 0)
			{
				bright *= 0.3;
				drX += xz * 5;
				drY += yz * 5;
				DPE_SetAlpha(0.3);
			}
			else
				DPE.TL = Wall_DLLogo;

			DPE_SetBright(bright, bright, bright);
			DrawRect(P_WHITEBOX, drX, drY, cellSpan, cellSpan);
			DPE_Reset();
		}
	}

	Wall_DLLogo->ExecuteAllTask();
}

// ÉQÅ[ÉÄÉXÉ^Å[Égån -->

static void LeaveFromMainMenu(int keepBGM = 0)
{
	if(!keepBGM)
		MusicFade();

	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();
}
static void ReturnToMainMenu(void)
{
	MusicPlay(MUS_TITLE);
	SetCurtain();
	FreezeInput();
}
static void PlayGame(void)
{
	LeaveFromMainMenu();
	FloorMasterMain(0, 0, 0);
	ReturnToMainMenu();
}

#define SUBMENU_WHITE_LV -0.3
#define SUBMENU_WINDOW_A 0.5

static void FloorSelect(int playEditMapMode = 0, int editMode = 0)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = FLOOR_NUM + 1; // ç≈å„ÇÕexit
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_4))
			selectIndexAdd = -4;

		if(GetPound(INP_DIR_6))
			selectIndexAdd = 4;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		do
		{
			selectIndex += selectIndexAdd;
		}
		while(m_isRange(selectIndex, Gnd.UnclearedFloorIndex + 1, FLOOR_NUM - 1));

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			if(selectIndex < FLOOR_NUM)
			{
				LeaveFromMainMenu();
				FloorMasterMain(selectIndex, playEditMapMode, editMode);
				ReturnToMainMenu();

				goto endMenu;
			}
			break; // ? exit
		}

		DrawWall();
		CurtainEachFrame();

		{
			const int WIN_L = 280;
			const int WIN_T = 188;
			const int WIN_W = 240;
			const int WIN_H = 224;

			DPE_SetAlpha(SUBMENU_WINDOW_A);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			const int COLOR = GetColor(255, 255, 255);
			const int BORDER_COLOR = DEF_BORDER_COLOR;

			SetPrint(345, 230, 40);
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			int c = 0;

			for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
			{
				if(floorIndex == Gnd.UnclearedFloorIndex + 1)
				{
					PE.Color = GetColor(100, 100, 100);
					PE_Border(GetColor(50, 50, 50));
				}
				Print_x(xcout("[%c] FLOOR %d", selectIndex == c++ ? '>' : ' ', floorIndex + 1));
				PrintRet();
			}
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);

			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	SetCurtain();
	FreezeInput();
}
static void EditModeMenu(void)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = 4;
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		do
		{
			selectIndex += selectIndexAdd;
		}
		while(m_isRange(selectIndex, Gnd.UnclearedFloorIndex + 1, FLOOR_NUM - 1));

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				RestoreOriginalMapsIfNeed();
				FloorSelect(1, 1);
				goto endMenu;

			case 1:
				{
					LeaveFromMainMenu();
					FloorMasterMain(0, 1, 0);
					ReturnToMainMenu();
				}
				goto endMenu;

			case 2:
				FloorSelect(1);
				goto endMenu;

			case 3:
				goto endMenu;

			default:
				error();
			}
		}

		DrawWall();
		CurtainEachFrame();

		{
			const int WIN_L = 200;
			const int WIN_T = 172;
			const int WIN_W = SCREEN_W - WIN_L * 2;
			const int WIN_H = SCREEN_H - WIN_T * 2;

			DPE_SetAlpha(SUBMENU_WINDOW_A);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			SetPrint(240, 230, 40);
			PE_Border(DEF_BORDER_COLOR);
			int c = 0;
			Print_x(xcout("[%c] EDIT FLOORS", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] PLAY EDITED FLOORS", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] PLAY EDITED FLOORS (FLOOR SELECT)", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	SetCurtain();
	FreezeInput();
}

// ê›íË -->

static void DrawSettingWall(void)
{
	DrawWall();

	DPE_SetAlpha(SUBMENU_WINDOW_A);
	DPE_SetBright(0.0, 0.0, 0.0);
	DrawRect(P_WHITEBOX, 0, 0, SCREEN_W / 2, SCREEN_H);
	DPE_Reset();

	CurtainEachFrame();
}
static int BC_GetPressPadBtnId(void)
{
	int padId = Gnd.PrimaryPadId;

	/*
		todo: ÉpÉbÉhÇíTÇ∑ÇÊÇ§Ç…Ç∑ÇÈÅB
	*/
	if(padId == -1) // ? ñ¢íËã`
		padId = 0; // ìKìñÇ»ÉfÉtÉH

	for(int btnId = 0; btnId < PAD_BUTTON_MAX; btnId++)
	{
		if(GetPadInput(padId, btnId) == 1)
		{
			return btnId;
		}
	}
	return -1; // not found
}
static void ButtonConfig(void)
{
	int *pPadBtns[] =
	{
		&Gnd.PadBtnId.Dir_8,
		&Gnd.PadBtnId.Dir_2,
		&Gnd.PadBtnId.Dir_4,
		&Gnd.PadBtnId.Dir_6,
		&Gnd.PadBtnId.A,
		&Gnd.PadBtnId.B,
		&Gnd.PadBtnId.C,
		&Gnd.PadBtnId.L,
		&Gnd.PadBtnId.R,
		&Gnd.PadBtnId.Pause,
	};
	char *btnNames[] =
	{
		"UP",
		"DOWN",
		"LEFT",
		"RIGHT",
		"JUMP / DECIDE",
		"SLOW / CANCEL",
		"CREATE RESPAWN POINT",
		"SCOPE",
		"QUICK RESPAWN",
		"PAUSE",
	};
	errorCase(lengthof(pPadBtns) != lengthof(btnNames));
	int btnMax = lengthof(pPadBtns);
	int btnIndex = 0;
	void *backup = memClone(&Gnd.PadBtnId, sizeof(Gnd.PadBtnId));

	/*
		ïségópÉ{É^ÉìÇÃÉNÉäÉA
	*/
	Gnd.PadBtnId.D = -1;
	Gnd.PadBtnId.E = -1;
	Gnd.PadBtnId.F = -1;
	Gnd.PadBtnId.Start = -1;

	FreezeInput();

	while(btnIndex < btnMax)
	{
		if(
			GetKeyInput(KEY_INPUT_Z) == 1 ||
			GetKeyInput(KEY_INPUT_X) == 1
			)
		{
			memcpy(&Gnd.PadBtnId, backup, sizeof(Gnd.PadBtnId)); // ïúå≥
			break;
		}

		{
			int btnId = BC_GetPressPadBtnId();

			if(btnId != -1)
			{
				for(int i = 0; i < btnIndex; i++)
					if(*pPadBtns[i] == btnId)
						goto endDecideBtnId;

				*pPadBtns[btnIndex] = btnId;
				btnIndex++;
			}
endDecideBtnId:;
		}

		if(GetKeyInput(KEY_INPUT_SPACE) == 1) // äÑÇËìñÇƒÉiÉV
		{
			*pPadBtns[btnIndex] = -1;
			btnIndex++;
		}

		DrawSettingWall();

		PE_Border(DEF_BORDER_COLOR);
		SetPrint(40, 40, 36);
		Print("**** PAD CONFIG ****");
		PrintRet();

		for(int index = 0; index < btnMax; index++)
		{
			Print_x(xcout("[%c] %s BUTTON", index == btnIndex ? '>' : ' ', btnNames[index]));

			if(index < btnIndex)
			{
				int btnId = *pPadBtns[index];

				if(btnId == -1)
					Print(" -> NOT ASSIGNED");
				else
					Print_x(xcout(" -> %d", *pPadBtns[index]));
			}
			PrintRet();
		}
		PrintRet();
		Print("# PRESS A BUTTON INDICATING CURSOR");
		PrintRet();
		Print("# PRESS [X] OR [Z] KEY TO CANCEL");
		PrintRet();
		Print("# PRESS SPACE KEY TO SKIP ASSIGN");
		PE_Reset();

		EachFrame();
	}
	FreezeInput();

	memFree(backup);
}
static void WindowSizeConfig(void)
{
	FreezeInput();

	const int selectMax = 12;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0: SetScreenSize(800, 600); break;
			case 1: SetScreenSize(900, 675); break;
			case 2: SetScreenSize(1000, 750); break;
			case 3: SetScreenSize(1100, 825); break;
			case 4: SetScreenSize(1200, 900); break;
			case 5: SetScreenSize(1300, 975); break;
			case 6: SetScreenSize(1400, 1050); break;
			case 7: SetScreenSize(1500, 1125); break;
			case 8: SetScreenSize(1600, 1200); break;

			case 9:
				SetScreenSize(
					GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN)
					);
				break;

			case 10:
				{
					int w = GetSystemMetrics(SM_CXSCREEN);
					int h = GetSystemMetrics(SM_CYSCREEN);

					if(w * SCREEN_H < h * SCREEN_W) // ècí∑ÉÇÉjÉ^ -> â°ïùÇ…çáÇÌÇπÇÈ
					{
						h = m_d2i(((double)w * SCREEN_H) / SCREEN_W);
					}
					else // â°í∑ÉÇÉjÉ^ -> ècïùÇ…çáÇÌÇπÇÈ
					{
						w = m_d2i(((double)h * SCREEN_W) / SCREEN_H);
					}
					SetScreenSize(w, h);
				}
				break;

			case 11:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingWall();

		SetPrint(40, 40, 40);
		PE_Border(DEF_BORDER_COLOR);
		Print("**** RESIZE WINDOW ****");
		PrintRet();
		int c = 0;
		Print_x(xcout("[%c] 800 x 600 (DEFAULT)", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 900 x 675", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1000 x 750", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1100 x 825", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1200 x 900", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1300 x 975", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1400 x 1050", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1500 x 1125", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1600 x 1200", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] ADJUST TO SCREEN", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] ADJUST TO SCREEN KEEP ASPECT RATIO", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
		PE_Reset();

		EachFrame();
	}
endMenu:
	FreezeInput();
}
static void TuneVolume(int seFlag)
{
	double dvol = seFlag ? Gnd.SEVolume : Gnd.MusicVolume;
	int volume = m_d2i(dvol * 100);
	int volorig = volume;
	int lastvol = volume;

	FreezeInput();

	for(; ; )
	{
		if(GetPound(INP_DIR_4))
			volume--;

		if(GetPound(INP_DIR_6))
			volume++;

		if(GetPound(INP_DIR_2))
			volume -= 10;

		if(GetPound(INP_DIR_8))
			volume += 10;

		if(GetPound(INP_B))
		{
			if(volume == volorig)
				break;

			volume = volorig;
		}
		m_range(volume, 0, 100);

		if(GetPound(INP_A))
			break;

		if(lastvol != volume)
		{
			if(seFlag)
			{
				Gnd.SEVolume = volume / 100.0;
				UpdateSEVolume();
			}
			else
			{
				Gnd.MusicVolume = volume / 100.0;
				UpdateMusicVolume();
			}
			lastvol = volume;
		}
		if(seFlag && ProcFrame % 60 == 0)
		{
			SEPlay(Rnd->DRnd() < 0.5 ? SE_PAUSE_IN : SE_PAUSE_OUT);
		}
		DrawSettingWall();

		PE_Border(DEF_BORDER_COLOR);
		SetPrint(40, 40, 40);
		Print_x(xcout("**** %s VOLUME ****", seFlag ? "SE" : "BGM"));
		PrintRet();
		Print_x(xcout("[ %3d ]", volume));
		PrintRet();
		PrintRet();
		Print("# [LEFT], [DOWN] KEY TO VOLUME DOWN");
		PrintRet();
		Print("# [RIGHT], [UP] KEY TO VOLUME UP");
		PE_Reset();

		EachFrame();
	}
	FreezeInput();
}
static int SettingConfirm(char *comment1, char *comment2)
{
	int retval = 0;

	FreezeInput();

	const int selectMax = 2;
	int selectIndex = 1;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				retval = 1;
				goto endMenu;

			case 1:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingWall();

		SetPrint(40, 40, 40);
		PE_Border(DEF_BORDER_COLOR);
		Print("**** ARE YOU SURE ? ****");
		PrintRet();
		int c = 0;
		Print_x(xcout("[%c] YES", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] NO", selectIndex == c++ ? '>' : ' '));

		if(comment1)
		{
			PrintRet();
			PrintRet();
			Print_x(xcout("# %s", comment1));

			if(comment2)
			{
				PrintRet();
				Print_x(xcout("# %s", comment2));
			}
		}
		PE_Reset();

		EachFrame();
	}
endMenu:
	FreezeInput();

	return retval;
}
static void UnlockAllFloor(void)
{
	Gnd.UnclearedFloorIndex = FLOOR_NUM;
	RestoreOriginalMapsIfNeed();
}
static void RespawnCoinStockMenu(void)
{
	FreezeInput();

	const int selectMax = RESPAWN_MAX_MAX + 2;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			if(selectIndex == selectMax - 1)
				break;

			Gnd.RespawnMax = selectIndex;
		}

		DrawSettingWall();

		SetPrint(40, 40, 36);
		PE_Border(DEF_BORDER_COLOR);
		Print("**** RESPAWN COIN STOCK ****");
		PrintRet();
		int c = 0;

		while(c <= RESPAWN_MAX_MAX)
		{
			Print_x(xcout(
				"[%c] %c %d COINS%s"
				,selectIndex == c ? '>' : ' '
				,c == Gnd.RespawnMax ? '*' : ' '
				,c
				,c == RESPAWN_DEF ? "Å@(DEFAULT)" : ""
				));
			PrintRet();
			c++;
		}
		Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
		PrintRet();

		PE_Reset();

		EachFrame();
	}
//endMenu:
	FreezeInput();
}
static void Setting(void)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = 8;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				ButtonConfig();
				break;

			case 1:
				WindowSizeConfig();
				break;

			case 2:
				TuneVolume(0);
				break;

			case 3:
				TuneVolume(1);
				break;

				// îpé~
				/*
			case 4:
				Gnd.GraphicalMode = (Gnd.GraphicalMode + 1) % GM_MAX;
				break;
				*/

			case 4:
				Gnd.RecordingMode = Gnd.RecordingMode ? 0 : 1;
				break;
				
			case 5:
				if(SettingConfirm(NULL, NULL)) {
//				if(SettingConfirm("ALL CLEAR SHITAKOTO-NI-SHITE", "EDIT MODE WO-KAIHOU-SURU-YO!")) {
					UnlockAllFloor();
					SEPlay(SE_FLOORCLEAR);
				}
				break;

			case 6:
				RespawnCoinStockMenu();
				break;

			case 7:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingWall();

		{
			SetPrint(40, 40, 48);
			PE_Border(DEF_BORDER_COLOR);
			Print("**** SETTING ****");
			PrintRet();
			int c = 0;
			Print_x(xcout("[%c] PAD CONFIG", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RESIZE WINDOW", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] BGM VOLUME", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] SE VOLUME", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			// îpé~
			/*
			Print_x(xcout("[%c] GRAPHIC MODE : %s", selectIndex == c++ ? '>' : ' ', GetGraphicalModeCaption(Gnd.GraphicalMode)));
			PrintRet();
			*/
			Print_x(xcout("[%c] SAVE REPLAY DATA : %s", selectIndex == c++ ? '>' : ' ', Gnd.RecordingMode ? "ON" : "OFF"));
			PrintRet();
			Print_x(xcout("[%c] UNLOCK ALL FLOORS", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RESPAWN COIN STOCK", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	ExportSaveData();

	SetCurtain();
	FreezeInput();
}

// ----

void MainMenu(void)
{
	Rnd = new Random((int)time(NULL));
	InitWall();
	MakeDummyMapsIfNeed();

	// <-- init

	if(UnlockAllFloorEnabled)
		UnlockAllFloor();

	MusicPlay(MUS_TITLE);
	SetCurtain();
	FreezeInput();

	const int selectMax = 5;
//	const int editModeIndex = 4; // îpé~ @ 2016.10.24
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		selectIndex += selectIndexAdd;

		// îpé~ @ 2016.10.24
		/*
		if(selectIndex == editModeIndex && Gnd.UnclearedFloorIndex < FLOOR_NUM) // ? edit mode && not editable
			selectIndex += selectIndexAdd;
			*/

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				PlayGame();
				break;

			case 1:
				FloorSelect();
				break;

			case 2:
				LeaveFromMainMenu(1);
				ReplayMainMenu();
				ReturnToMainMenu();
				break;

			case 3:
				Setting();
				break;

			// îpé~ @ 2016.10.24
			/*
			case 4:
				EditModeMenu();
				break;
				*/

			case 4:
				goto endMenu;

			default:
				error();
			}
		}

		// âBÇµÉÇÅ[Éh >

#if LOG_ENABLED
		if(GetKeyPound(KEY_INPUT_U))
		{
			UnlockAllFloor();
			SEPlay(SE_FLOORCLEAR);
		}
#endif

#if LOG_ENABLED
		if(GetKeyPound(KEY_INPUT_E))
#else
		if(GetKeyPound(KEY_INPUT_E) && EditModeEnabled)
#endif
		{
			UnlockAllFloor();
			SEPlay(SE_FLOORCLEAR);

			EditModeMenu();
		}

		// < âBÇµÉÇÅ[Éh

		DrawWall();
		DrawTitleLogo();

		PE_Border(GetColor(70, 20, 70));
		SetPrint(278, 584);
		Print(GetDatString(DATSTR_COPYRIGHT));
		PE_Reset();

		{
			const int WIN_L = 280;
			const int WIN_T = 320;
			const int WIN_W = 240;
			const int WIN_H = 240;

			DPE_SetAlpha(0.65);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			const int COLOR = GetColor(255, 255, 255);
			const int BORDER_COLOR = DEF_BORDER_COLOR;

			SetPrint(330, 352, 40);
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			int c = 0;
			Print_x(xcout("[%c] GAME START", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] FLOOR SELECT", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] REPLAY", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] SETTING", selectIndex == c++ ? '>' : ' '));
			PrintRet();

			// îpé~ @ 2016.10.24
			/*
			if(Gnd.UnclearedFloorIndex < FLOOR_NUM) // ? not editable
			{
				PE.Color = GetColor(100, 100, 100);
				PE_Border(GetColor(50, 50, 50));
			}
			Print_x(xcout("[%c] EDIT MODE", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			*/

			Print_x(xcout("[%c] EXIT", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
//	FreezeInput();
	MusicFade();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	// fnlz -->

	FnlzWall();
	delete Rnd;
}
