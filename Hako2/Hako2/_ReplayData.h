enum
{
	REPBIT_DIR_2,
	REPBIT_DIR_4,
	REPBIT_DIR_6,
	REPBIT_DIR_8,
	REPBIT_A,
	REPBIT_B,
	REPBIT_L,
	REPBIT_R, // reserved
};

#define REPLAY_DATA_MAX (3600 * 60)

autoList<uchar> *GetReplayMapHash(void);
void SetReplayMapHash_x(autoList<uchar> *hash);
autoList<uchar> *GetReplayInput(void);
autoList<uchar> *GetReplayPound(void);
void ClearReplayData(void);
void SwapReplayData(void);

void LoadReplayFile(char *replayFile);
void SaveReplayFile(char *replayFile);
