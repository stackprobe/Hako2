typedef struct SubScreen_st
{
	int W;
	int H;
	int AFlag;
	int Handle;
}
SubScreen_t;

SubScreen_t *CreateSubScreen(int w, int h, int aFlag = 0);
void ReleaseSubScreen(SubScreen_t *i);
int GetHandle(SubScreen_t *i);

extern int CurrDrawScreenHandle;

void ChangeDrawScreen(int handle);
void ChangeDrawScreen(SubScreen_t *screen);
void RestoreDrawScreen(void);

i2D_t GetSubScreenSize(SubScreen_t *screen);
i2D_t GetDrawScreenSize(void);
void UnloadAllSubScreenHandle(void);
