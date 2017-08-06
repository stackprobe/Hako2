#include "all.h"

// ---- STOKER ----

static fPInt_t GSD_DX;
static fPInt_t GSD_DY;

static void GetStokerDir(fPInt_t dx, fPInt_t dy)
{
	if(dx < 0)
	{
		GetStokerDir(-dx, dy);
		GSD_DX *= -1;
		return;
	}
	if(dy < 0)
	{
		GetStokerDir(dx, -dy);
		GSD_DY *= -1;
		return;
	}
	if(dx < dy) // ? 45°より大きい。
	{
		GetStokerDir(dy, dx);

		{
			fPInt_t tmp = GSD_DX;
			GSD_DX = GSD_DY;
			GSD_DY = tmp;
		}
		return;
	}
	if(dx * 2 < dy * 5) // ? 22.5°より大きい。-> 33.75°
	{
		GSD_DX = (fPInt_t)980;
		GSD_DY = (fPInt_t)200;
		return;
	}
	// ? 22.5°より小さい。-> 11.25°
	{
		GSD_DX = (fPInt_t)831;
		GSD_DY = (fPInt_t)555;
		return;
	}
}

static Enemy_t *ISI_Crashed;

static int IsStokerIn(Enemy_t *myself, fPInt_t l, fPInt_t t, fPInt_t r, fPInt_t b)
{
	for(int index = 0; index < GDc.EnemyList->GetCount(); index++)
	{
		Enemy_t *e = GDc.EnemyList->GetElement(index);

		if(e != myself)
		if(m_isRange(e->EnemyType, ET_STOKER_00, ET_STOKER_00 + STOKER_MAX - 1))
		if(m_isRange(e->X, l, r) && m_isRange(e->Y, t, b))
		{
			ISI_Crashed = e;
			return 1;
		}
	}
	return 0;
}

void EachFrameStoker(Enemy_t *i, int level)
{
	EnemyAnaIsWall = 1;
	Cell5x5_t cell5x5 = GetAroundCell5x5(i->X, i->Y);
	EnemyAnaIsWall = 0; // reset

	int wallNear2 =
		m_01(cell5x5.IsEnemyWall[1][3]) +
		m_01(cell5x5.IsEnemyWall[2][3]) +
		m_01(cell5x5.IsEnemyWall[3][3]);
	int wallNear4 =
		m_01(cell5x5.IsEnemyWall[1][1]) +
		m_01(cell5x5.IsEnemyWall[1][2]) +
		m_01(cell5x5.IsEnemyWall[1][3]);
	int wallNear6 =
		m_01(cell5x5.IsEnemyWall[3][1]) +
		m_01(cell5x5.IsEnemyWall[3][2]) +
		m_01(cell5x5.IsEnemyWall[3][3]);
	int wallNear8 =
		m_01(cell5x5.IsEnemyWall[1][1]) +
		m_01(cell5x5.IsEnemyWall[2][1]) +
		m_01(cell5x5.IsEnemyWall[3][1]);

	int wallFar2 =
		m_01(cell5x5.IsEnemyWall[0][4]) +
		m_01(cell5x5.IsEnemyWall[1][4]) +
		m_01(cell5x5.IsEnemyWall[2][4]) +
		m_01(cell5x5.IsEnemyWall[3][4]) +
		m_01(cell5x5.IsEnemyWall[4][4]);
	int wallFar4 =
		m_01(cell5x5.IsEnemyWall[0][0]) +
		m_01(cell5x5.IsEnemyWall[0][1]) +
		m_01(cell5x5.IsEnemyWall[0][2]) +
		m_01(cell5x5.IsEnemyWall[0][3]) +
		m_01(cell5x5.IsEnemyWall[0][4]);
	int wallFar6 =
		m_01(cell5x5.IsEnemyWall[4][0]) +
		m_01(cell5x5.IsEnemyWall[4][1]) +
		m_01(cell5x5.IsEnemyWall[4][2]) +
		m_01(cell5x5.IsEnemyWall[4][3]) +
		m_01(cell5x5.IsEnemyWall[4][4]);
	int wallFar8 =
		m_01(cell5x5.IsEnemyWall[0][0]) +
		m_01(cell5x5.IsEnemyWall[1][0]) +
		m_01(cell5x5.IsEnemyWall[2][0]) +
		m_01(cell5x5.IsEnemyWall[3][0]) +
		m_01(cell5x5.IsEnemyWall[4][0]);

	// Wall_相殺
	{
		while(wallNear2 && wallNear8)
		{
			wallNear2--;
			wallNear8--;
		}
		while(wallNear4 && wallNear6)
		{
			wallNear4--;
			wallNear6--;
		}

		while(wallFar2 && wallFar8)
		{
			wallFar2--;
			wallFar8--;
		}
		while(wallFar4 && wallFar6)
		{
			wallFar4--;
			wallFar6--;
		}
	}

	{
		fPInt_t plx = GDc.Player.IX * IDENOM;
		fPInt_t ply = GDc.Player.IY * IDENOM;

		fPInt_t dx = plx - i->X;
		fPInt_t dy = ply - i->Y;

		GetStokerDir(dx, dy);
		dx = GSD_DX;
		dy = GSD_DY;

		dx /= (STOKER_MAX - level) * 3;
		dy /= (STOKER_MAX - level) * 3;

		// WallEffect
		{
			const int NEAR_MUL = 2;
			const int FAR_DIV = 1;

			if(dx < 0) // 左へ
			{
				if(wallNear4)
				{
					dx *= -1;
					dx *= NEAR_MUL;
					goto endWallEffect_X;
				}
				if(wallFar4)
				{
					dx *= -1;
					dx /= FAR_DIV;
					goto endWallEffect_X;
				}
			}
			if(0 < dx) // 右へ
			{
				if(wallNear6)
				{
					dx *= -1;
					dx *= NEAR_MUL;
					goto endWallEffect_X;
				}
				if(wallFar6)
				{
					dx *= -1;
					dx /= FAR_DIV;
					goto endWallEffect_X;
				}
			}
endWallEffect_X:

			if(dy < 0) // 上へ
			{
				if(wallNear8)
				{
					dy *= -1;
					dy *= NEAR_MUL;
					goto endWallEffect_Y;
				}
				if(wallFar8)
				{
					dy *= -1;
					dy /= FAR_DIV;
					goto endWallEffect_Y;
				}
			}
			if(0 < dy) // 下へ
			{
				if(wallNear2)
				{
					dy *= -1;
					dy *= NEAR_MUL;
					goto endWallEffect_Y;
				}
				if(wallFar2)
				{
					dy *= -1;
					dy /= FAR_DIV;
					goto endWallEffect_Y;
				}
			}
endWallEffect_Y:;
		}

		// プレイヤーに指向して加速
		{
			i->u.Stoker.XAdd += dx;
			i->u.Stoker.YAdd += dy;
		}
	}

//	printfDx("%d %d\n", i->u.Stoker.XAdd, i->u.Stoker.YAdd); // test

	// 減速
	{
		i->u.Stoker.XAdd *= 200;
		i->u.Stoker.YAdd *= 200;

		i->u.Stoker.XAdd /= 201;
		i->u.Stoker.YAdd /= 201;

		const fPInt_t SPEED_LIMIT = 25000;

		m_range(i->u.Stoker.XAdd, -SPEED_LIMIT, SPEED_LIMIT);
		m_range(i->u.Stoker.YAdd, -SPEED_LIMIT, SPEED_LIMIT);
	}

	i->X += i->u.Stoker.XAdd;
	i->Y += i->u.Stoker.YAdd;

#if 1 // 跳ね返り
	EnemyAnaIsWall = 1;
	FourCell_t fc = GetFourCell(i->X, i->Y);
	EnemyAnaIsWall = 0; // reset

	if(
		fc.LT != NULL &&
		fc.RT != NULL &&
		fc.LB != NULL &&
		fc.RB != NULL
		)
	{
		int ltw = IsEnemyWall(fc.LT);
		int rtw = IsEnemyWall(fc.RT);
		int lbw = IsEnemyWall(fc.LB);
		int rbw = IsEnemyWall(fc.RB);

//		const fPInt_t K_RATE = 1010 + 10 * (STOKER_MAX - level);
		const fPInt_t K_RATE = 1000;
//		const fPInt_t K_RATE = 500;
		const fPInt_t HOSEI = 2000;

#if 0 // stoker同士の衝突
		if(!ltw && !rtw && !lbw && !rbw) // ? 空中
		{
			const fPInt_t SPAN = CELLSIZE * IDENOM;

			ISI_Crashed = NULL;

			if(IsStokerIn(i, i->X - SPAN, i->Y - SPAN, i->X,        i->Y        )) ltw = 1;
			if(IsStokerIn(i, i->X,        i->Y - SPAN, i->X + SPAN, i->Y        )) rtw = 1;
			if(IsStokerIn(i, i->X - SPAN, i->Y,        i->X,        i->Y + SPAN )) lbw = 1;
			if(IsStokerIn(i, i->X,        i->Y,        i->X + SPAN, i->Y + SPAN )) rbw = 1;

			if(ISI_Crashed) // 超加速防止！
			{
				K_RATE = 1000;

				AvgAbs(i->u.Stoker.XAdd, ISI_Crashed->u.Stoker.XAdd);
				AvgAbs(i->u.Stoker.YAdd, ISI_Crashed->u.Stoker.YAdd);
				/*
				SwapAbs(i->u.Stoker.XAdd, ISI_Crashed->u.Stoker.XAdd);
				SwapAbs(i->u.Stoker.YAdd, ISI_Crashed->u.Stoker.YAdd);
				*/
				/*
				i->u.Stoker.XAdd /= 2;
				i->u.Stoker.YAdd /= 2;
				ISI_Crashed->u.Stoker.XAdd /= 2;
				ISI_Crashed->u.Stoker.YAdd /= 2;
				*/
				/*
				i->u.Stoker.XAdd = GetAbsMin(i->u.Stoker.XAdd, ISI_Crashed->u.Stoker.XAdd);
				i->u.Stoker.YAdd = GetAbsMin(i->u.Stoker.YAdd, ISI_Crashed->u.Stoker.YAdd);
				*/

//				HOSEI = 0; // 補正ナシ！
			}
		}
#endif
		if(ltw && !rtw || lbw && !rbw) // ? 左に壁
		{
			if(i->u.Stoker.XAdd < 0) // ? 左に移動中
			{
				i->u.Stoker.XAdd = abs(i->u.Stoker.XAdd) * K_RATE / IDENOM;
			}
			i->X += HOSEI;
		}
		if(!ltw && rtw || !lbw && rbw) // ? 右に壁
		{
			if(0 < i->u.Stoker.XAdd) // ? 右に移動中
			{
				i->u.Stoker.XAdd = -abs(i->u.Stoker.XAdd) * K_RATE / IDENOM;
			}
			i->X -= HOSEI;
		}
		if(ltw && !lbw || rtw && !rbw) // ? 上に壁
		{
			if(i->u.Stoker.YAdd < 0) // ? 上に移動中
			{
				i->u.Stoker.YAdd = abs(i->u.Stoker.YAdd) * K_RATE / IDENOM;
			}
			i->Y += HOSEI;
		}
		if(!ltw && lbw || !rtw && rbw) // ? 下に壁
		{
			if(0 < i->u.Stoker.YAdd) // ? 下に移動中
			{
				i->u.Stoker.YAdd = -abs(i->u.Stoker.YAdd) * K_RATE / IDENOM;
			}
			i->Y -= HOSEI;
		}
	}
#endif

	AddEnemyCrash(i);
}
void DrawStoker(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	DPE_SetBright(0.8, 0.8, 0.2);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
	DPE_Reset();
}

// ---- GUNNER ----

int GetGunnerDir(int cellType)
{
	return ((cellType - ET_GUNNER_00) % 4 + 1) * 2;
}
int Gunner_IsFastBullet(int cellType)
{
	return 4 <= (cellType - ET_GUNNER_00) % 8;
}
int Gunner_IsRapidFire(int cellType)
{
	return 8 <= (cellType - ET_GUNNER_00);
}

void EachFrameGunner(Enemy_t *i)
{
	int period = 180;

	if(Gunner_IsRapidFire(i->EnemyType))
		period /= 2;

	if(!(i->Frame % period))
	{
		GDc.EnemyList_Add->AddElement(CreateEnemy(
			ET_BULLET_00 + (i->EnemyType - ET_GUNNER_00) % 8,
			i->X,
			i->Y
			));
	}
}
void DrawGunner(Enemy_t *i)
{
	// noop
}

// ---- BULLET ----

int GetBulletDir(int cellType)
{
	return ((cellType - ET_BULLET_00) % 4 + 1) * 2;
}
int IsFastBullet(int cellType)
{
	return 4 <= (cellType - ET_BULLET_00) % 8;
}

void EachFrameBullet(Enemy_t *i)
{
	int dirX;
	int dirY;

	switch(GetBulletDir(i->EnemyType))
	{
	case 2: dirX =  0; dirY =  1; break;
	case 4: dirX = -1; dirY =  0; break;
	case 6: dirX =  1; dirY =  0; break;
	case 8: dirX =  0; dirY = -1; break;

	default:
		error();
	}
	fPInt_t speed = 2100;

	if(IsFastBullet(i->EnemyType))
		speed *= 2;

	if(!i->Frame) // ? 初回
		speed = CELLSIZE * IDENOM;

	fPInt_t addX = dirX * speed;
	fPInt_t addY = dirY * speed;

	i->X += addX;
	i->Y += addY;

	i->Death = IsEnemyInWall(i);

	/*
		★★★Deathったら AddEnemyCrash しないこと！★★★
		-> Deathったフレームでプレイヤーが死亡すると、死亡期間中この敵は表示されなくなる。
			<- Deathったフレームで、この敵が削除されるため。
	*/
	if(!i->Death)
		AddEnemyCrash(i);

	if(!i->Frame) // ? 初回
		CookieBornEffect(i->X, i->Y, 1.0, 0.5, 1.0, 0.7);

	if(i->Death)
		BulletDeadEffect(i->X, i->Y, 1.0, 0.5, 1.0);
}
void DrawBullet(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	DPE_SetBright(1.0, 0.5, 1.0);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
	DPE_Reset();
}

// ---- DAIKAITEN ----

int GetDaikaitenRotStartPos(int cellType)
{
	return ((cellType - ET_DAIKAITEN_00) % 8) * (IDENOM / 2);
}
int GetDaikaitenRotDir(int cellType) // ret: 1 == 時計回り, -1 == 反時計回り
{
	return (cellType - ET_DAIKAITEN_00) / 8 ? 1 : -1;
}

void EachFrameDaikaiten(Enemy_t *i)
{
	const fPInt_t ISSHUU = 4000;
	const fPInt_t ROT_SPEED = 15;
	const fPInt_t ROT_HANKEI = 200000;
	const fPInt_t SATELLITE_ROT_SPEED = 50;
	const fPInt_t SATELLITE_ROT_HANKEI = 100000;

	if(!i->Frame) // init
	{
		i->u.Daikaiten.R = GetDaikaitenRotStartPos(i->EnemyType);
		i->u.Daikaiten.RAdd = GetDaikaitenRotDir(i->EnemyType) * ROT_SPEED;

		for(int index = 0; index < DAIKAITEN_SATELLITE_MAX; index++)
		{
			i->u.Daikaiten.Satellites[index].R = (ISSHUU / DAIKAITEN_SATELLITE_MAX) * index;
			i->u.Daikaiten.Satellites[index].RAdd = GetDaikaitenRotDir(i->EnemyType) * SATELLITE_ROT_SPEED;
		}
	}

	i->u.Daikaiten.R += i->u.Daikaiten.RAdd;

	if(i->u.Daikaiten.R < 0)      i->u.Daikaiten.R += ISSHUU;
	if(ISSHUU < i->u.Daikaiten.R) i->u.Daikaiten.R -= ISSHUU;

	i->u.Daikaiten.AxisX = iCos(i->u.Daikaiten.R, ROT_HANKEI);
	i->u.Daikaiten.AxisY = iSin(i->u.Daikaiten.R, ROT_HANKEI);

	// 当たり判定 {

	fPInt_t x = i->X + i->u.Daikaiten.AxisX;
	fPInt_t y = i->Y + i->u.Daikaiten.AxisY;

	AddEnemyCrash(x, y);

	// }

	if(!i->Frame) // init
		CookieBornEffect(x, y, 0.2, 1.0, 0.5);

	for(int index = 0; index < DAIKAITEN_SATELLITE_MAX; index++)
	{
		i->u.Daikaiten.Satellites[index].R += i->u.Daikaiten.Satellites[index].RAdd;

		if(i->u.Daikaiten.Satellites[index].R < 0)      i->u.Daikaiten.Satellites[index].R += ISSHUU;
		if(ISSHUU < i->u.Daikaiten.Satellites[index].R) i->u.Daikaiten.Satellites[index].R -= ISSHUU;

		i->u.Daikaiten.Satellites[index].AxisX = iCos(i->u.Daikaiten.Satellites[index].R, SATELLITE_ROT_HANKEI);
		i->u.Daikaiten.Satellites[index].AxisY = iSin(i->u.Daikaiten.Satellites[index].R, SATELLITE_ROT_HANKEI);

		// satellite_当たり判定
		{
			fPInt_t sate_x = x + i->u.Daikaiten.Satellites[index].AxisX;
			fPInt_t sate_y = y + i->u.Daikaiten.Satellites[index].AxisY;

			AddEnemyCrash(sate_x, sate_y);

			if(!i->Frame) // init
				CookieBornEffect(sate_x, sate_y, 0.2, 1.0, 0.5);
		}
	}
}
void DrawDaikaiten(Enemy_t *i)
{
	fPInt_t x = i->X + i->u.Daikaiten.AxisX;
	fPInt_t y = i->Y + i->u.Daikaiten.AxisY;

	i2D_t pos = GetScreenCellPos(x, y);

	DPE_SetBright(0.2, 1.0, 0.5);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
//	DPE_Reset();

	for(int index = 0; index < DAIKAITEN_SATELLITE_MAX; index++)
	{
		fPInt_t sate_x = x + i->u.Daikaiten.Satellites[index].AxisX;
		fPInt_t sate_y = y + i->u.Daikaiten.Satellites[index].AxisY;

		i2D_t sate_pos = GetScreenCellPos(sate_x, sate_y);

//		DPE_SetBright(0.2, 1.0, 0.5);
		DrawRect(P_WHITEBOX, sate_pos.X, sate_pos.Y, CELLSIZE, CELLSIZE);
//		DPE_Reset();
	}
	DPE_Reset(); // まとめて
}

// ----
