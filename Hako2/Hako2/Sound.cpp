#include "all.h"

int LoadSound(autoList<uchar> *fileData)
{
	int h = LoadSoundMemByMemImage(fileData->ElementAt(0), fileData->GetCount());

	errorCase(h == -1); // ? ¸”s
	return h;
}
int DuplSound(int h)
{
	int dupl_h = DuplicateSoundMem(h);

	errorCase(dupl_h == -1); // ? ¸”s
	return dupl_h;
}
void UnloadSound(int h)
{
	errorCase(DeleteSoundMem(h)); // ? ¸”s
}

void SoundPlay(int h, int once_mode, int resume_mode)
{
	switch(CheckSoundMem(h))
	{
	case 1: // ? Ä¶’†
		return;

	case 0: // ? Ä¶‚µ‚Ä‚¢‚È‚¢B
		break;

	case -1: // ? ƒGƒ‰[
		error();

	default: // ? •s–¾
		error();
	}
	errorCase(PlaySoundMem(h, once_mode ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_LOOP, resume_mode ? 0 : 1)); // ? ¸”s
}
void SetVolume(int h, double volume)
{
	m_range(volume, 0.0, 1.0);

	int pal = m_d2i(volume * 255.0);

	errorCase(pal < 0 || 255 < pal);
	errorCase(ChangeVolumeSoundMem(pal, h)); // ? ¸”s
}
void SoundStop(int h)
{
	errorCase(StopSoundMem(h)); // ? ¸”s
}

double MixVolume(double volume1, double volume2) // (volume1, volume2): 0.0 - 1.0, ret: 0.0 - 1.0
{
	m_range(volume1, 0.0, 1.0);
	m_range(volume2, 0.0, 1.0);

	double mixedVolume = volume1 * volume2 * 2.0; // 0.0 - 2.0

	m_range(mixedVolume, 0.0, 1.0);

	return mixedVolume;
}
