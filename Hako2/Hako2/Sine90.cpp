/*
	360Åã == IDENOM * 4
*/

#include "all.h"

/*
	Sine90.txt
		0Åã Å` 90Åã Ç (IDENOM + 1) ï™äÑ -> sin(0 Å` IDENOM) * IMAX
*/
static autoList<int> *GetSine90(void)
{
	static autoList<int> *sine90;

	if(!sine90)
	{
		autoList<uchar> *fileData = GetEtcRes()->GetHandle(ETC_SINE90TXT);
		int rIndex = 0;

		sine90 = new autoList<int>();

		for(int index = 0; index <= IDENOM; index++)
		{
			sine90->AddElement(atoi_x(readLine(fileData, rIndex)));
		}
		delete fileData->Ecdysis(); // ãÛÇ…Ç∑ÇÈÅB
	}
	return sine90;
}
int iSin(fPInt_t angle, int retScale)
{
	errorCase(angle < -IMAX || IMAX < angle);
	errorCase(retScale < 0 || IMAX < retScale);

	angle %= IDENOM * 4;
	angle += IDENOM * 4;
	angle %= IDENOM * 4;

	errorCase(!m_isRange(angle, 0, IDENOM * 4 - 1)); // 2bs
	int retSign = 1 - (angle / (IDENOM * 2)) * 2; // IDENOM * 2 < angle ? -1 : 1;

	angle %= IDENOM * 2;
	angle = IDENOM - angle;
	angle = abs(angle);
	angle = IDENOM - angle;

	__int64 retval = GetSine90()->GetElement(angle) * retSign;
	retval *= retScale;
	retval /= IMAX;
	return (int)retval;
}
int iCos(fPInt_t angle, int retScale)
{
	return iSin(angle + IDENOM, retScale);
}
