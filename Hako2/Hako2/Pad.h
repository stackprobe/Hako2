enum // SANWA Pad Btnno
{
	SNWPB_DIR_2,
	SNWPB_DIR_4,
	SNWPB_DIR_6,
	SNWPB_DIR_8,
	SNWPB_A1, // 下段左
	SNWPB_A2, // 下段中
	SNWPB_A3, // 下段右
	SNWPB_B1, // 上段左
	SNWPB_B2, // 上段中
	SNWPB_B3, // 上段右
	SNWPB_L,
	SNWPB_R,
	SNWPB_USTART, // 上スタート
	SNWPB_DSTART, // 下スタート
};

#define PAD_MAX 16
#define PAD_BUTTON_MAX 32

int GetPadCount(void);
void PadEachFrame(void);
int GetPadInput(int padId, int btnId);
int GetPadPound(int padId, int btnId);
