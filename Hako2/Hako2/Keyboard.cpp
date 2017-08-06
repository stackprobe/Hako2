#include "all.h"

static int KeyStatus[KEY_MAX];

void KeyEachFrame(void)
{
	if(WindowIsActive)
	{
		static uchar statusMap[KEY_MAX];

		errorCase(GetHitKeyStateAll((char *)statusMap)); // ? 失敗

		for(int keyId = 0; keyId < KEY_MAX; keyId++)
		{
			updateInput(KeyStatus[keyId], statusMap[keyId]);
		}
	}
	else // ? 非アクティブ -> 無入力
	{
		for(int keyId = 0; keyId < KEY_MAX; keyId++)
		{
			updateInput(KeyStatus[keyId], 0);
		}
	}
}
int GetKeyInput(int keyId)
{
	KEY_INPUT_BACK; // <- keyId

	errorCase(keyId < 0 || KEY_MAX <= keyId);

	return FreezeInputFrame ? 0 : KeyStatus[keyId];
}
int GetKeyPound(int keyId)
{
	return isPound(GetKeyInput(keyId));
}
