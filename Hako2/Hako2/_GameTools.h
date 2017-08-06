typedef struct FourCell_st
{
	MCell_t *LT;
	MCell_t *RT;
	MCell_t *LB;
	MCell_t *RB;

	/*
		LT, RT, LB, RB の中心からの差分
	*/
	fPInt_t X;
	fPInt_t Y;

	/*
		LT のセル座標
	*/
	int LTCellX;
	int LTCellY;
}
FourCell_t;

typedef struct Cell5x5_st
{
	/*
		[2][2] == 中心
	*/
	MCell_t *Cell[5][5];
	int IsPlayerWall[5][5];
	int IsEnemyWall[5][5];
}
Cell5x5_t;

i2D_t GetScreenCellPos(fPInt_t x, fPInt_t y);

FourCell_t GetFourCell(fPInt_t x, fPInt_t y);
void HTurn(FourCell_t &fc);
void VTurn(FourCell_t &fc);
void Rot180(FourCell_t &fc);

Cell5x5_t GetAroundCell5x5(fPInt_t x, fPInt_t y);

void DrawWindowFrame(int frmDerId_00, int l, int t, int w, int h);

void DrawMapChipWater(
	int dr_x,
	int dr_y,
	int lt_derId,
	int mc_w,
	MCell_t *cell,
	int (*isWater)(MCell_t *, int, int),
	int animeMax = 1,
	int komaFrmMax = 1
	);
