extern int IgnoreEscapeKey;

extern __int64 FrameStartTime;
extern int ProcFrame;
extern int ActFrame;
extern int FreezeInputFrame;
extern int WindowIsActive;
extern int FrameRateDropCount;
extern int NoFrameRateDropCount;

extern int InnerDrawScrHdl;

void EachFrame(void);
void FreezeInput(int frame = 1);
