//
// Level.cpp
// craig
//


#include "tgaload.h"
#include "opengl.h"
#include "Level.h"
#include "camera.h"
#include "Collision.h"
#include "sprite.h"
#include "Pickup.h"

#define NUM_HOUSES	400
#define NUM_TREES	400

#define LEVEL_SIZE	32
#define DRAW_SIZE 7
#define DRAW_INDEXES (DRAW_SIZE*DRAW_SIZE*48)
#define LEVEL_SCALE 4.f
#define LEVEL_BOXES 500

static int		iCurrentVert;
static float	fHouseVerts[NUM_HOUSES*68][3];
static float	fHouseTex[NUM_HOUSES*68][2];
static float	fHouseCol[NUM_HOUSES*68][4];
static int		iMaxIndex;
static int		iIndex[NUM_HOUSES*68];
static int		iDrawIndex[DRAW_INDEXES];
static int		iDrawMaxIndex;

static int		iCurrentTreeVert;
static float	fTreeVerts[NUM_TREES*16][3];
static float	fTreeTex[NUM_TREES*16][2];
static float	fTreeCol[NUM_TREES*16][4];
static int		iMaxTreeIndex;
static int		iIndexTree[NUM_TREES*16];
static int		iDrawTreeIndex[DRAW_INDEXES/2];
static int		iDrawTreeMaxIndex;

static i32		iLastIndexX;
static i32		iLastIndexY;

static i32		iCollBoxes = 0;
static ColBox *pCollBoxes[LEVEL_BOXES];

static bool	bLevelStarted = false;

#define MAX_BOXES_GRID 40

// NIK: pick ups
#include <vector>
std::vector<Pickup*> stdvPickups;

typedef struct _GridIndex
{
	i32 iStartIndex;
	i32 iNumIndex;

	i32 iNumBoxes;
	i32 iNumBoxesSurrounding;
	ColBox *pCollBoxes[MAX_BOXES_GRID];
	ColBox *pCollBoxesSurrounding[MAX_BOXES_GRID];
}GridIndex;

static GridIndex GIList[LEVEL_SIZE*LEVEL_SIZE];
static GridIndex GITreeList[LEVEL_SIZE*LEVEL_SIZE];

void Level_GridIndexAddBox(Vec3 *pPos, ColBox *pBox)
{
	// workout index position
	i32 iIndexX = (int)((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE));
	i32 iIndexY = (int)((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE));

	GridIndex *pGrid;

	if(iIndexX<0)
		iIndexX = 0;
	if(iIndexX > LEVEL_SIZE)
		iIndexX = LEVEL_SIZE;
	if(iIndexY<0)
		iIndexY = 0;
	if(iIndexY > LEVEL_SIZE)
		iIndexY = LEVEL_SIZE;

	pGrid = &GIList[iIndexY * LEVEL_SIZE + iIndexX];

	ASSERT(pGrid->iNumBoxes<MAX_BOXES_GRID, "too many boxes for grid");
	pGrid->pCollBoxes[pGrid->iNumBoxes] = pBox;
	pGrid->iNumBoxes++;
}

void Level_GridIndexAddBoxSurround(GridIndex *pGrid, GridIndex *pGridSurround)
{
	i32 i;

	ASSERT(pGrid->iNumBoxesSurrounding<MAX_BOXES_GRID, "too many boxes for grid");

	for(i=0;i<pGridSurround->iNumBoxes;i++)
	{
		pGrid->pCollBoxesSurrounding[pGrid->iNumBoxesSurrounding +i] = pGridSurround->pCollBoxes[i];
	}

	pGrid->iNumBoxesSurrounding += pGridSurround->iNumBoxes;
}

void Level_GridIndexAddSurrounds()
{
	i32 i,j;

	GridIndex *pGrid;
	for(j=0;j<LEVEL_SIZE;j++)
	{
		pGrid = &GIList[j * LEVEL_SIZE];
		for(i=0;i<LEVEL_SIZE;i++)
		{
			pGrid->iNumBoxesSurrounding = 0;
			if(i<LEVEL_SIZE-1)
				Level_GridIndexAddBoxSurround(pGrid, (pGrid+1));
			if(i>0)
				Level_GridIndexAddBoxSurround(pGrid, (pGrid-1));

			if(j>0)
			{
				if(i>0)
					Level_GridIndexAddBoxSurround(pGrid, (pGrid-LEVEL_SIZE-1));
				Level_GridIndexAddBoxSurround(pGrid, (pGrid-LEVEL_SIZE));
				if(i<LEVEL_SIZE-1)
					Level_GridIndexAddBoxSurround(pGrid, (pGrid-LEVEL_SIZE+1));
			}

			if(j<LEVEL_SIZE-1)
			{
				if(i>0)
					Level_GridIndexAddBoxSurround(pGrid, (pGrid+LEVEL_SIZE-1));
				Level_GridIndexAddBoxSurround(pGrid, (pGrid+LEVEL_SIZE));
				if(i<LEVEL_SIZE-1)
					Level_GridIndexAddBoxSurround(pGrid, (pGrid+LEVEL_SIZE+1));
			}
			pGrid++;
		}
	}
}

GridIndex* Level_GetIndexGrid(Vec3 *pPos)
{
	// workout index position
	i32 iIndexX = (int)((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE));
	i32 iIndexY = (int)((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE));

	if(iIndexX<0)
		iIndexX = 0;
	if(iIndexX > LEVEL_SIZE)
		iIndexX = LEVEL_SIZE;
	if(iIndexY<0)
		iIndexY = 0;
	if(iIndexY > LEVEL_SIZE)
		iIndexY = LEVEL_SIZE;

	return &GIList[iIndexY * LEVEL_SIZE + iIndexX];
}

static float	fSkyVerts[16][3];
static float	fSkyTex[16][2];
static int		iSkyIndex[24];

static i32 iLightCount;
static Vec3 vecLights[(LEVEL_SIZE*LEVEL_SIZE)/3];
static i32 LightIndexes[(LEVEL_SIZE*LEVEL_SIZE)/3][2];
static Sprite *pLampSprites[(LEVEL_SIZE*LEVEL_SIZE)/3];
static i32 iLamps;
static u32 iTreeTex;
static u32 iHouseTex;
static u32 iSkyTex;

static Matrix HouseMat;

static char *LevelNames[] =
{
	"data/levels/Level01.tga"
};

static i32 iNumLevels = 1;

static Image **ppLevelImages;

#define AddIndex(iIndex,iMaxIndex)\
	iIndex[iMaxIndex]=iMaxIndex;\
	iIndex[iMaxIndex+1]=iMaxIndex+1;\
	iIndex[iMaxIndex+2]=iMaxIndex+2;\
	iIndex[iMaxIndex+3]=iMaxIndex+3;\
	iMaxIndex+=4;

enum
{
	BLANK = 0,
	HOUSE,
	GRASS,
	ROAD,
	TREE
};

inline i32 PixelCode(u8 *pPixel)
{
	if(pPixel[0]==255 && pPixel[1]==0 && pPixel[2]==0)
	{
		return HOUSE;
	}
	if(pPixel[0]==0 && pPixel[1]==255 && pPixel[2]==0)
	{
		return GRASS;
	}
	if(pPixel[0]==0 && pPixel[1]==0 && pPixel[2]==255)
	{
		return TREE;
	}
	if(pPixel[0]==0 && pPixel[1]==0 && pPixel[2]==0)
	{
		return ROAD;
	}

	return BLANK;
}

static void GenerateLevel(i32 iLevel);

//------------------------------------------------------------------

void APIENTRY Level_Exit(Object *pObject)
{
	i32 i;

	Level *pLevel=(Level *)pObject;

	for(i=0;i<iNumLevels; i++)
	{
		ReleaseImage(ppLevelImages[i]);
	}

	for(i=0;i<12;i++)
	{
		ReleaseImage(pLevel->pImage[i]);
	}

	Mem_Free(ppLevelImages);

	Mem_Free(pLevel);

	for(i=0;i<iCollBoxes;i++)
	{
		Collision_DeleteBox(pCollBoxes[i]);
	}
	iCollBoxes = 0;
	iLamps = 0;

	// NIK	?????
	stdvPickups.clear();

	bLevelStarted = false;
}

//------------------------------------------------------------------

void APIENTRY Level_Update(Object *pObject)
{
	Level *pLevel=(Level *)pObject;
}

//------------------------------------------------------------------

void APIENTRY Level_Render(Object *pObject)
{
	Level *pLevel=(Level *)pObject;

	Camera *pCam = Camera_Current();
	Matrix mat;
	Matrix *pMat;
//	if(lpfnLockArrays)
//		lpfnLockArrays(0,iCurrentVert);

	glPushMatrix ();

	mat.Init();

	pMat = Camera_GetRenderMatrix();

	mat.SetColumn(3, pMat->GetColumn(3));
	glMultMatrixf (mat.Getfloat());

	

	GL_RenderMode(RENDER_MODE_TEXTURE);
	GL_SetTexture(iSkyTex);
	glDisable(GL_DEPTH_TEST);

	glColor4f(1.f,1.f,1.f,1.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, fSkyVerts);
	glTexCoordPointer(2, GL_FLOAT, 0, fSkyTex);

	glDrawElements(GL_QUADS,16,GL_UNSIGNED_INT,iSkyIndex);

	glPopMatrix ();

	glEnable(GL_DEPTH_TEST);
	glColor4f(1.f,1.f,1.f,1.f);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// setup for houses

	GL_RenderMode(RENDER_MODE_ALPHATEXTURE);

	GL_SetTexture(iHouseTex);

	glVertexPointer(3, GL_FLOAT, 0, fHouseVerts);
	glTexCoordPointer(2, GL_FLOAT, 0, fHouseTex);
	glColorPointer(4, GL_FLOAT, 0, fHouseCol);

	//glDrawElements(GL_QUADS,iMaxIndex,GL_UNSIGNED_INT,iIndex);
	glDrawElements(GL_QUADS,iDrawMaxIndex,GL_UNSIGNED_INT,iDrawIndex);
 
	// setup for trees
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);

	glDisable(GL_CULL_FACE);

	GL_RenderMode(RENDER_MODE_ALPHATEXTURE);
	GL_SetTexture(iTreeTex);

	glVertexPointer(3, GL_FLOAT, 0, fTreeVerts);
	glTexCoordPointer(2, GL_FLOAT, 0, fTreeTex);
	glColorPointer(4, GL_FLOAT, 0, fTreeCol);
	//glDrawElements(GL_QUADS,iMaxTreeIndex,GL_UNSIGNED_INT,iIndexTree);
	glDrawElements(GL_QUADS,iDrawTreeMaxIndex,GL_UNSIGNED_INT,iDrawTreeIndex);

	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
//	if(lpfnUnlockArrays)
//		lpfnUnlockArrays();

#ifdef _DEBUG
		uPolycount+=(iDrawMaxIndex/4)*2;
		uPolycount+=(iDrawTreeMaxIndex/4)*2;
#endif 

}

//------------------------------------------------------------------

Level* Level_Create( )
{
	i32 i;

	Level *pLevel;//=(Level *)Mem_New(sizeof(Level));
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Level_Exit;
	Create.renderfunc=Level_Render;
	Create.updatefunc=Level_Update;
	Create.iType=OBJECT_Level;
	Create.pName="Level";

	pLevel = (Level *)Object_Create(&Create, sizeof(Level));

	ppLevelImages = (Image**)Mem_New(sizeof(Image*)*iNumLevels);

	for(i=0;i<iNumLevels; i++)
	{
		ppLevelImages[i] = LoadTGA(LevelNames[i],false,true);

		ASSERT(ppLevelImages[i]->iWidth==LEVEL_SIZE, "level is not 32*32");
	}

	pLevel->pImage[0] =  LoadTGA("data/textures/house.tga",false,true);
	pLevel->pImage[1] =  LoadTGA("data/textures/house256.tga",false,true);
	pLevel->pImage[2] =  LoadTGA("data/textures/house128.tga",false,true);
	pLevel->pImage[3] =  LoadTGA("data/textures/house64.tga",false,true);
	pLevel->pImage[4] =  LoadTGA("data/textures/house32.tga",false,true);
	pLevel->pImage[5] =  LoadTGA("data/textures/house16.tga",false,true);
	pLevel->pImage[6] =  LoadTGA("data/textures/house8.tga",false,true);
	pLevel->pImage[7] =  LoadTGA("data/textures/house4.tga",false,true);
	pLevel->pImage[8] =  LoadTGA("data/textures/house2.tga",false,true);
	pLevel->pImage[9] =  LoadTGA("data/textures/house1.tga",false,true);

	iHouseTex = TgaLoad_genTexture(pLevel->pImage, 10);

	pLevel->pImage[10] =  LoadTGA("data/textures/trees.tga",false,true);
	iTreeTex = TgaLoad_genTexture(&pLevel->pImage[10]);

	pLevel->pImage[11] =  LoadTGA("data/textures/city2.tga",false,true);
	iSkyTex = TgaLoad_genTexture(&pLevel->pImage[11]);

	return pLevel;
}

//----------------------------------------------------------------------------------

void Level_Clear( )
{
	iCurrentVert = 0;
	iMaxIndex = 0;

	iCurrentTreeVert = 0;
	iMaxTreeIndex = 0;

	iLightCount = 0;

	while(iCollBoxes)
	{
		Collision_DeleteBox(pCollBoxes[iCollBoxes-1]);
		iCollBoxes--;
	}

	while(iLamps)
	{
		Sprite_Remove(pLampSprites[iLamps-1]);
		iLamps--;
	}

}

//----------------------------------------------------------------------------------

void Level_Start( i32 iLevelNum )
{
	Level_Clear();
	GenerateLevel(iLevelNum);
	bLevelStarted = true;
}

//----------------------------------------------------------------------------------

Vec3* Level_ClosestLight( Vec3 *pPos )
{
	i32 i;
	i32 closestX=100;
	i32 closestY=100;
	i32 iStore = 0;

	// workout index position
	i32 iIndexX = (int)((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE));
	i32 iIndexY = (int)((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE));

	for(i=0;i<iLightCount;i++)
	{
		if(qfabs(LightIndexes[i][0]-iIndexX)<=closestX && qfabs(LightIndexes[i][1]-iIndexY)<=closestY)
		{
			closestX = qfabs(LightIndexes[i][0]-iIndexX);
			closestY = qfabs(LightIndexes[i][1]-iIndexY);
			iStore = i;
		}
	}

	return &vecLights[iStore];
}

//----------------------------------------------------------------------------------

void Level_GenerateDraw( Vec3 *pPos )
{
	i32 i,j;

	i32 *pIndex;
	i32 *pDrawIndex;
	i32 IndexCount;
	GridIndex *pGrid;
	
	i32 *pIndexTree;
	i32 *pDrawTreeIndex;
	i32 IndexTreeCount;
	GridIndex *pGridTree;

	//(i*4)-(LEVEL_SIZE/2.f)*4.f;

	if(!bLevelStarted)
		return;

	// workout index position
	i32 iIndexX = (int)((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE));
	i32 iIndexY = (int)((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE));

	if(iIndexX == iLastIndexX && iLastIndexY == iIndexY)
	{
		return;
	}
	iLastIndexX = iIndexX;
	iLastIndexY = iIndexY;

	i32 iStartX = iIndexX - (DRAW_SIZE/2);
	i32 iStartY = iIndexY - (DRAW_SIZE/2);

	if(iStartX<0)
		iStartX = 0;
	if(iStartX + DRAW_SIZE > LEVEL_SIZE)
		iStartX = LEVEL_SIZE-DRAW_SIZE;
	if(iStartY<0)
		iStartY = 0;
	if(iStartY + DRAW_SIZE > LEVEL_SIZE)
		iStartY = LEVEL_SIZE-DRAW_SIZE;

	pDrawIndex = iDrawIndex;
	iDrawMaxIndex = 0;

	pDrawTreeIndex = iDrawTreeIndex;
	iDrawTreeMaxIndex = 0;

	i32 *pIndexNew;
	i32 *pIndexTreeNew = NULL;
	for(j=iStartY;j<iStartY + DRAW_SIZE;j++)
	{
		pIndex = NULL;
		IndexCount = 0;
		pGrid = &GIList[j*LEVEL_SIZE + iStartX];

		pIndexTree = NULL;
		IndexTreeCount = 0;
		pGridTree = &GITreeList[j*LEVEL_SIZE + iStartX];
		for(i=iStartX;i<iStartX + DRAW_SIZE;i++)
		{
			IndexCount += pGrid->iNumIndex;
			IndexTreeCount += pGridTree->iNumIndex;

			if(!pIndex && pGrid->iNumIndex)
			{
				pIndex = &iIndex[pGrid->iStartIndex];
				pIndexNew = pIndex;
			}

			if(!pIndexTree && pGridTree->iNumIndex)
			{
				pIndexTree = &iIndexTree[pGridTree->iStartIndex];
				pIndexTreeNew = pIndexTree;
			}

			i32 k;

			// set all alphas to 1
			for(k=0;k<pGrid->iNumIndex;k++)
			{
				fHouseCol[*pIndexNew][3] = 1.f;
				pIndexNew++;
			}
			if(pIndexTree)
			{
				for(k=0;k<pGridTree->iNumIndex;k++)
				{
					fTreeCol[*pIndexTreeNew][3] = 1.f;
					pIndexTreeNew++;
				}
			}

			pGrid++;
			pGridTree++;
		}

		iDrawMaxIndex += IndexCount;
		ASSERT(iDrawMaxIndex<DRAW_INDEXES, "too many draw indexes!!");

		memcpy(pDrawIndex,pIndex, sizeof(i32)*IndexCount);

		iDrawTreeMaxIndex += IndexTreeCount;
		ASSERT(iDrawTreeMaxIndex<DRAW_INDEXES/2, "too many draw tree indexes!!");

		memcpy(pDrawTreeIndex,pIndexTree, sizeof(i32)*IndexTreeCount);

		pDrawIndex += IndexCount;
		pDrawTreeIndex+=IndexTreeCount;
	}
}

//----------------------------------------------------------------------------------

void Level_GenerateAlphas( Vec3 *pPos )
{
	i32 i,j;

	i32 *pIndex;
	GridIndex *pGrid;
	
	i32 *pIndexTree;
	GridIndex *pGridTree;

	if(!bLevelStarted)
		return;

	// workout index position
	i32 iIndexX = (int)((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE));
	i32 iIndexY = (int)((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE));

	i32 iStartX = iIndexX - (DRAW_SIZE/2);
	i32 iStartY = iIndexY - (DRAW_SIZE/2);

	if(iStartX<0)
		iStartX = 0;
	if(iStartX + DRAW_SIZE > LEVEL_SIZE)
		iStartX = LEVEL_SIZE-DRAW_SIZE;
	if(iStartY<0)
		iStartY = 0;
	if(iStartY + DRAW_SIZE > LEVEL_SIZE)
		iStartY = LEVEL_SIZE-DRAW_SIZE;

	float fAlpha =  1.f - (((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE)) - iIndexY);

	if(fAlpha>1.f)
	{
		fAlpha=1.f;
	}
	if(fAlpha<0.f)
	{
		fAlpha=0.f;
	}

	pGrid = &GIList[iStartY*LEVEL_SIZE + iStartX];
	pGridTree = &GITreeList[iStartY*LEVEL_SIZE + iStartX];
	for(j=iStartX;j<iStartX + DRAW_SIZE;j++)
	{
		pIndex = &iIndex[pGrid->iStartIndex];

		for(i=0;i<pGrid->iNumIndex;i++)
		{
			fHouseCol[*pIndex][3] = fAlpha;
			pIndex++;
		}
		pGrid++;

		pIndexTree = &iIndexTree[pGridTree->iStartIndex];

		for(i=0;i<pGridTree->iNumIndex;i++)
		{
			fTreeCol[*pIndexTree][3] = fAlpha;
			pIndexTree++;
		}
		pGridTree++;
	}

	fAlpha = (((LEVEL_SIZE/2) + pPos->GetZ()/(4.f*LEVEL_SCALE)) - iIndexY);

	if(fAlpha>1.f)
	{
		fAlpha=1.f;
	}
	if(fAlpha<0.f)
	{
		fAlpha=0.f;
	}

	pGrid = &GIList[(iStartY + DRAW_SIZE-1)*LEVEL_SIZE + iStartX];
	pGridTree = &GITreeList[(iStartY + DRAW_SIZE-1)*LEVEL_SIZE + iStartX];
	for(j=iStartX;j<iStartX + DRAW_SIZE;j++)
	{
		pIndex = &iIndex[pGrid->iStartIndex];

		for(i=0;i<pGrid->iNumIndex;i++)
		{
			fHouseCol[*pIndex][3] = fAlpha;
			pIndex++;
		}
		pGrid++;

		pIndexTree = &iIndexTree[pGridTree->iStartIndex];

		for(i=0;i<pGridTree->iNumIndex;i++)
		{
			fTreeCol[*pIndexTree][3] = fAlpha;
			pIndexTree++;
		}
		pGridTree++;
	}

	fAlpha = 1.f - (((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE)) - iIndexX);

	if(fAlpha>1.f)
	{
		fAlpha=1.f;
	}
	if(fAlpha<0.f)
	{
		fAlpha=0.f;
	}

	pGrid = &GIList[iStartY*LEVEL_SIZE + iStartX];
	pGridTree = &GITreeList[(iStartY+1)*LEVEL_SIZE + iStartX];
	for(j=iStartY;j<iStartY + DRAW_SIZE;j++)
	{
		pIndex = &iIndex[pGrid->iStartIndex];

		for(i=0;i<pGrid->iNumIndex;i++)
		{
			fHouseCol[*pIndex][3] = fAlpha;
			pIndex++;
		}
		pGrid += LEVEL_SIZE;

		pIndexTree = &iIndexTree[pGridTree->iStartIndex];

		for(i=0;i<pGridTree->iNumIndex;i++)
		{
			fTreeCol[*pIndexTree][3] = fAlpha;
			pIndexTree++;
		}
		pGridTree += LEVEL_SIZE;
	}

	fAlpha = (((LEVEL_SIZE/2) + pPos->GetX()/(4.f*LEVEL_SCALE)) - iIndexX);

	if(fAlpha>1.f)
	{
		fAlpha=1.f;
	}
	if(fAlpha<0.f)
	{
		fAlpha=0.f;
	}

	pGrid = &GIList[iStartY*LEVEL_SIZE + iStartX + (DRAW_SIZE-1)];
	pGridTree = &GITreeList[(iStartY+1)*LEVEL_SIZE + iStartX + (DRAW_SIZE-1)];
	for(j=iStartY;j<iStartY + DRAW_SIZE;j++)
	{
		pIndex = &iIndex[pGrid->iStartIndex];

		for(i=0;i<pGrid->iNumIndex;i++)
		{
			fHouseCol[*pIndex][3] = fAlpha;
			pIndex++;
		}
		pGrid += LEVEL_SIZE;

		pIndexTree = &iIndexTree[pGridTree->iStartIndex];

		for(i=0;i<pGridTree->iNumIndex;i++)
		{
			fTreeCol[*pIndexTree][3] = fAlpha;
			pIndexTree++;
		}
		pGridTree += LEVEL_SIZE;
	}

	for(i=0;i<iLamps;i++)
	{
		fAlpha = 1.f - (*pPos - vecLights[i]).Length()/50.f;
		if(fAlpha<0.f)
			fAlpha=0.f;

		Sprite_SetColour( pLampSprites[i], ARGB(1.f, fAlpha, fAlpha, fAlpha) );
	}
}

//----------------------------------------------------------------------------------

bool Level_TestCollide( Vec3 *pPos )
{
	i32 i;
	Vec4 VecPoint;
	
	VecPoint.Set(pPos->GetX(), pPos->GetY(), pPos->GetZ(), 1.f);

	GridIndex *pGrid = Level_GetIndexGrid(pPos);

	for(i=0;i<iCollBoxes;i++)
	{
		if(Collision_BoxPointTest(pCollBoxes[i], VecPoint))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------

bool Level_TestLineCollide( Vec4 &VecStart, Vec4 &VecEnd )
{
	i32 i;
	
	GridIndex *pGrid = Level_GetIndexGrid(&VecStart);

	for(i=0;i<pGrid->iNumBoxes;i++)
	{
		if(Collision_BoxLineTest(pGrid->pCollBoxes[i],VecStart, VecEnd))
		{
			return true;
		}
	}

	for(i=0;i<pGrid->iNumBoxesSurrounding;i++)
	{
		if(Collision_BoxLineTest(pGrid->pCollBoxesSurrounding[i],VecStart, VecEnd))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------

bool Level_TestBoxCollide( ColBox *pBoxThis )
{
	i32 i;
	
	if(!bLevelStarted)
		return false;

	GridIndex *pGrid = Level_GetIndexGrid(&pBoxThis->mat.GetColumn(3));

	for(i=0;i<pGrid->iNumBoxes;i++)
	{
		if(Collision_BoxBoxTest(pGrid->pCollBoxes[i],pBoxThis))
		{
			return true;
		}
	}

	for(i=0;i<pGrid->iNumBoxesSurrounding;i++)
	{
		if(Collision_BoxBoxTest(pGrid->pCollBoxesSurrounding[i],pBoxThis))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------

// NIK
enum Pickup_TYPES Level_TestPickupsCollide( _Car *pCarThis )
{
	if(!bLevelStarted)
		return Pickup_NONE;

	for(std::vector<Pickup*>::iterator it=stdvPickups.begin(); it != stdvPickups.end(); ++it)
	{
		if(Collision_BoxBoxTest((*it)->pBox,pCarThis->pBox))
		{
			// response based on pickup type
			Pickup_TYPES type = (*it)->type; 
			switch (type)
			{
			case Pickup_NORMAL_AMMO:
				pCarThis->iNormalAmmo += 20;
				break;
			case Pickup_ROCKET_AMMO:
				pCarThis->iRocketAmmo += 10;
				break;
			}

			// erase pickup
			TrashCan_DeleteObject( (Object*)(*it) );
			stdvPickups.erase(it);
			return type;
		}
	}

	return Pickup_NONE;
}

//----------------------------------------------------------------------------------

static void VertexLight(float *pfCols,Vec4 &VecTemp)
{
	int i;

	float fIntensity=0.f;
	float fLight;
	for(i=0;i<iLightCount;i++)
	{
		fLight=(VecTemp-vecLights[i]).Length()/5.f;
		if(fLight>1.f)
			fLight=1.f;
		fLight*=(fLight*fLight);
		fLight=1.f-fLight;

		fIntensity+=fLight;
		if(fIntensity>=1.f)
		{
			fIntensity=1.f;
			break;
		}
	}

	float fIntensityR=fIntensity;
	float fIntensityG=fIntensity*(250.f/255.f);
	float fIntensityB=fIntensity*(160.f/255.f);
	float fAR=20.f/255.f;
	float fAG=20.f/255.f;
	float fAB=20.f/255.f;
	pfCols[0]=pfCols[0]*fIntensityR + fAR*(1.f-fIntensity);
	pfCols[1]=pfCols[1]*fIntensityG + fAG*(1.f-fIntensity);
	pfCols[2]=pfCols[2]*fIntensityB + fAB*(1.f-fIntensity);
}

//----------------------------------------------------------------------------------

static void HouseVertexAdd(float fX,float fY,float fZ,float fU,float fV,float fc1,float fc2,float fc3,float fc4)
{
	ASSERT(iCurrentVert<(NUM_HOUSES*68), "Adding too many verts!");

	//Vec4 VecTemp(fX,fY,fZ,1.f);
	Vec4 VecTemp(fY,fX,fZ,1.f);

	VecTemp=HouseMat*VecTemp;
	fHouseVerts[iCurrentVert][0]=VecTemp.GetX();
	fHouseVerts[iCurrentVert][1]=VecTemp.GetY();
	fHouseVerts[iCurrentVert][2]=VecTemp.GetZ();
	fHouseTex[iCurrentVert][0]=fU;
	fHouseTex[iCurrentVert][1]=fV;
	fHouseCol[iCurrentVert][0]=fc1;
	fHouseCol[iCurrentVert][1]=fc2;
	fHouseCol[iCurrentVert][2]=fc3;
	fHouseCol[iCurrentVert][3]=fc4;

	if(fc4>0.f)
		VertexLight(fHouseCol[iCurrentVert],VecTemp);

	iCurrentVert++;
}

//----------------------------------------------------------------------------------

static void TreeVertexAdd(float fX,float fY,float fZ,float fU,float fV,float fc1,float fc2,float fc3,float fc4)
{
	ASSERT(iCurrentTreeVert<(NUM_TREES*16), "Adding too many verts!");

	//Vec4 VecTemp(fX,fY,fZ,1.f);
	Vec4 VecTemp(fY,fX,fZ,1.f);

	VecTemp=HouseMat*VecTemp;
	fTreeVerts[iCurrentTreeVert][0]=VecTemp.GetX();
	fTreeVerts[iCurrentTreeVert][1]=VecTemp.GetY();
	fTreeVerts[iCurrentTreeVert][2]=VecTemp.GetZ();
	fTreeTex[iCurrentTreeVert][0]=fU;
	fTreeTex[iCurrentTreeVert][1]=fV;
	fTreeCol[iCurrentTreeVert][0]=fc1;
	fTreeCol[iCurrentTreeVert][1]=fc2;
	fTreeCol[iCurrentTreeVert][2]=fc3;
	fTreeCol[iCurrentTreeVert][3]=fc4;

	VertexLight(fTreeCol[iCurrentTreeVert],VecTemp);

	iCurrentTreeVert++;
}

#define TEXMUL 2.f
#define TEXSIZE (128.f*TEXMUL)

static bool InsertHouse()
{
	float fX=-2;//-1.5f;
	float fY=2;//-1.f;
	const float fW=3.f;
	const float fH=1.6f;
	const float fR=fH/1.5f;
	const float fRW=fW/2.f;
	const float fu1=0.f;
	const float fu2=(64.f*TEXMUL)/TEXSIZE;
	const float fv1=1.f;
	const float fv2=(64.f*TEXMUL)/TEXSIZE;

	float fRed=0.4f + random()*0.5f;
	float fGrn=0.4f + random()*0.3f;
	float fBlu=fGrn;
	float fY2=fY-3;

	// front
	HouseVertexAdd(fX,fY2,0 ,			fu1,fv1,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2,-fH ,			fu1,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2,-fH ,		fu2,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2,0 ,			fu2,fv1,	fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	// back
	HouseVertexAdd(fX+fW,fY2+fW,0 ,			fu1,fv1,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2+fW,-fH ,			fu1,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2+fW,-fH ,		fu2,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2+fW,0 ,			fu2,fv1,	fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	// right
	HouseVertexAdd(fX,fY2+fW,0 ,			fu1,fv1,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2+fW,-fH ,		fu1,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2,-fH ,			fu2,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2,0 ,				fu2,fv1,	fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	// left
	HouseVertexAdd(fX+fW,fY2,0 ,			fu1,fv1,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2,-fH ,		fu1,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2+fW,-fH ,		fu2,fv2,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2+fW,0 ,		fu2,fv1,	fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	const float fu12=0.f;//(64.f*TEXMUL)/TEXSIZE;
	const float fu22=128.f/512.f;//(96.f*TEXMUL)/TEXSIZE;
	const float fu32=256.f/512.f;//1.f;
	const float fv12=1.f;
	const float fv22=(75.f*TEXMUL)/TEXSIZE;
	// right end
	HouseVertexAdd(fX,fY2+fRW,-fH-fR ,	fu22,fv12,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2,-fH ,			fu12,fv22,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2+fW,-fH ,		fu32,fv22,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX,fY2+fW,-fH ,		fu32,fv22,	fRed,fGrn,fBlu,1.f);
	AddIndex(iIndex,iMaxIndex);
	// left end
	HouseVertexAdd(fX+fW,fY2+fRW,-fH-fR ,	fu22,fv12,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2+fW,-fH ,		fu12,fv22,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2,-fH ,		fu32,fv22,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fX+fW,fY2,-fH ,		fu32,fv22,	fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	float fhR=fRed;
	float fhG=fGrn;
	float fhB=fBlu;
	// front door 
	static float fDoorX=fX+1.f;
	static float fDoorY=1.2f;
	const float fu13=0.f;
	const float fu23=47.f/512.f;//(47.f*TEXMUL)/TEXSIZE;
	const float fv13=1.f-335.f/512.f;//(256.f*TEXMUL)/TEXSIZE;
	const float fv23=1.f-256.f/512.f;//(335.f*TEXMUL)/TEXSIZE;

	fRed=0.5f + random()*0.5f;
	fGrn=0.5f + random()*0.5f;
	fBlu=0.5f + random()*0.5f;

	HouseVertexAdd(fDoorX,fY2-0.03f,0 ,				fu13,fv13,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fDoorX,fY2-0.03f,-fDoorY ,		fu13,fv23,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fDoorX+0.6f,fY2-0.03f,-fDoorY ,	fu23,fv23,	fRed,fGrn,fBlu,1.f);
	HouseVertexAdd(fDoorX+0.6f,fY2-0.03f,0 ,		fu23,fv13,		fRed,fGrn,fBlu,1.f);

	AddIndex(iIndex,iMaxIndex);

	// left window 
	static float fWin1X=fX+0.2f;
	static float fWin1Y1=0.6f;
	static float fWin1Y2=1.5f;
	const float fu14=48.f/512.f;//(10.f*TEXMUL)/TEXSIZE;
	const float fu24=95.f/512.f;//(19.f*TEXMUL)/TEXSIZE;
	const float fv14=1.f-256.f/512.f;//(73.f*TEXMUL)/TEXSIZE;
	const float fv24=1.f-303.f/512.f;//(64.f*TEXMUL)/TEXSIZE;

	HouseVertexAdd(fWin1X,fY2-0.03f,-fWin1Y1 ,		fu14,fv14,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin1X,fY2-0.03f,-fWin1Y2 ,		fu14,fv24,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin1X+0.6f,fY2-0.03f,-fWin1Y2 ,	fu24,fv24,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin1X+0.6f,fY2-0.03f,-fWin1Y1 ,	fu24,fv14,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	// right window 
	static float fWin2X=fX+1.8f;
	HouseVertexAdd(fWin2X,fY2-0.03f,-fWin1Y1 ,		fu14,fv14,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin2X,fY2-0.03f,-fWin1Y2 ,		fu14,fv24,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin2X+0.8f,fY2-0.03f,-fWin1Y2 ,	fu24,fv24,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWin2X+0.8f,fY2-0.03f,-fWin1Y1 ,	fu24,fv14,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	// roof
	fRed=0.2f + random()*0.5f;

	HouseVertexAdd(fX-0.25f,fY2-0.25f,-fH+0.1f ,		fu1,fv1,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX-0.25f,fY2+fRW,-fH-fR ,		fu1,fv2,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX+fW+0.25f,fY2+fRW,-fH-fR ,	fu2,fv2,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX+fW+0.25f,fY2-0.25f,-fH+0.1f ,	fu2,fv1,	fRed,fRed,fRed,1.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(fX+fW+0.25f,fY2+fW+0.25f,-fH+0.1f ,	fu1,fv1,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX+fW+0.25f,fY2+fRW,-fH-fR ,	fu1,fv2,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX-0.25f,fY2+fRW,-fH-fR ,		fu2,fv2,	fRed,fRed,fRed,1.f);
	HouseVertexAdd(fX-0.25f,fY2+fW+0.25f,-fH+0.1f ,	fu2,fv1,	fRed,fRed,fRed,1.f);

	AddIndex(iIndex,iMaxIndex);

	//garden
	const float fu15=(385.f)/512.f;
	const float fu25=(512.f)/512.f;
	const float fv15=(512.f-256.f)/512.f;
	const float fv25=(512.f-383.f)/512.f;
	/*
	const float fu15=(64.5f*TEXMUL)/TEXSIZE;
	const float fu25=(95.5f*TEXMUL)/TEXSIZE;
	const float fv15=1.f-(95.5f*TEXMUL)/TEXSIZE;
	const float fv25=(63.5f*TEXMUL)/TEXSIZE;
*/
	float fGY=fY-4;
	int iVertGrass=iCurrentVert;
	HouseVertexAdd(fX,fGY,0 ,			fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX,fGY+4,0 ,			fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fGY+4,0 ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fGY,0 ,			fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);


	Matrix Mat, Mat2;
	Mat.Init();
	Mat.RotX(90);
	Mat = Mat*LEVEL_SCALE;
	Vec4 vecMin(-1.5f, -1.5f, 0, 1.f);
	Vec4 vecMax(1.5f, 1.5f, -fH, 1.f);
	vecMin	= Mat*vecMin;
	vecMax	= Mat*vecMax;

	Vec4 vecPos((fY2 + (fY2+fW))/2,(fX + (fX+fW))/2,0, 1.f);
	vecPos	= HouseMat * vecPos;
	vecPos	= Mat*vecPos;
	vecPos.SetW(1.f);
	Mat2.Init();
	Mat2.SetColumn(3, vecPos);
	ASSERT(iCollBoxes<LEVEL_BOXES, "addiing too many boxes");
	pCollBoxes[iCollBoxes] = Collision_CreateBox(&vecMax, &vecMin, &Mat2);

	Level_GridIndexAddBox(&vecPos, pCollBoxes[iCollBoxes]);

	iCollBoxes++;

	return false;
}

//----------------------------------------------------------------------------------

static void InsertGrass()
{
	float fX=-2;//-1.5f;
	float fY=2;//-1.f;
	//garden
	const float fu15=(64.5f*TEXMUL)/TEXSIZE;
	const float fu25=(95.5f*TEXMUL)/TEXSIZE;
	const float fv15=1.f-(95.5f*TEXMUL)/TEXSIZE;
	const float fv25=(63.5f*TEXMUL)/TEXSIZE;

	float fGY=fY-4;
	HouseVertexAdd(fX,fGY,0 ,			fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX,fGY+4,0 ,			fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fGY+4,0 ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fGY,0 ,			fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);
}

//----------------------------------------------------------------------------------

static void DrawRoad(u8 *cL,u8 *cR,u8 *cU,u8 *cD,float fX,float fY)
{
	Vec3 vecPos;

	float fu1=256.5f/512.f;//(96.5f*TEXMUL)/TEXSIZE;
	float fu2=383.5f/512.f;//(111.5f*TEXMUL)/TEXSIZE;
	float fv1=(512.f-0.5f)/512.f;//1.f-(79.5f*TEXMUL)/TEXSIZE;
	float fv2=(512.f-127.5f)/512.f;//(63.5f*TEXMUL)/TEXSIZE;

	// rotate graphic
	HouseMat.RotZ(90);

	if(PixelCode(cL)==ROAD && PixelCode(cR)==ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)!=ROAD)
	{
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)!=ROAD && PixelCode(cR)==ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)!=ROAD)
	{
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)!=ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)!=ROAD)
	{
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)!=ROAD && PixelCode(cR)==ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)==ROAD)//down right
	{
		fv1=(512.f-128.f)/512.f;//1.f-(80.5f*TEXMUL)/TEXSIZE;
		fv2=(512.f-255.f)/512.f;//1.f-(95.5f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(0);
	}
	else if(PixelCode(cL)!=ROAD && PixelCode(cR)==ROAD && PixelCode(cU)==ROAD && PixelCode(cD)!=ROAD)// up right
	{
		fv1=(512.f-255.f)/512.f;//1.f-(95.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-128.f)/512.f;//1.f-(80.f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)!=ROAD && PixelCode(cU)==ROAD && PixelCode(cD)!=ROAD)// up left
	{
		fu1=383.5f/512.f;//(111.f*TEXMUL)/TEXSIZE;
		fu2=256.5f/512.f;//(97.f*TEXMUL)/TEXSIZE;
		fv1=(512.f-255.5f)/512.f;//1.f-(95.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-128.5f)/512.f;//1.f-(80.f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(0);
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)!=ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)==ROAD)// down left
	{
		fu1=383.5f/512.f;//(111.f*TEXMUL)/TEXSIZE;
		fu2=256.5f/512.f;//(97.f*TEXMUL)/TEXSIZE;
		fv1=(512.f-128.5f)/512.f;//1.f-(80.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-255.5f)/512.f;//1.f-(95.f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)==ROAD && PixelCode(cU)!=ROAD && PixelCode(cD)==ROAD)// left right down
	{
		fu1=384.5f/512.f;//(112.5f*TEXMUL)/TEXSIZE;
		fu2=511.5f/512.f;//(127.5f*TEXMUL)/TEXSIZE;
		fv1=(512.f-0.5f)/512.f;//(63.5f*TEXMUL)/TEXSIZE;
		fv2=(512.f-127.5f)/512.f;//1.f-(79.5f*TEXMUL)/TEXSIZE;
		//return;
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)==ROAD && PixelCode(cU)==ROAD && PixelCode(cD)!=ROAD)// left right up
	{
		fu1=511.5f/512.f;//384.5f/512.f;//
		fu2=384.5f/512.f;//511.5f/512.f;//
		fv1=(512.f-127.5f)/512.f;//1.f-(79.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-0.5f)/512.f;//(63.f*TEXMUL)/TEXSIZE;
	//	return;
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)!=ROAD && PixelCode(cU)==ROAD && PixelCode(cD)==ROAD)// down up left
	{
		fu1=384.5f/512.f;//(112.5f*TEXMUL)/TEXSIZE;
		fu2=511.5f/512.f;//(127.5f*TEXMUL)/TEXSIZE;
		fv1=(512.f-0.5f)/512.f;//(63.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-127.5f)/512.f;//1.f-(79.f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(180);
	}
	else if(PixelCode(cL)!=ROAD && PixelCode(cR)==ROAD && PixelCode(cU)==ROAD && PixelCode(cD)==ROAD)// down up right
	{
		fu1=384.5f/512.f;//(112.5f*TEXMUL)/TEXSIZE;
		fu2=511.5f/512.f;//(127.5f*TEXMUL)/TEXSIZE;
		fv1=(512.f-0.5f)/512.f;//(63.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-127.5f)/512.f;//1.f-(79.f*TEXMUL)/TEXSIZE;
		// rotate graphic
		HouseMat.RotZ(0);
	}
	else if(PixelCode(cL)==ROAD && PixelCode(cR)==ROAD && PixelCode(cU)==ROAD && PixelCode(cD)==ROAD)// left right up down
	{
		fu1=384.5f/512.f;//(112.5f*TEXMUL)/TEXSIZE;
		fu2=511.5f/512.f;//(127.5f*TEXMUL)/TEXSIZE;
		fv1=(512.f-128.f)/512.f;//1.f-(81.f*TEXMUL)/TEXSIZE;
		fv2=(512.f-255.f)/512.f;//1.f-(95.f*TEXMUL)/TEXSIZE;
	}

	// NIK: Generate a Pickup
	if (rand()%2 == 0)	// not on all roads
	{
		Vec3 vecPickupPos;
		vecPickupPos.Set(fX*LEVEL_SCALE,1.0f,fY*LEVEL_SCALE);
		Pickup* pPickup;
		if (rand()%2 == 0)	// approx half the pickups will be for one of the weapons
		{
			pPickup = Pickup_Create(Pickup_NORMAL_AMMO, vecPickupPos);
		}
		else
		{
			pPickup = Pickup_Create(Pickup_ROCKET_AMMO, vecPickupPos);
		}
		stdvPickups.push_back(pPickup);
	}
	// ---

	vecPos.Set(fX,fY,0);
	HouseMat.SetColumn(3,vecPos);
	HouseVertexAdd(-2.f,-2.f,0 ,			fu1,fv1,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-2.f,-2.f+4.f,0 ,			fu1,fv2,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-2.f+4.f,-2.f+4.f,0 ,		fu2,fv2,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-2.f+4.f,-2.f,0 ,			fu2,fv1,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);
}

//----------------------------------------------------------------------------------

static void AddLampPost(float fX,float fY)
{
	const float fu15=176.5f/512.f;//(18.5f*TEXMUL)/TEXSIZE;
	const float fu25=191.5f/512.f;//(19.5f*TEXMUL)/TEXSIZE;
	const float fv15=(512.f-256.f)/512.f;//(TEXSIZE-(72.f*TEXMUL))/TEXSIZE;
	const float fv25=(512.f-351.f)/512.f;//(TEXSIZE-(66.f*TEXMUL))/TEXSIZE;

	const float fWidth=0.03f;
	const float fHeight=2.f;
	const float fWidth2=0.2f;
	const float fWidth3=0.1f;

	HouseVertexAdd(-fWidth,-fWidth,0 ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth,-fWidth,-fHeight ,		fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(+fWidth,-fWidth,-fHeight ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(+fWidth,-fWidth,0 ,				fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(-fWidth,fWidth,0 ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(+fWidth,fWidth,0 ,				fu25,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(+fWidth,fWidth,-fHeight ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth,fWidth,-fHeight ,		fu15,fv25,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(fWidth,-fWidth,0 ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth,-fWidth,-fHeight ,		fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth,fWidth,-fHeight ,			fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth,fWidth,0 ,				fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(-fWidth,fWidth,0 ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth,fWidth,-fHeight ,		fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth,-fWidth,-fHeight ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth,-fWidth,0 ,				fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	// top bit
	HouseVertexAdd(-fWidth2,-fWidth2,-fHeight ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth2,fWidth2,-fHeight ,		fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth2,fWidth2,-fHeight ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth2,-fWidth2,-fHeight ,				fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	const float fus15=(25.f*TEXMUL)/TEXSIZE;
	const float fus25=(28.f*TEXMUL)/TEXSIZE;
	const float fvs15=(TEXSIZE-(70.f*TEXMUL))/TEXSIZE;
	const float fvs25=(TEXSIZE-(66.f*TEXMUL))/TEXSIZE;
	
	HouseVertexAdd(-fWidth3,-fWidth3,-fHeight ,				fus15,fvs15,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,-fWidth3,-fHeight-fWidth2 ,		fus15,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(+fWidth3,-fWidth3,-fHeight-fWidth2 ,		fus25,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(+fWidth3,-fWidth3,-fHeight ,				fus25,fvs15,	1.f,1.f,1.f,0.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(fWidth3,fWidth3,-fHeight ,				fus15,fvs15,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(fWidth3,fWidth3,-fHeight-fWidth2 ,		fus15,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,fWidth3,-fHeight-fWidth2 ,		fus25,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,fWidth3,-fHeight ,				fus25,fvs15,	1.f,1.f,1.f,0.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(fWidth3,-fWidth3,-fHeight ,				fus15,fvs15,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(fWidth3,-fWidth3,-fHeight-fWidth2 ,		fus15,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(fWidth3,fWidth3,-fHeight-fWidth2 ,		fus25,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(fWidth3,fWidth3,-fHeight ,				fus25,fvs15,	1.f,1.f,1.f,0.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(-fWidth3,fWidth3,-fHeight ,				fus15,fvs15,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,fWidth3,-fHeight-fWidth2 ,		fus15,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,-fWidth3,-fHeight-fWidth2 ,		fus25,fvs25,	1.f,1.f,1.f,0.f);
	HouseVertexAdd(-fWidth3,-fWidth3,-fHeight ,				fus25,fvs15,	1.f,1.f,1.f,0.f);

	AddIndex(iIndex,iMaxIndex);

	HouseVertexAdd(-fWidth3,-fWidth3,-fHeight-fWidth2 ,				fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(-fWidth3,fWidth3,-fHeight-fWidth2 ,		fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth3,fWidth3,-fHeight-fWidth2 ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fWidth3,-fWidth3,-fHeight-fWidth2 ,				fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);


	Matrix Mat, Mat2;
	Mat.Init();
	Mat.RotX(90);
	Mat = Mat*LEVEL_SCALE;
	Vec4 vecMin(-fWidth3, -fWidth3, 0, 1.f);
	Vec4 vecMax(fWidth3, fWidth3, -fHeight, 1.f);
	vecMin	= Mat*vecMin;
	vecMax	= Mat*vecMax;

	Vec4 vecPos(0,0,0, 1.f);
	vecPos	= HouseMat*vecPos;
	vecPos	= Mat*vecPos;
	vecPos.SetW(1.f);
	Mat2.Init();
	Mat2.SetColumn(3, vecPos);
	ASSERT(iCollBoxes<LEVEL_BOXES, "addiing too many boxes");
	pCollBoxes[iCollBoxes] = Collision_CreateBox(&vecMax, &vecMin, &Mat2);

	Level_GridIndexAddBox(&vecPos, pCollBoxes[iCollBoxes]);
	iCollBoxes++;

	ASSERT(iLamps<(LEVEL_SIZE*LEVEL_SIZE)/3, "adding too many lamp sprites");

	vecPos.Set(0.f, 0.f, -2.2f,1.f);
	vecPos	= HouseMat*vecPos;
	vecPos	= Mat*vecPos;
	vecPos.SetW(1.f);

	pLampSprites[iLamps] = Sprite_Add(vecPos, Vec4(0.f/128.f, 0.f, 16.f/128.f, 16.f/128.f),3.f,0xffffffff,SPR_ADD);
	iLamps++;
}

//----------------------------------------------------------------------------------

static void AddTree(float fX,float fY,float fWidth)
{
	float fu15=0.f/64.f;
	float fu25=32.f/64.f;
	float fv15=1.f;
	float fv25=32.f/64.f;

	float fRand=random();
	if(fRand<0.25f)
	{
		fu15=32.f/64.f;
		fu25=1.f;
		fv15=1.f;
		fv25=32.f/64.f;
	}
	else if(fRand<0.75f)
	{
		fu15=0.f/64.f;
		fu25=32.f/64.f;
		fv15=32.f/64.f;
		fv25=0.f;
	}
	else if(fRand>0.95f)
	{
		fu15=32.f/64.f;
		fu25=1.f;
		fv15=32.f/64.f;
		fv25=0.f;
	}

	float hW=fWidth*0.75f;
	float fH=fWidth*1.5f + (random()*fWidth)*0.5f;

	fRand=0.8f + random()*0.2f;
	TreeVertexAdd(fX-hW,fY,0 ,			fu15,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX-hW,fY,-fH ,			fu15,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX+hW,fY,-fH ,			fu25,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX+hW,fY,0 ,			fu25,fv25,		1.f,fRand,1.f,1.f);

	AddIndex(iIndexTree,iMaxTreeIndex);

	/*TreeVertexAdd(fX-hW,fY,-fH ,			fu15,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX-hW,fY,0 ,			fu15,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX+hW,fY,0 ,			fu25,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX+hW,fY,-fH ,			fu25,fv15,	1.f,fRand,1.f,1.f);

	AddIndex(iIndexTree,iMaxTreeIndex);*/

	TreeVertexAdd(fX,fY-hW,0 ,			fu15,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY-hW,-fH ,			fu15,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY+hW,-fH ,			fu25,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY+hW,0 ,			fu25,fv25,		1.f,fRand,1.f,1.f);

	AddIndex(iIndexTree,iMaxTreeIndex);

	/*TreeVertexAdd(fX,fY-hW,-fH ,			fu15,fv15,	1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY-hW,0 ,			fu15,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY+hW,0 ,			fu25,fv25,		1.f,fRand,1.f,1.f);
	TreeVertexAdd(fX,fY+hW,-fH ,			fu25,fv15,	1.f,fRand,1.f,1.f);

	AddIndex(iIndexTree,iMaxTreeIndex);*/

	Matrix Mat, Mat2;
	Mat.Init();
	Mat.RotX(90);
	Mat = Mat*LEVEL_SCALE;
	Vec4 vecMin(-hW*0.2f, -hW*0.2f, 0, 1.f);
	Vec4 vecMax(hW*0.2f, hW*0.2f, -fH, 1.f);
	vecMin	= Mat*vecMin;
	vecMax	= Mat*vecMax;

	Vec4 vecPos(fY,fX,0, 1.f);
	vecPos	= HouseMat*vecPos;
	vecPos	= Mat*vecPos;
	vecPos.SetW(1.f);
	Mat2.Init();
	Mat2.SetColumn(3, vecPos);
	ASSERT(iCollBoxes<LEVEL_BOXES, "addiing too many boxes");
	pCollBoxes[iCollBoxes] = Collision_CreateBox(&vecMax, &vecMin, &Mat2);

	Level_GridIndexAddBox(&vecPos, pCollBoxes[iCollBoxes]);

	iCollBoxes++;

	
}

static void Tree()
{
	float fX=-2;
	float fY=-2;
	const float fu15=(64.5f*TEXMUL)/TEXSIZE;
	const float fu25=(95.5f*TEXMUL)/TEXSIZE;
	const float fv15=1.f-(95.5f*TEXMUL)/TEXSIZE;
	const float fv25=(63.5f*TEXMUL)/TEXSIZE;

	// grass
	HouseVertexAdd(fX,fY,0 ,			fu15,fv15,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX,fY+4,0 ,			fu15,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fY+4,0 ,		fu25,fv25,	1.f,1.f,1.f,1.f);
	HouseVertexAdd(fX+4,fY,0 ,			fu25,fv15,	1.f,1.f,1.f,1.f);

	AddIndex(iIndex,iMaxIndex);

	AddTree(-(1.f+random()),-(1.f+random()),1.5f+random());
	AddTree(-(1.f+random()),1.f+random(),1.5f+random());
	AddTree(1.f+random(),-(1.f+random()),1.5f+random());
	AddTree(1.f+random(),1.f+random(),1.5f+random());
}

static void GenerateLevel(i32 iLevel)
{
	i32 i;
	i32 j;
	float fX;
	float fY;
	u8 *pPixel;
	Vec3 vecPos;

	ASSERT(iLevel<iNumLevels, "no level data!");

	pPixel = ppLevelImages[iLevel]->pData;

	memset(GIList, 0, sizeof(GIList));

	for(j=0;j<LEVEL_SIZE;j++)
	{
		for(i=0;i<LEVEL_SIZE;i++)
		{
			// setup light positions
			if(PixelCode(pPixel) != BLANK && (!(i%3) && !(j%3)) )
			{
				fX=(i - (LEVEL_SIZE/2.f))*4.f;
				fY=(j - (LEVEL_SIZE/2.f))*4.f;

				vecLights[iLightCount].Set(fX+1.5f,fY+1.5f,0);
				LightIndexes[iLightCount][0] = i;
				LightIndexes[iLightCount][1] = j;
				iLightCount++;
				ASSERT(iLightCount<((LEVEL_SIZE*LEVEL_SIZE)/3), "too many lights!");
			}
			pPixel += ppLevelImages[iLevel]->iBPP;
		}
	}

	pPixel = ppLevelImages[iLevel]->pData;
	for(j=0;j<LEVEL_SIZE;j++)
	{
		for(i=0;i<LEVEL_SIZE;i++)
		{
			fX=(i-(LEVEL_SIZE/2.f))*4.f;
			fY=(j-(LEVEL_SIZE/2.f))*4.f;

			GITreeList[j*LEVEL_SIZE + i].iStartIndex = 0;
			GITreeList[j*LEVEL_SIZE + i].iNumIndex = 0;
			if(PixelCode(pPixel) == HOUSE)
			{
				GIList[j*LEVEL_SIZE + i].iStartIndex = iMaxIndex;

				if(i-1>-1 && PixelCode(pPixel-ppLevelImages[iLevel]->iBPP)==ROAD)
				{
					HouseMat.Init();
					HouseMat.RotZ(-90);
					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
					InsertHouse();
				}
				else if(i+1<LEVEL_SIZE && PixelCode(pPixel+ppLevelImages[iLevel]->iBPP)==ROAD)
				{
					HouseMat.Init();
					HouseMat.RotZ(90);
					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
					InsertHouse();
				}
				else if(j-1>-1 && PixelCode(pPixel-(ppLevelImages[iLevel]->iWidth*ppLevelImages[iLevel]->iBPP))==ROAD)
				{
					HouseMat.Init();
					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
					InsertHouse();
				}
				else if(j+1<LEVEL_SIZE && PixelCode(pPixel+(ppLevelImages[iLevel]->iWidth*ppLevelImages[iLevel]->iBPP))==ROAD)
				{
					HouseMat.Init();
					HouseMat.RotZ(180);
					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
					InsertHouse();
				}
				GIList[j*LEVEL_SIZE + i].iNumIndex = iMaxIndex - GIList[j*LEVEL_SIZE + i].iStartIndex;
			}
			else
			{
				HouseMat.Init();
				if(PixelCode(pPixel) == ROAD)
				{
					GIList[j*LEVEL_SIZE + i].iStartIndex = iMaxIndex;
					DrawRoad(pPixel-ppLevelImages[iLevel]->iBPP,pPixel+ppLevelImages[iLevel]->iBPP,pPixel-(ppLevelImages[iLevel]->iWidth*ppLevelImages[iLevel]->iBPP),pPixel+(ppLevelImages[iLevel]->iWidth*ppLevelImages[iLevel]->iBPP),fX,fY);	
					GIList[j*LEVEL_SIZE + i].iNumIndex = iMaxIndex - GIList[j*LEVEL_SIZE + i].iStartIndex;
				}
				else if(PixelCode(pPixel) == TREE)
				{
					GIList[j*LEVEL_SIZE + i].iStartIndex = iMaxIndex;
					GITreeList[j*LEVEL_SIZE + i].iStartIndex = iMaxTreeIndex;
			
					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
				//	iStartTIndex[j*LEVEL_SIZE + i]=iCurrentTreeVert;
					Tree();

					GITreeList[j*LEVEL_SIZE + i].iNumIndex = iMaxTreeIndex - GITreeList[j*LEVEL_SIZE + i].iStartIndex;
					GIList[j*LEVEL_SIZE + i].iNumIndex = iMaxIndex - GIList[j*LEVEL_SIZE + i].iStartIndex;
					
				}
				else  if(PixelCode(pPixel) == GRASS)
				{
					GIList[j*LEVEL_SIZE + i].iStartIndex = iMaxIndex;

					vecPos.Set(fX,fY,0);
					HouseMat.SetColumn(3,vecPos);
					InsertGrass();
					
					GIList[j*LEVEL_SIZE + i].iNumIndex = iMaxIndex - GIList[j*LEVEL_SIZE + i].iStartIndex;
				/*	if(Zombie.fPlayerX==0.f && Zombie.fPlayerY==0.f)
					{
						Zombie.fPlayerX=(i*4.f);
						Zombie.fPlayerY=(j*4.f);
						Zombie.fPlayerLastX=Zombie.fPlayerX;
						Zombie.fPlayerLastY=Zombie.fPlayerY;
					}*/
				}
			}

			if(PixelCode(pPixel) != BLANK && (!(i%3) && !(j%3)) )
			{
				i32 iNewStart = iMaxIndex;
				//GIList[j*LEVEL_SIZE + i].iStartIndex = iMaxIndex;
				HouseMat.Init();
				vecPos.Set(fX+1.5f,fY+1.5f,0);
				HouseMat.SetColumn(3,vecPos);
				AddLampPost(fX,fY);
				GIList[j*LEVEL_SIZE + i].iNumIndex += iMaxIndex - iNewStart;
			}

			pPixel += ppLevelImages[iLevel]->iBPP;
		}
	}

	// NIK:
	Level_AddLevelEdgesCols();

	Level_GridIndexAddSurrounds();

	// rotate everything so y is up
	HouseMat.Init();
	HouseMat.RotX(90);
	HouseMat = HouseMat*LEVEL_SCALE;
	for(i=0;i<iCurrentVert;i++)
	{
		vecPos.Set(fHouseVerts[i][0],fHouseVerts[i][1],fHouseVerts[i][2]);
		vecPos = HouseMat*vecPos;
		fHouseVerts[i][0] = vecPos.GetX();
		fHouseVerts[i][1] = vecPos.GetY();
		fHouseVerts[i][2] = vecPos.GetZ();
	}

	for(i=0;i<iCurrentTreeVert;i++)
	{
		vecPos.Set(fTreeVerts[i][0],fTreeVerts[i][1],fTreeVerts[i][2]);
		vecPos = HouseMat*vecPos;
		fTreeVerts[i][0] = vecPos.GetX();
		fTreeVerts[i][1] = vecPos.GetY();
		fTreeVerts[i][2] = vecPos.GetZ();
	}

	for(i=0;i<iLightCount;i++)
	{
		vecLights[i] = HouseMat*vecLights[i];
	}

#define SKY_SIZE LEVEL_SIZE*2

	fSkyVerts[0][0] = -SKY_SIZE;
	fSkyVerts[0][1] = SKY_SIZE;
	fSkyVerts[0][2] = SKY_SIZE;

	fSkyVerts[1][0] = -SKY_SIZE;
	fSkyVerts[1][1] = -SKY_SIZE;
	fSkyVerts[1][2] = SKY_SIZE;

	fSkyVerts[2][0] = SKY_SIZE;
	fSkyVerts[2][1] = -SKY_SIZE;
	fSkyVerts[2][2] = SKY_SIZE;

	fSkyVerts[3][0] = SKY_SIZE;
	fSkyVerts[3][1] = SKY_SIZE;
	fSkyVerts[3][2] = SKY_SIZE;


	fSkyVerts[4][0] = SKY_SIZE;
	fSkyVerts[4][1] = SKY_SIZE;
	fSkyVerts[4][2] = -SKY_SIZE;

	fSkyVerts[5][0] = SKY_SIZE;
	fSkyVerts[5][1] = SKY_SIZE;
	fSkyVerts[5][2] = SKY_SIZE;

	fSkyVerts[6][0] = SKY_SIZE;
	fSkyVerts[6][1] = -SKY_SIZE;
	fSkyVerts[6][2] = SKY_SIZE;

	fSkyVerts[7][0] = SKY_SIZE;
	fSkyVerts[7][1] = -SKY_SIZE;
	fSkyVerts[7][2] = -SKY_SIZE;


	fSkyVerts[8][0] = -SKY_SIZE;
	fSkyVerts[8][1] = SKY_SIZE;
	fSkyVerts[8][2] = -SKY_SIZE;

	fSkyVerts[9][0] = SKY_SIZE;
	fSkyVerts[9][1] = SKY_SIZE;
	fSkyVerts[9][2] = -SKY_SIZE;

	fSkyVerts[10][0] = SKY_SIZE;
	fSkyVerts[10][1] = -SKY_SIZE;
	fSkyVerts[10][2] = -SKY_SIZE;

	fSkyVerts[11][0] = -SKY_SIZE;
	fSkyVerts[11][1] = -SKY_SIZE;
	fSkyVerts[11][2] = -SKY_SIZE;


	
	fSkyVerts[12][0] = -SKY_SIZE;
	fSkyVerts[12][1] = SKY_SIZE;
	fSkyVerts[12][2] = -SKY_SIZE;

	fSkyVerts[13][0] = -SKY_SIZE;
	fSkyVerts[13][1] = -SKY_SIZE;
	fSkyVerts[13][2] = -SKY_SIZE;

	fSkyVerts[14][0] = -SKY_SIZE;
	fSkyVerts[14][1] = -SKY_SIZE;
	fSkyVerts[14][2] = SKY_SIZE;

	fSkyVerts[15][0] = -SKY_SIZE;
	fSkyVerts[15][1] = SKY_SIZE;
	fSkyVerts[15][2] = SKY_SIZE;

	
	fSkyTex[0][0] = 0.f;
	fSkyTex[0][1] = 1.f;

	fSkyTex[1][0] = 0.f;
	fSkyTex[1][1] = 0.f;

	fSkyTex[2][0] = 128.f/512.f;
	fSkyTex[2][1] = 0.f;

	fSkyTex[3][0] = 128.f/512.f;
	fSkyTex[3][1] = 1.f;
	

	fSkyTex[4][0] = 256.f/512.f;
	fSkyTex[4][1] = 1.f;

	fSkyTex[5][0] = 128.f/512.f;
	fSkyTex[5][1] = 1.f;

	fSkyTex[6][0] = 128.f/512.f;
	fSkyTex[6][1] = 0.f;

	fSkyTex[7][0] = 256.f/512.f;
	fSkyTex[7][1] = 0.f;


	fSkyTex[8][0] = 384.f/512.f;
	fSkyTex[8][1] = 1.f;

	fSkyTex[9][0] = 256.f/512.f;
	fSkyTex[9][1] = 1.f;

	fSkyTex[10][0] = 256.f/512.f;
	fSkyTex[10][1] = 0.f;

	fSkyTex[11][0] = 384.f/512.f;
	fSkyTex[11][1] = 0.f;
	
	

	fSkyTex[12][0] = 384.f/512.f;
	fSkyTex[12][1] = 1.f;

	fSkyTex[13][0] = 384.f/512.f;
	fSkyTex[13][1] = 0.f;

	fSkyTex[14][0] = 1.f;
	fSkyTex[14][1] = 0.f;

	fSkyTex[15][0] = 1.f;
	fSkyTex[15][1] = 1.f;
	//fSkyTex[16][2];

	for(i=0;i<16;i++)
	{
		fSkyTex[i][0] *= 3.f;
		iSkyIndex[i] = i;
	}

	Level_GenerateDraw( &Vec3(3.f,0.f,0.f) );
}

void Level_AddLevelEdgesCols()
{
	// NIK: Add collision boxes around the edges of the level
	for (int j = -6; j < 6; ++j)
	{
		for (int i = -6; i < 6; ++i)
		{
			if ((i == -6) || (i == 5) || (j == -6) || (j == 5))
			{
				Matrix Mat, Mat2;
				Mat.Init();
				Mat.RotX(90);
				Mat = Mat*LEVEL_SCALE;
				Vec4 vecMin(-2.0f, -2.0f, 0, 1.f);
				Vec4 vecMax(2.0f, 2.0f, -1.f, 1.f);
				vecMin	= Mat*vecMin;
				vecMax	= Mat*vecMax;

				Vec4 vecPos(i*4.f,j*4.f,0.f, 1.f);
				vecPos	= Mat*vecPos;
				vecPos.SetW(1.f);
				Mat2.Init();
				Mat2.SetColumn(3, vecPos);
				ASSERT(iCollBoxes<LEVEL_BOXES, "addiing too many boxes");
				pCollBoxes[iCollBoxes] = Collision_CreateBox(&vecMax, &vecMin, &Mat2);

				Level_GridIndexAddBox(&vecPos, pCollBoxes[iCollBoxes]);

				iCollBoxes++;
			}
		}
	}
}