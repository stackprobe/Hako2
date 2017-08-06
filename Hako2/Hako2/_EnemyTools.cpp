#include "all.h"

int EnemyAnaIsWall;

int IsEnemyWall(MCell_t *cell)
{
	return
		IsPlayerWall(cell->CellType) && cell->CellType != ET_PLAYER_WALL ||
		cell->CellType == CT_DEATH ||
		cell->CellType == CT_GOAL ||
		cell->CellType == ET_ENEMY_WALL ||
		cell->EnemyAnaFlag && EnemyAnaIsWall;
}
void AddEnemyCrash(fPInt_t cent_x, fPInt_t cent_y)
{
	AddEnemyCrash(
		cent_x - (CELLSIZE / 2) * IDENOM,
		cent_y - (CELLSIZE / 2) * IDENOM,
		CELLSIZE * IDENOM,
		CELLSIZE * IDENOM
		);
}
void AddEnemyCrash(Enemy_t *i)
{
	AddEnemyCrash(i->X, i->Y);
}
int IsEnemyInWall(Enemy_t *i)
{
	FourCell_t fc = GetFourCell(i->X, i->Y);

	if(
		fc.LT != NULL &&
		fc.RT != NULL &&
		fc.LB != NULL &&
		fc.RB != NULL
		)
	{
		return
			IsEnemyWall(fc.LT) ||
			IsEnemyWall(fc.RT) ||
			IsEnemyWall(fc.LB) ||
			IsEnemyWall(fc.LB);
	}
	return 1; // 画面外？？？
}
void AdjustEnemyPos(Enemy_t *i)
{
	i->X /= CELLSIZE * IDENOM;
	i->X *= CELLSIZE * IDENOM;
	i->X += (CELLSIZE / 2) * IDENOM;

	i->Y /= CELLSIZE * IDENOM;
	i->Y *= CELLSIZE * IDENOM;
	i->Y += (CELLSIZE / 2) * IDENOM;
}
int GetEnemyOrderWeight(Enemy_t *i)
{
	if(i->EnemyType == ET_PLAYER_WALL)
		return 1;

	return 0;
}

// ---- extra tools ----

void CookieBornEffect(fPInt_t enemy_x, fPInt_t enemy_y, double cR, double cG, double cB, double scale)
{
	if(IsInCamera(m_fpi2i(enemy_x), m_fpi2i(enemy_y), 100))
	{
		for(int c = 0; c < 10; c++)
		{
			double s = 2.0 + 2.0 * dRnd();
			double r = PI * 2.0 * (c / 10.0) + 0.3 * eRnd();

			CEE_SetBright(cR, cG, cB);
			AddCommonEffect(
				Gnd.EL,
				0,
				D_STAR_W_00 + 6 | DTP,
				m_fpi2i(enemy_x),
				m_fpi2i(enemy_y),
				PI * 2.0 * dRnd(),
				0.5 + 0.25 * dRnd() * scale,
				0.8,
				cos(r) * s * scale,
				sin(r) * s * scale,
				0.1 * eRnd(),
				0.0,
				-0.02 / scale
				);
			CEE_Reset();
		}
	}
}
void BulletDeadEffect(fPInt_t enemy_x, fPInt_t enemy_y, double cR, double cG, double cB)
{
	if(IsInCamera(m_fpi2i(enemy_x), m_fpi2i(enemy_y), 100))
	{
		CEE_SetBright(cR, cG, cB);
		AddCommonEffect(
			Gnd.EL,
			0,
			D_STAR_W_00 + 6 | DTP,
			m_fpi2i(enemy_x),
			m_fpi2i(enemy_y),
			PI * 2.0 * dRnd(),
			1.0,
			1.0,
			0.0,
			0.0,
			0.1,
			-0.01,
			-0.02
			);
		CEE_Reset();
	}
}

// ----
