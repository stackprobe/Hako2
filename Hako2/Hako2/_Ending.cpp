#include "all.h"

static Random *Rnd;

static void DrawWall(void)
{
	int epi = EndingPicIndex;

	DrawCurtain();
//	DrawRect(P_ENDING, 0, 0, SCREEN_W, SCREEN_H);
	DrawSimple(P_ENDING, 0, 0);
//	DrawSimple(P_ENDING + epi, 0, 0);
}
void Ending(void)
{
	Rnd = new Random((int)time(NULL));

	// <-- init

	SetCurtain(0, -1.0);
	ActFrame = 0;

	forscene(60)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	MusicPlay(MUS_ENDING);

	forscene(60)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	SetCurtain(240);

	for(; ; )
	{
		if(420 < ActFrame && (GetInput(INP_A) || GetInput(INP_B)))
		{
			break;
		}
		DrawWall();
		EachFrame();
	}
	MusicFade(300);
	SetCurtain(300, -1.0);

	forscene(420)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	Gnd.EL->Clear(); // ¯‚ðÁ‚³‚È‚¢‚ÆŽc‚Á‚¿‚á‚¤‚æB

	// fnlz -->

	delete Rnd;
}
