//
// font.cpp
// craig
//

#include "font.h"
#include "opengl.h"
#include "tgaload.h"

static float xViewPortX;
static float xViewPortY;
static float xViewPortW;
static float xViewPortH;

static Font *pThisFont;
static FONT_JUSTIFY Font_Just;

//------------------------------------------------------------------

void Font_Init()
{
	xViewPortX=0.f;
	xViewPortY=0.f;
	xViewPortW=320;
	xViewPortH=240;
	Font_Just=FONT_LEFT;
}

//------------------------------------------------------------------

void Font_Exit()
{
//	glDeleteTextures(1, &uFontID);
}

//------------------------------------------------------------------

void SetFont(FONT_JUSTIFY justify,Font *pFont)								
{
	pThisFont=pFont;
	Font_Just=justify;
	GL_SetTexture(pThisFont->iTexID);
	GL_RenderMode(RENDER_MODE_ALPHATEXTURE);
}

//----------------------------------------------------------------------------------

void GetStringWH(const char *pString,float fScale,float *pfWidth,float *pfHeight)								
{
	char c;
	const char *pChar=pString;
//	float fSizeX=Fixed(30.f)*fScale;
	float fSizeY=pThisFont->elements['A'].height*fScale;
	float fMaxWidth=0.f;
	float fLineWidth=0.f;
	float fHeight=fSizeY;
	FontElement *pEle=pThisFont->elements;

	while(*pChar)
	{
		c=(*pChar);

		if(c=='\n')
		{
			if(fLineWidth>fMaxWidth)
				fMaxWidth=fLineWidth;
			fLineWidth=0.f;
			fHeight+=(fSizeY);
		}
		else
		{
			fLineWidth+=(pEle[c].width*fScale);
		}
		pChar++;
	}
	if(fLineWidth>fMaxWidth)
		fMaxWidth=fLineWidth;

	if(pfWidth)
		*pfWidth=fMaxWidth;

	if(pfHeight)
		*pfHeight=fHeight;
}



//----------------------------------------------------------------------------------

static void PrintString2(const char *pString,float fX,float fY,u32 iColour,float fScale)								
{
	char c;
	const char *pChar=pString;
	float fXStart;
	u32 iColour2=iColour;
	FontElement *pEle=pThisFont->elements;
	
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	GL_Ortho(SWIDTH,SHEIGHT);
	glDepthMask(0);	// disable zbuffer writes
	GL_PrimitiveStart(PRIM_TYPE_QUADLIST, RENDER_COLOUR|RENDER_UVs);

	float fSizeX=pEle['A'].width*fScale;
	float fSizeY=pEle['A'].height*fScale;

	float fWidth;
	float fHeight;
	switch(Font_Just)
	{
	case FONT_LEFT:
		GetStringWH(pString,fScale,&fWidth,&fHeight);
		break;
	case FONT_MIDDLE:
		GetStringWH(pString,fScale,&fWidth,&fHeight);
		fX=fX-fWidth/2.f;
		break;
	case FONT_RIGHT:
		GetStringWH(pString,fScale,&fWidth,&fHeight);
		fX=fX-fWidth;
		break;
	}

	fX+=xViewPortX;
	fY+=xViewPortY;

	fXStart=fX;

	char cWord[64];
	char *pcWord;
	const char *pChar2;
	bool CheckWord=false;
	float xWordLen;
	if(fWidth>xViewPortW)
	{
		CheckWord=true;
	}
	
	while(*pChar)
	{
		c=(*pChar);

		if(c=='^')
		{
			pChar++;
			switch((*pChar))
			{
			case '0':
				iColour2=0xff0000ff;
				break;
			case '1':
				iColour2=0xff00ff00;
				break;
			case '2':
				iColour2=0xffff0000;
				break;
			case '3':
				iColour2=0xff0000ff;
				break;
			case '4':
				iColour2=0xffffffff;
				break;
			case '5':
				iColour2=0xff0000ff;
				break;
			case '6':
				iColour2=0xff0000ff;
				break;
			}
		}
		else
		{
			if(c<32)
			{
				fX=fXStart;
				fY+=(fSizeY);
				iColour2=iColour;
			}
			else
			{
				if(CheckWord && c==32)
				{
					pcWord=cWord;
					pChar2=pChar;
					xWordLen=0.f;
					while(true)
					{
						*pcWord=*pChar2;
						pcWord++;
						pChar2++;
						
						if(/**pChar2=='\0' || *pChar2=='\n' || *pChar2=='\r' ||
							*pChar2==' '*/*pChar2<=32)
						{
							*pcWord=0;
							break;
						}
						xWordLen+=pEle[c].width;
						
					}
					if(xWordLen+fX>xViewPortW)
					{
						// move to a new line
						fX=fXStart;
						fY+=(fSizeY);
						iColour2=iColour;
					}
				}
#ifdef _DEBUG
				char cString[128];
				if(!pEle[c].bInitalised)
				{
					sprintf(cString,"%c not in font",c);
					ASSERT(0,cString);
				}
#endif
				fSizeX=pEle[c].width*fScale;

				GL_Quad(fX,fY, 0.f, 
					fSizeX, pEle[c].height*fScale,
					pEle[c].u1,pEle[c].v1,
					pEle[c].u2,pEle[c].v2,
					iColour2);


					fX+=fSizeX;
			}
		}
		pChar++;
	}

	GL_RenderPrimitives();

	glDepthMask(1);	// enable zbuffer writes

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------------

void PrintString(float fX,float fY,u32 iColour,float fScale,const char *pString,...)								
{
	char ctext[1024];
	va_list ap;

	va_start(ap, pString);
	vsprintf(ctext,pString,ap);
	va_end(ap);

	PrintString2(ctext,fX,fY,iColour,fScale);
}

//------------------------------------------------------------------

static i32 _GetNum(char *pLine)

{	
	char cNumber[64];	
	char *pNumber=cNumber;
	
	while(*pLine!='"')
		pLine++;
	
	pLine++;
	
	while(*pLine!='"')	
	{
		*pNumber=*pLine;	
		pNumber++;
		pLine++;
	}
	*pNumber=0;

	return atoi(cNumber);	
}

//------------------------------------------------------------------

Font* Font_Load(const char *FontName)
{
	char cBuff[256];	
	char cLine[512];
	char *pLine;
	
	Font *pFont=(Font*)Mem_New(sizeof(Font));
	char *pFileBuff;
	char *pFileBuffPtr;
	i32 iNum;
	
	memset(pFont,0,sizeof(Font));
	FontElement *pCurrentChar;
	
	sprintf(cBuff,"%s.tga",FontName);
	
	// load tga
	
	Image *pImage[1];
	
	pImage[0]=LoadTGA(cBuff,true,true);
	pFont->iTexID=TgaLoad_genTexture(pImage);
	
	
	sprintf(cBuff,"%s.xml",FontName);
	
	// load xml
	
	FILE *pXML;
	pXML=fopen(cBuff,"rb");
	
	if(!pXML)		
	{
		return 0;	
	}
	else	
	{
		pFileBuff=(char*)Mem_New(sizeof(char)*36*1024);	
		pFileBuffPtr=pFileBuff;
		u32 uSize=fread(pFileBuff,sizeof(char),36*1024,pXML);
	
		while(uSize)		
		{
			pLine=cLine;	
			
			while(uSize)	
			{
				*pLine=*pFileBuffPtr;
				
				pFileBuffPtr++;
				
				if(*pLine==' ')		
				{
					*pLine=0;	
					break;
				}
				
				pLine++;
				
				uSize--;
			}
			
			if(strstr(cLine,"id="))
			{
				iNum=_GetNum(cLine);
				
				ASSERT(iNum<256,"illegal font number");
				pCurrentChar=&pFont->elements[(unsigned char)iNum];	
#ifdef _DEBUG
				pCurrentChar->bInitalised=true;
#endif
			}
			else if(strstr(cLine,"x="))	
			{
				iNum=_GetNum(cLine);
				
				ASSERT(pCurrentChar,"error parsing font");
				pCurrentChar->u1=iNum/(float)pImage[0]->iWidth;	
			}
			else if(strstr(cLine,"y="))	
			{
				iNum=_GetNum(cLine);
				
				ASSERT(pCurrentChar,"error parsing font");
				pCurrentChar->v1=iNum/(float)pImage[0]->iHeight;	
			}
			else if(strstr(cLine,"width="))	
			{
				iNum=_GetNum(cLine);
				
				ASSERT(pCurrentChar,"error parsing font");
				pCurrentChar->u2=pCurrentChar->u1 + iNum/(float)pImage[0]->iWidth;	
				pCurrentChar->width=(float)iNum;
			}
			else if(strstr(cLine,"height="))	
			{
				iNum=_GetNum(cLine);
				
				ASSERT(pCurrentChar,"error parsing font");
				pCurrentChar->v2=pCurrentChar->v1 + iNum/(float)pImage[0]->iHeight;
				pCurrentChar->height=(float)iNum;
			}
			else
			{
				pCurrentChar=NULL;
			}
		}
	}
	
	fclose(pXML);
	
	Mem_Free(pFileBuff);
	
	return pFont;
}

//------------------------------------------------------------------

void Font_Destroy(Font *pFont)
{
	ReleaseTexture(pFont->iTexID);
	
	Mem_Free(pFont);	
}

//------------------------------------------------------------------

void Font_ViewPort(float xnewViewPortX,float xnewViewPortY,float xnewViewPortW,float xnewViewPortH)
{
	xViewPortX=xnewViewPortX;
	xViewPortY=xnewViewPortY;
	xViewPortW=xnewViewPortW;
	xViewPortH=xnewViewPortH;
}