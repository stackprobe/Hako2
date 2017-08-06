#include "all.h"

/*
	SHA-512
*/
static autoList<uchar> *ReplayMapHash;

/*
	各バイトの各ビットは REPBIT_* に対応

	長さの上限: REPLAY_DATA_MAX
	長さ == REPLAY_DATA_MAX のときはオーバーフローと見なし、保存しない。
*/
static autoList<uchar> *ReplayInput;
static autoList<uchar> *ReplayPound;

static autoList<uchar> *BehindReplayInput;
static autoList<uchar> *BehindReplayPound;

autoList<uchar> *GetReplayMapHash(void)
{
	errorCase(!ReplayMapHash);
	return ReplayMapHash;
}
void SetReplayMapHash_x(autoList<uchar> *hash)
{
	delete ReplayMapHash;
	ReplayMapHash = hash;
}
autoList<uchar> *GetReplayInput(void)
{
	if(!ReplayInput)
		ReplayInput = new autoList<uchar>();

	return ReplayInput;
}
autoList<uchar> *GetReplayPound(void)
{
	if(!ReplayPound)
		ReplayPound = new autoList<uchar>();

	return ReplayPound;
}
void ClearReplayData(void)
{
	delete ReplayInput;
	ReplayInput = new autoList<uchar>(REPLAY_DATA_MAX);

	delete ReplayPound;
	ReplayPound = new autoList<uchar>(REPLAY_DATA_MAX);
}
void SwapReplayData(void)
{
	m_swap(ReplayInput, BehindReplayInput, autoList<uchar> *);
	m_swap(ReplayPound, BehindReplayPound, autoList<uchar> *);
}

void LoadReplayFile(char *replayFile)
{
	autoList<uchar> *fileData = readFile(replayFile);
	int rIndex = 0;

	delete ReplayMapHash;
	ReplayMapHash = readBlock(fileData, rIndex, 64);

	delete ReplayInput;
	ReplayInput = readBlock(fileData, rIndex, (fileData->GetCount() - rIndex) / 2); // 残り半分

	delete ReplayPound;
	ReplayPound = readBlock(fileData, rIndex, fileData->GetCount() - rIndex); // 残り全部

	delete fileData;
}
void SaveReplayFile(char *replayFile)
{
	autoList<uchar> *fileData = new autoList<uchar>();

	errorCase(!ReplayMapHash);
	errorCase(!ReplayInput);
	errorCase(!ReplayPound);

	fileData->AddElements(ReplayMapHash);
	fileData->AddElements(ReplayInput);
	fileData->AddElements(ReplayPound);

	writeFile_cx(replayFile, fileData);
}
