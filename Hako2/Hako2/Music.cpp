#include "all.h"

typedef struct PlayInfo_st
{
	int Command; // "PVS"
	MusicInfo_t *Music;
	int OnceMode;
	int ResumeMode;
	double VolumeRate;
}
PlayInfo_t;

static PlayInfo_t *CreatePI(int command, MusicInfo_t *music, int once_mode, int resume_mode, double volume_rate)
{
	PlayInfo_t *i = nb(PlayInfo_t);

	i->Command = command;
	i->Music = music;
	i->OnceMode = once_mode;
	i->ResumeMode = resume_mode;
	i->VolumeRate = volume_rate;

	return i;
}
static void ReleasePI(PlayInfo_t *i)
{
	memFree(i);
}
static oneObject(autoQueue<PlayInfo_t *>, new autoQueue<PlayInfo_t *>(), GetPlayList);

MusicInfo_t *CurrDestMusic;
double CurrDestMusicVolumeRate;

static MusicInfo_t *LoadMusic(autoList<uchar> *fileData)
{
	MusicInfo_t *i = nb(MusicInfo_t);

	i->Handle = LoadSound(fileData);
	i->Volume = 0.5;

	// app >

	switch(RC_ResId) // musId
	{
	/*
	case MUS_xxx:
		break;
	*/

	case -1: // dummy
	default:
		break;
	}

	// < app

	SetVolume(i->Handle, 0.0); // ! ƒ~ƒ…[ƒg‚µ‚Ä‚¨‚­B
	return i;
}
static void UnloadMusic(MusicInfo_t *i)
{
	// reset {
	GetPlayList()->Clear(ReleasePI);

	CurrDestMusic = NULL;
	CurrDestMusicVolumeRate = 0.0;
	// }

	UnloadSound(i->Handle);
	memFree(i);
}
static oneObject(
	resCluster<MusicInfo_t *>,
	new resCluster<MusicInfo_t *>("Music.dat", "..\\..\\Music.txt", MUS_MAX, 120000000, LoadMusic, UnloadMusic),
	GetMusRes
	);

void MusicEachFrame(void)
{
	PlayInfo_t *i = GetPlayList()->Dequeue(NULL);

	if(i)
	{
		switch(i->Command)
		{
		case 'P':
			SoundPlay(i->Music->Handle, i->OnceMode, i->ResumeMode);
			break;

		case 'V':
			SetVolume(i->Music->Handle, MixVolume(Gnd.MusicVolume, i->Music->Volume) * i->VolumeRate);
			break;

		case 'S':
			SoundStop(i->Music->Handle);
			break;

		default:
			error();
		}
		memFree(i);
	}
}
void MusicPlay(int musId, int once_mode, int resume_mode, double volumeRate, int fadeFrameMax)
{
	errorCase(musId < 0 || MUS_MAX <= musId);

	MusicInfo_t *i = GetMusRes()->GetHandle(musId);

	if(CurrDestMusic)
	{
		if(CurrDestMusic == i)
			return;

		if(fadeFrameMax)
			MusicFade(fadeFrameMax);
		else
			MusicStop();
	}
	GetPlayList()->Enqueue(CreatePI('P', i, once_mode, resume_mode, 0.0));
//	GetPlayList()->Enqueue(NULL, 1);
	GetPlayList()->Enqueue(CreatePI('V', i, 0, 0, volumeRate));
	GetPlayList()->Enqueue(NULL, 3);

	CurrDestMusic = i;
	CurrDestMusicVolumeRate = volumeRate;
}
void MusicFade(int frameMax, double destVRate, double startVRate)
{
	if(!CurrDestMusic)
		return;

	m_range(frameMax, 1, 3600); // 1 frame - 1 min
	m_range(destVRate, 0.0, 1.0);
	m_range(startVRate, 0.0, 1.0);

	for(int frmcnt = 0; frmcnt <= frameMax; frmcnt++)
	{
		double vRate;

		if(!frmcnt)
			vRate = startVRate;
		else if(frmcnt == frameMax)
			vRate = destVRate;
		else
			vRate = startVRate + ((destVRate - startVRate) * frmcnt) / frameMax;

		GetPlayList()->Enqueue(CreatePI('V', CurrDestMusic, 0, 0, vRate));
	}
	CurrDestMusicVolumeRate = destVRate;

	if(destVRate == 0.0) // ? ‰¹—Êƒ[ƒ -> ‹È’âŽ~
	{
		MusicStop();
	}
}
void MusicStop(void)
{
	if(!CurrDestMusic)
		return;

	GetPlayList()->Enqueue(CreatePI('V', CurrDestMusic, 0, 0, 0.0));
	GetPlayList()->Enqueue(NULL, 3);
	GetPlayList()->Enqueue(CreatePI('S', CurrDestMusic, 0, 0, 0.0));
	GetPlayList()->Enqueue(NULL, 3);

	CurrDestMusic = NULL;
	CurrDestMusicVolumeRate = 0.0;
}
void MusicTouch(int musId)
{
	GetMusRes()->GetHandle(musId);
}
void UpdateMusicVolume(void)
{
	MusicFade(0, 1.0);
}
