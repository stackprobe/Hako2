void addFinalizer(void (*func)(void));
void removeFinalizer(void (*func)(void));

void termination(int errorLevel = 0);

#define errorCase(status) \
	do { \
	if((status)) { \
		error(); \
	} \
	} while(0)

#define error() \
	error2(__FILE__, __LINE__, __FUNCTION__)

void error2(char *file, int lineno, char *function);

FILE *GetLogFp(void);

#if LOG_ENABLED
#define LOG(format, ...) (fprintf(GetLogFp(), format, ## __VA_ARGS__), fflush(GetLogFp()))
#else
#define LOG(format, ...) 1
#endif

#define LOGPOS() \
	LOG("[POS] %s %d %s\n", __FILE__, __LINE__, __FUNCTION__)

#if LOG_ENABLED
#define PDXLOGPOS() printfDx("[POS] %s %d %s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define PDXLOGPOS() 1
#endif

int hasArgs(int count);
int argIs(char *spell);
char *getArg(int index);
char *nextArg(void);

int IsWindowActive(void);
__int64 GetCurrTime(void);
