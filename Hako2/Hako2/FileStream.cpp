#include "all.h"

FILE *fileOpen(char *file, char *mode)
{
	errorCase(m_isEmpty(file));
#if LOG_ENABLED == 0
	// ? ÉäÉäÅ[ÉXî≈Ç≈ C:\\tmp\\*, C:\\temp\\* ÇéQè∆ÇµÇƒÇ¢ÇÈÅB-> âΩÇ©Ç®Ç©ÇµÇ¢ÅB
	errorCase(!_strnicmp(file, "C:\\tmp\\", 7));
	errorCase(!_strnicmp(file, "C:\\temp\\", 7));
#endif

	FILE *fp = fopen(file, mode);

	if(!fp) // ? é∏îs
	{
		LOG("[fileOpen] %s %s\n", file, mode);
		error();
	}
	return fp;
}
void fileClose(FILE *fp)
{
	errorCase(fclose(fp)); // ? é∏îs
}

static int GetFileSize(FILE *fp)
{
	errorCase(_fseeki64(fp, 0I64, SEEK_END)); // ? é∏îs

	__int64 size = _ftelli64(fp);

	errorCase(size < 0i64 || (__int64)IMAX < size);
	errorCase(_fseeki64(fp, 0, SEEK_SET)); // ? é∏îs

	return (int)size;
}
autoList<uchar> *readFile(char *file)
{
	FILE *fp = fileOpen(file, "rb");
	int size = GetFileSize(fp);
	uchar *fileData = (uchar *)memAlloc_NC(size);

	if(size)
	{
		errorCase(fread(fileData, 1, size, fp) != size); // ? read error
	}
	fileClose(fp);
	return new autoList<uchar>(fileData, size);
}
autoList<uchar> *readFile_x(char *file)
{
	autoList<uchar> *out = readFile(file);
	memFree(file);
	return out;
}
char *readLine(autoList<uchar> *fileData, int &rIndex)
{
	autoList<char> *line = new autoList<char>();

	while(rIndex < fileData->GetCount())
	{
		int chr = fileData->GetElement(rIndex++);

		if(chr == '\r')
		{
			1; // ì«Ç›îÚÇŒÇ∑ÅB
		}
		else if(chr == '\n')
		{
			break;
		}
		else
		{
			line->AddElement(chr);
		}
	}
	if(line->GetCount() == 0 && rIndex == fileData->GetCount()) // ? ì«Ç›çûÇ›èIóπ
	{
		delete line;
		return NULL;
	}
	line->AddElement('\0');

	char *result = line->UnbindBuffer();
	delete line;
	return result;
}
char *anReadLine(autoList<uchar> *fileData, int &rIndex)
{
	char *line = readLine(fileData, rIndex);
	errorCase(!line);
	return line;
}
char *nnReadLine(autoList<uchar> *fileData, int &rIndex, char *defaultLine)
{
	char *line = readLine(fileData, rIndex);

	if(!line)
		line = strx(defaultLine);

	return line;
}
autoList<char *> *readLines(autoList<uchar> *fileData)
{
	autoList<char *> *lines = new autoList<char *>();
	int rIndex = 0;

	for(; ; )
	{
		char *line = readLine(fileData, rIndex);

		if(!line)
			break;

		lines->AddElement(line);
	}
	return lines;
}
autoList<char *> *readLines_x(autoList<uchar> *fileData)
{
	autoList<char *> *lines = readLines(fileData);
	delete fileData;
	return lines;
}

void writeFile(char *file, autoList<uchar> *fileData)
{
	FILE *fp = fileOpen(file, "wb");

	errorCase(fwrite(fileData->ElementAt(0), 1, fileData->GetCount(), fp) != fileData->GetCount()); // ? write error

	fileClose(fp);
}
void writeFile_cx(char *file, autoList<uchar> *fileData)
{
	writeFile(file, fileData);
	delete fileData;
}
void writeToken(autoList<uchar> *fileData, char *token)
{
	for(char *p = token; *p; p++)
	{
		fileData->AddElement(*p);
	}
}
void writeLine(autoList<uchar> *fileData, char *line)
{
	writeToken(fileData, line);
	fileData->AddElement('\n');
}
void writeLine_x(autoList<uchar> *fileData, char *line)
{
	writeLine(fileData, line);
	memFree(line);
}

uint64 readUI64(autoList<uchar> *fileData, int &rIndex, int width)
{
	uint64 value = 0;

	for(int c = 0; c < width; c++)
	{
		value |= (uint64)fileData->GetElement(rIndex++) << c * 8;
	}
	return value;
}
uint readUI32(autoList<uchar> *fileData, int &rIndex, int width)
{
	return (uint)readUI64(fileData, rIndex, width);
}

void writeUI64(autoList<uchar> *fileData, uint64 value, int width)
{
	for(int c = 0; c < width; c++)
	{
		fileData->AddElement((uchar)(value >> c * 8 & 0xff));
	}
}
void writeUI32(autoList<uchar> *fileData, uint value, int width)
{
	writeUI64(fileData, (uint64)value, width);
}

autoList<uchar> *readBlock(autoList<uchar> *fileData, int &rIndex, int width)
{
	autoList<uchar> *retBlock = new autoList<uchar>();

	for(int index = 0; index < width; index++)
	{
		retBlock->AddElement(fileData->RefElement(rIndex++, 0x00));
	}
	return retBlock;
}
