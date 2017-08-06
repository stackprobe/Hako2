#include "all.h"

static int GetRndSeed(autoTable<uint> *src)
{
	uint ret = 0xffffffff;

	for(int x = 0; x < src->GetWidth(); x++)
	for(int y = 0; y < src->GetHeight(); y++)
	{
		ret ^= src->GetCell(x, y);
		ret = (uint)((uint64)ret * ret >> 16);
//		LOG("*%08x\n", ret);
	}
	return (int)ret;
}

static void ChangeInfoCaption_x(char *caption, int frame = -1)
{
	if(frame == -1)
		frame = 180; // def

	errorCase(!caption);
	errorCase(!m_isRange(frame, 1, 3600)); // 1 ～ 1 min

	memFree(GDc.InfoCaption);
	GDc.InfoCaption = caption;
	GDc.InfoCaptionFrame = frame;
}
static void ChangeInfoCaption(char *caption, int frame = -1)
{
	errorCase(!caption);
	ChangeInfoCaption_x(strx(caption), frame);
}

/*
	リプレイに対応した入力の取得
*/
static int GetGameInputPound(int inpId, int mode)
{
	GameInput_t *status;
	
	switch(mode)
	{
	case 'I': status = &GDc.CurrInput; break;
	case 'L': status = &GDc.LastInput; break;
	case 'P': status = &GDc.CurrPound; break;

	default:
		error();
	}

	switch(inpId)
	{
	case INP_DIR_2: return status->Dir2; break;
	case INP_DIR_4: return status->Dir4; break;
	case INP_DIR_6: return status->Dir6; break;
	case INP_DIR_8: return status->Dir8; break;
	case INP_A: return status->A; break;
	case INP_B: return status->B; break;
	case INP_L: return status->L; break;
	case INP_R: return status->R; break;

	default:
		error();
	}
}
static int GetGameInput(int inpId)
{
	return GetGameInputPound(inpId, 'I');
}
static int GetLastGameInput(int inpId)
{
	return GetGameInputPound(inpId, 'L');
}
static int GetGamePound(int inpId)
{
	return GetGameInputPound(inpId, 'P');
}

static void PlayerWallProc(void)
{
	const fPInt_t fldRng = (CELLSIZE / 2) * IDENOM;

	fPInt_t fldX = GDc.Player.X;
	fPInt_t fldY = GDc.Player.Y;

	// 画面外対策
	m_maxim(fldX, 0);
	m_maxim(fldY, 0);

	/*
		今いる2x2の左上
	*/
	int mapX = (fldX + fldRng) / (CELLSIZE * IDENOM) - 1;
	int mapY = (fldY + fldRng) / (CELLSIZE * IDENOM) - 1;

	fldX -= (mapX + 1) * (CELLSIZE * IDENOM);
	fldY -= (mapY + 1) * (CELLSIZE * IDENOM);

	errorCase(mapX < -IMAX || IMAX < mapX); // 2bs
	errorCase(mapY < -IMAX || IMAX < mapY); // 2bs
	/*
		fldX, fldY: -fldRng ～ (fldRng - 1)
	*/
	errorCase(fldX < -fldRng || fldRng <= fldX);
	errorCase(fldY < -fldRng || fldRng <= fldY);

	int fldXOrig = fldX;
	int fldYOrig = fldY;

	MCell_t *cell1 = GDc.Map->RefCell(mapX + 0, mapY + 1, NULL);
	MCell_t *cell3 = GDc.Map->RefCell(mapX + 1, mapY + 1, NULL);
	MCell_t *cell7 = GDc.Map->RefCell(mapX + 0, mapY + 0, NULL);
	MCell_t *cell9 = GDc.Map->RefCell(mapX + 1, mapY + 0, NULL);

	int onGround = 0;
	int touchRoofOn = 0;
	Touch_t touchSideOn = TOUCH_NONE;
	int onDeath = 0;
	int onGoal = 0;

	// ? out of field
	if(
		!cell1 ||
		!cell3 ||
		!cell7 ||
		!cell9
		)
	{
		onDeath = 1;
		goto endFunc;
	}

	// CT_DEATH より優先する
	if(
		(cell1->CellType == CT_GOAL || cell7->CellType == CT_GOAL) ||
		(cell3->CellType == CT_GOAL || cell9->CellType == CT_GOAL) && -fldRng < fldX
		)
	{
		onGoal = 1;
		goto endFunc;
	}

	fPInt_t fldXAdd = GDc.Player.XAdd;
	fPInt_t fldYAdd = GDc.Player.YAdd;
	int lrReversed = 0;
restartSwitch:
	switch(
		(IsPlayerWall(cell1->CellType) ? 1 : 0) |
		(IsPlayerWall(cell3->CellType) ? 2 : 0) |
		(IsPlayerWall(cell7->CellType) ? 4 : 0) |
		(IsPlayerWall(cell9->CellType) ? 8 : 0)
		)
	{
	case 0: // []
		break;

	case 3: // [13]
		fldY = -fldRng;
		onGround = 1;
		break;

	case 12: // [79]
		fldY = fldRng;
		touchRoofOn = 1;
		break;

	case 15: // [1379]
//		onDeath = 1; // 壁の中 -> 死亡 -- ET_ASHIBA_00 の追加により、殺さない。
		fldY = -fldRng * 2; // 上に押し出す。
		onGround = 1;
		break;

	case 8: // [9]
	case 1: // [1]
	case 10: // [39]
	case 14: // [379]
	case 7: // [137]
	case 9: // [19]
		m_swap(cell1, cell3, MCell_t *);
		m_swap(cell7, cell9, MCell_t *);
		fldX *= -1;
		fldXAdd *= -1;
		lrReversed = 1;
		goto restartSwitch;

	case 4: // [7]
		{
			int forceThrough = 0;
			int forceUnthrough = 0;

			if(
				GetGameInput(INP_A) ||
				GetGameInput(INP_DIR_8)
				)
			{
				int zure = fldRng / 2 - fldX; // 0 ～ fldRng

				if(zure < 7500)
				{
					forceThrough = 1;
				}
			}
			if(fldX < fldRng && cell9->CellType == CT_DEATH) // 壁と溶岩の天井 -> ただの天井と同じ
			{
				forceUnthrough = 1;
			}

			if((fldX < fldY && !forceThrough) || forceUnthrough) // forceUnthrough 優先
			{
				// 上に通さない
				fldY = fldRng;
				touchRoofOn = 1;
			}
			else
			{
				// 上に通す
				fldX = fldRng;
				touchSideOn = TOUCH_L;
			}
		}
		break;

	case 2: // [3]
		{
			int forceThrough = 0;
			int forceUnthrough = 0;
			int camSlideOn = GetGameInput(INP_L);

			if(!camSlideOn)
			{
				if(GetGameInput(INP_DIR_2))
				{
					if(cell1->CellType != CT_DEATH) // 溶岩の中には落とさない。
					{
						int zure = fldX + fldRng / 2; // 0 ～ fldRng

						if(zure < 5000)
						{
							forceThrough = 1;
						}
						else if(!GetGameInput(INP_DIR_4) && !GetGameInput(INP_DIR_6)) // ? 左右を押していない。
						{
							fldX -= 500;
						}
					}
				}
				else // ? 下を押していない。
				{
					if(-fldRng < fldX) // ? めり込んでいる。
					{
						if(fldXAdd < 0) // ? 陸側から来た。
						{
							forceUnthrough = 1;
//							ChangeInfoCaption("FUT-RKT", 30); // test
						}
						else if(fldY - fldYAdd < fldX - fldXAdd) // ? 斜め45°より上から来た。
						{
							forceUnthrough = 1;
//							ChangeInfoCaption("FUT-45", 30); // test
						}
					}
				}
			}

			if(
				GetGameInput(INP_DIR_4) ||
				GetGameInput(INP_DIR_6)
				)
			{
				int zure = fldY + fldRng / 2; // 0 ～ fldRng

				if(zure < 500)
				{
					forceUnthrough = 1;
				}
			}

			if((fldX < fldY && !forceUnthrough) || forceThrough)
//			if((fldX < fldY || forceThrough) && !forceUnthrough) // old -- forceUnthrough 優先
			{
				// 下に通す
				fldX = -fldRng;
				touchSideOn = TOUCH_R;
			}
			else
			{
				// 下に通さない
				fldY = -fldRng;
				onGround = 1;
			}
		}
		break;

	case 5: // [17]
		fldX = fldRng;
		touchSideOn = TOUCH_L;
		break;

	case 13: // [179]
		fldX = fldRng;
		fldY = fldRng;
		touchRoofOn = 1;
		touchSideOn = TOUCH_L;
		break;

	case 11: // [139]
		fldX = -fldRng;
		fldY = -fldRng;
		onGround = 1;
		touchSideOn = TOUCH_R;
		break;

	case 6: // [37]
		{
			if(fldX < fldY)
			{
				fldX = -fldRng;
				fldY = fldRng;
				touchRoofOn = 1;
				touchSideOn = TOUCH_R;
			}
			else
			{
				fldX = fldRng;
				fldY = -fldRng;
				onGround = 1;
				touchSideOn = TOUCH_L;
			}
		}
		break;

	default:
		error();
	}
	if(lrReversed)
	{
		m_swap(cell1, cell3, MCell_t *);
		m_swap(cell7, cell9, MCell_t *);
		fldX = -fldX;

		switch(touchSideOn)
		{
		case TOUCH_L: touchSideOn = TOUCH_R; break;
		case TOUCH_R: touchSideOn = TOUCH_L; break;
		}
	}
	if(fldXOrig == -fldRng)
	{
		touchSideOn = TOUCH_NONE;
	}

	if(
		cell7->CellType == CT_DEATH &&  fldX < fldRng &&  fldY < fldRng ||
		cell9->CellType == CT_DEATH && -fldRng < fldX &&  fldY < fldRng ||
		cell1->CellType == CT_DEATH &&  fldX < fldRng && -fldRng < fldY ||
		cell3->CellType == CT_DEATH && -fldRng < fldX && -fldRng < fldY
		)
	{
		onDeath = 1;
	}

endFunc:
	GDc.Player.OnGround = onGround;
	GDc.Player.TouchRoofOn = touchRoofOn;
	GDc.Player.TouchSideOn = touchSideOn;
	GDc.Player.OnDeath = onDeath;
	GDc.Player.OnGoal = onGoal;

	// フィードバック
	GDc.Player.X = (mapX + 1) * (CELLSIZE * IDENOM) + fldX;
	GDc.Player.Y = (mapY + 1) * (CELLSIZE * IDENOM) + fldY;
}

// Draw >

static void DrawWall(void)
{
	FloorWallMasterMain();

	if(!ActFrame) // フロア_初回のみ
		PostInitKabe();

	// Post_FloorWallMasterMain()
	{
		if(GDc.KabeDrawFunc != GDc.FrontKabeDrawFunc)
		{
			GDc.BackKabeDrawFunc = GDc.FrontKabeDrawFunc;
			GDc.FrontKabeDrawFunc = GDc.KabeDrawFunc;
			GDc.FrontKabe_A = 0.0;
		}
		if(0.98 < GDc.FrontKabe_A)
		{
			GDc.BackKabeDrawFunc = NULL;
			GDc.FrontKabe_A = 1.0;
		}
		else
			m_approach(GDc.FrontKabe_A, 1.0, 0.97);

		const double KABE_BR_APPRRATE = 0.5;

		m_approach(GDc.KabeBrList_A[0], GDc.KabeBr_A, KABE_BR_APPRRATE);
		m_approach(GDc.KabeBrList_R[0], GDc.KabeBr_R, KABE_BR_APPRRATE);
		m_approach(GDc.KabeBrList_G[0], GDc.KabeBr_G, KABE_BR_APPRRATE);
		m_approach(GDc.KabeBrList_B[0], GDc.KabeBr_B, KABE_BR_APPRRATE);

		for(int index = 1; index < KABE_BR_MAX; index++)
		{
			int i = index - 1;
			int j = index;

			m_approach(GDc.KabeBrList_A[j], GDc.KabeBrList_A[i], KABE_BR_APPRRATE);
			m_approach(GDc.KabeBrList_R[j], GDc.KabeBrList_R[i], KABE_BR_APPRRATE);
			m_approach(GDc.KabeBrList_G[j], GDc.KabeBrList_G[i], KABE_BR_APPRRATE);
			m_approach(GDc.KabeBrList_B[j], GDc.KabeBrList_B[i], KABE_BR_APPRRATE);
		}
	}

	int bCellSize;

	switch(Gnd.GraphicalMode)
	{
	default:
		bCellSize = 24;
		break;

	case GM_DEFAULT:
		{
			DrawCurtain();

			if(GDc.BackKabeDrawFunc)
			{
				GDc.BackKabeDrawFunc(1.0);
				GDc.FrontKabeDrawFunc(GDc.FrontKabe_A);
			}
			else
				GDc.FrontKabeDrawFunc(1.0);
		}
		return;

	case GM_03:
	case GM_04:
	case GM_04_SOFT:
		bCellSize = 32;
		break;

	case GM_METEOR:
	case GM_METEOR_SOFT:
		return; // 何も描画しない。

	case GM_GHOST:
		DrawCurtain(-0.1);
		return;
	}

	const int w = SCREEN_W / bCellSize + 2;
	const int h = SCREEN_H / bCellSize + 2;

	int bCamX = GDc.ICameraX / 3;
	int bCamY = GDc.ICameraY / 3;
	int cellXOrig = bCamX / bCellSize;
	int cellYOrig = bCamY / bCellSize;
	int drawXOrig = cellXOrig * bCellSize - bCamX;
	int drawYOrig = cellYOrig * bCellSize - bCamY;

	for(int x = 0; x < w; x++)
	for(int y = 0; y < h; y++)
	{
		int cellX = cellXOrig + x;
		int cellY = cellYOrig + y;
		int dr_x = drawXOrig + x * bCellSize;
		int dr_y = drawYOrig + y * bCellSize;

		// 重い
		/*
		if(Gnd.GraphicalMode == GM_03)
		{
			const int KOMA_MAX = 3;
			const int FRM_MAX = 45;
			int koma = ProcFrame / FRM_MAX;
			int nKoma = koma + 1;
			int frm = ProcFrame % FRM_MAX;

			koma %= KOMA_MAX;
			nKoma %= KOMA_MAX;

			for(int sx = 0; sx < 2; sx++)
			for(int sy = 0; sy < 2; sy++)
			{
				int si = sx + sy * 32;

				DrawSimple(D_MAPCHIP_A1_MINI_00 + 673 + si + koma * 4 | DTP, dr_x + sx * 16, dr_y + sy * 16);
				DPE_SetAlpha((double)frm / FRM_MAX);
				DrawSimple(D_MAPCHIP_A1_MINI_00 + 673 + si + nKoma * 4 | DTP, dr_x + sx * 16, dr_y + sy * 16);
				DPE_Reset();
			}
		}
		else*/ if(Gnd.GraphicalMode == GM_04 || Gnd.GraphicalMode == GM_04_SOFT)
		{
			int sx = cellX & 1;
			int sy = cellY & 1;

			if(Gnd.GraphicalMode == GM_04_SOFT)
			{
				DPE_SetBright(0.3, 0.5, 0.8);
				DrawSimple(D_MAPCHIP_B_00 + 102 + sx + sy * 16 | DTP, dr_x, dr_y);
				DPE_Reset();
			}
			else
				DrawSimple(D_MAPCHIP_B_00 + 102 + sx + sy * 16 | DTP, dr_x, dr_y);
		}
		else
		{
			double b = (cellX + cellY) & 1 ? 0.11 : 0.13;
			double bR = 0.0;
			double bG = 1.0;
			double bB = 0.0;

			switch(Gnd.GraphicalMode)
			{
			case GM_DEFAULT:
				error(); // inaccessible

			case GM_LEGACY:
				break;

			case GM_01:
				bR = 0.5;
				bG = 0.7;
				bB = 1.0;
				break;

			case GM_02:
				bR = 1.0;
				bG = 0.7;
				bB = 0.5;
				break;

			case GM_03:
				bR = 0.0;
				bG = 0.8;
				bB = 0.8;
				break;

			case GM_04:
			case GM_04_SOFT:
			case GM_METEOR:
			case GM_METEOR_SOFT:
			case GM_GHOST:
				error(); // inaccessible

			case GM_EARTH:
				bR = 0.75;
				bG = 0.8;
				bB = 0.85;
				break;

			default:
				error();
			}

			DPE_SetBright(b * bR, b * bG, b * bB);
			DrawRect(P_WHITEBOX, dr_x, dr_y, bCellSize, bCellSize);
			DPE_Reset();
		}
	}
	if(Gnd.GraphicalMode == GM_04 || Gnd.GraphicalMode == GM_04_SOFT)
	{
		int cloud_w = 640;
		int cloud_h = 480;

#if 0 // ときどきガクってズレる...追求面倒なので諦めようorz
		if(Gnd.GraphicalMode == GM_04_SOFT)
		{
			cloud_w = 800;
			cloud_h = 600;
		}
#endif

		int dr_x = cellXOrig % (cloud_w / bCellSize);
		int dr_y = cellYOrig % (cloud_h / bCellSize);

		dr_x *= -bCellSize;
		dr_y *= -bCellSize;

		dr_x += drawXOrig;
		dr_y += drawYOrig;

		dr_x -= (ProcFrame % cloud_w);
		dr_y += (ProcFrame % cloud_h);

		dr_x %= cloud_w;
		dr_y %= cloud_h;

		if(dr_x < 0) dr_x += cloud_w;
		if(dr_y < 0) dr_y += cloud_h;

		for(int x = -1; x <= 1; x++)
		for(int y = -1; y <= 1; y++)
		{
			if(Gnd.GraphicalMode == GM_04_SOFT)
			{
				DPE_SetAlpha(0.3);
				DrawSimple(P_CLOUD, dr_x + x * cloud_w, dr_y + y * cloud_h);
//				DrawRect(P_CLOUD, dr_x + x * cloud_w, dr_y + y * cloud_h, cloud_w, cloud_h);
				DPE_Reset();
			}
			else
				DrawSimple(P_CLOUD, dr_x + x * cloud_w, dr_y + y * cloud_h);
		}
	}
}

static int DGC_IsWall(MCell_t *cell, int rx, int ry)
{
	MCell_t *rCell = GDc.Map->RefCell(
		(int)cell->CellX + rx,
		(int)cell->CellY + ry,
		NULL
		);
	return !rCell || IsPlayerWall(rCell->CellType);
}
static int DGC_IsKusa(MCell_t *cell)
{
	if(cell->CellType != CT_WALL)
		return 0;

	for(int rx = -1; rx <= 1; rx++)
	for(int ry = -1; ry <= 1; ry++)
	{
		if((rx || ry) && !DGC_IsWall(cell, rx, ry))
			return 0;
	}
	return 1;
}
static void DrawGraphicalCell(MCell_t *cell, int dr_x, int dr_y)
{
	switch(Gnd.GraphicalMode)
	{
	case GM_01:
		{
			double b = 1.0 - 0.5 * cell->Bright;

			DPE_SetBright(b, b, b);
			DrawSimple(D_MAPCHIP_A5_00 + 35 | DTP, dr_x, dr_y);
			DPE_Reset();
		}
		break;

	case GM_02:
		{
			int f = cell->ItaFlag;
			int k = cell->ItaKind;
			int d = cell->DecoFlag;
			int i;

			if(k)
			{
				switch(f)
				{
				case ITA_C:  i = 21; break;
				case ITA_HL: i = 35; break;
				case ITA_H:  i = 36; break;
				case ITA_HR: i = 37; break;
				case ITA_VT: i =  6; break;
				case ITA_V:  i = 22; break;
				case ITA_VB: i = 38; break;

				default:
					error();
				}
			}
			else
			{
				switch(f)
				{
				case ITA_C:  i =  5; break;
				case ITA_HL: i = 51; break;
				case ITA_H:  i = d ? 54 : 52; break;
				case ITA_HR: i = 53; break;
				case ITA_VT: i =  7; break;
				case ITA_V:  i = d ? 55 : 23; break;
				case ITA_VB: i = 39; break;

				default: // 編集モードで壁が新設されることもある。
					i = 21;
					break;
				}
			}
			DrawSimple(D_MAPCHIP_B_00 + i | DTP, dr_x, dr_y);
		}
		break;

	case GM_03:
		{
			DrawMapChipWater(
				dr_x,
				dr_y,
				D_MAPCHIP_A1_MINI_00 + 400,
				32,
				cell,
				DGC_IsWall,
				3,
				30
				);
		}
		break;

	case GM_04:
	case GM_04_SOFT:
		{
			DrawMapChipWater(
				dr_x,
				dr_y,
				D_MAPCHIP_A4_MINI_00,
				32,
				cell,
				DGC_IsWall
				);

			if(cell->DecoFlag && DGC_IsKusa(cell))
			{
				int i;

				switch(cell->KusaId)
				{
				case 0: i = 65; break;
				case 1: i = 68; break;
				case 2: i = 69; break;
				case 3: i = 82; break;
				case 4: i = 99; break;
				case 5: i = 149; break;
				case 6: i = 213; break;
				case 7: i = 215; break;
				}
				DrawSimple(D_MAPCHIP_B_00 + i | DTP, dr_x, dr_y);
			}
		}
		break;

	case GM_EARTH:
		{
			int i = 39;

			if(cell->DecoFlag)
				i += 8;

			DrawSimple(D_MAPCHIP_A5_00 + i | DTP, dr_x, dr_y);
		}
		break;

	default:
		error();
	}
}
static void DrawField(int editMode = 0)
{
	int xOrig = GDc.ICameraX / CELLSIZE;
	int yOrig = GDc.ICameraY / CELLSIZE;
	int drawXOrig = xOrig * CELLSIZE - GDc.ICameraX;
	int drawYOrig = yOrig * CELLSIZE - GDc.ICameraY;

	const int w = SCREEN_W / CELLSIZE + 2;
	const int h = SCREEN_H / CELLSIZE + 2;

	for(int x = 0; x < w; x++)
	for(int y = 0; y < h; y++)
	{
		int cellX = xOrig + x;
		int cellY = yOrig + y;
		MCell_t *cell = GDc.Map->RefCell(cellX, cellY, NULL);

		if(cell)
		{
			int drawX = drawXOrig + x * CELLSIZE;
			int drawY = drawYOrig + y * CELLSIZE;

			if(IsPlayerWall(cell->CellType) && cell->CellType != ET_PLAYER_WALL)
			{
				switch(Gnd.GraphicalMode)
				{
				default:
					DrawGraphicalCell(cell, drawX, drawY);
					break;

				case GM_DEFAULT:
					{
						double b = ((cellX + cellY) & 1 ? 0.3 : 0.45) + 0.15 * cell->Bright;
						double d;

						{
							double cx = SCREEN_W / 2.0;
							double cy = SCREEN_H / 2.0;
							double dx = drawX + CELLSIZE / 2.0;
							double dy = drawY + CELLSIZE / 2.0;

							d = sqrt(m_pow(cx - dx) + m_pow(cy - dy));
						}

						int i = m_d2i(d / 10.0);
						m_range(i, 0, KABE_BR_MAX - 1);

						DPE_SetAlpha(GDc.KabeBrList_A[i]);
						DPE_SetBright(
							GDc.KabeBrList_R[i] * b,
							GDc.KabeBrList_G[i] * b,
							GDc.KabeBrList_B[i] * b
							);
						DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
						DPE_Reset();
					}
					break;

				case GM_LEGACY:
					{
						double b = ((cellX + cellY) & 1 ? 0.3 : 0.45) + 0.15 * cell->Bright;

						DPE_SetBright(b, b, 0.0);
						DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
						DPE_Reset();
					}
					break;

				case GM_METEOR:
				case GM_METEOR_SOFT:
					DPE_SetBright(0.5, 0.5, 0.5);
					DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
					DPE_Reset();
					break;

				case GM_GHOST:
					DPE_SetAlpha(0.3);
					DPE_SetBright(0.3, 0.6, 0.6);
					DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
					DPE_Reset();
					break;
				}

				// ---- 壁の中の敵 ----

				if(m_isRange(cell->CellType, ET_COOKIE_FIRST, ET_COOKIE_LAST))
				{
					switch(Gnd.GraphicalMode)
					{
					default:
						DrawSimple(D_MAPCHIP_D_00 + 191 | DTP, drawX, drawY);
						break;

					case GM_DEFAULT:
					case GM_LEGACY:
					case GM_METEOR:
					case GM_METEOR_SOFT:
					case GM_GHOST:
						{
							const int MARGIN = 8;

							DPE_SetBright(0.3, 1.0, 0.2);
							DrawRect(P_WHITEBOX, drawX + MARGIN, drawY + MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - MARGIN * 2);
							DPE_Reset();
						}
						break;
					}
				}
				if(m_isRange(cell->CellType, ET_GUNNER_00, ET_GUNNER_00 + GUNNER_MAX - 1))
				{
					const int MARGIN = 8;
					const int MARGIN_B = 12;

					DPE_SetBright(1.0, 0.5, 1.0);
					DrawRect(P_WHITEBOX, drawX + MARGIN, drawY + MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - MARGIN * 2);

					switch(GetGunnerDir(cell->CellType))
					{
					case 2:
						DrawRect(
							P_WHITEBOX,
							drawX + MARGIN_B,
							drawY + CELLSIZE / 2,
							CELLSIZE - MARGIN_B * 2,
							CELLSIZE / 2
							);
						break;
					case 4:
						DrawRect(
							P_WHITEBOX,
							drawX,
							drawY + MARGIN_B,
							CELLSIZE / 2,
							CELLSIZE - MARGIN_B * 2
							);
						break;
					case 6:
						DrawRect(
							P_WHITEBOX,
							drawX + CELLSIZE / 2,
							drawY + MARGIN_B,
							CELLSIZE / 2,
							CELLSIZE - MARGIN_B * 2
							);
						break;
					case 8:
						DrawRect(
							P_WHITEBOX,
							drawX + MARGIN_B,
							drawY,
							CELLSIZE - MARGIN_B * 2,
							CELLSIZE / 2
							);
						break;

					default:
						error();
					}
					DPE_Reset();
				}
				if(m_isRange(cell->CellType, ET_DAIKAITEN_00, ET_DAIKAITEN_00 + DAIKAITEN_MAX - 1))
				{
					const int MARGIN = 8;

					DPE_SetBright(0.2, 1.0, 0.4);
					DrawRect(P_WHITEBOX, drawX + MARGIN, drawY + MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - MARGIN * 2);
					DPE_Reset();
				}
				if(m_isRange(cell->CellType, ET_ASHIBA_00, ET_ASHIBA_00 + ASHIBA_MAX - 1))
				{
					const int MARGIN = 6;
					const int MARGIN_B = 10;

					DPE_SetBright(1.0, 1.0, 1.0);
					DrawRect(P_WHITEBOX, drawX + MARGIN, drawY + MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - MARGIN * 2);
					DPE_SetBright(0.5, 0.5, 0.5);
					DrawRect(P_WHITEBOX, drawX + MARGIN_B, drawY + MARGIN_B, CELLSIZE - MARGIN_B * 2, CELLSIZE - MARGIN_B * 2);
					DPE_Reset();
				}

				// @ AddEnemy 壁の中

				// ----
			}
			else if(cell->CellType == CT_DEATH)
			{
				int f = ProcFrame + cellX + cellY;
				double b = (sin(((f % 100) / 100.0) * PI * 2.0) * 0.5 + 0.5) * 0.7 + 0.3;
				m_range(b, 0.0, 1.0);

				DPE_SetBright(b, 0.0, 0.0);
				DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
				DPE_Reset();
			}
			else if(cell->CellType == CT_GOAL)
			{
				int f = ProcFrame;
				double b = (sin(((f % 30) / 30.0) * PI * 2.0) * 0.5 + 0.5) * 0.7 + 0.3;
				m_range(b, 0.0, 1.0);

				DPE_SetBright(0.0, b, b);
				DrawRect(P_WHITEBOX, drawX, drawY, CELLSIZE, CELLSIZE);
				DPE_Reset();
			}

			if(editMode)
			{
				DrawEditCell(cell->CellType, drawX, drawY);
			}
		}
	}
}

static void DrawPlayer(void)
{
	if(GDc.Player.BornFrame)
		return;

	if(DEADFRAME_BURN <= GDc.Player.DeadFrame)
		return;

	if(FLOORCLEARFRAME_ERASE <= GDc.Player.FloorClearFrame)
		return;

	int dr_x = GDc.Player.IX - GDc.ICameraX - CELLSIZE / 2;
	int dr_y = GDc.Player.IY - GDc.ICameraY - CELLSIZE / 2;

	switch(Gnd.GraphicalMode)
	{
	case GM_DEFAULT:
	case GM_LEGACY:
	case GM_METEOR:
	case GM_METEOR_SOFT:
	case GM_GHOST:
		DrawRect(P_WHITEBOX, dr_x, dr_y, CELLSIZE, CELLSIZE);
		break;

	case GM_01:
		DrawSimple(D_MAPCHIP_B_00 + 40 | DTP, dr_x, dr_y);
		break;

	case GM_02:
	case GM_03:
	case GM_04:
	case GM_04_SOFT:
		DrawSimple(D_MAPCHIP_B_00 + 56 | DTP, dr_x, dr_y);
		break;

	case GM_EARTH:
		DPE.PicRes = GetGSPicRes();
		DrawSimple(D_MAPCHIP_D_00 + 168 | DTP, dr_x, dr_y);
//		DrawSimple(D_MAPCHIP_B_00 + 40 | DTP, dr_x, dr_y);
		DPE_Reset();
		break;

	default:
		error();
	}
}

// < Draw

static void ResetBornPos(void)
{
	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		if(GDc.Map->GetCell(x, y)->CellType == CT_START)
		{
			GDc.BornX = (x * CELLSIZE + CELLSIZE / 2) * IDENOM;
			GDc.BornY = (y * CELLSIZE + CELLSIZE / 2) * IDENOM;

			return;
		}
	}
	error(); // CT_START not found
}
static void ReplaceInMap(int fromCellType, int toCellType)
{
	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		if(GDc.Map->GetCell(x, y)->CellType == fromCellType)
		{
			GDc.Map->GetCell(x, y)->CellType = toCellType;
		}
	}
}
static int GetCountInMap(int cellType)
{
	int count = 0;

	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		if(GDc.Map->GetCell(x, y)->CellType == cellType)
		{
			count++;
		}
	}
	return count;
}

static void EditMain(void)
{
	SetMouseDispMode(1);
	FreezeInput();

	for(; ; )
	{
		if(GetKeyPound(KEY_INPUT_E))
			break;

		if(GetKeyPound(KEY_INPUT_L))
			GDc.Edit.LeftWindowFlag = GDc.Edit.LeftWindowFlag ? 0 : 1;

		// mouse...

		int lastMouseX = MouseX;
		int lastMouseY = MouseY;

		UpdateMousePos();
		EditMenuEachFrame();

		switch(GDc.Edit.MenuClicked)
		{
		case EMC_NONE:
			break;

		case EMC_EMPTY:
			goto endClicked;

		case EMC_MENUPOS:
			GDc.Edit.LeftWindowFlag = GDc.Edit.LeftWindowFlag ? 0 : 1;
			goto endClicked;

		case EMC_CELLTYPE:
			GDc.Edit.SelectCellType = GDc.Edit.ClickedCellType;
			goto endClicked;

		case EMC_EXIT:
			goto endEdit;

		default:
			error();
		}

		if(1 <= GetKeyInput(KEY_INPUT_LSHIFT) || 1 <= GetKeyInput(KEY_INPUT_RSHIFT))
		{
			if(1 <= GetMouInput(MOUBTN_L))
			{
				CameraX -= MouseX - lastMouseX;
				CameraY -= MouseY - lastMouseY;

				goto endClicked;
			}
		}

		int mouMapX = (m_d2i(CameraX) + MouseX) / CELLSIZE;
		int mouMapY = (m_d2i(CameraY) + MouseY) / CELLSIZE;

		MCell_t *cell = GDc.Map->RefCell(mouMapX, mouMapY, NULL);

		if(cell)
		{
			if(1 <= GetMouInput(MOUBTN_L))
			{
				if(GDc.Edit.SelectCellType == CT_START)
				{
					ReplaceInMap(CT_START, CT_EMPTY);
				}
				// ゴールはいくつあっても良い。
				/*
				if(GDc.Edit.SelectCellType == CT_GOAL)
				{
					ReplaceInMap(CT_GOAL, CT_EMPTY);
				}
				*/

				cell->CellType = GDc.Edit.SelectCellType;
			}
			if(1 <= GetMouInput(MOUBTN_R))
			{
				cell->CellType = CT_EMPTY;
			}

			if(GetCountInMap(CT_START) < 1) // ? スタートが無くなる -> 復元する
			{
				cell->CellType = CT_START;
			}

			if(GetKeyPound(KEY_INPUT_W)) // zantei warp
			{
				GDc.WarpX = (mouMapX * CELLSIZE + CELLSIZE / 2) * IDENOM;
				GDc.WarpY = (mouMapY * CELLSIZE + CELLSIZE / 2) * IDENOM;

				Pub_CreateRespawnPoint(GDc.WarpX, GDc.WarpY, 1);

				//SEPlay(SE_MISS);
			}
		}
endClicked:

		// Camera >

		m_range(CameraX, 0, GDc.Field_W - SCREEN_W);
		m_range(CameraY, 0, GDc.Field_H - SCREEN_H);

		GDc.ICameraX = m_d2i(CameraX);
		GDc.ICameraY = m_d2i(CameraY);

		// < Camera

		GDc.Edit.Win_L = GDc.Edit.LeftWindowFlag ? 4 : SCREEN_W - EDIT_WIN_W - 4;
		GDc.Edit.Win_T = 4;

		// draw...

		MeteorBeforeDraw();

		DrawWall();
		DrawField(1);

		MeteorAfterDraw();

		// window
		{
			DPE_SetAlpha(0.8);
			DPE_SetBright(0, 0, 0);
			DrawRect(P_WHITEBOX, GDc.Edit.Win_L, GDc.Edit.Win_T, EDIT_WIN_W, EDIT_WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_G_00, GDc.Edit.Win_L, GDc.Edit.Win_T, EDIT_WIN_W, EDIT_WIN_H);
		}
		DrawEditMenuEntity();

		EachFrame();
	}
endEdit:
	SetMouseDispMode(0);
	FreezeInput();

	// 編集による再設定...
	// 中途半端な初期化

	LoadEnemy();
	ResetBornPos();
	GDc.PlayFrame = 0;

	if(GDc.MapBmpFile)
		SaveMap(GDc.MapBmpFile); // すぐにセーブする!!
}
static void PauseGame(void)
{
	zeroclear(&GDc.Pause);

	SEPlay(SE_PAUSE_IN);

	FreezeInput();

	const int selectMax = 3; // num of item
	const int exitIndex = 0;
	const int editModeIndex = 2;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetInput(INP_PAUSE) == 1)
			break;

		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		selectIndex += selectIndexAdd;

		// 廃止 @ 2016.10.24
		/*
		if(!GDc.EditableFlag && selectIndex == editModeIndex)
			selectIndex += selectIndexAdd;
			*/

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == exitIndex)
				break;

			selectIndex = exitIndex;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				goto endMenu;

			case 1:
				GDc.Pause.RestartFlag = 1;
				goto endMenu;

			// 廃止 @ 2016.10.24
			/*
			case 2:
				EditMain();
				break;
				*/

			case 2:
				GDc.Pause.BackToMenuFlag = 1;
				goto endMenu;

			default:
				error();
			}
		}

		// draw...

		MeteorBeforeDraw();

		DrawWall();
		DrawField();

		MeteorAfterDraw();

		DrawCurtain(-0.3);

		// window
		{
			const int WIN_L = 280;
			const int WIN_T = 196;
			const int WIN_W = 240;
			const int WIN_H = 208;

			DPE_SetAlpha(0.3);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			const int X_SIZE = WIN_W / 16;
			const int Y_SIZE = WIN_H / 16;

			for(int x = 0; x < X_SIZE; x++)
			for(int y = 0; y < Y_SIZE; y++)
			{
				int derId = 0;

				if(x == 0 && y == 0)
				{
					derId = D_WINDOW_B_00;
				}
				else if(x == X_SIZE - 1 && y == 0)
				{
					derId = D_WINDOW_B_00 + 3;
				}
				else if(x == 0 && y == Y_SIZE - 1)
				{
					derId = D_WINDOW_B_00 + 12;
				}
				else if(x == X_SIZE - 1 && y == Y_SIZE - 1)
				{
					derId = D_WINDOW_B_00 + 15;
				}
				else if(x == 0)
				{
					derId = D_WINDOW_B_00 + 4 + (1 - y % 2) * 4;
				}
				else if(x == X_SIZE - 1)
				{
					derId = D_WINDOW_B_00 + 7 + (1 - y % 2) * 4;
				}
				else if(y == 0)
				{
					derId = D_WINDOW_B_00 + 1 + (1 - x % 2) * 1;
				}
				else if(y == Y_SIZE - 1)
				{
					derId = D_WINDOW_B_00 + 13 + (1 - x % 2) * 1;
				}

				if(derId)
					DrawSimple(derId | DTP, WIN_L + x * 16, WIN_T + y * 16);
			}
		}

		// menu item >
		{
			const int COLOR = GetColor(200, 200, 255);
			const int BORDER_COLOR = GetColor(100, 100, 200);

			SetPrint(320, 240, 50);
//			SetPrint(320, 230, 40); // 廃止 @ 2016.10.24
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			int c = 0;
			Print_x(xcout("[%c] CONTINUE", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RESTART", selectIndex == c++ ? '>' : ' '));
			PrintRet();

			// 廃止 @ 2016.10.24
			/*
			if(!GDc.EditableFlag)
			{
				PE.Color = GetColor(100, 100, 100);
				PE_Border(GetColor(50, 50, 50));
			}
			Print_x(xcout("[%c] EDIT FLOOR", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			*/

			Print_x(xcout("[%c] BACK TO TITLE", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	FreezeInput();

	SEPlay(SE_PAUSE_OUT);
}
static int CreateRespawnPoint(fPInt_t x, fPInt_t y, int forceMode = 0) // ret: ? created
{
	if(Gnd.RespawnMax <= GDc.RespawnPointCreatedCount && !forceMode)
	{
		SEPlay(SE_CREATE_RESPAWN_POINT_FAILED);
		return 0;
	}
	GDc.RespawnPointCreatedCount++;
	m_minim(GDc.RespawnPointCreatedCount, IMAX);
	GDc.RespawnX = x;
	GDc.RespawnY = y;
	GDc.RespawnPoint_Z = 2.0;
	SEPlay(SE_CREATE_RESPAWN_POINT);
	SaveEnemy();
	return 1;
}
void Pub_CreateRespawnPoint(fPInt_t x, fPInt_t y, int forceMode)
{
	CreateRespawnPoint(x, y, forceMode);
}
static void ResetRespawnPoint(void)
{
	if(!GDc.RespawnPointCreatedCount)
		return;

	GDc.RespawnPointCreatedCount = 0;
	SEPlay(SE_REMOVE_RESPAWN_POINT);
}
static void DrawRespawnPoint(void)
{
	if(!GDc.RespawnPointCreatedCount)
		return;

	{
//		int x = m_fpi2i(GDc.RespawnX) + CELLSIZE / 2;
//		int y = m_fpi2i(GDc.RespawnY) + CELLSIZE / 2;
		// 中心座標でした...
		int x = m_fpi2i(GDc.RespawnX);
		int y = m_fpi2i(GDc.RespawnY);

		x -= m_d2i(CameraX);
		y -= m_d2i(CameraY);

		// old
		/*
//		x -= 0;
		y += 15;
		*/
		y -= 3;

		const int MARGIN = 30;

		if(
			m_isRange(x, 0 - MARGIN, SCREEN_W + MARGIN) &&
			m_isRange(y, 0 - MARGIN, SCREEN_H + MARGIN)
			)
		{
			DPE_SetAlpha(0.5);
			DrawBegin(D_RESPAWN_POINT_00 + (ActFrame / 3) % 10 | DTP, x, y);
			DrawZoom(GDc.RespawnPoint_Z);
			DrawEnd();
			DPE_Reset();
		}

		m_approach(GDc.RespawnPoint_Z, 0.5, 0.99);
	}
}
static void ResetForRestart(void)
{
	zeroclear(&GDc.Player);

	if(GDc.RespawnPointCreatedCount)
	{
		GDc.Player.X = GDc.RespawnX;
		GDc.Player.Y = GDc.RespawnY;
		GDc.RestartedFromRespawnPoint = 1;
	}
	else
	{
		GDc.Player.X = GDc.BornX;
		GDc.Player.Y = GDc.BornY;
		GDc.RestartedFromRespawnPoint = 0;
	}
	GDc.Player.BornFrame = 1;

	GDc.CamSlideTrueX = 0;
	GDc.CamSlideTrueY = 0;

	GDc.PlaySeekPos = 0;

	GDc.JmpBtnDwnFrm = 0;
	GDc.JmpBtnUpFlg = 0;

	if(GDc.ReplayMode)
	{
		// noop
	}
	else
	{
		ClearReplayData();
	}

//	initRnd(GDc.FloorIndex * 373000 + GDc.Player.X + GDc.Player.Y); // エフェクトとかの動きも電源パタン化 .. そこまでやるか？
	// やる ->
	// やっぱり止めとく、様子見
	/*
	delete GDc.PlayRnd;
	GDc.PlayRnd = new Random(373000);
	*/

	GDc.PlayFrame = 0;
}
void GameMain(void)
{
	errorCase(GDc.EditableFlag && GDc.ReplayMode);

	if(!GDc.MapBmp)
	{
		if(!GDc.MapBmpImage)
		{
			errorCase(!GDc.MapBmpFile);
			GDc.MapBmpImage = readFile(GDc.MapBmpFile);
		}
		GDc.MapBmp = readBmpFile(GDc.MapBmpImage);
	}
	GDc.Rnd = new Random(GetRndSeed(GDc.MapBmp));
	LoadMap(GDc.MapBmp);

	// **** 出張 ****
	{
		LOGPOS();
		if(!GDc.MapBmpImage)
		{
			LOG("GDc.MapBmpImage is null");
		}
		else if(GDc.ReplayMode)
		{
			LOG("GDc.ReplayMode is true");
		}
		else
		{
			SetReplayMapHash_x(MakeMapHash(GDc.MapBmpImage));
		}
		LOGPOS();
	}

	GDc.Field_W = GDc.Map->GetWidth() * CELLSIZE;
	GDc.Field_H = GDc.Map->GetHeight() * CELLSIZE;

	errorCase(!m_isRange(GDc.Field_W, 1, IMAX));
	errorCase(!m_isRange(GDc.Field_H, 1, IMAX));

	//LoadEnemy(); // move to bornPlayer

	SetCurtain();
	FreezeInput();
	ActFrame = 0;

	MusicPlay(MUS_FLOOR_01);

	GDc.Edit.SelectCellType = CT_WALL;

	GDc.WarpX = GDc.BornX;
	GDc.WarpY = GDc.BornY;

	ResetBornPos();
	ResetForRestart();

	// moved
//	FloorWallMasterMain();
//	PostInitKabe();

	// touch...
	{
		for(int seId = 0; seId < SE_MAX; seId++)
			SETouch(seId);
	}

	for(; ; )
	{
		int redScreenFlag = 0;

		if(GetInput(INP_PAUSE) == 1)
		{
			PauseGame();

			if(GDc.Pause.RestartFlag)
			{
				ResetRespawnPoint();
				ResetForRestart();
			}
			if(GDc.Pause.BackToMenuFlag)
			{
				GDc.ReturnMenuFlag = 1;
				break;
			}
			GDc.FreezeAButton = 2;
		}
		if(!GDc.FreezeRestart && GetInput(INP_R) == 1)
//		if(!GDc.Player.BornFrame && GetInput(INP_R) == 1) // old
		{
			GDc.FreezeRestart = 30;
			ResetForRestart();
			SEPlay(SE_RESTART);
		}
		if(GDc.EditableFlag && GetKeyPound(KEY_INPUT_E))
		{
			EditMain();
		}
		/*
			GDc.ReplayMode != 0 のときは GDc.LastDeadReplaySaveOk != 0 にはならない。
			リスポーン地点から開始していたらリプレイを保存出来ない。<- GDc.LastDeadReplaySaveOk != 0 にならない。
		*/
		if(
			Gnd.RecordingMode &&
			GDc.LastDeadReplaySaveOk &&
			GetKeyPound(KEY_INPUT_R)
			)
		{
			SwapReplayData(); // 直前の死亡時のリプレイの復元

			if(GetReplayInput()->GetCount() < REPLAY_DATA_MAX) // ? not overflow
			{
				char *replayFile = GetSaveReplayFilePath(GDc.FloorIndex, 0, 1);

				SaveReplayFile(replayFile);

				ChangeInfoCaption_x(xcout("REPLAY > %s", replayFile));
				SEPlay(SE_ACCEPT);

				memFree(replayFile);
			}
			SwapReplayData(); // 復帰
			GDc.LastDeadReplaySaveOk = 0;
		}
		if(GetKeyPound(KEY_INPUT_T) && ChangeBackgroundByTKeyEnabled)
		{
			Gnd.GraphicalMode = (Gnd.GraphicalMode + 1) % GM_MAX;
			ChangeInfoCaption_x(xcout("GRAPHIC MODE : %s", GetGraphicalModeCaption(Gnd.GraphicalMode)));
		}
#if LOG_ENABLED
		if(GetKeyPound(KEY_INPUT_3)) // Floor 3 ゴール付近へワープ
		{
			GDc.Player.X = 795 * 32 * 1000;
		}
#endif

		// GameInput >

		/*
			スタート・リスタート時に初期化しないけど、
			開始直後は操作が効かないので、影響はないはず！
		*/
		GDc.LastInput = GDc.CurrInput;

		if(GDc.ReplayMode)
		{
			int recInput = GetReplayInput()->GetElement(GDc.PlaySeekPos);
			int recPound = GetReplayPound()->GetElement(GDc.PlaySeekPos);

			GDc.PlaySeekPos++;

#if LOG_ENABLED
//			clsDx();
			printfDx(
				"%d %d(%d) %d(%d)\n"
				,GDc.PlaySeekPos
				,GetReplayInput()->GetCount()
				,GetReplayPound()->GetCount()
				,GetReplayInput()->GetCount() - GDc.PlaySeekPos
				,GetReplayPound()->GetCount() - GDc.PlaySeekPos
				);
#endif

			GDc.CurrInput.Dir2 = recInput >> REPBIT_DIR_2 & 1;
			GDc.CurrInput.Dir4 = recInput >> REPBIT_DIR_4 & 1;
			GDc.CurrInput.Dir6 = recInput >> REPBIT_DIR_6 & 1;
			GDc.CurrInput.Dir8 = recInput >> REPBIT_DIR_8 & 1;
			GDc.CurrInput.A = recInput >> REPBIT_A & 1;
			GDc.CurrInput.B = recInput >> REPBIT_B & 1;
			GDc.CurrInput.L = recInput >> REPBIT_L & 1;
			GDc.CurrInput.R = recInput >> REPBIT_R & 1;

			GDc.CurrPound.Dir2 = recPound >> REPBIT_DIR_2 & 1;
			GDc.CurrPound.Dir4 = recPound >> REPBIT_DIR_4 & 1;
			GDc.CurrPound.Dir6 = recPound >> REPBIT_DIR_6 & 1;
			GDc.CurrPound.Dir8 = recPound >> REPBIT_DIR_8 & 1;
			GDc.CurrPound.A = recPound >> REPBIT_A & 1;
			GDc.CurrPound.B = recPound >> REPBIT_B & 1;
			GDc.CurrPound.L = recPound >> REPBIT_L & 1;
			GDc.CurrPound.R = recPound >> REPBIT_R & 1;
		}
		else
		{
#if LOG_ENABLED
//			clsDx();
			printfDx(
				"%d %d\n"
				,GDc.RestartedFromRespawnPoint
				,GDc.LastDeadReplaySaveOk
				);
#endif

			GDc.CurrInput.Dir2 = 1 <= GetInput(INP_DIR_2);
			GDc.CurrInput.Dir4 = 1 <= GetInput(INP_DIR_4);
			GDc.CurrInput.Dir6 = 1 <= GetInput(INP_DIR_6);
			GDc.CurrInput.Dir8 = 1 <= GetInput(INP_DIR_8);
			GDc.CurrInput.A = 1 <= GetInput(INP_A);
			GDc.CurrInput.B = 1 <= GetInput(INP_B);
			GDc.CurrInput.L = 1 <= GetInput(INP_L);
			GDc.CurrInput.R = 1 <= GetInput(INP_R);

			GDc.CurrPound.Dir2 = GetPound(INP_DIR_2);
			GDc.CurrPound.Dir4 = GetPound(INP_DIR_4);
			GDc.CurrPound.Dir6 = GetPound(INP_DIR_6);
			GDc.CurrPound.Dir8 = GetPound(INP_DIR_8);
			GDc.CurrPound.A = GetPound(INP_A);
			GDc.CurrPound.B = GetPound(INP_B);
			GDc.CurrPound.L = GetPound(INP_L);
			GDc.CurrPound.R = GetPound(INP_R);

			if(GDc.CurrInput.A)
			{
				if(GDc.FreezeAButton)
				{
					GDc.CurrInput.A = 0;
					GDc.CurrPound.A = 0;
				}
			}
			else
				m_countDown(GDc.FreezeAButton);

			if(GetReplayInput()->GetCount() < REPLAY_DATA_MAX) // ? not overflow
			{
				int recInput = 0;
				int recPound = 0;

				if(GDc.CurrInput.Dir2) recInput |= 1 << REPBIT_DIR_2;
				if(GDc.CurrInput.Dir4) recInput |= 1 << REPBIT_DIR_4;
				if(GDc.CurrInput.Dir6) recInput |= 1 << REPBIT_DIR_6;
				if(GDc.CurrInput.Dir8) recInput |= 1 << REPBIT_DIR_8;
				if(GDc.CurrInput.A) recInput |= 1 << REPBIT_A;
				if(GDc.CurrInput.B) recInput |= 1 << REPBIT_B;
				if(GDc.CurrInput.L) recInput |= 1 << REPBIT_L;
				if(GDc.CurrInput.R) recInput |= 1 << REPBIT_R;

				if(GDc.CurrPound.Dir2) recPound |= 1 << REPBIT_DIR_2;
				if(GDc.CurrPound.Dir4) recPound |= 1 << REPBIT_DIR_4;
				if(GDc.CurrPound.Dir6) recPound |= 1 << REPBIT_DIR_6;
				if(GDc.CurrPound.Dir8) recPound |= 1 << REPBIT_DIR_8;
				if(GDc.CurrPound.A) recPound |= 1 << REPBIT_A;
				if(GDc.CurrPound.B) recPound |= 1 << REPBIT_B;
				if(GDc.CurrPound.L) recPound |= 1 << REPBIT_L;
				if(GDc.CurrPound.R) recPound |= 1 << REPBIT_R;

				GetReplayInput()->AddElement(recInput);
				GetReplayPound()->AddElement(recPound);
			}
		}

		// < GameInput

		// Camera >

		GDc.CameraTargetX = m_fpi2i(GDc.Player.X) - SCREEN_CENTER_X;
		GDc.CameraTargetY = m_fpi2i(GDc.Player.Y) - SCREEN_CENTER_Y;

		GDc.CameraTargetX += GDc.CamSlideRealX;
		GDc.CameraTargetY += GDc.CamSlideRealY;

		m_range(GDc.CameraTargetX, 0, GDc.Field_W - SCREEN_W);
		m_range(GDc.CameraTargetY, 0, GDc.Field_H - SCREEN_H);

		if(ActFrame)
		{
			m_approach(CameraX, GDc.CameraTargetX, 0.8);
			m_approach(CameraY, GDc.CameraTargetY, 0.8);
		}
		else
		{
			CameraX = GDc.CameraTargetX;
			CameraY = GDc.CameraTargetY;
		}
		m_range(CameraX, 0, GDc.Field_W - SCREEN_W);
		m_range(CameraY, 0, GDc.Field_H - SCREEN_H);

		GDc.ICameraX = m_d2i(CameraX);
		GDc.ICameraY = m_d2i(CameraY);

		m_range(GDc.ICameraX, 0, GDc.Field_W - SCREEN_W);
		m_range(GDc.ICameraY, 0, GDc.Field_H - SCREEN_H);

		// < Camera

		if(GDc.Player.BornFrame)
		{
			if(BORNFRAME_MAX < GDc.Player.BornFrame)
			{
				GDc.Player.BornFrame = 0;
				goto endBornPlayer;
			}

			{
				int bf = GDc.Player.BornFrame; // 1 ～ BORNFRAME_MAX

				if(bf == 1)
				{
					if(GDc.RestartedFromRespawnPoint && !GDc.EditableFlag)
						LoadSavedEnemy();
					else
						LoadEnemy();

					for(int index = 0; index < GDc.EnemyList->GetCount(); index++)
					{
						Enemy_t *i = GDc.EnemyList->GetElement(index);

						if(
							m_isRange(i->EnemyType, ET_ASHIBA_00, ET_ASHIBA_00 + ASHIBA_MAX - 1) ||
							i->EnemyType == ET_ENEMY_WALL ||
							i->EnemyType == ET_PLAYER_WALL
							)
							continue;

						/*
							カメラがスタート地点へ戻る前なので、判定は意味がない。
						*/
//						if(IsInCamera(m_fpi2i(i->X), m_fpi2i(i->Y), 100))
						{
							CEE.EndPicId = D_MAHOUJIN_00 + 13 | DTP;
							CEE.PicIdFrmPerInc = 2;
							AddCommonEffect(
								Gnd.EL,
								0,
								D_MAHOUJIN_00 | DTP,
								m_fpi2i(i->X),
								m_fpi2i(i->Y)
								);
							CEE_Reset();
						}
					}
				}
				if(bf == BORNFRAME_MAX)
				{
					double a = 1.0;

					if(GDc.RestartedFromRespawnPoint && GDc.CurrInput.R) // ? リスポーン && リスポーン_ボタン_ホールド中
						a = 0.0;

					for(int c = 0; c < 12; c++)
					{
						double s = 10.0 + 10.0 * dRnd();
//						double r = PI * 2.0 * dRnd();
						double r = PI * 2.0 * (c / 10.0);

						CEE.SpeedRate = 0.9;
						CEE_SetBright(
							0.7 + 0.3 * dRnd(),
							0.7 + 0.3 * dRnd(),
							0.7 + 0.3 * dRnd()
							);
						AddCommonEffect(
							Gnd.EL,
							0,
							D_STAR_W_00 + 6 | DTP,
							m_fpi2i(GDc.Player.X),
							m_fpi2i(GDc.Player.Y),
							PI * 2.0 * dRnd(),
							0.5 + 0.5 * dRnd(),
							a,
							cos(r) * s,
							sin(r) * s,
							0.1 * eRnd(),
							0.0,
							-0.03
							);
						CEE_Reset();

#if 1
						s = 2.0 + 2.0 * dRnd();
//						r = PI * 2.0 * dRnd();
						r = PI * 2.0 * (c / 10.0) + 0.3 * eRnd();

						CEE_SetBright(
							0.5 + 0.5 * dRnd(),
							0.5 + 0.5 * dRnd(),
							0.5 + 0.5 * dRnd()
							);
						AddCommonEffect(
							Gnd.EL,
							0,
							D_STAR_W_00 + 6 | DTP,
							m_fpi2i(GDc.Player.X),
							m_fpi2i(GDc.Player.Y),
							PI * 2.0 * dRnd(),
							1.0 + 0.5 * dRnd(),
							a * 0.8,
							cos(r) * s,
							sin(r) * s,
							0.1 * eRnd(),
							0.0,
							-0.01
							);
						CEE_Reset();
#endif
					}
					SEPlay(SE_START);
				}
			}

			GDc.Player.BornFrame++;
			goto endPlayer;
		}
endBornPlayer:

		if(GDc.Player.DeadFrame)
		{
			{
				int df = GDc.Player.DeadFrame; // 1 ～ DEADFRAME_MAX

				if(df == 1)
				{
					SEPlay(SE_MISS);
				}
				if(df < DEADFRAME_BURN)
				{
					redScreenFlag = 1;
				}
				if(df == DEADFRAME_BURN)
				{
					CEE.EndPicId = D_FIRE_R_00_END | DTP;
					CEE.PicIdFrmPerInc = 2;
					AddCommonEffect(
						Gnd.EL,
						0,
						D_FIRE_R_00 | DTP,
						m_fpi2i(GDc.Player.X),
						m_fpi2i(GDc.Player.Y) - 10
						);
					CEE_Reset();

					SEPlay(SE_FIRE);
				}
				if(df == DEADFRAME_MAX)
				{
					if(GDc.ReplayMode)
					{
						break;
					}
					SwapReplayData();
					GDc.LastDeadReplaySaveOk = !GDc.RestartedFromRespawnPoint;
					ResetForRestart();
					goto endPlayer;
				}
			}

			GDc.Player.DeadFrame++;
			goto endPlayer;
		}
//endDeadPlayer:
		if(GDc.Player.FloorClearFrame)
		{
			{
				int cf = GDc.Player.FloorClearFrame; // 1 ～ FLOORCLEARFRAME_MAX

				if(cf == FLOORCLEARFRAME_ERASE)
				{
					CEE.EndPicId = D_BLUEEXPLOSION_00_END | DTP;
					CEE.PicIdFrmPerInc = 3;
					AddCommonEffect(
						Gnd.EL,
						0,
						D_BLUEEXPLOSION_00 | DTP,
						m_fpi2i(GDc.Player.X),
						m_fpi2i(GDc.Player.Y)
						);
					CEE_Reset();

					SEPlay(SE_FLOORCLEAR);
				}
				if(cf == FLOORCLEARFRAME_MAX)
				{
					GDc.FloorClearFlag = 1;
					break;
				}
			}

			GDc.Player.FloorClearFrame++;
			goto endPlayer;
		}
		EachFrameAllEnemy();
		GDc.PlayFrame++;

		if(GDc.SoundAshibaActiveFlag)
		{
			SEPlay(SE_ASHIBA_ACTIVE, GDc.SoundAshibaActiveVolPct);

			LOG("GDc.SoundAshibaActiveVolPct = %d\n", GDc.SoundAshibaActiveVolPct);

			GDc.SoundAshibaActiveFlag = 0;
			GDc.SoundAshibaActiveVolPct = 0;
		}

		if(GDc.EditableFlag && GetKeyPound(KEY_INPUT_W)) // zantei warp
		{
			GDc.Player.X = GDc.WarpX;
			GDc.Player.Y = GDc.WarpY;

			SEPlay(SE_MISS);
		}
//		if(!m_countDown(GDc.RespawnYokushiFrame))
		if(!GDc.ReplayMode && GetInput(INP_C) == 1)
		{
			if(CreateRespawnPoint(GDc.Player.X, GDc.Player.Y, GDc.EditableFlag))
			{
//				GDc.RespawnYokushiFrame = 30;
			}
		}

		// Player
		{
			const fPInt_t SPEED = 7000;
			const fPInt_t SLOW_SPEED = 3000;
			int speed;

			if(GetGameInput(INP_B)) // 低速移動
			{
				const int FADE_MAX = 20;
				int wc = GDc.Player.WalkFrame + 1;

				if(wc < FADE_MAX && GDc.Player.OnGround)
					speed = (SLOW_SPEED / FADE_MAX) * wc;
				else
					speed = SLOW_SPEED;

				GDc.Player.SlowFrame++;
			}
			else // 高速(通常)移動
			{
				speed = SPEED;
				GDc.Player.SlowFrame = 0;
			}

			int camSlideOn = GetGameInput(INP_L);

			if(!camSlideOn && GetGameInput(INP_DIR_4))
			{
				m_iApproach(GDc.Player.XAdd, -speed, 2, 3);
				GDc.Player.WalkFrame++;
			}
			else if(!camSlideOn && GetGameInput(INP_DIR_6))
			{
				m_iApproach(GDc.Player.XAdd, speed, 2, 3);
				GDc.Player.WalkFrame++;
			}
			else // 左右移動ナシ
			{
				GDc.Player.XAdd /= 2;
				GDc.Player.WalkFrame = 0;
			}

//			const int JUMPFRAME_MAX = 22;
			int jumpOn = 0;

			if(GetGameInput(INP_A))
			{
				GDc.JmpBtnDwnFrm++;
			}
			else
			{
				GDc.JmpBtnDwnFrm = 0;
				GDc.JmpBtnUpFlg = 1;
			}

			if(GDc.Player.JumpFrame)
			{
				if(GetGameInput(INP_A) && GDc.Player.JumpFrame < JUMPFRAME_MAX)
					jumpOn = 1;
			}
			else
			{
				/*
					滞空中に次のジャンプ開始を受け付けるマージン付き
				*/
				if(GDc.JmpBtnUpFlg && m_isRange(GDc.JmpBtnDwnFrm, 1, 10))
//				if(GetGameInput(INP_A) && !GetLastGameInput(INP_A)) // old
				{
					jumpOn = 1;
					GDc.JmpBtnUpFlg = 0;
				}
			}
			if(jumpOn)
			{
				if(GDc.Player.JumpFrame == 0)
					SEPlay(SE_JUMP);

				GDc.Player.YAdd = -7000;
				GDc.Player.JumpFrame++;
			}
			else
			{
				GDc.Player.YAdd += 450; // 重力
				m_minim(GDc.Player.YAdd, 7000);
			}
			int lastOnGround = GDc.Player.OnGround;

			GDc.Player.X += GDc.Player.XAdd;
			GDc.Player.Y += GDc.Player.YAdd;

			PlayerWallProc();

			GDc.Player.IX = m_fpi2i(GDc.Player.X);
			GDc.Player.IY = m_fpi2i(GDc.Player.Y);

			if(lastOnGround)
			{
				if(!jumpOn)
//				if(!GetGameInput(INP_A)) // old
				{
					GDc.Player.JumpFrame = 0;
				}
				m_minim(GDc.Player.YAdd, 0);
			}
			else
			{
				if(Gnd.GraphicalMode == GM_EARTH)
				{
					if(GDc.Player.OnGround) // ? (このフレームで)接地した。
					{
						double power = GDc.Player.YAdd / 7000.0;
						m_range(power, 0.0, 1.0);

						GDc.QuakePower += power;

						SEPlay(SE_BOMB, m_d2i(50.0 + 50.0 * power));
					}
				}
				if(!jumpOn) // ? 空中 && 非ジャンプ中
				{
					GDc.Player.JumpFrame = JUMPFRAME_MAX;
				}
			}
			if(GDc.Player.TouchRoofOn)
			{
				GDc.Player.YAdd = abs(GDc.Player.YAdd);
				GDc.Player.JumpFrame = JUMPFRAME_MAX;
			}
			switch(GDc.Player.TouchSideOn)
			{
			case TOUCH_L: m_maxim(GDc.Player.XAdd, 0); break;
			case TOUCH_R: m_minim(GDc.Player.XAdd, 0); break;
			}
			if(GDc.Player.OnDeath)
			{
				GDc.Player.DeadFrame = 1;
			}
			if(GDc.Player.OnGoal)
			{
				GDc.Player.FloorClearFrame = 1;
			}

			if(IsEnemyCrashed(
				GDc.Player.X - (CELLSIZE / 2) * IDENOM,
				GDc.Player.Y - (CELLSIZE / 2) * IDENOM,
				CELLSIZE * IDENOM,
				CELLSIZE * IDENOM
				))
			{
				GDc.Player.DeadFrame = 1;
			}
		}

		// Camera Slide...

		if(GDc.CamSlideBtnOn && !GetGameInput(INP_L) && !GDc.CamSlid)
		{
			GDc.CamSlideTrueX = 0;
			GDc.CamSlideTrueY = 0;
		}
		if(!GDc.CamSlideBtnOn && GetGameInput(INP_L))
		{
			GDc.CamSlid = 0;
		}
		GDc.CamSlideBtnOn = GetGameInput(INP_L);
		if(GDc.CamSlideBtnOn)
		{
			const int SLIDE_X_RANGE = 300;
			const int SLIDE_Y_RANGE = 200;

			if(GetGamePound(INP_DIR_2))
			{
				GDc.CamSlid = 1;
				GDc.CamSlideTrueY += SLIDE_Y_RANGE;
				m_minim(GDc.CamSlideTrueY, SLIDE_Y_RANGE);
			}
			if(GetGamePound(INP_DIR_4))
			{
				GDc.CamSlid = 1;
				GDc.CamSlideTrueX -= SLIDE_X_RANGE;
				m_maxim(GDc.CamSlideTrueX, -SLIDE_X_RANGE);
			}
			if(GetGamePound(INP_DIR_6))
			{
				GDc.CamSlid = 1;
				GDc.CamSlideTrueX += SLIDE_X_RANGE;
				m_minim(GDc.CamSlideTrueX, SLIDE_X_RANGE);
			}
			if(GetGamePound(INP_DIR_8))
			{
				GDc.CamSlid = 1;
				GDc.CamSlideTrueY -= SLIDE_Y_RANGE;
				m_maxim(GDc.CamSlideTrueY, -SLIDE_Y_RANGE);
			}
		}
endPlayer:
		m_iApproach(GDc.CamSlideRealX, GDc.CamSlideTrueX, 3, 4);
		m_iApproach(GDc.CamSlideRealY, GDc.CamSlideTrueY, 3, 4);

		// draw...

		if(Gnd.GraphicalMode == GM_EARTH)
		{
			ScreenShift_X += m_d2i(eRnd() * GDc.QuakePower * 30.0);
			ScreenShift_Y += m_d2i(eRnd() * GDc.QuakePower * 30.0);

			GDc.QuakePower *= 0.9;
		}

		MeteorBeforeDraw();

		DrawWall();
		DrawRespawnPoint();
		DrawField();
		DrawPlayer();
		DrawAllEnemy();

		MeteorAfterDraw();

		ScreenShift_X = 0;
		ScreenShift_Y = 0;

		if(redScreenFlag)
		{
			DPE_SetAlpha(0.2);
			DPE_SetBright(1.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, 0, 0, SCREEN_W, SCREEN_H);
			DPE_Reset();

			int plX = GDc.Player.IX - GDc.ICameraX;
			int plY = GDc.Player.IY - GDc.ICameraY;

#if 1 // 画像で '？'
			DPE.MosaicOn = 1;
			DrawBegin(P_BIKKURI, plX + 5, plY - 30);
//			DrawBegin(P_BIKKURI, plX + (ProcFrame / 5 & 1 ? 10 : 0), plY - 30); // old
			DrawZoom(2.0);
			DrawEnd();
			DPE_Reset();
//			DrawCenter(D_KIGOU_DW_00 + 10 | DTP, plX, plY - 45); // old
#else // 文字で '？'
			SetPrint(plX - 8, plY - CELLSIZE / 2 - (16 + 4));
			PE_Border(GetColor(64, 0, 0));
			Print("？");
			PE_Reset();
#endif
		}

		if(!GDc.ReplayMode) // three-coin
		{
			const int DRAW_X_STEP = 27;
			const int DRAW_L = 810 - DRAW_X_STEP * Gnd.RespawnMax;
			const int DRAW_T = 580;
			double a;
			static double aa = 0.0;
			int camSliding =
				GDc.CamSlideTrueX != 0 ||
				GDc.CamSlideTrueY != 0;

			if(camSliding)
				a = 0.05;
			else
				a = 0.45;

			m_approach(aa, a, 0.85);

			for(int index = 0; index < Gnd.RespawnMax; index++)
			{
				double x = DRAW_L + DRAW_X_STEP * index;
				double y = DRAW_T;

				DPE_SetAlpha(aa);

				if(index < GDc.RespawnPointCreatedCount) // ? 使用済みコイン
				{
					DPE_SetBright(0.0, 0.0, 1.0);
				}
				else // ? 未使用コイン
				{
					// noop
				}
				DrawBegin(D_RESPAWN_POINT_00 + (ActFrame / 5) % 10 | DTP, x, y);
				DrawZoom(0.15);
				DrawEnd();
				DPE_Reset();
			}
		}

		if(m_countDown(GDc.InfoCaptionFrame))
		{
			errorCase(!GDc.InfoCaption);

			SetPrint(2, 1);
			PE_Border(GetColor(64, 64, 128));
			PE.Color = GetColor(255, 255, 255);
			Print(GDc.InfoCaption);
			PE_Reset();

			if(!GDc.InfoCaptionFrame)
			{
				memFree(GDc.InfoCaption);
				GDc.InfoCaption = NULL;
			}
		}

		if(GDc.EditableFlag)
		{
			int tentou = ProcFrame / 30 % 2;

			SetPrint(660, 1);
			PE_Border(tentou ? GetColor(64, 128, 64) : GetColor(32, 64, 32));
			PE.Color = tentou ? GetColor(255, 255, 255) : GetColor(0, 128, 0);
			Print("+-------------+"); PrintRet();
			Print("|  EDIT MODE  |"); PrintRet();
			Print("+-------------+");
			PE_Reset();
		}
		if(GDc.ReplayMode)
		{
			int tentou = ProcFrame / 30 % 2;

			SetPrint(684, 1);
			PE_Border(tentou ? GetColor(128, 64, 64) : GetColor(64, 32, 32));
			PE.Color = tentou ? GetColor(255, 255, 255) : GetColor(128, 0, 0);
			Print("+----------+"); PrintRet();
			Print("|  REPLAY  |"); PrintRet();
			Print("+----------+");
			PE_Reset();
		}

		EachFrame();

		m_countDown(GDc.FreezeRestart);

		// 敵の追加と削除 {

		GDc.EnemyList->AddElements(GDc.EnemyList_Add);
		GDc.EnemyList_Add->Clear();

		for(int index = 0; index < GDc.EnemyList->GetCount(); index++)
		{
			Enemy_t *e = GDc.EnemyList->GetElement(index);

			if(e->Death)
			{
				ReleaseEnemy(e);
				GDc.EnemyList->FastDesertElement(index);
				index--;
			}
		}
		for(int index = 0; index < GDc.EnemyList->GetCount(); index++) // ただの検査っぽい。
		{
			Enemy_t *e = GDc.EnemyList->GetElement(index);

			errorCase(e->Death);
		}

		// }

		// 敵の並べ替え {

		for(int index = 1; index < GDc.EnemyList->GetCount(); index++)
		{
			Enemy_t *e1 = GDc.EnemyList->GetElement(index - 1);
			Enemy_t *e2 = GDc.EnemyList->GetElement(index);

			if(GetEnemyOrderWeight(e2) < GetEnemyOrderWeight(e1))
			{
				GDc.EnemyList->Swap(index - 1, index);
			}
		}

		// }
	}
	if(GDc.ReplayMode && !GDc.ReturnMenuFlag)
	{
		if(
			GDc.PlaySeekPos != GetReplayInput()->GetCount() ||
			GDc.PlaySeekPos != GetReplayPound()->GetCount()
			)
		{
			printfDx(
				"%d %d %d\n"
				,GDc.PlaySeekPos
				,GetReplayInput()->GetCount()
				,GetReplayPound()->GetCount()
				);
			error();
		}
	}

//	FreezeInput();
	MusicFade();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		MeteorBeforeDraw();

		DrawWall();
		DrawField();

		MeteorAfterDraw();

		EachFrame();
	}
	sceneLeave();

	/*
		エフェクトのために、カメラ位置を戻す必要がある。
	*/
	CameraX = 0.0;
	CameraY = 0.0;
}
