//
// Pickup.cpp
// nikolay
//

#include "Pickup.h"
#include "opengl.h"
#include "level.h"
#include "collision.h"

static void WeaponRocket_Update(Object *pObject);
static void WeaponRocket_Render(Object *pObject);

//------------------------------------------------------------------

void APIENTRY Pickup_Exit(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;

	Mem_Free(pPickup);
}

//------------------------------------------------------------------

void APIENTRY Pickup_Update(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;
	
	switch(pPickup->type)
	{
	case Pickup_WEAPON_ROCKET:
		WeaponRocket_Update(pObject);
		break;
	}
}

//------------------------------------------------------------------

void APIENTRY Pickup_Render(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;

	switch(pPickup->type)
	{
	case Pickup_WEAPON_ROCKET:
		WeaponRocket_Render(pObject);
		break;
	}
}

//------------------------------------------------------------------

Pickup* Pickup_Create( Pickup_TYPES type, Vec3 vecPos)
{
	Pickup *pPickup;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Pickup_Exit;
	Create.renderfunc=Pickup_Render;
	Create.updatefunc=Pickup_Update;
	Create.iType=OBJECT_Pickup;
	Create.pName="Pickup";

	pPickup = (Pickup*)Object_Create(&Create, sizeof(Pickup));

	pPickup->type = type;

	Matrix Mat;

	Mat.Init();
	Mat.SetColumn(3,vecPos);

	Object_SetAllMatrix((Object *)pPickup, &Mat);

	switch(type)
	{
	case Pickup_WEAPON_ROCKET:
		WeaponRocket_Update(&pPickup->pObject);
		break;
	}

	Vec3 vecMax(0.5f, 0.5f, 0.5f);
	Vec3 vecMin(-0.5f, -0.5f, -0.5f);
	pPickup->pBox = Collision_CreateBox(&vecMax, &vecMin, &Mat);

	return pPickup;
}

//------------------------------------------------------------------

void WeaponRocket_Update(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;

	/* // collision with level
	Pickup *pPickup=(Pickup *)pObject;

	Matrix mat;
	Matrix *pLast;

	Object_GetMatrix(pObject,&mat);
	Object_GetMatrixPtrLast(pObject,&pLast);

	if(Level_TestLineCollide( pLast->GetColumn(3), mat.GetColumn(3) ))
	{
		ColData Data;

		// Get the collision data
		Collision_GetColData(&Data);

		// Create particle effect at collision position
		//Particles_Create( 1, Data.vecPoint );
	}
	
	Object_SetMatrix(pObject, &mat);*/
}

//------------------------------------------------------------------

void WeaponRocket_Render(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;

	Matrix *mat;

	Object_GetInterpMatrix(pObject, &mat);

	glPushMatrix ();

	glMultMatrixf (mat->Getfloat());

	GL_SetTexture(0);
	GL_RenderMode(RENDER_MODE_ADD);
	
	GL_PrimitiveStart(PRIM_TYPE_TRIANGLELIST, RENDER_COLOUR);

	// INDEX Buffer?

	// Left
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);

	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xffffffff);

	// Right
	GL_Vert(0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	
	GL_Vert(0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	
	// Top
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xffffffff);
	
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xffffffff);
	
	// Bottom
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, -0.4f, -0.4f, 0xffffffff);
	
	GL_Vert(0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xffffffff);
	
	// Front
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(0.4f, -0.4f, -0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	
	GL_Vert(0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xffffffff);
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xffffffff);

	// Back
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xffffffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xffffffff);
	
	GL_Vert(0.4f, 0.4f, 0.4f, 0xffffffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xffffffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xffffffff);
	
	GL_RenderPrimitives();

	glPopMatrix ();
}

//------------------------------------------------------------------
