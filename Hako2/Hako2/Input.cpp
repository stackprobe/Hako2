#include "all.h"

// app >

static int HookedFlag;
static int HookedInput;
static int HookedPound;

void HookInput(int hookInput, int hookPound)
{
	HookedFlag = 1;
	HookedInput = hookInput;
	HookedPound = hookPound;
}
void UnhookInput(void)
{
	HookedFlag = 0;
}

static int GetHookedInputPound(int inpId, int &retval, int poundFlag)
{
	if(!HookedFlag)
		return 0;

	int bit;

	switch(inpId)
	{
	case INP_DIR_2: bit = REPBIT_DIR_2; break;
	case INP_DIR_4: bit = REPBIT_DIR_4; break;
	case INP_DIR_6: bit = REPBIT_DIR_6; break;
	case INP_DIR_8: bit = REPBIT_DIR_8; break;
	case INP_A: bit = REPBIT_A; break;
	case INP_B: bit = REPBIT_B; break;
	case INP_L: bit = REPBIT_L; break;

	default:
		return 0;
	}

	int status = poundFlag ? HookedPound : HookedInput;
	status >>= bit;
	return status & 1;
}
static int GetHookedInput(int inpId, int &retval)
{
	return GetHookedInputPound(inpId, retval, 0);
}
static int GetHookedPound(int inpId, int &retval)
{
	return GetHookedInputPound(inpId, retval, 1);
}

// < app

static int InputStatus[INP_MAX];

static void MixInput(int inpId, int keyId, int btnId)
{
	int keyDown = 1 <= GetKeyInput(keyId);
	int btnDown = 1 <= GetPadInput(Gnd.PrimaryPadId, btnId);

	updateInput(InputStatus[inpId], keyDown || btnDown);
}
void InputEachFrame(void)
{
	MixInput(INP_DIR_2, KEY_INPUT_DOWN, Gnd.PadBtnId.Dir_2);
	MixInput(INP_DIR_4, KEY_INPUT_LEFT, Gnd.PadBtnId.Dir_4);
	MixInput(INP_DIR_6, KEY_INPUT_RIGHT, Gnd.PadBtnId.Dir_6);
	MixInput(INP_DIR_8, KEY_INPUT_UP, Gnd.PadBtnId.Dir_8);
	MixInput(INP_A, KEY_INPUT_Z, Gnd.PadBtnId.A);
	MixInput(INP_B, KEY_INPUT_X, Gnd.PadBtnId.B);
	MixInput(INP_C, KEY_INPUT_C, Gnd.PadBtnId.C);
	MixInput(INP_D, KEY_INPUT_V, Gnd.PadBtnId.D);
	MixInput(INP_E, KEY_INPUT_B, Gnd.PadBtnId.C);
	MixInput(INP_F, KEY_INPUT_N, Gnd.PadBtnId.D);
	MixInput(INP_L, KEY_INPUT_A, Gnd.PadBtnId.L);
	MixInput(INP_R, KEY_INPUT_S, Gnd.PadBtnId.R);
	MixInput(INP_PAUSE, KEY_INPUT_SPACE, Gnd.PadBtnId.Pause);
	MixInput(INP_START, KEY_INPUT_RETURN, Gnd.PadBtnId.Start);
}
int GetInput(int inpId)
{
	errorCase(inpId < 0 || INP_MAX < inpId);

	// app
	{
		int retval;

		if(GetHookedInput(inpId, retval))
		{
			return retval;
		}
	}

	return FreezeInputFrame ? 0 : InputStatus[inpId];
}
int GetPound(int inpId)
{
	errorCase(inpId < 0 || INP_MAX < inpId);

	// app
	{
		int retval;

		if(GetHookedPound(inpId, retval))
		{
			return retval;
		}
	}

	return FreezeInputFrame ? 0 : isPound(InputStatus[inpId]);
}
