/*
	ItaFlag == ITA_C
		ItaDeco == ITA_H, ITA_V

	ItaFlag == ITA_HT
*/
enum
{
	// ItaFlag : 3
	ITA_C = 1,
	ITA_HL,
	ITA_H,
	ITA_HR,
	ITA_VT,
	ITA_V,
	ITA_VB,
};

typedef struct MCell_st
{
	int CellType;
	double Bright;
	uint PlayerWallFlag : 1; // ? IsPlayerWall()
	uint ItaFlag : 3;
	uint ItaKind : 1; // “¯‚¶ VH ‚Å‚Í“¯‚¶
	uint DecoFlag : 1; // 5%
	uint CellX : 10;
	uint CellY : 10;
	uint KusaId : 3;
	uint EnemyAnaFlag : 1;
}
MCell_t;

MCell_t *CreateMCell(void);
void ReleaseMCell(MCell_t *i);
