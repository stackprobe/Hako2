#include "all.h"

#define CONF_FILE "Hako2.conf"

// ---- conf setting (app) ----

int ChangeBackgroundByTKeyEnabled = 0;
int UnlockAllFloorEnabled = 0;
int EditModeEnabled = 0;
int EndingPicIndex = 0;

// ----

static autoList<uchar> *FImage;
static int RIndex;

static char *ReadConfLine()
{
	char *line;

	for(; ; )
	{
		line = anReadLine(FImage, RIndex);

		if(*line && *line != ';')
			break;

		memFree(line);
	}
	return line;
}
void LoadConfFile(void)
{
	if(_access(CONF_FILE, 0)) // ? not exists CONF_FILE
		return;

	FImage = readFile(CONF_FILE);
	RIndex = 0;

	ChangeBackgroundByTKeyEnabled = atoi_x(ReadConfLine());
	UnlockAllFloorEnabled         = atoi_x(ReadConfLine());
	EditModeEnabled               = atoi_x(ReadConfLine());
	EndingPicIndex                = atoi_x(ReadConfLine());

	delete FImage;
}
