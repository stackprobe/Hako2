#include "all.h"

static int IIF_IsNullIta(MCell_t *cell)
{
	return cell && cell->PlayerWallFlag && !cell->ItaFlag;
}
static int IIF_IsChooseV(MCell_t *cell, int x, int y)
{
	if(IIF_IsNullIta(cell))
	{
		if(IIF_IsNullIta(GDc.Map->RefCell(x - 1, y, NULL))) return 1;
		if(IIF_IsNullIta(GDc.Map->RefCell(x + 1, y, NULL))) return 1;
	}
	return 0;
}
static int IIF_IsChooseH(MCell_t *cell, int x, int y)
{
	if(IIF_IsNullIta(cell))
	{
		if(IIF_IsNullIta(GDc.Map->RefCell(x, y - 1, NULL))) return 1;
		if(IIF_IsNullIta(GDc.Map->RefCell(x, y + 1, NULL))) return 1;
	}
	return 0;
}
static void InitItaFlag(void)
{
	SetCurtain(0);

	for(int loopcnt = 1; ; loopcnt++)
	{
		int chose = 0;
		autoList<i2D_t> *posList;
		i2D_t pos;
		int x;
		int y;
		int kind;
		int vcrlc;
		int hcrlc;
		int crnum = IMAX / loopcnt < loopcnt ? IMAX : loopcnt * loopcnt;

		posList = chooseRnd(GDc.Map, IIF_IsChooseV, GDc.Rnd, crnum);
		vcrlc = chooseRnd_LastCount;

		while(posList->GetCount())
		{
			pos = posList->UnaddElement();
			x = pos.X;
			y = pos.Y;

			if(!IIF_IsChooseV(GDc.Map->GetCell(x, y), x, y))
				continue;

			kind = GDc.Rnd->RndPct(50) ? 1 : 0;

			while(IIF_IsNullIta(GDc.Map->RefCell(x - 1, y, NULL)))
				x--;

			GDc.Map->GetCell(x, y)->ItaFlag = ITA_HL;
			GDc.Map->GetCell(x, y)->ItaKind = kind;
			x++;

			while(IIF_IsNullIta(GDc.Map->RefCell(x + 1, y, NULL)))
			{
				GDc.Map->GetCell(x, y)->ItaFlag = ITA_H;
				GDc.Map->GetCell(x, y)->ItaKind = kind;
				x++;
			}
			GDc.Map->GetCell(x, y)->ItaFlag = ITA_HR;
			GDc.Map->GetCell(x, y)->ItaKind = kind;
			chose = 1;
		}
		delete posList;
		posList = chooseRnd(GDc.Map, IIF_IsChooseH, GDc.Rnd, crnum);
		hcrlc = chooseRnd_LastCount;

		while(posList->GetCount())
		{
			pos = posList->UnaddElement();
			x = pos.X;
			y = pos.Y;

			if(!IIF_IsChooseH(GDc.Map->GetCell(x, y), x, y))
				continue;

			kind = GDc.Rnd->RndPct(50) ? 1 : 0;

			while(IIF_IsNullIta(GDc.Map->RefCell(x, y - 1, NULL)))
				y--;

			GDc.Map->GetCell(x, y)->ItaFlag = ITA_VT;
			GDc.Map->GetCell(x, y)->ItaKind = kind;
			y++;

			while(IIF_IsNullIta(GDc.Map->RefCell(x, y + 1, NULL)))
			{
				GDc.Map->GetCell(x, y)->ItaFlag = ITA_V;
				GDc.Map->GetCell(x, y)->ItaKind = kind;
				y++;
			}
			GDc.Map->GetCell(x, y)->ItaFlag = ITA_VB;
			GDc.Map->GetCell(x, y)->ItaKind = kind;
			chose = 1;
		}
		delete posList;

		if(!chose)
			break;

		if(FrameStartTime + 10 < GetCurrTime())
		{
			DrawCurtain();
			SetPrint();
			PE_Border(GetColor(30, 60, 90));
			Print_x(xcout("Initializing... lc:%d v:%d h:%d", loopcnt, vcrlc, hcrlc));
			PE_Reset();
			EachFrame();
		}
	}

	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		MCell_t *cell = GDc.Map->GetCell(x, y);

		if(cell->PlayerWallFlag && !cell->ItaFlag)
		{
			cell->ItaFlag = ITA_C;
			cell->ItaKind = GDc.Rnd->RndPct(50) ? 1 : 0;
		}
		cell->CellX = x;
		cell->CellY = y;
	}
}

static const int COLOR_ARKANOID = m_bmpColor(63, 72, 204); // 青(上段)
static const int COLOR_PATA     = m_bmpColor(255, 174, 201); // 桃色(下段)
static const int COLOR_COOKIE   = m_bmpColor(34, 177, 76); // 緑
static const int COLOR_OTHER    = m_bmpColor(128, 0, 0); // その他の敵

void LoadMap(autoTable<uint> *bmp)
{
	errorCase(!bmp);
	errorCase(!m_isRange(bmp->GetWidth(), MAP_W_MIN, MAP_W_MAX));
	errorCase(!m_isRange(bmp->GetHeight(), MAP_H_MIN, MAP_H_MAX));

	GDc.Map->Clear(bmp->GetWidth(), bmp->GetHeight());

	for(int x = 0; x < bmp->GetWidth(); x++)
	for(int y = 0; y < bmp->GetHeight(); y++)
	{
		uint color = bmp->GetCell(x, y);
		MCell_t *cell = GDc.Map->GetCell(x, y);

		if(COLOR_ARKANOID <= color && color <= COLOR_ARKANOID + (ET_ARKANOID_LAST - ET_ARKANOID_FIRST))
		{
			cell->CellType = ET_ARKANOID_FIRST + color - COLOR_ARKANOID;
			goto endCellType;
		}
		if(COLOR_PATA <= color && color <= COLOR_PATA + (ET_PATA_LAST - ET_PATA_FIRST))
		{
			cell->CellType = ET_PATA_FIRST + color - COLOR_PATA;
			goto endCellType;
		}
		if(COLOR_COOKIE <= color && color <= COLOR_COOKIE + (ET_COOKIE_LAST - ET_COOKIE_FIRST))
		{
			cell->CellType = ET_COOKIE_FIRST + color - COLOR_COOKIE;
			goto endCellType;
		}
		if(COLOR_OTHER <= color && color <= COLOR_OTHER + (ET_OTHER_LAST - ET_OTHER_FIRST))
		{
			cell->CellType = ET_OTHER_FIRST + color - COLOR_OTHER;
			goto endCellType;
		}

		switch(color)
		{
		case m_bmpColor(0, 0, 0): // 黒
			//cell->CellType = CT_EMPTY;
			break;

		case m_bmpColor(255, 242, 0): // 黄
			cell->CellType = CT_WALL;
			break;

		case m_bmpColor(237, 28, 36): // 赤
			cell->CellType = CT_DEATH;
			break;

		case m_bmpColor(255, 255, 255): // 白
			cell->CellType = CT_START;
			break;

		case m_bmpColor(0, 162, 232): // 水色(上段)
			cell->CellType = CT_GOAL;
			break;

		default:
			LOG("Invalid map bmp dot color: %d %d %06x\n", x, y, color);
			//error();
			break;
		}

endCellType:
		cell->Bright = GDc.Rnd->DRnd();
		cell->PlayerWallFlag = IsPlayerWall(cell->CellType) ? 1 : 0;
	}
	InitItaFlag();

	Ashiba_Disabled = 1;

	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		MCell_t *cell = GDc.Map->GetCell(x, y);

		if(GDc.Rnd->RndPct(5))
		{
			cell->DecoFlag = 1;
		}
		cell->KusaId = GDc.Rnd->Rnd(8);

		if(
			2 <= x &&
			IsEnemyWall(GDc.Map->GetCell(x - 2, y)) &&
			IsEnemyWall(GDc.Map->GetCell(x - 1, y)) == 0 &&
			IsEnemyWall(GDc.Map->GetCell(x, y))
			)
			GDc.Map->GetCell(x - 1, y)->EnemyAnaFlag = 1;

		if(
			2 <= y &&
			IsEnemyWall(GDc.Map->GetCell(x, y - 2)) &&
			IsEnemyWall(GDc.Map->GetCell(x, y - 1)) == 0 &&
			IsEnemyWall(GDc.Map->GetCell(x, y))
			)
			GDc.Map->GetCell(x, y - 1)->EnemyAnaFlag = 1;
	}
	Ashiba_Disabled = 0; // reset
}
void LoadMap_x(autoTable<uint> *bmp)
{
	LoadMap(bmp);
	delete bmp;
}
void SaveMap(autoTable<uint> *bmp)
{
	errorCase(!bmp);

	bmp->Resize(GDc.Map->GetWidth(), GDc.Map->GetHeight());

	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		MCell_t *cell = GDc.Map->GetCell(x, y);
		uint color = 0;

		switch(cell->CellType)
		{
		case CT_EMPTY:
			color = m_bmpColor(0, 0, 0); // 黒
			break;

		case CT_WALL:
			color = m_bmpColor(255, 242, 0); // 黄
			break;

		case CT_DEATH:
			color = m_bmpColor(237, 28, 36); // 赤
			break;

		case CT_START:
			color = m_bmpColor(255, 255, 255); // 白
			break;

		case CT_GOAL:
			color = m_bmpColor(0, 162, 232); // 水色(上段)
			break;

		case m_bmpColor(63, 72, 204): // 青(上段)
			cell->CellType = ET_ARKANOID_1;
			break;

		case m_bmpColor(255, 174, 201): // 桃色(下段)
			cell->CellType = ET_LPATA;
			break;

		case m_bmpColor(34, 177, 76): // 緑
			cell->CellType = ET_COOKIE_1;
			break;

		default:
			{
				if(m_isRange(cell->CellType, ET_ARKANOID_FIRST, ET_ARKANOID_LAST))
				{
					color = COLOR_ARKANOID + cell->CellType - ET_ARKANOID_FIRST;
					break;
				}
				if(m_isRange(cell->CellType, ET_PATA_FIRST, ET_PATA_LAST))
				{
					color = COLOR_PATA + cell->CellType - ET_PATA_FIRST;
					break;
				}
				if(m_isRange(cell->CellType, ET_COOKIE_FIRST, ET_COOKIE_LAST))
				{
					color = COLOR_COOKIE + cell->CellType - ET_COOKIE_FIRST;
					break;
				}
				if(m_isRange(cell->CellType, ET_OTHER_FIRST, ET_OTHER_LAST))
				{
					color = COLOR_OTHER + cell->CellType - ET_OTHER_FIRST;
					break;
				}
			}
			LOG("Invalid map cell type: %d %d %d\n", x, y, cell->CellType);
			error();
		}
		bmp->SetCell(x, y, color);
	}
}

void SaveMap(autoList<uchar> *bmpImage)
{
	errorCase(!bmpImage);

	autoTable<uint> *bmp = new autoTable<uint>(getUIZero, noop_ui);
	SaveMap(bmp);
	writeBmpFile_x(bmpImage, bmp);
}
void SaveMap(char *bmpFile)
{
	errorCase(!bmpFile);

	autoList<uchar> *bmpImage = new autoList<uchar>();
	SaveMap(bmpImage);
	writeFile_cx(bmpFile, bmpImage);
}
void SaveMap_x(char *bmpFile)
{
	SaveMap(bmpFile);
	memFree(bmpFile);
}

// リストア -->

void RestoreOriginalMapsIfNeed(void)
{
	LOGPOS();
	autoList<uchar> *dummyMapBmpImage = GetEtcRes()->GetHandle(ETC_FLOOR_DUMMY);

	for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
	{
		LOG("floorIndex: %d\n", floorIndex);
		char *mapFile = xcout("Map\\%04d.bmp", floorIndex + 1);
		LOG("mapFile: %s\n", mapFile);

		if(accessible(mapFile))
		{
			LOGPOS();
			autoList<uchar> *mapBmpImage = readFile(mapFile);
			LOGPOS();

			if(isSame(dummyMapBmpImage, mapBmpImage)) // ? マップファイルがダミーのまま
			{
				LOGPOS();
				autoList<uchar> *origMapBmpImage = GetEtcRes()->GetHandle(ETC_FLOOR_00 + floorIndex);
				LOGPOS();
				writeFile(mapFile, origMapBmpImage);
				origMapBmpImage = NULL; // don't release!
				LOGPOS();
			}
			LOGPOS();
		}
		memFree(mapFile);
	}
	dummyMapBmpImage = NULL; // don't release!
	LOGPOS();
}
void MakeDummyMapsIfNeed(void)
{
	LOGPOS();
	if(!accessible("Map")) // ? Map フォルダが無い -> 開発環境
	{
		LOGPOS();
		return;
	}
	LOGPOS();
	autoList<uchar> *dummyMapBmpImage = GetEtcRes()->GetHandle(ETC_FLOOR_DUMMY);

	for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
	{
		LOG("floorIndex: %d\n", floorIndex);
		char *mapFile = xcout("Map\\%04d.bmp", floorIndex + 1);
		LOG("mapFile: %s\n", mapFile);

		if(!accessible(mapFile))
		{
			LOGPOS();
			writeFile(mapFile, dummyMapBmpImage);
			LOGPOS();
		}
		memFree(mapFile);
	}
	dummyMapBmpImage = NULL; // don't release!
	LOGPOS();
}
