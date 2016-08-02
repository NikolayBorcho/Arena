//
// Trail.cpp
// craig
//

#include "includeall.h"
#include "opengl.h"
#include "Trail.h"

void Trail_AddPointInternall(Object *pObject, Vec3 &vecPoint, Vec3 &vecDir);

//------------------------------------------------------------------

void APIENTRY Trail_Exit(Object *pObject)
{
	Trail *pTrail=(Trail *)pObject;

	if(pTrail->pImage)
		ReleaseImage(pTrail->pImage);

	Mem_Free(pTrail);
}

//------------------------------------------------------------------

void APIENTRY Trail_Update(Object *pObject)
{
	Trail *pTrail=(Trail *)pObject;

	while(pTrail->iAddPoints)
	{
		pTrail->iAddPoints --;
		Trail_AddPointInternall(pObject, 
			pTrail->AddBuffer[pTrail->iAddPoints].vecPoint,
			pTrail->AddBuffer[pTrail->iAddPoints].vecDir);
	}

	TrailPoint *pPoint = pTrail->pList;
	TrailPoint *pPointLast = NULL;

	while(pPoint)
	{
		pPoint->fAge -= pTrail->fDecAge;

		if(pPoint->fAge<0.01f)
		{
			pPoint->fAge = 0.f;

			ASSERT(pTrail->pList != pTrail->pList->pNext, "noooo!!!");
			if(pPointLast)
			{
				ASSERT(pPoint != pPointLast, "noooo!!!");
				pPointLast->pNext = pPoint->pNext;
				ASSERT(pPoint != pPoint->pNext, "noooo!!!");
				ASSERT(pTrail->pList != pTrail->pList->pNext, "noooo!!!");
			}
			else
			{
				pTrail->pList = pPoint->pNext;
				ASSERT(pPoint != pPoint->pNext, "noooo!!!");
//				ASSERT(pTrail->pList != pTrail->pList->pNext, "noooo!!!");
			}
		//	ASSERT(pTrail->pList != pTrail->pList->pNext, "noooo!!!");
		}
		else
		{
			pPointLast = pPoint;
		}
		pPoint = pPoint->pNext;
	}

	pPoint = pTrail->pList;
	pTrail->iVertCount = 0;
	Vertex *pVert = pTrail->vertBuff;
	float fTex = 0.f;
	while(pPoint)
	{
		u32 colour;

		if(pTrail->bAlpha )
		{
			colour = ((u32)(pPoint->fAge*pTrail->fA)<<24) + 
				(((u32)pTrail->fR)<<16) + 
				(((u32)pTrail->fG)<<8) + 
				(((u32)pTrail->fB));
		}
		else
		{
			colour =(((u32)(pPoint->fAge*pTrail->fR))<<16) + 
				(((u32)(pPoint->fAge*pTrail->fG))<<8) + 
				(((u32)(pPoint->fAge*pTrail->fB)));
		}

		pVert->x = pPoint->vecPoint[0].GetX();
		pVert->y = pPoint->vecPoint[0].GetY();
		pVert->z = pPoint->vecPoint[0].GetZ();
		pVert->u = fTex;
		pVert->v = 0.f;
		pVert->colour = colour;

		pVert++;
		
		pVert->x = pPoint->vecPoint[1].GetX();
		pVert->y = pPoint->vecPoint[1].GetY();
		pVert->z = pPoint->vecPoint[1].GetZ();
		pVert->u = fTex;
		pVert->v = 1.f;
		pVert->colour = colour;

		pVert++;

		pPoint = pPoint->pNext;

		fTex += 1.f;
		pTrail->iVertCount += 2;
	}

	if(pTrail->bDelete && !pTrail->pList)
	{
		TrashCan_DeleteObject( (Object*)pTrail );
	}
}

//------------------------------------------------------------------

void APIENTRY Trail_Render(Object *pObject)
{
	Trail *pTrail=(Trail *)pObject;
	Vec3 vecPoint;
	Vec3 vecPointRender;
	Vec3 vecDir;

	if(!pTrail->pList)
	{
		return;
	}

	if(!Object_bTransparent)
	{
		Object_AddTransparent(pObject);
		return;
	}

	TrailPoint *pPoint = pTrail->pList;
	
	glDisable(GL_CULL_FACE);

	if(pTrail->pImage)
	{
		float fTex = 0.f;

		GL_SetTexture(pTrail->uTex);

		if(pTrail->bAlpha )
		{
			GL_RenderMode(RENDER_MODE_ALPHATEXTURE);
		}
		else
		{
			GL_RenderMode(RENDER_MODE_TEXTURE_ADD);
		}

		if(pTrail->leadingPointCallback)
		{
			u32 colour;

			if(pTrail->bAlpha )
			{
				colour = ((u32)(pPoint->fAge*pTrail->fA)<<24) + 
					(((u32)pTrail->fR)<<16) + 
					(((u32)pTrail->fG)<<8) + 
					(((u32)pTrail->fB));
			}
			else
			{
				colour =(((u32)(pPoint->fAge*pTrail->fR))<<16) + 
					(((u32)(pPoint->fAge*pTrail->fG))<<8) + 
					(((u32)(pPoint->fAge*pTrail->fB)));
			}

			pTrail->leadingPointCallback(pTrail->pSendData, &vecPoint);

			vecDir = (pPoint->vecPoint[1] - pPoint->vecPoint[0])*0.5f;
			vecPointRender = vecPoint - vecDir;

			GL_PrimitiveStart(PRIM_TYPE_TRIANGLESTRIP, RENDER_UVs|RENDER_COLOUR);

			GL_Vert(vecPointRender.GetX(),
				vecPointRender.GetY(), 
				vecPointRender.GetZ(), 0.f, 0.f, colour);

			vecPointRender = vecPoint + vecDir;
			GL_Vert(vecPointRender.GetX(),
				vecPointRender.GetY(), 
				vecPointRender.GetZ(), 0.f, 1.f, colour);


			GL_Vert(pPoint->vecPoint[0].GetX(),
				pPoint->vecPoint[0].GetY(), 
				pPoint->vecPoint[0].GetZ(), 1.f, 0.f, colour);

			GL_Vert(pPoint->vecPoint[1].GetX(),
				pPoint->vecPoint[1].GetY(), 
				pPoint->vecPoint[1].GetZ(), 1.f, 1.f, colour);

			GL_RenderPrimitives();
		}

		GL_RenderVerts(PRIM_TYPE_TRIANGLESTRIP, RENDER_UVs|RENDER_COLOUR, pTrail->vertBuff, pTrail->iVertCount);
	}
	else
	{
		GL_SetTexture(0);
		if(pTrail->bAlpha )
		{
			GL_RenderMode(RENDER_MODE_ALPHA);
		}
		else
		{
			GL_RenderMode(RENDER_MODE_ADD);
		}

		GL_RenderVerts(PRIM_TYPE_TRIANGLESTRIP, RENDER_COLOUR, pTrail->vertBuff, pTrail->iVertCount);
	}

	glEnable(GL_CULL_FACE);
}

//------------------------------------------------------------------

Trail* Trail_Create( float fDecay, bool bAlpha, u32 uColour, char *pTexture )
{
	Trail *pTrail;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Trail_Exit;
	Create.renderfunc=Trail_Render;
	Create.updatefunc=Trail_Update;
	Create.iType=OBJECT_Trail;
	Create.pName="Trail";

	pTrail = (Trail*)Object_Create(&Create, sizeof(Trail));

	memset(pTrail->vecpoints, 0, sizeof(pTrail->vecpoints));
	pTrail->pList = NULL;
	pTrail->iLastPoint = 0;
	pTrail->fDecAge = fDecay;
	pTrail->bAlpha = bAlpha;

	pTrail->fR = (float)((uColour&0x00ff0000)>>16);
	pTrail->fG = (float)((uColour&0x0000ff00)>>8);
	pTrail->fB = (float)((uColour&0x000000ff));
	pTrail->fA = (float)((uColour&0xff000000)>>24);

	if(pTexture)
	{
		pTrail->pImage = LoadTGA(pTexture, true, true);
		pTrail->uTex = TgaLoad_genTexture(&pTrail->pImage);
	}

	return pTrail;
}

//------------------------------------------------------------------

TrailPoint* Trail_NewPoint(Trail *pTrail)
{
	TrailPoint *pPoints = &pTrail->vecpoints[pTrail->iLastPoint];

	if(pTrail->iLastPoint<MAX_TRAIL_POINTS)
	{
		ASSERT(pPoints->fAge==0.f, "no free points");
		pTrail->iLastPoint = pTrail->iLastPoint+1;
		pPoints->fAge = 1.f;
		pPoints->pNext = NULL;

		return pPoints;
	}

	pTrail->iLastPoint = 1;

	ASSERT(pTrail->vecpoints[0].fAge==0.f, "no free points");

	pTrail->vecpoints[0].fAge = 1.f;
	pTrail->vecpoints[0].pNext = NULL;

	return &pTrail->vecpoints[0];
}

//------------------------------------------------------------------

void Trail_AddPointInternall(Object *pObject, Vec3 &vecPoint, Vec3 &vecDir)
{
	Trail *pTrail=(Trail *)pObject;

	TrailPoint *pPoint = Trail_NewPoint(pTrail);

	ASSERT(pPoint != pPoint->pNext, "noooo!!!");

	if(!pTrail->pList)
	{
		pTrail->pList = pPoint;
		pPoint->pNext = NULL;
	}
	else
	{
		ASSERT(pPoint != pTrail->pList, "noooo!!!");
	//	if(pPoint != pTrail->pList)
		{
			pPoint->pNext = pTrail->pList;
			pTrail->pList = pPoint;
		}
		ASSERT(pPoint != pPoint->pNext, "noooo!!!");
	}

	pPoint->vecPoint[0] = vecPoint - vecDir;
	pPoint->vecPoint[1] = vecPoint + vecDir;
}

//------------------------------------------------------------------

void Trail_AddPoint(Object *pObject, Vec3 &vecPoint, Vec3 &vecDir)
{
	Trail *pTrail=(Trail *)pObject;

	ASSERT(pTrail->iAddPoints<MAX_TRAIL_ADD_BUFFER, "adding to many points to trail");

	pTrail->AddBuffer[pTrail->iAddPoints].vecPoint = vecPoint;
	pTrail->AddBuffer[pTrail->iAddPoints].vecDir = vecDir;

	pTrail->iAddPoints++;
}


//------------------------------------------------------------------

void Trail_SetDelete(Object *pObject, bool bDelete)
{
	Trail *pTrail=(Trail *)pObject;

	pTrail->bDelete = bDelete;

	pTrail->leadingPointCallback = NULL;
}

//------------------------------------------------------------------

void Trail_SetLeadingPointCallback(Object *pObject, void (*leadingPointCallback)(void *pData, Vec3 *pVec))
{
	Trail *pTrail=(Trail *)pObject;

	pTrail->leadingPointCallback = leadingPointCallback;
}
