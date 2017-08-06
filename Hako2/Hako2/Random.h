#define MT19937_N 624

void initRnd(int seed);

void enterRnd(unsigned long *lmt, int lmti);
void leaveRnd(int *p_lmti);

int rnd(int modulo);
int bRnd(int minval, int maxval);
int sRnd(void);
double dRnd(void);
double eRnd(void);
int rndPct(int pct);
int rndPermil(int permil);

class Random
{
private:
	unsigned long *mt;
	int mti;

	void Enter()
	{
		enterRnd(this->mt, this->mti);
	}
	void Leave()
	{
		leaveRnd(&this->mti);
	}

public:
	Random(int seed)
	{
		this->mt = (unsigned long *)memAlloc_NC(MT19937_N * sizeof(unsigned long));
		this->Enter();
		initRnd(seed);
		this->Leave();
	}
	Random(const Random &source)
	{
		error();
	}
	~Random()
	{
		memFree(this->mt);
	}

#define WRAPPER(RET_TYPE, DEF_FUNC, CALL_FUNC) \
	DEF_FUNC \
	{ \
		this->Enter(); \
		RET_TYPE retval = CALL_FUNC; \
		this->Leave(); \
		return retval; \
	}

	WRAPPER(int, int Rnd(int modulo), rnd(modulo))
	WRAPPER(int, int BRnd(int minval, int maxval), bRnd(minval, maxval))
	WRAPPER(int, int SRnd(), sRnd())
	WRAPPER(double, double DRnd(), dRnd())
	WRAPPER(double, double ERnd(), eRnd())
	WRAPPER(int, int RndPct(int pct), rndPct(pct))
	WRAPPER(int, int RndPermil(int permil), rndPermil(permil))

#undef WRAPPER
};
