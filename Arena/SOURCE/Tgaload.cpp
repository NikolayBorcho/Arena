//
// tgaload.cpp
// craig
//

#include <stdio.h>
#include "tgaload.h"
#include "opengl.h"

#define MAX_IMAGES	60

typedef struct _ImageStore
{
	Image		*pImage;
	char name[32];
	bool bFlip;
	bool brbSwap;
	i32		iRefCount;
	u32		uTexID;
}ImageStore;

typedef struct _ImageBucket
{
	u32		ImageCount;	
	ImageStore	ImageList[MAX_IMAGES];
}ImageBucket;

static ImageBucket Bucket;

//------------------------------------------------------------------

void TgaLoad_Init()
{
	memset(&Bucket, 0, sizeof(ImageBucket));
}

//------------------------------------------------------------------

void TgaLoad_Exit()
{
	while(Bucket.ImageCount)
	{
		if(Bucket.ImageList[Bucket.ImageCount-1].uTexID)
		{
			glDeleteTextures(1,&Bucket.ImageList[Bucket.ImageCount-1].uTexID);
		}
		ReleaseImage(Bucket.ImageList[Bucket.ImageCount-1].pImage);
	}
}

//------------------------------------------------------------------

Image* LoadTGA(const char *pFilename,bool bFlip,bool brbSwap)
{
	u8		TGAcompare[12];	
	u8		header[6];
	u32		bytesPerPixel;
	u32		imageSize;
	u32		type=GL_RGBA;
	FILE *file ;
	const char *pName;
	const char *pNamePtr;
	i32 i;

	pName = pFilename;
	pNamePtr = pFilename;

	while(*pNamePtr)
	{
		if(*pNamePtr == '/' || *pNamePtr == '\\')
		{
			pName = pNamePtr+1;
		}

		pNamePtr++;
	}
	ASSERT(*pName, "no image name");
	ASSERT(strlen(pName)<32, "image name too long");

	for(i=0;(u32)i<Bucket.ImageCount;i++)
	{
		if(strcmp(pName,Bucket.ImageList[i].name)==0)
		{
			if(Bucket.ImageList[i].bFlip == bFlip && Bucket.ImageList[i].brbSwap == brbSwap)
			{
				Bucket.ImageList[i].iRefCount++;
				return Bucket.ImageList[i].pImage;
			}
		}
	}

	if((file = fopen(pFilename,"rb"))==NULL) 
		return NULL;					
	
	if(	fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare))	
	{
		fclose(file);							
		return NULL;						
	}
	
	if(	fread(header,1,sizeof(header),file)!=sizeof(header))		
	{
		fclose(file);							
		return NULL;						
	}

	i32 width  = header[1] * 256 + header[0];				
	i32 height = header[3] * 256 + header[2];			

	if(	width	<=0	||					
		height	<=0	||					
		(header[4]!=8 && header[4]!=24 && header[4]!=32))				
	{
		fclose(file);						
		return NULL;								
	}
		
	i32 bpp	= header[4];			
	bytesPerPixel	= bpp/8;						
	imageSize	= width*height*bytesPerPixel;			
	
	u8 *imageData=(u8 *)Mem_New(sizeof(u8)*imageSize);	
	
	if(	imageData==NULL ||					
		fread(imageData, 1, imageSize, file)!=imageSize)	
	{
		if(imageData!=NULL)				
			Mem_Free(imageData);				
		
		fclose(file);							
		return NULL;				
	}
	
	fclose (file);

	i32 iY;
	if(brbSwap)
	{
		u8 imageTemp;
		u32 uCount;
		for(uCount=0;uCount<imageSize;uCount+=bytesPerPixel)
		{
			imageTemp=imageData[uCount + 0];
			imageData[uCount + 0]=imageData[uCount + 2];
			imageData[uCount + 2]=imageTemp;
		}
	}

	if(bFlip)
	{
		u8 *imageDataFlip=(u8 *)Mem_New(sizeof(u8)*imageSize);
		
		for(iY=0;iY<height;iY++)
		{
			memcpy(&imageDataFlip[((height-1)-iY)*width*bytesPerPixel],&imageData[iY*width*bytesPerPixel],width*bytesPerPixel);
		}

		Mem_Free(imageData);
		imageData=imageDataFlip;
	}

	Image *pImage=(Image *)Mem_New(sizeof(Image));

	pImage->iWidth=width;
	pImage->iHeight=height;
	pImage->iBPP=bytesPerPixel;
	pImage->pData=imageData;

	Bucket.ImageList[Bucket.ImageCount].pImage = pImage;
	Bucket.ImageList[Bucket.ImageCount].bFlip = bFlip;
	Bucket.ImageList[Bucket.ImageCount].brbSwap = brbSwap;
	strcpy(Bucket.ImageList[Bucket.ImageCount].name,pName);
	Bucket.ImageList[i].iRefCount++;

	Bucket.ImageCount++;

	ASSERT(Bucket.ImageCount<MAX_IMAGES, "too many images");

	return pImage;
}

//------------------------------------------------------------------

void ReleaseImage(Image *pImage)
{
	i32 i;

	for(i=0;(u32)i<Bucket.ImageCount;i++)
	{
		if(Bucket.ImageList[i].pImage == pImage)
		{
			Bucket.ImageList[i].iRefCount--;
			if(Bucket.ImageList[i].iRefCount)
			{
				return;
			}
			Mem_Free(Bucket.ImageList[i].pImage->pData);
			Mem_Free(Bucket.ImageList[i].pImage);

			if(Bucket.ImageList[i].uTexID)
			{
				glDeleteTextures(1,&Bucket.ImageList[i].uTexID);
			}

			Bucket.ImageCount--;
			Bucket.ImageList[i] = Bucket.ImageList[Bucket.ImageCount];
			Bucket.ImageList[Bucket.ImageCount].uTexID = 0;
			
			break;
		}
	}
}

//------------------------------------------------------------------

void ReleaseTexture(u32 uTexID)
{
	i32 i;

	for(i=0;(u32)i<Bucket.ImageCount;i++)
	{
		if(Bucket.ImageList[i].uTexID == uTexID)
		{
			ReleaseImage(Bucket.ImageList[i].pImage);
			break;
		}
	}
}

//------------------------------------------------------------------

u32 TgaLoad_genTexture(Image **ppImage, i32 iLevels)
{
	u32 uID;
	i32 i;
	ImageStore *pStore = NULL;

	for(i=0;(u32)i<Bucket.ImageCount;i++)
	{
		if(Bucket.ImageList[i].pImage == ppImage[0])
		{
			if(Bucket.ImageList[i].uTexID)
			{
				return Bucket.ImageList[i].uTexID;
			}
			pStore = &Bucket.ImageList[i];
		}
	}

	glGenTextures(1, &uID);						// Generate OpenGL texture IDs	
	
	glBindTexture(GL_TEXTURE_2D, uID);

	if(iLevels>1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR/*GL_NEAREST*/);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for(i=0;i<iLevels;i++)
	{
		if(ppImage[i]->iBPP==4)
		{
			glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, ppImage[i]->iWidth, ppImage[i]->iHeight, 
				0, GL_RGBA, GL_UNSIGNED_BYTE, ppImage[i]->pData);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, ppImage[i]->iWidth, ppImage[i]->iHeight, 
				0, GL_RGB, GL_UNSIGNED_BYTE, ppImage[i]->pData);
		}
	}

	if(pStore)
	{
		pStore->uTexID = uID;
	}

	return uID;
}
