#include "all.h"

double toDouble(char *line)
{
	int numer = 0;
	int denom = 0;
	int minus = 0;

	for(char *p = line; *p; p++)
	{
		if('0' <= *p && *p <= '9')
		{
			if(
				IMAX / 10 < numer ||
				IMAX / 10 < denom
				)
				break; // overflow -> ë≈ÇøêÿÇË

			numer *= 10;
			numer += *p - '0';
			denom *= 10;
		}
		else if(*p == '.')
		{
			denom = 1;
		}
		else if(*p == '-')
		{
			minus = 1;
		}
	}
	if(!denom)
		denom = 1;

	return (double)numer / denom * (minus ? -1 : 1);
}
char *xcout(char *format, ...)
{
	char *buffer;
	va_list marker;

	va_start(marker, format);

	for(int size = strlen(format) + 128; ; size *= 2)
	{
		errorCase(IMAX < size);

		buffer = (char *)memAlloc(size + 20);
		int retval = _vsnprintf(buffer, size + 10, format, marker);
		buffer[size + 10] = '\0';

		if(0 <= retval && retval <= size)
			break;

		memFree(buffer);
	}
	va_end(marker);

	return buffer;
}
char *strx(char *line)
{
	return (char *)memClone(line, strlen(line) + 1);
}

#define POUND_FIRST_DELAY 17
#define POUND_DELAY 4

int isPound(int counter)
{
	return (counter) == 1 || POUND_FIRST_DELAY < (counter) && ((counter) - POUND_FIRST_DELAY) % POUND_DELAY == 1;
}
int atoi_x(char *line)
{
	int value = atoi(line);
	memFree(line);
	return value;
}
autoList<char *> *tokenize(char *line, char *delimiters)
{
	autoList<char *> *tokens = new autoList<char *>();
	autoList<char> *token = new autoList<char>();

	for(char *p = line; *p; p++)
	{
		char *d;

		for(d = delimiters; *d; d++)
			if(*d == *p)
				break;

		if(*d)
		{
			token->AddElement('\0');
			tokens->AddElement(token->UnbindBuffer());
		}
		else
			token->AddElement(*p);
	}
	token->AddElement('\0');
	tokens->AddElement(token->UnbindBuffer());

	delete token;
	return tokens;
}

int getZero(void)
{
	return 0;
}
uint getUIZero(void)
{
	return 0;
}
__int64 getI64Zero(void)
{
	return 0;
}
void noop(void)
{
	1; // noop
}
void noop_i(int dummy)
{
	1; // noop
}
void noop_ui(uint dummy)
{
	1; // noop
}
void noop_i64(__int64 dummy)
{
	1; // noop
}

i2D_t makeI2D(int x, int y)
{
	i2D_t pos;

	pos.X = x;
	pos.Y = y;

	return pos;
}
d2D_t makeD2D(double x, double y)
{
	d2D_t pos;

	pos.X = x;
	pos.Y = y;

	return pos;
}

bitList *createBitList(void)
{
	return new bitList();
}
void releaseBitList(bitList *bl)
{
	delete bl;
}

void my_memset(void *block, int fillValue, int size)
{
	for(int index = 0; index < size; index++)
	{
		((uchar *)block)[index] = fillValue;
	}
}

int isSame(autoList<uchar> *binData1, autoList<uchar> *binData2)
{
	if(binData1->GetCount() != binData2->GetCount())
		return 0;

	for(int index = 0; index < binData1->GetCount(); index++)
		if(binData1->GetElement(index) != binData2->GetElement(index))
			return 0;

	return 1;
}
int intComp(int v1, int v2)
{
	if(v1 < v2) return -1;
	if(v2 < v1) return 1;
	return 0;
}

int chooseRnd_LastCount = -1;

char *jstrrchr(char *line, int chr)
{
	char *ret = NULL;

	for(char *p = line; *p; p = mbcNext(p))
		if(*p == chr)
			ret = p;

	return ret;
}

int GetAbsMin(int v1, int v2)
{
	return abs(v1) < abs(v2) ? v1 : v2;
}
int GetAbsMax(int v1, int v2)
{
	return abs(v1) < abs(v2) ? v2 : v1;
}
void SwapAbs(int &v1, int &v2)
{
	int s1 = v1 < 0 ? -1 : 1;
	int s2 = v2 < 0 ? -1 : 1;
	int a1 = v1 * s1;
	int a2 = v2 * s2;

	v1 = a2 * s1;
	v2 = a1 * s2;
}
void AvgAbs(int &v1, int &v2)
{
	int s1 = v1 < 0 ? -1 : 1;
	int s2 = v2 < 0 ? -1 : 1;
	int a1 = v1 * s1;
	int a2 = v2 * s2;

	int avg = (a1 + a2) / 2;

	v1 = avg * s1;
	v2 = avg * s2;
}
