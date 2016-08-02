//
// TrashCan.cpp
// craig
//

#include "TrashCan.h"
#include "object.h"

#define DELETE_OBJECTS 100

typedef struct _TrashBin
{
	Object *pObj;
	bool bInRoom;
	u32 uRemoveTime;
}TrashBin;

static TrashBin Trash[DELETE_OBJECTS];
static i32 iObjectsInTrash;

//------------------------------------------------------------------

void TrashCan_Init()
{
	iObjectsInTrash=0;
}

//------------------------------------------------------------------

void TrashCan_Exit()
{
}

//------------------------------------------------------------------

void TrashCan_Update()
{
	i32 i;

	for(i=0;i<iObjectsInTrash;i++)
	{
		if(Trash[i].bInRoom)
		{
			Trash[i].bInRoom=false;
		}
		if(Trash[i].uRemoveTime<Object_GetUpdateFrame())
		{
			iObjectsInTrash--;
			Object_Destroy(Trash[i].pObj);
			Trash[i]=Trash[iObjectsInTrash];
			i--;
		}
	}
}

//------------------------------------------------------------------

void TrashCan_Render()
{
}

//------------------------------------------------------------------

void TrashCan_DeleteObject( Object *pObj )
{
	ASSERT(iObjectsInTrash<DELETE_OBJECTS,"deleting too many objects");

	Object_SetFlag(pObj,OBJ_DISABLED);
	Object_AddFlag(pObj,OBJ_DELETING);

	Trash[iObjectsInTrash].pObj=pObj;
	Trash[iObjectsInTrash].uRemoveTime=2+Object_GetUpdateFrame();
	Trash[iObjectsInTrash].bInRoom=true;
	
	iObjectsInTrash++;
}