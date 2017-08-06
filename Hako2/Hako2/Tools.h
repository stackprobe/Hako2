double toDouble(char *line);
char *xcout(char *format, ...);
char *strx(char *line);
int isPound(int counter);
int atoi_x(char *line);
autoList<char *> *tokenize(char *line, char *delimiters);

template <class Var_t>
void t_swap(Var_t &a, Var_t &b)
{
	Var_t tmp = a;
	a = b;
	b = tmp;
}

int getZero(void);
uint getUIZero(void);
__int64 getI64Zero(void);
void noop(void);
void noop_i(int dummy);
void noop_ui(uint dummy);
void noop_i64(__int64 dummy);

i2D_t makeI2D(int x, int y);
d2D_t makeD2D(double x, double y);

bitList *createBitList(void);
void releaseBitList(bitList *bl);

void my_memset(void *block, int fillValue, int size);

template <class Var_t>
void zeroclear(Var_t *var, int num = 1)
{
	my_memset(var, 0x00, num * sizeof(Var_t));
}

template <class Deletable_t>
void callDelete(Deletable_t *i)
{
	delete i;
}

template <class Deletable_t>
void deleteList(autoList<Deletable_t> *list)
{
	list->CallAllElement(callDelete);
	delete list;
}

int isSame(autoList<uchar> *binData1, autoList<uchar> *binData2);
int intComp(int v1, int v2);

extern int chooseRnd_LastCount;

template <class Element_t>
autoList<i2D_t> *chooseRnd(autoTable<Element_t> *table, int (*isChoose)(Element_t, int, int), Random *rnd, int num)
{
	errorCase(!table);
	errorCase(!isChoose);
	errorCase(!rnd);
	errorCase(!m_isRange(num, 0, IMAX));

	autoList<int> *odrPosList = new autoList<int>(num);
	autoList<i2D_t> *ret = new autoList<i2D_t>(num);
	int count = 0;
	int index;

	for(int x = 0; x < table->GetWidth(); x++)
	for(int y = 0; y < table->GetHeight(); y++)
	{
		if(isChoose(table->GetCell(x, y), x, y))
		{
			count++;
		}
	}
	chooseRnd_LastCount = count;

	if(!count) // ? not found
		goto endFunc;

	m_minim(num, count);

	/*
		num ‚Æ count ‚ª‹ß‚­‚È‚é‚Ù‚ÇƒŠƒgƒ‰ƒC‚Ì‰ñ”‚ª‘‚¦‚é‚Í‚¸B
		b’è‘Îô
	*/
	if(10 < count && count / 2 < num)
	{
		num = count / 2;
	}

	for(index = 0; index < num; index++)
	{
resetOdrPos:
		int odrPos = rnd->Rnd(count);

		for(int i = 0; i < index; i++)
			if(odrPosList->GetElement(i) == odrPos)
				goto resetOdrPos;

		odrPosList->AddElement(odrPos);
	}
	odrPosList->Sort(intComp);
	count = 0;
	index = 0;

	for(int x = 0; x < table->GetWidth(); x++)
	for(int y = 0; y < table->GetHeight(); y++)
	{
		if(isChoose(table->GetCell(x, y), x, y))
		{
			if(count == odrPosList->GetElement(index))
			{
				ret->AddElement(makeI2D(x, y));
				index++;

				if(odrPosList->GetCount() <= index)
					goto endFunc;
			}
			count++;
		}
	}
	error(); // 2bs

endFunc:
	delete odrPosList;
	return ret;
}

#define mbcNext(p) \
	((p) + (_ismbblead(*(p)) && (p)[1] ? 2 : 1))

char *jstrrchr(char *line, int chr);

int GetAbsMin(int v1, int v2);
int GetAbsMax(int v1, int v2);
void SwapAbs(int &v1, int &v2);
void AvgAbs(int &v1, int &v2);
