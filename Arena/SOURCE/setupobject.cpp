//
// setup.cpp
// craig
//

#include "setup.h"

//------------------------------------------------------------------

void APIENTRY setup_Exit(Object *pObject)
{
	setup *psetup=(setup *)pObject;

	Mem_Free(psetup);
}

//------------------------------------------------------------------

void APIENTRY setup_Update(Object *pObject)
{
	setup *psetup=(setup *)pObject;
}

//------------------------------------------------------------------

void APIENTRY setup_Render(Object *pObject)
{
	setup *psetup=(setup *)pObject;
}

//------------------------------------------------------------------

setup* setup_Create( )
{
	setup *psetup;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=setup_Exit;
	Create.renderfunc=setup_Render;
	Create.updatefunc=setup_Update;
	Create.iType=OBJECT_setup;
	Create.pName="setup";

	psetup = (setup*)Object_Create(&Create, sizeof(setup));

	return psetup;
}