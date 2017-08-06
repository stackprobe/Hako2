#include "all.h"

#define BTN_L 30
#define BTN_T 70
#define BTN_ROWCNT 9
#define BTN_COLCNT 4
#define BTN_SPAN 50

static int CellTypeBase = CT_EDITABLE_FIRST;

static int IsMouseInRect(int l, int t, int w, int h)
{
	return
		m_isRange(MouseX, GDc.Edit.Win_L + l, GDc.Edit.Win_L + l + w) &&
		m_isRange(MouseY, GDc.Edit.Win_T + t, GDc.Edit.Win_T + t + h);
}
/*
	事前にやること
		UpdateMousePos();
*/
void EditMenuEachFrame(void)
{
	GDc.Edit.MenuClicked = EMC_NONE;
	GDc.Edit.ClickedCellType = CT_EMPTY;

	if(GetMouInput(MOUBTN_L) == 0) // ? ボタン開放中
		goto endLClick;

	if(1 <= GetMouInput(MOUBTN_L)) // ? ボタン押し下げ中
	{
		if(IsMouseInRect(0, 0, EDIT_WIN_W, EDIT_WIN_H))
		{
			GDc.Edit.MenuClicked = EMC_EMPTY;
			goto endLClick;
		}
	}

	// ? ボタンを離した。

	if(IsMouseInRect(20, 20, 200, 40))
	{
		GDc.Edit.MenuClicked = EMC_MENUPOS;
		goto endLClick;
	}

	for(int index = 0; index < BTN_ROWCNT * BTN_COLCNT; index++)
	{
		int x = index % BTN_COLCNT;
		int y = index / BTN_COLCNT;

		if(IsMouseInRect(BTN_L + x * BTN_SPAN, BTN_T + y * BTN_SPAN, CELLSIZE, CELLSIZE))
		{
			int cellType = CellTypeBase + index;

			if(m_isRange(cellType, CT_EDITABLE_FIRST, CT_EDITABLE_LAST))
			{
				GDc.Edit.MenuClicked = EMC_CELLTYPE;
				GDc.Edit.ClickedCellType = cellType;
				goto endLClick;
			}
		}
	}

	if(IsMouseInRect(160, 520, 50, 50))
	{
		GDc.Edit.MenuClicked = EMC_EXIT;
		goto endLClick;
	}
	if(IsMouseInRect(0, 0, EDIT_WIN_W, EDIT_WIN_H))
	{
		GDc.Edit.MenuClicked = EMC_EMPTY;
		goto endLClick;
	}
endLClick:

	CellTypeBase -= MouseRot * BTN_COLCNT;

	m_range(
		CellTypeBase,
		CT_EDITABLE_FIRST,
		CT_EDITABLE_FIRST + ((CT_EDITABLE_LAST - CT_EDITABLE_FIRST) / BTN_COLCNT) * BTN_COLCNT
		);
}

static void DrawButton(int l, int t, int w, int h, char *caption)
{
	DPE_SetBright(0.5, 0.5, 0.5);
	DrawRect(P_WHITEBOX, GDc.Edit.Win_L + l, GDc.Edit.Win_T + t, w, h);
	DPE_Reset();

	DrawBox(
		GDc.Edit.Win_L + l,
		GDc.Edit.Win_T + t,
		GDc.Edit.Win_L + l + w,
		GDc.Edit.Win_T + t + h,
		GetColor(200, 200, 200),
		0
		);

	int cap_w = strlen(caption) * 9;
	int cap_h = 16;

//	PE.Color = GetColor(200, 200, 200);
	PE_Border(GetColor(100, 100, 100));
	SetPrint(GDc.Edit.Win_L + l + (w - cap_w) / 2, GDc.Edit.Win_T + t + (h - cap_h) / 2);
	Print(caption);
	PE_Reset();
}
/*
	EditMenuEachFrame() -> DrawEditMenuEntity()
*/
void DrawEditMenuEntity(void)
{
	DrawButton(20, 20, 200, 30, "MENU POS");

	for(int index = 0; index < BTN_ROWCNT * BTN_COLCNT; index++)
	{
		int cellType = CellTypeBase + index;

		if(m_isRange(cellType, CT_EDITABLE_FIRST, CT_EDITABLE_LAST))
		{
			int x = index % BTN_COLCNT;
			int y = index / BTN_COLCNT;

			DrawEditCell(cellType, GDc.Edit.Win_L + BTN_L + x * BTN_SPAN, GDc.Edit.Win_T + BTN_T + y * BTN_SPAN, 1);
		}
	}

	// select cell
	{
		DrawBox(
			GDc.Edit.Win_L + 20,
			GDc.Edit.Win_T + 520,
			GDc.Edit.Win_L + 150,
			GDc.Edit.Win_T + 570,
			GetColor(255, 255, 255),
			0
			);

		PE_Border(GetColor(100, 100, 100));
		SetPrint(GDc.Edit.Win_L + 33, GDc.Edit.Win_T + 538);
		Print("SELECT:");
		PE_Reset();

		DrawEditCell(GDc.Edit.SelectCellType, GDc.Edit.Win_L + 105, GDc.Edit.Win_T + 530, 1);
	}

	DrawButton(160, 520, 50, 50, "X");
}

static void DrawCell(int dr_x, int dr_y, double r, double g, double b, char *caption, int capBackColor, int capColor = GetColor(255, 255, 255))
{
	DPE_SetBright(r, g, b);
	DrawRect(P_WHITEBOX, dr_x, dr_y, CELLSIZE, CELLSIZE);
	DPE_Reset();

	int cap_w = strlen(caption) * 9;
	int cap_h = 16;

	PE_Border(capBackColor);
	SetPrint(dr_x + (CELLSIZE - cap_w) / 2, dr_y + (CELLSIZE - cap_h) / 2);
	Print(caption);
	PE_Reset();
}
static char *GetCaption(int num)
{
	const char *capList[] =
	{
		NULL, NULL, NULL, NULL, NULL,
		"6", "3", "2", "1", "4", "7", "8", "9",
		"L", "LS", "LF", "R", "RS", "RF",
		"T6", "T3", "T2", "T1", "T4", "T7", "T8", "T9",
		"H6", "H3", "H2", "H1", "H4", "H7", "H8", "H9",
		"--",
		"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9",
		"G2", "G4", "G6", "G8", "g2", "g4", "g6", "g8", "R2", "R4", "R6", "R8", "r2", "r4", "r6", "r8",
		"B2", "B4", "B6", "B8", "b2", "b4", "b6", "b8",
		"DT6", "DT3", "DT2", "DT1", "DT4", "DT7", "DT8", "DT9",
		"DH6", "DH3", "DH2", "DH1", "DH4", "DH7", "DH8", "DH9",
		"T1", "T2", "T3", "T4", "Y1", "Y2", "Y3", "Y4",
		"A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
		"EW", "PW",
		"-=",

		// @ AddEnemy
	};
	errorCase(lengthof(capList) != CT_MAX); // 2bs
	errorCase(num < 0 || CT_MAX <= num);
	char *caption = (char *)capList[num];
	errorCase(!caption);
	return caption;
}
void DrawEditCell(int cellType, int dr_x, int dr_y, int allFlag)
{
	if(!allFlag)
		goto noAll;

	if(cellType == CT_EMPTY)
	{
		DrawCell(dr_x, dr_y, 0.5, 0.5, 0.5, "EM", GetColor(50, 50, 50));
		return;
	}
	if(cellType == CT_WALL)
	{
		DrawCell(dr_x, dr_y, 0.7, 0.7, 0.0, "WL", GetColor(50, 50, 0));
		return;
	}
	if(cellType == CT_DEATH)
	{
		DrawCell(dr_x, dr_y, 1.0, 0.0, 0.0, "DT", GetColor(100, 0, 0));
		return;
	}
	if(cellType == CT_GOAL)
	{
		DrawCell(dr_x, dr_y, 1.0, 1.0, 1.0, "GL", GetColor(150, 150, 150));
		return;
	}
noAll:
	if(cellType == CT_START)
	{
		DrawCell(dr_x, dr_y, 1.0, 1.0, 1.0, "ST", GetColor(150, 150, 150));
		return;
	}
	if(m_isRange(cellType, ET_ARKANOID_FIRST, ET_ARKANOID_LAST))
	{
		DrawCell(dr_x, dr_y, 0.2, 0.3, 1.0, GetCaption(cellType), GetColor(0, 50, 100));
		return;
	}
	if(m_isRange(cellType, ET_PATA_FIRST, ET_PATA_LAST))
	{
		DrawCell(dr_x, dr_y, 1.0, 0.2, 0.3, GetCaption(cellType), GetColor(100, 50, 0));
		return;
	}
	if(m_isRange(cellType, ET_COOKIE_FIRST, ET_COOKIE_LAST))
	{
		DrawCell(dr_x, dr_y, 0.3, 1.0, 0.2, GetCaption(cellType), GetColor(50, 100, 0));
		return;
	}
	if(m_isRange(cellType, ET_STOKER_00, ET_STOKER_00 + STOKER_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 1.0, 1.0, 0.0, GetCaption(cellType), GetColor(128, 128, 0));
		return;
	}
	if(m_isRange(cellType, ET_GUNNER_00, ET_GUNNER_00 + GUNNER_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 0.3, 0.3, 0.3, GetCaption(cellType), GetColor(128, 0, 128), GetColor(255, 128, 255));
		return;
	}
	if(m_isRange(cellType, ET_BULLET_00, ET_BULLET_00 + BULLET_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 1.0, 0.5, 1.0, GetCaption(cellType), GetColor(100, 100, 100), GetColor(255, 128, 255));
		return;
	}
	if(m_isRange(cellType, ET_DAIKAITEN_00, ET_DAIKAITEN_00 + DAIKAITEN_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 0.3, 0.3, 0.3, GetCaption(cellType), GetColor(0, 100, 50), GetColor(50, 255, 128));
		return;
	}
	if(m_isRange(cellType, ET_TRAP_00, ET_TRAP_00 + TRAP_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 0.8, 1.0, 0.6, GetCaption(cellType), GetColor(50, 100, 0));
		return;
	}
	if(m_isRange(cellType, ET_ASHIBA_00, ET_ASHIBA_00 + ASHIBA_MAX - 1))
	{
		DrawCell(dr_x, dr_y, 0.7, 0.7, 0.7, GetCaption(cellType), GetColor(50, 50, 50));
		return;
	}

	// @ AddEnemy

	if(m_isRange(cellType, ET_OTHER_FIRST, ET_OTHER_LAST)) // ? その他
	{
		DrawCell(dr_x, dr_y, 0.5, 0.5, 0.5, GetCaption(cellType), GetColor(75, 75, 0));
		return;
	}
}
