//
// tgaload.h
// craig
//

#ifndef _TGALOAD_H
#define _TGALOAD_H

#include "includeall.h"

typedef struct _Image
{
	i32 iWidth;
	i32 iHeight;
	i32 iBPP;
	u8	*pData;
}Image;

void TgaLoad_Init();
void TgaLoad_Exit();
Image* LoadTGA(const char *pFilename,bool bFlip=true,bool brbSwap=false);
void ReleaseImage(Image *pImage);
void ReleaseTexture(u32 uTexID);
u32 TgaLoad_genTexture(Image **ppImage, i32 iLevels = 1);
#endif

