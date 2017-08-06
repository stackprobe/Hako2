#include "all.h"

i2D_t GetScreenCellPos(fPInt_t x, fPInt_t y) // (x, y): キャラの中心
{
	i2D_t pos;

	pos.X = m_fpi2i(x) - GDc.ICameraX - CELLSIZE / 2;
	pos.Y = m_fpi2i(y) - GDc.ICameraY - CELLSIZE / 2;

	return pos;
}

FourCell_t GetFourCell(fPInt_t x, fPInt_t y) // (x, y): キャラの中心
{
	FourCell_t fc;

	int cx = (x + (CELLSIZE * IDENOM) / 2) / (CELLSIZE * IDENOM);
	int cy = (y + (CELLSIZE * IDENOM) / 2) / (CELLSIZE * IDENOM);

	fc.LT = GDc.Map->RefCell(cx - 1, cy - 1, NULL);
	fc.RT = GDc.Map->RefCell(cx - 0, cy - 1, NULL);
	fc.LB = GDc.Map->RefCell(cx - 1, cy - 0, NULL);
	fc.RB = GDc.Map->RefCell(cx - 0, cy - 0, NULL);

	fc.X = x - (cx * CELLSIZE * IDENOM);
	fc.Y = y - (cy * CELLSIZE * IDENOM);

	const fPInt_t hcrng = (CELLSIZE / 2) * IDENOM;

	errorCase(fc.X < -hcrng || hcrng <= fc.X);
	errorCase(fc.Y < -hcrng || hcrng <= fc.Y);

	fc.LTCellX = cx - 1;
	fc.LTCellY = cy - 1;

	if(x % (CELLSIZE * IDENOM) == (CELLSIZE * IDENOM) / 2)
	{
		fc.RT = fc.LT;
		fc.RB = fc.LB;
	}
	if(y % (CELLSIZE * IDENOM) == (CELLSIZE * IDENOM) / 2)
	{
		fc.LB = fc.LT;
		fc.RB = fc.RT;
	}
	return fc;
}
void HTurn(FourCell_t &fc)
{
	m_swap(fc.LT, fc.RT, MCell_t *);
	m_swap(fc.LB, fc.RB, MCell_t *);

	fc.X *= -1;
}
void VTurn(FourCell_t &fc)
{
	m_swap(fc.LT, fc.LB, MCell_t *);
	m_swap(fc.RT, fc.RB, MCell_t *);

	fc.Y *= -1;
}
void Rot180(FourCell_t &fc)
{
	HTurn(fc);
	VTurn(fc);
}

Cell5x5_t GetAroundCell5x5(fPInt_t x, fPInt_t y) // (x, y): キャラの中心
{
	int cx = (x + (CELLSIZE * IDENOM) / 2) / (CELLSIZE * IDENOM);
	int cy = (y + (CELLSIZE * IDENOM) / 2) / (CELLSIZE * IDENOM);

	Cell5x5_t ret;

	for(int sx = 0; sx < 5; sx++)
	for(int sy = 0; sy < 5; sy++)
	{
		int rx = cx + sx - 2;
		int ry = cy + sy - 2;

		ret.Cell[sx][sy] = GDc.Map->RefCell(rx, ry, NULL);

		if(ret.Cell[sx][sy] != NULL)
		{
			ret.IsPlayerWall[sx][sy] = IsPlayerWall(ret.Cell[sx][sy]->CellType);
			ret.IsEnemyWall[sx][sy] = IsEnemyWall(ret.Cell[sx][sy]);
		}
		else
		{
			ret.IsPlayerWall[sx][sy] = 1;
			ret.IsEnemyWall[sx][sy] = 1;
		}
	}
	return ret;
}

void DrawWindowFrame(int frmDerId_00, int l, int t, int w, int h) // frmDerId_00: D_WINDOW_B_00 etc.
{
	errorCase(l < -IMAX || IMAX < l);
	errorCase(t < -IMAX || IMAX < t);
	errorCase(w < 1 || w % 16 != 0);
	errorCase(h < 1 || h % 16 != 0);

	int X_SIZE = w / 16;
	int Y_SIZE = h / 16;

	for(int x = 0; x < X_SIZE; x++)
	for(int y = 0; y < Y_SIZE; y++)
	{
		int derId = 0;

		if(x == 0 && y == 0)
		{
			derId = frmDerId_00;
		}
		else if(x == X_SIZE - 1 && y == 0)
		{
			derId = frmDerId_00 + 3;
		}
		else if(x == 0 && y == Y_SIZE - 1)
		{
			derId = frmDerId_00 + 12;
		}
		else if(x == X_SIZE - 1 && y == Y_SIZE - 1)
		{
			derId = frmDerId_00 + 15;
		}
		else if(x == 0)
		{
			derId = frmDerId_00 + 4 + (1 - y % 2) * 4;
		}
		else if(x == X_SIZE - 1)
		{
			derId = frmDerId_00 + 7 + (1 - y % 2) * 4;
		}
		else if(y == 0)
		{
			derId = frmDerId_00 + 1 + (1 - x % 2) * 1;
		}
		else if(y == Y_SIZE - 1)
		{
			derId = frmDerId_00 + 13 + (1 - x % 2) * 1;
		}

		if(derId)
			DrawSimple(derId | DTP, l + x * 16, t + y * 16);
	}
}

/*
	dr_x: 描画位置(左)
	dr_y: 描画位置(上)
	lt_derId: マップチップ上の左上のD_*
	mc_w: マップチップの幅(横のドット数/16)
	cell: セル
	isWater: 上下左右斜めについて同じ水域であるか判定する。isWater(0, 0)を除くisWater(-1...1, -1...1)の８つについて判定する。
	animeMax: アニメーションのコマ数
	komaFrmMax: アニメーション１コマのフレーム数
*/
void DrawMapChipWater(
	int dr_x,
	int dr_y,
	int lt_derId,
	int mc_w,
	MCell_t *cell,
	int (*isWater)(MCell_t *, int, int),
	int animeMax,
	int komaFrmMax
	)
{
	int n1 = isWater(cell, -1,  1);
	int n2 = isWater(cell,  0,  1);
	int n3 = isWater(cell,  1,  1);
	int n4 = isWater(cell, -1,  0);
	int n6 = isWater(cell,  1,  0);
	int n7 = isWater(cell, -1, -1);
	int n8 = isWater(cell,  0, -1);
	int n9 = isWater(cell,  1, -1);

	i2D_t mc1;
	i2D_t mc3;
	i2D_t mc7;
	i2D_t mc9;

	     if( n2 &&  n4) mc1 = n1 ? makeI2D(2, 3) : makeI2D(2, 1);
	else if( n2 && !n4) mc1 = makeI2D(0, 3);
	else if(!n2 &&  n4) mc1 = makeI2D(2, 5);
	else                mc1 = makeI2D(0, 5);

	     if( n2 &&  n6) mc3 = n3 ? makeI2D(1, 3) : makeI2D(3, 1);
	else if( n2 && !n6) mc3 = makeI2D(3, 3);
	else if(!n2 &&  n6) mc3 = makeI2D(1, 5);
	else                mc3 = makeI2D(3, 5);

	     if( n8 &&  n4) mc7 = n7 ? makeI2D(2, 4) : makeI2D(2, 0);
	else if( n8 && !n4) mc7 = makeI2D(0, 4);
	else if(!n8 &&  n4) mc7 = makeI2D(2, 2);
	else                mc7 = makeI2D(0, 2);

	     if( n8 &&  n6) mc9 = n9 ? makeI2D(1, 4) : makeI2D(3, 0);
	else if( n8 && !n6) mc9 = makeI2D(3, 4);
	else if(!n8 &&  n6) mc9 = makeI2D(1, 2);
	else                mc9 = makeI2D(3, 2);

	int koma = ProcFrame / komaFrmMax;
	int nKoma = koma + 1;
	int frm = ProcFrame % komaFrmMax;

	koma %= animeMax;
	nKoma %= animeMax;

	int i1 = mc1.X + mc1.Y * mc_w;
	int i3 = mc3.X + mc3.Y * mc_w;
	int i7 = mc7.X + mc7.Y * mc_w;
	int i9 = mc9.X + mc9.Y * mc_w;

	DrawSimple(lt_derId + i7 + koma * 4 | DTP, dr_x + 16 * 0, dr_y + 16 * 0);
	DrawSimple(lt_derId + i9 + koma * 4 | DTP, dr_x + 16 * 1, dr_y + 16 * 0);
	DrawSimple(lt_derId + i1 + koma * 4 | DTP, dr_x + 16 * 0, dr_y + 16 * 1);
	DrawSimple(lt_derId + i3 + koma * 4 | DTP, dr_x + 16 * 1, dr_y + 16 * 1);

	if(frm)
	{
		DPE_SetAlpha((double)frm / komaFrmMax);

		DrawSimple(lt_derId + i7 + nKoma * 4 | DTP, dr_x + 16 * 0, dr_y + 16 * 0);
		DrawSimple(lt_derId + i9 + nKoma * 4 | DTP, dr_x + 16 * 1, dr_y + 16 * 0);
		DrawSimple(lt_derId + i1 + nKoma * 4 | DTP, dr_x + 16 * 0, dr_y + 16 * 1);
		DrawSimple(lt_derId + i3 + nKoma * 4 | DTP, dr_x + 16 * 1, dr_y + 16 * 1);

		DPE_Reset();
	}
}
