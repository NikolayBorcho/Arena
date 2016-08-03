//
// object.h
// craig
//

#ifndef _OBJECT_H
#define _OBJECT_H

#include "includeall.h"
#include "matrix.h"
#include "TrashCan.h"

enum OBJECT_TYPES
{
	OBJECT_NONE=0,
	OBJECT_Camera,
	OBJECT_Level,
	OBJECT_Car,
	OBJECT_Trail,
	OBJECT_Player,
	OBJECT_Collision,
	OBJECT_Projectile,
	OBJECT_Sprites,
	OBJECT_Particle,
	OBJECT_Pickup
};

#define OBJ_DISABLED	0x000000001 // not rendered or updated
#define OBJ_ENABLED		0x000000002 // rendered and updated
#define OBJ_HIDDEN		0x000000004 // updated not rendered
#define OBJ_STATIC		0x000000008 // updated not rendered
#define OBJ_COLLIDE		0x000000010 // included in collisions
#define OBJ_DELETING	0x000000020 // marked for deletion

typedef struct _Object Object;

typedef void (APIENTRY *RenderFunc)(Object *Obj);
typedef void (APIENTRY *UpdateFunc)(Object *Obj);
typedef void (APIENTRY *ExitFunc)(Object *Obj);
typedef float (APIENTRY *BoundsFunc)(Object *Obj);
typedef void (APIENTRY *CollisionFunc)(Object *Obj);
typedef void (APIENTRY *DamageFunc)(Object *Obj, float xDamage,Vec3 vecHit);

typedef struct _ObjectCreate
{
	RenderFunc renderfunc;
	UpdateFunc updatefunc;
	ExitFunc exitfunc;
	BoundsFunc boundsfunc;
	CollisionFunc collisionfunc;
	DamageFunc damagefunc;
	OBJECT_TYPES iType;
	char *pName;
}ObjectCreate;

struct _Object
{
	OBJECT_TYPES eType;
	u32 uFlag;
	u32 uMatrixFrame;
	Matrix matrix[2];
	Matrix interpmatrix;

	RenderFunc renderfunc;
	UpdateFunc updatefunc;
	ExitFunc exitfunc;
	BoundsFunc boundsfunc;
	CollisionFunc collisionfunc;
	DamageFunc damagefunc;
	u32 uObjectNum;
	i32 iMatrix;
	bool bUpdateRoom;
	u32 uRenderFrame;
	u32 uUpdateFrame;

	Vec4 vecBounds;

	struct _Object *pNext;
#ifdef _DEBUG
	char cName[64];
#endif
};

void Object_Init();
void Object_Exit();
void Object_Update();
void Object_Render();

Object *Object_Create(ObjectCreate *pCreate, u32 size);
void Object_Destroy(Object *pObj);
Object* Object_FindStart();
Object* Object_FindNext();
Object* Object_Find(u32 uObjectNum);
void Object_AddTransparent(Object *pObject);
void Object_SetRoom(Object *pObj,struct _Room *pRoom);
void Object_SetMatrix(Object *pObj,Matrix *pMat);
void Object_SetAllMatrix(Object *pObj,Matrix *pMat);
void Object_GetMatrix(Object *pObj,Matrix *pMat);
void Object_GetMatrixPtr(Object *pObj,Matrix **pMat);
void Object_GetMatrixLast(Object *pObj,Matrix *pMat);
void Object_GetMatrixPtrLast(Object *pObj,Matrix **pMat);
void Object_SetInterpMatrix(Object *pObj,Matrix *pMat);
void Object_GetInterpMatrix(Object *pObj,Matrix **pMat);
void Object_IncUpdateFrame();
void Object_IncRenderFrame();
u32 Object_GetUpdateFrame();
u32 Object_GetRenderFrame();
// set all objects current and last frame matrices to current
void Object_SetAllMatricesAbsolute();
inline void Object_SetFlag(Object *pObj,u32 uFlag) { pObj->uFlag=uFlag; };
inline void Object_AddFlag(Object *pObj,u32 uFlag) { pObj->uFlag|=uFlag; };
inline void Object_DelFlag(Object *pObj,u32 uFlag) { pObj->uFlag&=~uFlag; };
//inline bool Object_GetFlag(Object *pObj,u32 uFlag) { return ((pObj->uFlag&uFlag)==uFlag); };
inline bool Object_GetFlag(Object *pObj,u32 uFlag) { return ((pObj->uFlag&uFlag) ? true:false); };
inline bool Object_IsType(Object *pObj,OBJECT_TYPES Type) { return (pObj->eType==Type); };

inline void Object_SetLink(Object *pObj,Object *pObjLink) 
{ 
	pObjLink->pNext = pObj->pNext;

	pObj->pNext = pObjLink;
};

void Object_Disable(Object *pObj, bool bDisable);

extern float Object_RenderFrac;
extern bool	Object_bTransparent;
#endif

