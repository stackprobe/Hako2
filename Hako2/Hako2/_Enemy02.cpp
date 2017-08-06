#include "all.h"

// ---- TRAP ----

void EachFrameTrap(Enemy_t *i)
{
	int kind = i->EnemyType - ET_TRAP_00;
	int speedLv = kind % 4;
	int modeYoko = kind / 4;

	fPInt_t speed;

	switch(speedLv)
	{
	case 0: speed = 3200; break;
	case 1: speed = 4800; break;
	case 2: speed = 6400; break;
	case 3: speed = 8000; break;

	default:
		error();
	}

	if(modeYoko) // Yoko
	{
		if(abs(GDc.Player.Y - i->Y) < CELLSIZE * IDENOM)
		{
			if(GDc.Player.X < i->X)
				i->X -= speed;
			else
				i->X += speed;
		}
	}
	else // Tate
	{
		if(abs(GDc.Player.X - i->X) < CELLSIZE * IDENOM)
		{
			if(GDc.Player.Y < i->Y)
				i->Y -= speed;
			else
				i->Y += speed;
		}
	}
	if(IsEnemyInWall(i))
	{
		AdjustEnemyPos(i);
	}
	AddEnemyCrash(i);
}
void DrawTrap(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	DPE_SetBright(0.8, 1.0, 0.6);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
//	DPE_Reset();

	int kind = i->EnemyType - ET_TRAP_00;
	int speedLv = kind % 4;
	int modeYoko = kind / 4;

	const int MARGIN = 4;
	const int INN_MARGIN = 12 - speedLv;

	DPE_SetBright(0.6, 0.8, 0.4);
	DrawRect(P_WHITEBOX, pos.X + MARGIN, pos.Y + MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - MARGIN * 2);
//	DPE_Reset();

	DPE_SetBright(0.4, 0.6, 0.2);

	if(modeYoko) // Yoko
		DrawRect(P_WHITEBOX, pos.X + MARGIN, pos.Y + INN_MARGIN, CELLSIZE - MARGIN * 2, CELLSIZE - INN_MARGIN * 2);
	else // Tate
		DrawRect(P_WHITEBOX, pos.X + INN_MARGIN, pos.Y + MARGIN, CELLSIZE - INN_MARGIN * 2, CELLSIZE - MARGIN * 2);

	DPE_Reset();
}

// ---- ASHIBA ----

int Ashiba_Disabled;

int GetAshibaFrame(int kind)
{
	errorCase(!m_isRange(kind, 0, 7));

	int frm = GDc.PlayFrame;
	frm += 240;
	frm -= 30 * kind;
	frm %= 240;

	return frm;
}
int AshibaIsActive(int kind)
{
	return !Ashiba_Disabled && GetAshibaFrame(kind) < 115;
}

void EachFrameAshiba(Enemy_t *i)
{
	if(i->Frame != GDc.PlayFrame) // 2bs ていうか TEST
	{
		printfDx("iF, PF: %d, %d\n", i->Frame, GDc.PlayFrame);
		error();
	}

	/*
		カメラ位置(double)基準なので、鳴る・鳴らないの境界もぼかす。
	*/
	if(GetAshibaFrame(i->EnemyType - ET_ASHIBA_00) == 0)
	{
		double scrX = (double)i->X / IDENOM - CameraX;
		double scrY = (double)i->Y / IDENOM - CameraY;

		double farX;
		double farY;

		if(scrX < 0.0) farX = -scrX;
		else if(scrX < SCREEN_W) farX = 0.0;
		else farX = scrX - SCREEN_W;

		if(scrY < 0.0) farY = -scrY;
		else if(scrY < SCREEN_H) farY = 0.0;
		else farY = scrY - SCREEN_H;

		const double FAR_MAX = 900.0;

		if(farX < FAR_MAX && farY < FAR_MAX)
		{
			double farXY = m_max(farX, farY);
			farXY /= FAR_MAX;
			farXY *= 100.0;
			int pct = 100 - m_d2i(farXY);

			m_range(pct, 0, 100); // 2bs

			GDc.SoundAshibaActiveFlag = 1;
			GDc.SoundAshibaActiveVolPct = m_max(GDc.SoundAshibaActiveVolPct, pct);
		}
	}
}
void DrawAshiba(Enemy_t *i)
{
	// noop
}

// ---- ENEMY_WALL ----

void EachFrameEnemyWall(Enemy_t *i)
{
	// noop
}
void DrawEnemyWall(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	DPE_SetBright(0.8, 0.8, 0.0);
	DPE_SetAlpha(0.2);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
	DrawRect(P_WHITEBOX, pos.X + 10, pos.Y, CELLSIZE - 20, CELLSIZE);
	DrawRect(P_WHITEBOX, pos.X, pos.Y + 10, CELLSIZE, CELLSIZE - 20);
	DPE_Reset();
}

// ---- PLAYER_WALL ----

void EachFramePlayerWall(Enemy_t *i)
{
	// noop
}
void DrawPlayerWall(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	DPE_SetBright(0.0, 0.8, 0.8);
	DPE_SetAlpha(0.2);
	DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
	DrawRect(P_WHITEBOX, pos.X + 10, pos.Y, CELLSIZE - 20, CELLSIZE);
	DrawRect(P_WHITEBOX, pos.X, pos.Y + 10, CELLSIZE, CELLSIZE - 20);
	DPE_Reset();
}

// ----
