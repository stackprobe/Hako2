void SW_Start2(void);
void SW_Lap2(char *source, int lineno, char *function);
void SW_End2(char *source, int lineno, char *function);

__int64 GetPerformanceCounter(void);
__int64 GetPerformanceFrequency(void);

#if LOG_ENABLED

#define SW_Start() SW_Start2()
#define SW_Lap() SW_Lap2(__FILE__, __LINE__, __FUNCTION__)
#define SW_End() SW_End2(__FILE__, __LINE__, __FUNCTION__)

#else

#define SW_Start() 1
#define SW_Lap() 1
#define SW_End() 1

#endif
