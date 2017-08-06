enum
{
	INP_DIR_2,
	INP_DIR_4,
	INP_DIR_6,
	INP_DIR_8,
	INP_A,
	INP_B,
	INP_C,
	INP_D,
	INP_E,
	INP_F,
	INP_L,
	INP_R,
	INP_PAUSE,
	INP_START,

	INP_MAX, // num of member
};

void InputEachFrame(void);
int GetInput(int inpId);
int GetPound(int inpId);
