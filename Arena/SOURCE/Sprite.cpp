//
// sprite.cpp
// craig
//

#include "sprite.h"
#include "opengl.h"
#include "camera.h"
#include "tgaload.h"

#define MAX_SPRITES 400
#define MAX_SPRITEVERTS (MAX_SPRITES*4)
#define MAX_SPRITEINDEX (MAX_SPRITES*6)

typedef struct _SpriteBlock
{
	u16 iSpriteCount;
	u16 iSpriteLast;
	u16 iRenderCount;

	SpriteVertex SpriteVerts[MAX_SPRITEVERTS];

	Sprite SpriteList[MAX_SPRITES];

	_Sprite *pList;
}SpriteBlock;

static SpriteBlock Sprites[2];
static u16 pSpriteIndexes[MAX_SPRITEINDEX];
static u32 iSpriteTex;
static Image *pSpriteImage;

void Sprite_Init();
void Sprite_Exit();
void Sprite_Update();
void Sprite_Render();

//------------------------------------------------------------------

void APIENTRY Sprite_Exit(Object *pObject)
{
	Sprite_Exit();

	Mem_Free(pObject);
}

//------------------------------------------------------------------

void APIENTRY Sprite_Update(Object *pObject)
{
	Sprite_Update();
}

//------------------------------------------------------------------

void APIENTRY Sprite_Render(Object *pObject)
{
	if(!Object_bTransparent)
	{
		Object_AddTransparent(pObject);
		return;
	}

	Sprite_Render();
}

//------------------------------------------------------------------

Object *Sprite_Create()
{
	Object *pObject;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.updatefunc = Sprite_Update;
	Create.renderfunc = Sprite_Render;
	Create.exitfunc=Sprite_Exit;
	Create.iType=OBJECT_Sprites;
	Create.pName="sprites";

	pObject = (Object*)Object_Create(&Create, sizeof(Object));

	Sprite_Init();

	return pObject;
}

//------------------------------------------------------------------

void Sprite_Init()
{
	u16 i;
	u16 iCount=0;
	Sprites[SPR_ADD].iSpriteCount=0;
	Sprites[SPR_ADD].iSpriteLast=0;
	Sprites[SPR_ADD].pList=NULL;
	Sprites[SPR_ALPHA].iSpriteCount=0;
	Sprites[SPR_ALPHA].iSpriteLast=0;
	Sprites[SPR_ALPHA].pList=NULL;
	

	for(i=0;i<MAX_SPRITEINDEX;i+=6)
	{
		pSpriteIndexes[i+0]=iCount+0;
		pSpriteIndexes[i+1]=iCount+1;
		pSpriteIndexes[i+2]=iCount+2;
		pSpriteIndexes[i+3]=iCount+2;
		pSpriteIndexes[i+4]=iCount+3;
		pSpriteIndexes[i+5]=iCount+0;
		iCount+=4;
	}

	for(i=0;i<MAX_SPRITES;i++)
	{
		Sprites[SPR_ADD].SpriteList[i].bActive=false;
		Sprites[SPR_ALPHA].SpriteList[i].bActive=false;
	}

	pSpriteImage = LoadTGA("data/textures/sprites.tga",true,true);
	iSpriteTex = TgaLoad_genTexture(&pSpriteImage);
}

//------------------------------------------------------------------

void Sprite_Exit()
{
	ReleaseImage(pSpriteImage);
}

//------------------------------------------------------------------

void Sprite_UpdateBlock(SpriteBlock *pBlock)
{
	Camera *pCam=Camera_Current();

	pBlock->iRenderCount=0;

	if(!pCam)
		return;

	Matrix *pMat;

	Object_GetMatrixPtr((Object*)pCam, &pMat);

	Vec3 vecUp=pMat->GetColumn(1);
	Vec3 vecRight=pMat->GetColumn(0);

//	u16 i;

	Vec3 *pVec;
	Vec4 *pVecUV;

	float fSizeRX;
	float fSizeRY;
	float fSizeRZ;
	float fSizeUX;
	float fSizeUY;
	float fSizeUZ;

//	Sprite **ppSprite=pBlock->pSpriteList-1;
	Sprite *pSprite=pBlock->pList;
	SpriteVertex *pSVtx=&pBlock->SpriteVerts[0];
	pSVtx-=4;

//	for(i=0;i<MAX_SPRITES/*pBlock->iSpriteCount*/;i++)
	while(pSprite)
	{
	//	ppSprite++;
	//	pSprite=*ppSprite;
//pSprite++;
		if(!pSprite->bActive || !pSprite->uColour)
		{
			pSprite=pSprite->pNext;
			continue;
		}

		pBlock->iRenderCount++;
		pSVtx+=4;

		pVec=&pSprite->vecPos;
		pVecUV=&pSprite->vecUVs;
		pSVtx[0].uColour=pSprite->uColour;
		pSVtx[1].uColour=pSprite->uColour;
		pSVtx[2].uColour=pSprite->uColour;
		pSVtx[3].uColour=pSprite->uColour;

		if(pSprite->bYFlat)
		{
			if(pSprite->fRot==0.f)
			{
				fSizeRX=-pSprite->fSize;
				fSizeRY=0.f;
				fSizeRZ=0.f;
				fSizeUX=0.f;
				fSizeUY=0.f;
				fSizeUZ=pSprite->fSize;
			}
			else
			{
				float fSin=g_MathLib.Sin(pSprite->fRot);
				float fCos=g_MathLib.Cos(pSprite->fRot);
				fSizeRX=-pSprite->fSize*(fSin);
				fSizeRY=0.f;
				fSizeRZ=-pSprite->fSize*(fCos);
				fSizeUX=-pSprite->fSize*(fCos);
				fSizeUY=0.f;
				fSizeUZ=pSprite->fSize*(fSin);
			}
		}
		else
		{
			if(pSprite->fRot==0.f)
			{
				fSizeRX=pSprite->fSize*vecRight.GetX();
				fSizeRY=pSprite->fSize*vecRight.GetY();
				fSizeRZ=pSprite->fSize*vecRight.GetZ();
				fSizeUX=pSprite->fSize*vecUp.GetX();
				fSizeUY=pSprite->fSize*vecUp.GetY();
				fSizeUZ=pSprite->fSize*vecUp.GetZ();
			}
			else
			{
				float fSin=g_MathLib.Sin(pSprite->fRot);
				float fCos=g_MathLib.Cos(pSprite->fRot);
				fSizeRX=pSprite->fSize*(vecRight.GetX()*fSin - vecUp.GetX()*fCos);
				fSizeRY=pSprite->fSize*(vecRight.GetY()*fSin - vecUp.GetY()*fCos);
				fSizeRZ=pSprite->fSize*(vecRight.GetZ()*fSin - vecUp.GetZ()*fCos);
				fSizeUX=pSprite->fSize*(vecUp.GetX()*fSin + vecRight.GetX()*fCos);
				fSizeUY=pSprite->fSize*(vecUp.GetY()*fSin + vecRight.GetY()*fCos);
				fSizeUZ=pSprite->fSize*(vecUp.GetZ()*fSin + vecRight.GetZ()*fCos);
			}
		}

		pSVtx[0].x=pVec->GetX() - fSizeRX + fSizeUX;
		pSVtx[0].y=pVec->GetY() - fSizeRY + fSizeUY;
		pSVtx[0].z=pVec->GetZ() - fSizeRZ + fSizeUZ;
		pSVtx[0].u=pVecUV->GetX();
		pSVtx[0].v=pVecUV->GetY();

		pSVtx[1].x=pVec->GetX() + fSizeRX + fSizeUX;
		pSVtx[1].y=pVec->GetY() + fSizeRY + fSizeUY;
		pSVtx[1].z=pVec->GetZ() + fSizeRZ + fSizeUZ;
		pSVtx[1].u=pVecUV->GetZ();
		pSVtx[1].v=pVecUV->GetY();

		pSVtx[2].x=pVec->GetX() + fSizeRX - fSizeUX;
		pSVtx[2].y=pVec->GetY() + fSizeRY - fSizeUY;
		pSVtx[2].z=pVec->GetZ() + fSizeRZ - fSizeUZ;
		pSVtx[2].u=pVecUV->GetZ();
		pSVtx[2].v=pVecUV->GetW();

		pSVtx[3].x=pVec->GetX() - fSizeRX - fSizeUX;
		pSVtx[3].y=pVec->GetY() - fSizeRY - fSizeUY;
		pSVtx[3].z=pVec->GetZ() - fSizeRZ - fSizeUZ;
		pSVtx[3].u=pVecUV->GetX();
		pSVtx[3].v=pVecUV->GetW();

		pSprite=pSprite->pNext;
	}
}

//------------------------------------------------------------------
typedef struct _SpriteSort
{
	float fZ;
	Sprite *pmySprite;
}SpriteSort;

i32 _SortSprite(const void *A, const void *B)
{
	SpriteSort *sortA=(SpriteSort*)A;
	SpriteSort *sortB=(SpriteSort*)B;

	if(sortA->fZ<sortB->fZ)
		return true;

	return false;
}

void Sprite_Zsort()
{
	Camera *pCam=Camera_Current();

	Sprites[SPR_ALPHA].iRenderCount=0;

	if(!pCam)
		return;

	Matrix *matInv;
	i32 i=0;
	
	Matrix *pMat;

	Object_GetMatrixPtr((Object*)pCam, &pMat);

	matInv = Camera_Inverse();

	SpriteSort *sort=(SpriteSort*)Mem_New(sizeof(SpriteSort)*Sprites[SPR_ALPHA].iSpriteCount);
//	Sprite **ppSprite=Sprites[SPR_ALPHA].pSpriteList;
//	Sprite *pSprite=Sprites[SPR_ALPHA].SpriteList;//*ppSprite;
	Sprite *pSprite=Sprites[SPR_ALPHA].pList;
	Vec4 vecCheck;

//	for(i=0;i<Sprites[SPR_ALPHA].iSpriteCount;i++)
	while(pSprite)
	{
		vecCheck=pSprite->vecPos;
		vecCheck=*matInv*vecCheck;
		sort[i].fZ=vecCheck.GetZ();
		sort[i].pmySprite=pSprite;
		pSprite=pSprite->pNext;
		i++;
	//	ppSprite++;
	//	pSprite++;//=*ppSprite;
	}

	qsort(sort,Sprites[SPR_ALPHA].iSpriteCount,sizeof(SpriteSort),_SortSprite);

	Vec3 vecUp=pMat->GetColumn(1);
	Vec3 vecRight=pMat->GetColumn(0);

	Vec3 *pVec;
	Vec4 *pVecUV;

	float fSizeRX;
	float fSizeRY;
	float fSizeRZ;
	float fSizeUX;
	float fSizeUY;
	float fSizeUZ;

	SpriteVertex *pSVtx=&Sprites[SPR_ALPHA].SpriteVerts[0];
	pSVtx-=4;

	for(i=0;i<Sprites[SPR_ALPHA].iSpriteCount;i++)
	{
		pSprite=sort[i].pmySprite;
		
		if(!pSprite->uColour)
			continue;

		Sprites[SPR_ALPHA].iRenderCount++;
		pSVtx+=4;

		pVec=&pSprite->vecPos;
		pVecUV=&pSprite->vecUVs;
		
		pSVtx[0].uColour=pSprite->uColour;
		pSVtx[1].uColour=pSprite->uColour;
		pSVtx[2].uColour=pSprite->uColour;
		pSVtx[3].uColour=pSprite->uColour;

		if(pSprite->fRot==0.f)
		{
			fSizeRX=pSprite->fSize*vecRight.GetX();
			fSizeRY=pSprite->fSize*vecRight.GetY();
			fSizeRZ=pSprite->fSize*vecRight.GetZ();
			fSizeUX=pSprite->fSize*vecUp.GetX();
			fSizeUY=pSprite->fSize*vecUp.GetY();
			fSizeUZ=pSprite->fSize*vecUp.GetZ();
		}
		else
		{
			float fSin=g_MathLib.Sin(pSprite->fRot);
			float fCos=g_MathLib.Cos(pSprite->fRot);
			fSizeRX=pSprite->fSize*(vecRight.GetX()*fSin - vecUp.GetX()*fCos);
			fSizeRY=pSprite->fSize*(vecRight.GetY()*fSin - vecUp.GetY()*fCos);
			fSizeRZ=pSprite->fSize*(vecRight.GetZ()*fSin - vecUp.GetZ()*fCos);
			fSizeUX=pSprite->fSize*(vecUp.GetX()*fSin + vecRight.GetX()*fCos);
			fSizeUY=pSprite->fSize*(vecUp.GetY()*fSin + vecRight.GetY()*fCos);
			fSizeUZ=pSprite->fSize*(vecUp.GetZ()*fSin + vecRight.GetZ()*fCos);
		}

		pSVtx[0].x=pVec->GetX() - fSizeRX + fSizeUX;
		pSVtx[0].y=pVec->GetY() - fSizeRY + fSizeUY;
		pSVtx[0].z=pVec->GetZ() - fSizeRZ + fSizeUZ;
		pSVtx[0].u=pVecUV->GetX();
		pSVtx[0].v=pVecUV->GetY();

		pSVtx[1].x=pVec->GetX() + fSizeRX + fSizeUX;
		pSVtx[1].y=pVec->GetY() + fSizeRY + fSizeUY;
		pSVtx[1].z=pVec->GetZ() + fSizeRZ + fSizeUZ;
		pSVtx[1].u=pVecUV->GetZ();
		pSVtx[1].v=pVecUV->GetY();

		pSVtx[2].x=pVec->GetX() + fSizeRX - fSizeUX;
		pSVtx[2].y=pVec->GetY() + fSizeRY - fSizeUY;
		pSVtx[2].z=pVec->GetZ() + fSizeRZ - fSizeUZ;
		pSVtx[2].u=pVecUV->GetZ();
		pSVtx[2].v=pVecUV->GetW();

		pSVtx[3].x=pVec->GetX() - fSizeRX - fSizeUX;
		pSVtx[3].y=pVec->GetY() - fSizeRY - fSizeUY;
		pSVtx[3].z=pVec->GetZ() - fSizeRZ - fSizeUZ;
		pSVtx[3].u=pVecUV->GetX();
		pSVtx[3].v=pVecUV->GetW();

	}

	Mem_Free(sort);
}

//------------------------------------------------------------------

void Sprite_Update()
{
	Sprite_UpdateBlock(&Sprites[SPR_ADD]);
//	Sprite_UpdateBlock(&Sprites[SPR_ALPHA]);

	Sprite_Zsort();
}

//------------------------------------------------------------------

void Sprite_Render()
{
	if(Sprites[SPR_ADD].iRenderCount)
	{
		GL_RenderMode(RENDER_MODE_TEXTURE_ADD);
		GL_SetTexture(iSpriteTex);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);


		glVertexPointer(3, GL_FLOAT, sizeof(SpriteVertex), &Sprites[SPR_ADD].SpriteVerts[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &Sprites[SPR_ADD].SpriteVerts[0].u);
		
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SpriteVertex), &Sprites[SPR_ADD].SpriteVerts[0].uColour);

		glDrawElements(GL_TRIANGLES,Sprites[SPR_ADD].iRenderCount*6,GL_UNSIGNED_SHORT,&pSpriteIndexes[0]);

#ifdef _DEBUG
		uPolycount+=(Sprites[SPR_ADD].iRenderCount*2);
#endif

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	if(Sprites[SPR_ALPHA].iRenderCount)
	{
		GL_RenderMode(RENDER_MODE_ALPHATEXTURE);
		GL_SetTexture(iSpriteTex);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(SpriteVertex), &Sprites[SPR_ALPHA].SpriteVerts[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &Sprites[SPR_ALPHA].SpriteVerts[0].u);
		
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SpriteVertex), &Sprites[SPR_ALPHA].SpriteVerts[0].uColour);

		glDrawElements(GL_TRIANGLES,Sprites[SPR_ALPHA].iRenderCount*6,GL_UNSIGNED_SHORT,&pSpriteIndexes[0]);

#ifdef _DEBUG
		uPolycount+=(Sprites[SPR_ALPHA].iRenderCount*2);
#endif

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
}

//------------------------------------------------------------------

Sprite* Sprite_Add(Vec3 &vecPos,Vec4 &vecUVs,float fSize,u32 uColour,i32 iMode)
{
	u16 i;

	ASSERT(Sprites[iMode].iSpriteCount<MAX_SPRITES,"Adding to many sprites");

	SpriteBlock *pBlock=&Sprites[iMode];
	Sprite *pSprite=NULL;

	for(i=pBlock->iSpriteLast;i<MAX_SPRITES;i++)
	{
		if(!pBlock->SpriteList[i].bActive)
		{
			pSprite=&pBlock->SpriteList[i];
			break;
		}
	}

	if(i==MAX_SPRITES && pSprite==NULL)
	{
		pBlock->iSpriteLast=0;
		for(i=pBlock->iSpriteLast;i<MAX_SPRITES;i++)
		{
			if(!pBlock->SpriteList[i].bActive)
			{
				pSprite=&pBlock->SpriteList[i];
				break;
			}
		}
	}
	pBlock->iSpriteLast=i;

	ASSERT(!pSprite->bActive,"sprite already active");

	pSprite->vecPos=vecPos;
	pSprite->vecUVs=vecUVs;
	pSprite->fSize=fSize;
	pSprite->uColour=uColour;
	pSprite->fRot=0.f;
	pSprite->iMode=iMode;
	pSprite->bActive=true;
	pSprite->bYFlat=false;

//	pBlock->pSpriteList[i]=pSprite;

	pBlock->iSpriteCount++;

	if(!pBlock->pList)
	{
		pBlock->pList=pSprite;
		pSprite->pNext=NULL;
	}
	else
	{
		pSprite->pNext=pBlock->pList;
		pBlock->pList=pSprite;
	}

	return pSprite;
}

//------------------------------------------------------------------

void Sprite_Remove(Sprite *pSprite)
{
//	u16 i;

	
	SpriteBlock *pBlock=&Sprites[pSprite->iMode];

	pBlock->iSpriteCount--;

//	for(i=0;i<MAX_SPRITES;i++)
//	{
//		if(pBlock->pSpriteList[i]==pSprite)
//		{
//			pBlock->pSpriteList[i]=pBlock->pSpriteList[pBlock->iSpriteCount];
//			break;
//		}
//	}

	pSprite->bActive=false;

	Sprite *pSpriteList=pBlock->pList;
	Sprite *pSpriteLast=NULL;

	while(pSpriteList)
	{
		if(pSpriteList==pSprite)
		{
			if(pSpriteLast)
			{
				pSpriteLast->pNext=pSpriteList->pNext;
			}
			else
			{
				pBlock->pList=pBlock->pList->pNext;
			}
			break;
		}
		pSpriteLast=pSpriteList;
		pSpriteList=pSpriteList->pNext;
	}
}

//------------------------------------------------------------------

