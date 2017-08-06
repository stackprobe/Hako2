#include "all.h"

static autoList<char *> *GetDatStrings(void)
{
	static autoList<char *> *datStrings;

	if(!datStrings)
	{
		autoList<uchar> *fileData = GetEtcRes()->GetHandle(ETC_DATSTRINGS);
		int rIndex = 0;

		datStrings = new autoList<char *>();

		for(int index = 0; index < DATSTR_MAX; index++)
		{
			char *datString = readLine(fileData, rIndex);
			errorCase(!datString); // ? 行が少ない。

			datStrings->AddElement(datString);
		}
		errorCase(readLine(fileData, rIndex)); // ? 行が多い。

		delete fileData->Ecdysis(); // 空にする。
	}
	return datStrings;
}
char *GetDatString(int datStrId)
{
	errorCase(datStrId < 0 || DATSTR_MAX <= datStrId);

	return GetDatStrings()->GetElement(datStrId);
}
