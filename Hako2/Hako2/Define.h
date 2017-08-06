#define _CRT_SECURE_NO_WARNINGS

// 定番 {
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <mbstring.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h> // sys/stat.h より先であること。
#include <sys\stat.h>
#include <time.h>

#include <windows.h>
// }

#include <DxLib.h>

//*
#define LOG_ENABLED 0
/*/
#define LOG_ENABLED 1
//*/

#define SCREEN_W 800
#define SCREEN_H 600
#define SCREEN_CENTER_X (SCREEN_W / 2)
#define SCREEN_CENTER_Y (SCREEN_H / 2)
#define SCREEN_W_MAX (SCREEN_W * 5)
#define SCREEN_H_MAX (SCREEN_H * 5)

typedef unsigned char uchar;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint;
typedef unsigned __int64 uint64;

#define IMAX 1000000000
#define PI 3.141592653589793

#define lengthof(list) \
	(sizeof((list)) / sizeof(*(list)))

#define m_isEmpty(str) \
	(!(str) || !*(str))

#define m_min(value1, value2) \
	((value1) < (value2) ? (value1) : (value2))

#define m_max(value1, value2) \
	((value1) < (value2) ? (value2) : (value1))

#define m_minim(var, value) \
	((var) = m_min((var), (value)))

#define m_maxim(var, value) \
	((var) = m_max((var), (value)))

#define m_range(var, minval, maxval) \
	do { \
	m_maxim((var), (minval)); \
	m_minim((var), (maxval)); \
	} while(0)

#define m_rate(var) \
	m_range((var), 0.0, 1.0)

#define m_isRange(value, minval, maxval) \
	((minval) <= (value) && (value) <= (maxval))

#define m_iSign(value) \
	((value) < 0 ? -1 : 0 < (value) ? 1 : 0)

#define m_d2i(value) \
	((int)((value) + 0.5))

#define m_nd2i(value) \
	((int)((value) + ((value) < 0.0 ? -0.5 : 0.5)))

#define m_accur(var, denom) \
	(m_d2i((var) * (denom)) / (denom))

#define m_pow(value) \
	((value) * (value))

#define ENUM_RANGE(enum_member, num) \
	enum_member, \
	enum_member##_END = enum_member + (num) - 1,

/*
	ex.
		oneObject(int, (int *)na(int, 100), GetIntList);         ... プロトタイプ宣言は oneObjectProto(int, GetIntList);
		static oneObject(ClassABC, new ClassABC(), GetClassABC); ... static の場合
*/
#define oneObject(type_t, init_op, getter) \
	type_t *(getter)(void) { \
		static type_t *p; \
		if(!p) { \
			p = (init_op); \
		} \
		return p; \
	}

#define oneObjectProto(type_t, getter) \
	type_t *(getter)(void)

/*
	終了時の counter ... (-1): 放した, 0: 放している, 1: 押した, 2-: 押している
*/
#define updateInput(counter, status) \
	if((status)) { \
		(counter) = m_max(0, (counter)); \
		(counter)++; \
	} \
	else { \
		(counter) = 0 < (counter) ? -1 : 0; \
	}

#define m_countDown(var) \
	((var) < 0 ? (var)++ : 0 < (var) ? (var)-- : (var))

#define m_approach(var, dest, rate) \
	do { \
	(var) -= (dest); \
	(var) *= (rate); \
	(var) += (dest); \
	} while(0)

#define m_iApproach(var, dest, rateNumer, rateDenom) \
	do { \
	(var) -= (dest); \
	(var) *= (rateNumer); \
	(var) /= (rateDenom); \
	(var) += (dest); \
	} while(0)

#define m_hypot(x, y) \
	(sqrt(m_pow((double)(x)) * m_pow((double)(y))))

typedef struct i2D_st
{
	int X;
	int Y;
}
i2D_t;

typedef struct d2D_st
{
	double X;
	double Y;
}
d2D_t;

#define m_swap(var1, var2, TYPE_T) \
	do { \
	TYPE_T tmp = (TYPE_T)var1; \
	*(TYPE_T *)&var1 = (TYPE_T)var2; \
	*(TYPE_T *)&var2 = (TYPE_T)tmp; \
	} while(0)

#define m_simpleComp(v1, v2) \
	((v1) < (v2) ? -1 : ((v2) < (v1) ? 1 : 0))

#define m_01(flag) \
	((flag) == 0 ? 0 : 1)
