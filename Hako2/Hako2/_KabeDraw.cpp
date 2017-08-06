#include "all.h"

void KabeDraw_Default(double a)
{
	const int W = 300;
	const int H = 400;
	int sx = m_d2i(CameraX / -10.0);
	int sy = m_d2i(CameraY / -10.0);
	sx %= W;
	sy %= H;

	DPE_SetBright(0.0, 0.3, 0.3);
	DPE_SetAlpha(a);

	for(int x = sx; x < SCREEN_W; x += W)
	for(int y = sy; y < SCREEN_H; y += H)
	{
		DrawRect(P_WALL_FLOOR_DEFAULT, x, y, W, H);
	}
	DPE_Reset();
}

// ---- 02 ----
/*
	0 - 3040, 0 - 2280
*/

/*
	下
*/
static void KabeDraw_01(double a)
{
	double x = CameraX / 3040.0;
	double y = CameraY / 2280.0;
	m_rate(x);
	m_rate(y);
	x = x * -100.0 - 50.0;
	y = y * -100.0 - 50.0;
	double w = SCREEN_W + 200.0;
	double h = SCREEN_H + 200.0;

	DPE_SetBright(0.0, 0.3, 0.0);
	DPE_SetAlpha(a);
	DrawRect(P_WALL_FLOOR_01, x, y, w, h);
	DPE_Reset();
}
/*
	中
*/
static void KabeDraw_01B(double a)
{
	double x = CameraX / 3040.0;
	double y = CameraY / 2280.0;
	m_rate(x);
	m_rate(y);
	x = x * -100.0 - 50.0;
	y = y * -100.0 - 50.0;
	double w = SCREEN_W + 200.0;
	double h = SCREEN_H + 200.0;

	DPE_SetBright(0.2, 0.2, 0.2);
	DPE_SetAlpha(a);
	DrawRect(P_WALL_FLOOR_01B, x, y, w, h);
	DPE_Reset();
}
/*
	上
*/
static void KabeDraw_01C(double a)
{
	double x = CameraX / 3040.0;
	double y = CameraY / 2280.0;
	m_rate(x);
	m_rate(y);
	x = x * -100.0 - 50.0;
	y = y * -100.0 - 50.0;
	double w = SCREEN_W + 200.0;
	double h = SCREEN_H + 200.0;

	DPE_SetBright(0.45, 0.45, 0.45);
	DPE_SetAlpha(a);
	DrawRect(P_WALL_FLOOR_01C, x, y, w, h);
	DPE_Reset();
}
void SetKabe_01(void)
{
	// touch
	{
		Pic(P_WALL_FLOOR_01);
		Pic(P_WALL_FLOOR_01B);
		Pic(P_WALL_FLOOR_01C);
	}

	if(CameraY < 1300.0)
	{
		GDc.KabeDrawFunc = KabeDraw_01C;
		GDc.KabeBr_A = 0.8;
		GDc.KabeBr_R = 0.1;
		GDc.KabeBr_G = 0.15;
		GDc.KabeBr_B = 0.2;
	}
	else if(CameraY < 2600.0)
	{
		GDc.KabeDrawFunc = KabeDraw_01B;
		GDc.KabeBr_R = 0.8;
		GDc.KabeBr_G = 1.0;
		GDc.KabeBr_B = 0.6;
	}
	else
	{
		GDc.KabeDrawFunc = KabeDraw_01;
		GDc.KabeBr_R = 1.0;
		GDc.KabeBr_G = 0.7;
		GDc.KabeBr_B = 0.1;
	}
}

// ---- 02 ----
/*
	0 - 9000, 0 - 5600
*/

static void KabeDraw_02(double a)
{
	/*
		カメラ位置レート (x, y) == 0.0 - 1.0
	*/
	double x = CameraX / 9000.0;
	double y = CameraY / 5600.0;

	m_rate(x);
	m_rate(y);

	x *= 0.98;
	y *= 0.98;
	x += 0.01;
	y += 0.01;

	/*
		画像の表示サイズ(画像サイズでなくても良い)
	*/
	double w = 880.0;
	double h = 660.0;

	x *= w - SCREEN_W;
	y *= h - SCREEN_H;

	DPE_SetBright(0.4, 0.4, 0.4);
	DPE_SetAlpha(a);

	{
		const int ANIME_KOMA = 10;

		int c = ProcFrame / ANIME_KOMA;
		c %= FLOOR_02_WALL_MAX;
		int d = c + 1;
		d %= FLOOR_02_WALL_MAX;
		int e = ProcFrame % ANIME_KOMA;

		DrawRect(P_WALL_FLOOR_02_00 + c, -x, -y, w, h);
		DPE_SetAlpha((double)e / ANIME_KOMA);
		DrawRect(P_WALL_FLOOR_02_00 + d, -x, -y, w, h);
		DPE_Reset();
	}

	DPE_Reset();
}
void SetKabe_02(void)
{
	// touch
	{
		for(int c = 0; c < FLOOR_02_WALL_MAX; c++)
			Pic(P_WALL_FLOOR_02_00 + c);
	}

	GDc.KabeDrawFunc = KabeDraw_02;
#if 1
	GDc.KabeBr_A = 0.9;
	GDc.KabeBr_R = 0.075;
	GDc.KabeBr_G = 0.15;
	GDc.KabeBr_B = 0.225;
#else // old
	GDc.KabeBr_R = 0.1;
	GDc.KabeBr_G = 0.2;
	GDc.KabeBr_B = 0.3;
#endif
}

// ---- 03 ----
/*
	0 - 24800, 0 - 360
*/

static void KabeDraw_03(double a)
{
	static double RealY = 0.0;
	double x = CameraX;
	double y = CameraY;
	double r = x;

	/*
		想定マップサイズ
	*/
	m_range(x, 0.0, 24800.0);
	m_range(y, 0.0, 360.0);

	{
		double xRate = x / 24800.0;
		double yRate = y / 360.0;

		x = xRate;
		y = (1.0 - xRate) * 0.9 + yRate * 0.1;
	}

	x *= 160.0;
	y *= 120.0;

	m_range(x, 0.0, 160.0);
	m_range(y, 0.0, 120.0);

	RealY = y;
//	m_approach(RealY, y, 0.5);

	if(r < 12400.0)
	{
		r /= 12400.0;

		DPE_SetBright(
			0.5,
			0.5,
			0.5
			);

		r = 1.0 - r;

		DrawRect(P_WALL_FLOOR_03B, -x, -RealY, SCREEN_W + 160.0, SCREEN_H + 120.0);
		DPE_SetAlpha(r);
		DrawRect(P_WALL_FLOOR_03,  -x, -RealY, SCREEN_W + 160.0, SCREEN_H + 120.0);
	}
	else
	{
		r -= 12400.0;
		r /= 12400.0;

		DPE_SetBright(
			0.5 - r * r * 0.3,
			0.5 - r * r * 0.3,
			0.5 - r * r * 0.3
			);

		r = 1.0 - r;

		DrawRect(P_WALL_FLOOR_03C, -x, -RealY, SCREEN_W + 160.0, SCREEN_H + 120.0);
		DPE_SetAlpha(r);
		DrawRect(P_WALL_FLOOR_03B, -x, -RealY, SCREEN_W + 160.0, SCREEN_H + 120.0);
	}
	DPE_Reset();
}
void SetKabe_03(void)
{
	// touch
	{
		Pic(P_WALL_FLOOR_03);
		Pic(P_WALL_FLOOR_03B);
		Pic(P_WALL_FLOOR_03C);
	}

	GDc.KabeDrawFunc = KabeDraw_03;
//	GDc.KabeBr_A = 1.0; // これデフォ..

	double r = CameraX / 24800.0;

	m_rate(r);

	r *= 100.0;
	r = (double)m_d2i(r);
	r /= 100.0;

	if(r < 0.5)
	{
		r *= 2.0;
		m_range(r, 0.0, 1.0);

		GDc.KabeBr_R = 0.8 - r * 0.7;
		GDc.KabeBr_G = 0.4 - r * 0.2;
		GDc.KabeBr_B = 0.2 + r * 0.2;
		/*
		GDc.KabeBr_R = 0.8 - r * 0.6;
		GDc.KabeBr_G = 0.4 + r * 0.1;
		GDc.KabeBr_B = 0.2 + r * 0.6;
		*/
	}
	else
	{
		r -= 0.5;
		r *= 2.0;
		m_range(r, 0.0, 1.0);

		GDc.KabeBr_R = 0.1 + r * 0.5;
		GDc.KabeBr_G = 0.2 + r * 0.6;
		GDc.KabeBr_B = 0.4 + r * 0.6;
		/*
		GDc.KabeBr_R = 0.1 + r * 0.1;
		GDc.KabeBr_G = 0.2 + r * 0.6;
		GDc.KabeBr_B = 0.4 + r * 0.2;
		*/
		/*
		GDc.KabeBr_R = 0.2;
		GDc.KabeBr_G = 0.5 + r * 0.3;
		GDc.KabeBr_B = 0.8 - r * 0.3;
		*/
	}
}
