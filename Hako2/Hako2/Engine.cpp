#include "all.h"

int IgnoreEscapeKey;

// 他ファイルからは read only {
__int64 FrameStartTime;
int ProcFrame;
int ActFrame; // = 0; ok
int FreezeInputFrame;
int WindowIsActive;
int FrameRateDropCount;
int NoFrameRateDropCount;
// }

static void CheckHz(void)
{
	__int64 currTime = GetCurrTime();
	__int64 diffTime = currTime - FrameStartTime;

	if(diffTime < 15 || 18 < diffTime) // ? frame rate drop
		FrameRateDropCount++;
	else
		NoFrameRateDropCount++;

	FrameStartTime = currTime;
}
int InnerDrawScrHdl = -1;

void EachFrame(void)
{
	if(!SEEachFrame())
	{
		MusicEachFrame();
	}
	Gnd.EL->ExecuteAllTask();
	CurtainEachFrame();

	if(Gnd.MainScreen && CurrDrawScreenHandle == GetHandle(Gnd.MainScreen))
	{
		ChangeDrawScreen(DX_SCREEN_BACK);
		errorCase(DrawExtendGraph(0, 0, Gnd.RealScreen_W, Gnd.RealScreen_H, GetHandle(Gnd.MainScreen), 0)); // ? 失敗
	}

	// DxLib >

	ScreenFlip();

	if(!IgnoreEscapeKey && CheckHitKey(KEY_INPUT_ESCAPE) == 1 || ProcessMessage() == -1)
	{
		EndProc();
	}

	// < DxLib

	CheckHz();

	ProcFrame++;
	errorCase(IMAX < ProcFrame); // 192.9日程度でカンスト
	ActFrame++;
	m_countDown(FreezeInputFrame);
	WindowIsActive = IsWindowActive();

	PadEachFrame();
	KeyEachFrame();
	InputEachFrame();
	MouseEachFrame();

	if(Gnd.RealScreen_W != SCREEN_W || Gnd.RealScreen_H != SCREEN_H)
	{
		if(!Gnd.MainScreen)
			Gnd.MainScreen = CreateSubScreen(SCREEN_W, SCREEN_H);

		ChangeDrawScreen(Gnd.MainScreen);
	}
#if LOG_ENABLED
		clsDx();
#endif
}
void FreezeInput(int frame) // frame: 1 == このフレームのみ, 2 == このフレームと次のフレーム ...
{
	errorCase(frame < 1 || IMAX < frame);
	FreezeInputFrame = frame;
}
