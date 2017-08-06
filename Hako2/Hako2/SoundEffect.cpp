#include "all.h"

static oneObject(autoQueue<SEInfo_t *>, new autoQueue<SEInfo_t *>(), GetPlayList);

static void UpdateSEVolumeFunc(SEInfo_t *i)
{
	for(int index = 0; index < SE_HANDLE_MAX; index++)
	{
		SetVolume(i->HandleList[index], MixVolume(Gnd.SEVolume, i->Volume));
	}
}
static SEInfo_t *LoadSE(autoList<uchar> *fileData)
{
	SEInfo_t *i = nb(SEInfo_t);

	i->HandleList[0] = LoadSound(fileData);

	for(int index = 1; index < SE_HANDLE_MAX; index++)
	{
		i->HandleList[index] = DuplSound(i->HandleList[0]);
		i->PrvVolumePctList[index] = -1;
	}
	i->Volume = 0.5;

	// app >

	switch(RC_ResId) // seId
	{
	/*
	case SE_xxx:
		break;
	*/

	case SE_CREATE_RESPAWN_POINT:
		i->Volume = 0.6;
		break;

	case -1: // dummy
	default:
		break;
	}

	// < app

	UpdateSEVolumeFunc(i);
	return i;
}
static void UnloadSE(SEInfo_t *i)
{
	GetPlayList()->Clear();

	for(int index = 0; index < SE_HANDLE_MAX; index++)
	{
		UnloadSound(i->HandleList[index]);
	}
	memFree(i);
}
static oneObject(
	resCluster<SEInfo_t *>,
	new resCluster<SEInfo_t *>("SoundEffect.dat", "..\\..\\SoundEffect.txt", SE_MAX, 130000000, LoadSE, UnloadSE),
	GetSERes
	);

int SEEachFrame(void) // ret: Œø‰Ê‰¹‚ðˆ—‚µ‚½B
{
	SEInfo_t *i = GetPlayList()->Dequeue(NULL);

	if(i)
	{
		int hdl = i->HandleList[i->HandleIndex];
		int prvVolumePct = i->PrvVolumePctList[i->HandleIndex];

		if(prvVolumePct != -1)
			SetVolume(hdl, MixVolume(Gnd.SEVolume, i->Volume) * (prvVolumePct / 100.0));

		SoundPlay(hdl);
		i->HandleIndex = (i->HandleIndex + 1) % SE_HANDLE_MAX;

		return 1;
	}
	return 0;
}
void SEPlay(int seId, int prvVolumePct)
{
	errorCase(seId < 0 || SE_MAX <= seId);
	errorCase(!m_isRange(prvVolumePct, -1, 100));

	SEInfo_t *i = GetSERes()->GetHandle(seId);
	int count = 0;

	/*
		“¯‚¶‰¹‚ª‚Q‚ÂˆÈã‘Ò‹@’†‚Å‚ ‚ê‚Î‰½‚à‚µ‚È‚¢B
	*/
	for(int index = GetPlayList()->GetTopIndex_DIRECT(); index < GetPlayList()->GetList_DIRECT()->GetCount(); index++)
		if(GetPlayList()->GetList_DIRECT()->GetElement(index) == i && 2 <= ++count)
			return;

	i->PrvVolumePctList[i->HandleIndex] = prvVolumePct;

	GetPlayList()->Enqueue(i);
	GetPlayList()->Enqueue(NULL);
}
void SETouch(int seId)
{
	GetSERes()->GetHandle(seId);
}
void UpdateSEVolume(void)
{
	GetSERes()->CallAllLoadedHandle(UpdateSEVolumeFunc);
}
