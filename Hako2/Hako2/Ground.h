typedef struct Gnd_st
{
	taskList *EL; // EffectList
	int PrimaryPadId; // -1 == 未設定
	SubScreen_t *MainScreen; // NULL == 不使用

	// app -->

	// SaveData -->

	int RealScreen_W;
	int RealScreen_H;

	/*
		音量, 0.0 - 1.0, def: 0.5
	*/
	double MusicVolume;
	double SEVolume;

	struct // 0 - (PAD_BUTTON_MAX - 1), def: SNWPB_*
	{
		int Dir_2;
		int Dir_4;
		int Dir_6;
		int Dir_8;
		int A;
		int B;
		int C;
		int D;
		int E;
		int F;
		int L;
		int R;
		int Pause;
		int Start;
	}
	PadBtnId;

	// app -->

	/*
		値域: 0 ～ FLOOR_NUM

		FLOOR_NUM == all cleared && editable
		FLOOR_NUM != not all cleared && not editable
	*/
	int UnclearedFloorIndex;

	int GraphicalMode; // GM_*
	int RecordingMode;

	int RespawnMax;
}
Gnd_t;

extern Gnd_t Gnd;

void Gnd_INIT(void);
void Gnd_FNLZ(void);

void ImportSaveData(void);
void ExportSaveData(void);
