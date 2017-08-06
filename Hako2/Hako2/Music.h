enum
{
	MUS_DUMMY,

	// app >

	MUS_TITLE,
	MUS_FLOOR_01,
	MUS_ENDING,

	// < app

	MUS_MAX, // num of member
};

typedef struct MusicInfo_st
{
	int Handle;
	double Volume; // 0.0 - 1.0, def: 0.5
}
MusicInfo_t;

extern MusicInfo_t *CurrDestMusic;
extern double CurrDestMusicVolumeRate;

void MusicEachFrame(void);
void MusicPlay(int musId, int once_mode = 0, int resume_mode = 0, double volumeRate = 1.0, int fadeFrameMax = 30);
void MusicFade(int frameMax = 30, double destVRate = 0.0, double startVRate = CurrDestMusicVolumeRate);
void MusicStop(void);
void MusicTouch(int musId);
void UpdateMusicVolume(void);
