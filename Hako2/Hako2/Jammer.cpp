#include "all.h"

static void Kaisa(autoList<uchar> *block, int downFlag)
{
	uchar *buffer = block->ElementAt(0);

	if(downFlag)
	{
		for(int index = block->GetCount() - 1; 1 <= index; index--)
		{
			buffer[index] = (buffer[index] + 256 - buffer[index - 1]) % 256;
		}
	}
	else
	{
		for(int index = 1; index < block->GetCount(); index++)
		{
			buffer[index] = (buffer[index] + buffer[index - 1]) % 256;
		}
	}
}
static void KRK(autoList<uchar> *block, int downFlag)
{
	Kaisa(block, downFlag);
	block->Reverse();
	Kaisa(block, downFlag);
}

#define MULTIVAL 157
#define POWERVAL 31

static void AddTrailZero(autoList<uchar> *block, int tznum)
{
	block->AddRepeat(0x00, tznum);
}
static int UnaddTrailZero(autoList<uchar> *block)
{
	return block->UnaddRepeat(0x00);
}
static int SingleJammer(autoList<uchar> *block, int encodeFlag)
{
	int tznum = UnaddTrailZero(block);

	if(block->GetCount() == 0)
	{
		if(encodeFlag)
		{
			tznum++;
		}
		else
		{
			if(!tznum)
				return 0;

			tznum--;
		}
	}
	else if(encodeFlag)
	{
		int value = 0;

		for(int index = 0; index < block->GetCount() || value; index++)
		{
			value += block->RefElement(index, 0) * MULTIVAL;
			block->PutElement(index, value & 0xff, 0);
			value >>= 8;
		}
	}
	else
	{
		int value = 0;

		for(int index = block->GetCount() - 1; 0 <= index; index--)
		{
			value |= block->GetElement(index);
			block->SetElement(index, value / MULTIVAL);
			value %= MULTIVAL;
			value <<= 8;
		}
		if(value)
			return 0;

		UnaddTrailZero(block);
	}
	AddTrailZero(block, tznum);
	return 1;
}
static int MultiJammer(autoList<uchar> *block, int encodeFlag)
{
	for(int count = 0; count < POWERVAL; count++)
		if(!SingleJammer(block, encodeFlag))
			return 0;

	return 1;
}
int Jammer(autoList<uchar> *block, int encodeFlag) // ret: ? ê¨å˜
{
	int retval;

	if(encodeFlag)
	{
		errorCase(!MultiJammer(block, 1));
		KRK(block, 0);
		retval = 1;
	}
	else
	{
		KRK(block, 1);
		retval = MultiJammer(block, 0);
	}
	return retval;
}
