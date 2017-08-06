int LoadSound(autoList<uchar> *fileData);
int DuplSound(int h);
void UnloadSound(int h);

void SoundPlay(int h, int once_mode = 1, int resume_mode = 0);
void SetVolume(int h, double volume);
void SoundStop(int h);

double MixVolume(double volume1, double volume2);
