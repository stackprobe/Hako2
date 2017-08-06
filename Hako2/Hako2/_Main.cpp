#include "all.h"

void TestPlay(void)
{
	GameInit();

	GDc.MapBmpFile = strx("..\\..\\Map\\0003.bmp");
	GDc.FloorIndex = 2;
	GDc.EditableFlag = 1;

	GameMain();
	GameFnlz();
}
void ProcMain(void)
{
#if LOG_ENABLED == 0
	Logo();
	MainMenu();
#elif 0 // test
	TestPlay();
#elif 0 // test
	Ending();
#elif 1 // test
	MainMenu();
#endif
}
