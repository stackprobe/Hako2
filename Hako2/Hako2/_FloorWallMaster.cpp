#include "all.h"

void FloorWallMasterMain(void)
{
#if LOG_ENABLED
	printfDx("カメラ位置:%d,%d\n", m_d2i(CameraX), m_d2i(CameraY));
#endif

	// デフォルト {

	GDc.KabeDrawFunc = KabeDraw_Default;
	GDc.KabeBr_A = 1.0;
	GDc.KabeBr_R = 1.0;
	GDc.KabeBr_G = 0.7;
	GDc.KabeBr_B = 0.1;

	// }

	if(GDc.FloorIndex == 0)
	{
		SetKabe_01();
	}
	else if(GDc.FloorIndex == 1)
	{
		SetKabe_02();
	}
	else if(GDc.FloorIndex == 2)
	{
		SetKabe_03();
	}
	// ここへ追加..
	else
	{
		error();
	}
}
