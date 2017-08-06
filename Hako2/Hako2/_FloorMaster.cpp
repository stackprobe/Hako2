#include "all.h"

static SubScreen_t *GetSSFloorNo(int floorNo)
{
	char *dispPtn;

	// zantei
//	if(floorNo == FLOOR_NUM)
//		dispPtn = strx("FINAL FLOOR");
//	else
		dispPtn = xcout("FLOOR %d", floorNo);

	const int MARGIN = 10;

	int x = MARGIN;
	int y = MARGIN;
	int w = MARGIN * 2 + strlen(dispPtn) * 9;
	int h = MARGIN * 2 + 16;

	SubScreen_t *ss = CreateSubScreen(w, h, 1);

	ChangeDrawScreen(ss);
	SetPrint(x, y);
	PE_Border(GetColor(30, 60, 90));
	Print(dispPtn);
	PE_Reset();
	RestoreDrawScreen();

	memFree(dispPtn);
	return ss;
}
static void DrawFloorNo(SubScreen_t *fn, int xPos, int yPos)
{
	DPE.GraphicHandleFlag = 1;
	DPE.GraphicSize = GetSubScreenSize(fn);
	DPE.MosaicOn = 1;
	DrawBegin(GetHandle(fn), xPos, yPos);
	DrawZoom(7.0);
	DrawEnd();
	DPE_Reset();
}
static void FloorInterval(int floorNo)
{
	SubScreen_t *fn = GetSSFloorNo(floorNo);
	SetCurtain(0, -1.0);
	SetCurtain();

	int xPos = 400;
	int yPos = 300 + 60;

	forscene(120)
	{
		if(sc_numer == 90)
		{
			SetCurtain(30, -1.0);
		}
//		DrawCurtain();

//		DPE_SetBright(0.5, 0.5, 0.5);
		DrawBegin(P_WALL_FLOOR_INTERVAL, 400.0, 270.0 + sc_numer * 0.5);
//		DrawZoom(1.0);
		DrawEnd();
		DPE_Reset();

		DrawFloorNo(fn, xPos, yPos);
		EachFrame();

		yPos--;
	}
	sceneLeave();

	ReleaseSubScreen(fn);
}

void FloorMasterMain(int entryFloorIndex, int playEditMapMode, int editMode) // entryFloorIndex: 0 ～ (FLOOR_NUM - 1)
{
	errorCase(!m_isRange(entryFloorIndex, 0, FLOOR_NUM - 1));

	int floorIndex = entryFloorIndex;

	for(; ; )
	{
		FloorInterval(floorIndex + 1);

		GameInit();
		GDc.FloorIndex = floorIndex;

		if(playEditMapMode)
		{
			if(accessible("Map")) // リリース環境
			{
				GDc.MapBmpFile = xcout("Map\\%04u.bmp", floorIndex + 1);
			}
			else // 開発環境
			{
				GDc.MapBmpFile = xcout("..\\..\\Map\\%04u.bmp", floorIndex + 1);
			}
		}
		else
		{
			GDc.MapBmpImage = GetEtcRes()->GetHandle(ETC_FLOOR_00 + floorIndex);
			GDc.MapBmpImageUnreleaseFlag = 1;
		}
		GDc.EditableFlag = editMode;

		GameMain();
		int clearFlag = GDc.FloorClearFlag;
		int rfrp = GDc.RestartedFromRespawnPoint;
		GameFnlz();

		if(!clearFlag)
			break;

		floorIndex++;
		m_maxim(Gnd.UnclearedFloorIndex, floorIndex);
		ExportSaveData();

		if(
			Gnd.RecordingMode &&
			!editMode &&
			!rfrp &&
			GetReplayInput()->GetCount() < REPLAY_DATA_MAX // リプレイの長さがオーバフローしていない
			)
			SaveReplayConfirm(floorIndex - 1, playEditMapMode);

		if(FLOOR_NUM <= floorIndex) // ? オールクリア
		{
			if(!editMode)
				Ending();

			break;
		}
	}
}
