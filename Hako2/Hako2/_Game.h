/*
	zd == zero is disabled
*/

typedef struct GameInput_st
{
	int Dir2;
	int Dir4;
	int Dir6;
	int Dir8;
	int A;
	int B;
	int L;
	int R; // reserved
}
GameInput_t;

typedef struct GDc_st
{
	// Params {

	int FloorIndex; // 0 - (FLOOR_NUM - 1)

	/*
		�ǂꂩ�P�͗L���ł��邱�ƁB
	*/
	char *MapBmpFile; // bind
	autoList<uchar> *MapBmpImage; // bind
	autoTable<uint> *MapBmp; // bind

	int MapBmpImageUnreleaseFlag;
	int EditableFlag;
	int ReplayMode;

	// }

	Random *Rnd;
//	Random *PlayRnd; // 1�v���C�ɓd���p�^���̂��闐��, �t���A�J�n�E���X�|�[�����ɏ���������B�G�t�F�N�g�p

	autoTable<MCell_t *> *Map;

	int Field_W;
	int Field_H;

	autoList<Enemy_t *> *EnemyList;
	autoList<Enemy_t *> *EnemyList_Add;
	autoList<Enemy_t *> *EnemyList_Saved;
	int PlayFrame_Saved;
	int PlayerMovement_Saved[6];

	int CameraTargetX;
	int CameraTargetY;
	int ICameraX;
	int ICameraY;

	struct
	{
		/*
			�Z���^�[���W
		*/
		fPInt_t X;
		fPInt_t Y;
		fPInt_t XAdd;
		fPInt_t YAdd;
		int IX;
		int IY;

		int BornFrame; // zd
		int DeadFrame; // zd
		int FloorClearFrame; // zd

		int WalkFrame; // zd
		int SlowFrame; // zd

		// PlayerWallProc() ret {
		int OnGround;
		int TouchRoofOn;
		Touch_t TouchSideOn;
		int OnDeath;
		int OnGoal;
		// }

		int JumpFrame; // zd
	}
	Player;

	// Player's Born Pos
	fPInt_t BornX;
	fPInt_t BornY;

	int RespawnPointCreatedCount;
	fPInt_t RespawnX;
	fPInt_t RespawnY;
	double RespawnPoint_Z;
	int RestartedFromRespawnPoint;

	int CamSlideBtnOn;
	int CamSlid;
	int CamSlideTrueX;
	int CamSlideTrueY;
	int CamSlideRealX;
	int CamSlideRealY;

	struct
	{
		int RestartFlag;
		int BackToMenuFlag;
	}
	Pause;

	struct
	{
		int LeftWindowFlag;
		int Win_L;
		int Win_T;

		int MenuClicked;
		int ClickedCellType;

		int SelectCellType;
	}
	Edit;

	/*
	ret:
		1 - �^�C�g���֖߂��I������
		0 - ���̑�
	*/
	int ReturnMenuFlag;

	/*
	ret:
		1 - �N���A����
		0 - ���̑� ... �^�C�g���֖߂��I������ or ���v���C�Ŏ��S����
	*/
	int FloorClearFlag;

	// zantei warp
	fPInt_t WarpX;
	fPInt_t WarpY;

	int PlaySeekPos;

	GameInput_t CurrInput; // 1 <= GetInput(inpId);   <- ���ӁI
	GameInput_t LastInput;
	GameInput_t CurrPound; // GetPount(inpId);

	int FreezeAButton; // zd
	int FreezeRestart; // zd
	int LastDeadReplaySaveOk;

	// Use: ChangeInfoCaption()
	char *InfoCaption;
	int InfoCaptionFrame; // zd

	struct
	{
		int Frame; // zd
		taskList *EL;
		SubScreen_t *WallScreen;
		SubScreen_t *ExWallScreen;
		SubScreen_t *FieldScreen;
	}
	Meteor;

	double QuakePower;

	int JmpBtnDwnFrm; // zd
	int JmpBtnUpFlg;

	// Kabe {

	void (*KabeDrawFunc)(double);
	double KabeBr_A;
	double KabeBr_R;
	double KabeBr_G;
	double KabeBr_B;

	// }

	void (*BackKabeDrawFunc)(double); // NULL == ��_�؂�ւ�莞
	void (*FrontKabeDrawFunc)(double);
	double FrontKabe_A; // �؂�ւ�莞 0.0 -> 1.0
	double KabeBrList_A[KABE_BR_MAX];
	double KabeBrList_R[KABE_BR_MAX];
	double KabeBrList_G[KABE_BR_MAX];
	double KabeBrList_B[KABE_BR_MAX];

	int PlayFrame; // �X�^�[�g�E���X�|�[�����Ă���̌o�߃t���[�����B�ŏ������ʂɋ���G�� Frame �ƈ�v����͂��B<- ���Ȃ��Ȃ�܂����B@ 2016.12.11
		// <- �X�^�[�g���Ă���̌o�߃t���[�����ɕύX�B@ 2016.12.11
	int SoundAshibaActiveFlag;
	int SoundAshibaActiveVolPct;
	int RespawnYokushiFrame; // zd
}
GDc_t;

extern GDc_t GDc;

void GameInit(void);
void GameFnlz(void);

// ----

enum // GraphicalMode
{
	GM_DEFAULT, // as 0
	GM_LEGACY,
	GM_01,
	GM_02,
	GM_03,
	GM_04,
	GM_04_SOFT,
	GM_METEOR,
	GM_METEOR_SOFT,
	GM_EARTH,
	GM_GHOST,

	GM_MAX, // num of member
};

void PostInitKabe(void);
char *GetGraphicalModeCaption(int grphMd);
