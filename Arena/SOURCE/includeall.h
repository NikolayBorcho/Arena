//
// includeall.h
// craig
//

#ifndef _INCLUDEALL_H_
#define _INCLUDEALL_H_
#include <windows.h>
#include <CRTDBG.H>
#include "mathlib.h"

typedef unsigned int u32;
typedef int i32;
typedef unsigned short int u16;
typedef short int i16;
typedef unsigned char u8;

#define SWITCHCOL(a) ( ((a&0xff000000)) + ((a&0x00ff0000)>>16) + ((a&0x0000ff00)) + ((a&0x000000ff)<<16) )
#define SWIDTH 640
#define SHEIGHT 480

u32 StringKey( const char *pString );

#define WM_SOCKET (WM_USER+1)
#include "memory.h"
#include <stdio.h>
#ifdef _DEBUG

extern struct _Font *pDebugFont;

extern u32 uPolycount;



#define ASSERT(a,b)\
if( !(a) )\
{\
	char cBuff[256];\
	sprintf_s(cBuff,"%s\nFile: %s\nLine: %d",b,__FILE__,__LINE__);\
	MessageBox(NULL,cBuff,"ERROR",MB_OK|MB_ICONEXCLAMATION);\
	_asm {int 3};\
};
#else
#define ASSERT(a,b)
#endif

#include "vector.h"
#include "matrix.h"

#endif //_INCLUDEALL_H_