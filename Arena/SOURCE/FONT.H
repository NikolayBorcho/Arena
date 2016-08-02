//
// font.h
// craig
//

#ifndef _FONT_H
#define _FONT_H

#include "includeall.h"

typedef struct _FontElement

{
	float width;
	float height;
	float u1;
	float u2;
	float v1;	
	float v2;	
#ifdef _DEBUG
	bool bInitalised;
#endif
}FontElement;

typedef struct _Font
{
	FontElement elements[256];	
	u32 iTexID;
}Font;

enum FONT_JUSTIFY
{
	FONT_LEFT=0,
	FONT_MIDDLE,
	FONT_RIGHT
};

void Font_Init();
void Font_Exit();
void SetFont(FONT_JUSTIFY justify,Font *pFont);
void GetStringWH(const char *pString,float fScale,float *pfWidth,float *pfHeight)	;
void PrintString(float fX,float fY,u32 iColour,float fScale,const char *pString,...);
Font* Font_Load(const char *FontName);
void Font_Destroy(Font *pFont);
void Font_ViewPort(float xnewViewPortX,float xnewViewPortY,float xnewViewPortW,float xnewViewPortH);
#endif

