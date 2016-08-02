//
// object.cpp
// craig
//

#include "object.h"
#include "opengl.h"

#define MAX_OBJECTS 1000

static Object *pObjectList[MAX_OBJECTS];
static u32 iNumObjects;
static Object *pObjectListTransparent[MAX_OBJECTS];
static u32 iNumTransparentObjects;
static u32 iFind;
static u32 uUpdateFrame;
static u32 uRenderFrame;
float Object_RenderFrac;
bool Object_bTransparent;

//------------------------------------------------------------------

void Object_Init()
{
	iNumObjects=0;
	uUpdateFrame=0;
	uRenderFrame=0;
	Object_RenderFrac=0;
	iNumTransparentObjects = 0;
}

//------------------------------------------------------------------

void Object_Exit()
{
	u32 i;

	for(i=0;i<iNumObjects;i++)
	{
		if(pObjectList[i]->exitfunc)
			pObjectList[i]->exitfunc(pObjectList[i]);

//		Mem_Free(pObjectList[i]);
	}
	iNumObjects=0;
}

//------------------------------------------------------------------

void Object_Update()
{
	u32 i;
	Object **pObjects = &pObjectList[0];

	for(i=0;i<iNumObjects;i++)
	{
		if(!Object_GetFlag(*pObjects,OBJ_DISABLED))
		{
			if((*pObjects)->updatefunc)
			{
				(*pObjects)->updatefunc(*pObjects);
			}
		}
		*pObjects++;
	}
}

//------------------------------------------------------------------

void Object_Render()
{
	u32 i;
	Object **pObjects = &pObjectList[0];

	iNumTransparentObjects = 0;
	Object_bTransparent = false;
	for(i=0;i<iNumObjects;i++)
	{
		if(!Object_GetFlag(*pObjects, (OBJ_DISABLED|OBJ_HIDDEN) ))
		{
			if((*pObjects)->renderfunc)
			{
				(*pObjects)->renderfunc(*pObjects);
			}
		}
		*pObjects++;
	}

	glDepthMask(0);

	Object_bTransparent = true;
	pObjects = &pObjectListTransparent[0];
	for(i=0;i<iNumTransparentObjects;i++)
	{
		(*pObjects)->renderfunc(*pObjects);
		*pObjects++;
	}

	glDepthMask(1);
}

//------------------------------------------------------------------

void Object_AddTransparent(Object *pObject)
{
	ASSERT(iNumTransparentObjects<MAX_OBJECTS, "too many transparent objects");

	pObjectListTransparent[iNumTransparentObjects] = pObject;

	iNumTransparentObjects++;
}

//------------------------------------------------------------------

float APIENTRY Object_Bounds(Object *pObject)
{
	return 0.005f;
}

//------------------------------------------------------------------

Object *Object_Create(ObjectCreate *pCreate, u32 size)
{
	Object *pObj=(Object *)Mem_New(size);

	memset(pObj, 0, size);

	pObj->eType=pCreate->iType;
	pObj->matrix[0].Init();
	pObj->matrix[1].Init();
	pObj->interpmatrix.Init();
	pObj->renderfunc=pCreate->renderfunc;
	pObj->updatefunc=pCreate->updatefunc;
	pObj->exitfunc=pCreate->exitfunc;
	pObj->boundsfunc=pCreate->boundsfunc;
	pObj->collisionfunc=pCreate->collisionfunc;
	pObj->damagefunc=pCreate->damagefunc;
	if(!pObj->boundsfunc)
	{
		pObj->boundsfunc=Object_Bounds;
	}

	pObj->bUpdateRoom=false;
	pObj->uFlag=OBJ_ENABLED;

	pObj->uObjectNum=iNumObjects;

#ifdef _DEBUG
	strcpy(pObj->cName,pCreate->pName);
#endif

	pObjectList[iNumObjects]=pObj;
	iNumObjects++;

	ASSERT(iNumObjects<MAX_OBJECTS, "Too many objects");

	pObj->vecBounds.Set(0.f,0.f,0.f,0.f);

#ifdef _DEBUG
	ASSERT(_CrtCheckMemory(),"bad memory");
#endif

	return pObj;
}

//------------------------------------------------------------------

void Object_Destroy(Object *pObj)
{
	u32 i;

	if(pObj->exitfunc)
	{
		pObj->exitfunc(pObj);
	}

	for(i=0;i<iNumObjects;i++)
	{
		if(pObjectList[i]==pObj)
		{
			iNumObjects--;
		//	Mem_Free(pObjectList[i]);
			pObjectList[i]=pObjectList[iNumObjects];
			break;
		}
	}
	ASSERT(iNumObjects<MAX_OBJECTS,"deleting more objects than there are!!");
}

//------------------------------------------------------------------

void Object_SetMatrix(Object *pObj,Matrix *pMat)
{
	if(pObj->uMatrixFrame != Object_GetUpdateFrame())
	{
		pObj->iMatrix = 1 - pObj->iMatrix;
	}

	pObj->uMatrixFrame = Object_GetUpdateFrame();
	pObj->matrix[pObj->iMatrix] = *pMat;
}

//------------------------------------------------------------------

void Object_SetAllMatrix(Object *pObj,Matrix *pMat)
{
	pObj->matrix[0] = *pMat;
	pObj->matrix[1] = *pMat;
}

//------------------------------------------------------------------

void Object_GetMatrix(Object *pObj,Matrix *pMat)
{
	*pMat = pObj->matrix[pObj->iMatrix];
}

//------------------------------------------------------------------

void Object_GetMatrixPtr(Object *pObj,Matrix **pMat)
{
	*pMat = &pObj->matrix[pObj->iMatrix];
}

//------------------------------------------------------------------

void Object_GetMatrixLast(Object *pObj,Matrix *pMat)
{
	*pMat = pObj->matrix[1 - pObj->iMatrix];
}

//------------------------------------------------------------------

void Object_GetMatrixPtrLast(Object *pObj,Matrix **pMat)
{
	*pMat = &pObj->matrix[1 - pObj->iMatrix];
}

//------------------------------------------------------------------

void Object_SetInterpMatrix(Object *pObj,Matrix *pMat)
{
	pObj->interpmatrix=*pMat;
}

//------------------------------------------------------------------

void Object_GetInterpMatrix(Object *pObj,Matrix **pMat)
{
	if(pObj->uRenderFrame != Object_GetRenderFrame())
	{
		pObj->uRenderFrame = Object_GetRenderFrame();

	//	pObj->interpmatrix = pObj->matrix[1 - pObj->iMatrix];
		pObj->interpmatrix = 
			pObj->matrix[1 - pObj->iMatrix].Interp(Object_RenderFrac, pObj->matrix[pObj->iMatrix]);
	}
	*pMat = &pObj->interpmatrix;
}

//------------------------------------------------------------------

Object* Object_FindStart()
{
	iFind=0;

	return pObjectList[0];
}

//------------------------------------------------------------------

Object* Object_FindNext()
{
	iFind++;

	if(iFind<iNumObjects)
	{
		return pObjectList[iFind];
	}

	return NULL;
}

//------------------------------------------------------------------

void Object_IncUpdateFrame()
{
	uUpdateFrame++;
}

//------------------------------------------------------------------

void Object_IncRenderFrame()
{
	uRenderFrame++;
}

//------------------------------------------------------------------

u32 Object_GetUpdateFrame()
{
	return uUpdateFrame;
}

//------------------------------------------------------------------

u32 Object_GetRenderFrame()
{
	return uRenderFrame;
}

//------------------------------------------------------------------

Object* Object_Find(u32 uObjectNum)
{
	u32 i;

	for(i=0;i<iNumObjects;i++)
	{
		if(pObjectList[i]->uObjectNum==uObjectNum)
		{
			return pObjectList[i];
		}
	}
	return 0;
}

//------------------------------------------------------------------

void Object_Collisions()
{
}

//------------------------------------------------------------------

void Object_Disable(Object *pObj, bool bDisable) 
{ 
	Object *pObj2 = pObj;

	while(pObj2)
	{
		if(bDisable)
		{
			Object_AddFlag(pObj2,OBJ_DISABLED);
		}
		else
		{
			Object_DelFlag(pObj2,OBJ_DISABLED);
		}

		pObj2 = pObj2->pNext;
	}
}

//------------------------------------------------------------------

void Object_SetAllMatricesAbsolute()
{
	u32 i;
	Matrix Mat;

	for(i=0;i<iNumObjects;i++)
	{
		Object_GetMatrix(pObjectList[i], &Mat);
		Object_SetAllMatrix(pObjectList[i], &Mat);
	}
}
