//
// Pickup.cpp
// nikolay
//

#include "Pickup.h"
#include "opengl.h"
#include "level.h"
#include "collision.h"

static void RocketAmmo_Update(Object *pObject);
static void RocketAmmo_Render(Object *pObject);

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
	case Pickup_NORMAL_AMMO:
		NormalAmmo_Update(&pPickup->pObject);
		break;
	case Pickup_ROCKET_AMMO:
		RocketAmmo_Update(&pPickup->pObject);
		break;
	}
}

//------------------------------------------------------------------

void APIENTRY Pickup_Render(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;

	switch(pPickup->type)
	{
	case Pickup_NORMAL_AMMO:
		NormalAmmo_Render(&pPickup->pObject);
		break;
	case Pickup_ROCKET_AMMO:
		RocketAmmo_Render(&pPickup->pObject);
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

	Vec3 vecMax(0.5f, 0.5f, 0.5f);
	Vec3 vecMin(-0.5f, -0.5f, -0.5f);
	pPickup->pBox = Collision_CreateBox(&vecMax, &vecMin, &Mat);

	return pPickup;
}

//------------------------------------------------------------------

void NormalAmmo_Update(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;
}

//------------------------------------------------------------------

void NormalAmmo_Render(Object *pObject)
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
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);

	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xff00ffff);

	// Right
	GL_Vert(0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	
	GL_Vert(0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	
	// Top
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xff00ffff);
	
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xff00ffff);
	
	// Bottom
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, -0.4f, -0.4f, 0xff00ffff);
	
	GL_Vert(0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xff00ffff);
	
	// Front
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(0.4f, -0.4f, -0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	
	GL_Vert(0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, -0.4f, 0xff00ffff);
	GL_Vert(-0.4f, -0.4f, -0.4f, 0xff00ffff);

	// Back
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, 0.4f, 0.4f, 0xff00ffff);
	GL_Vert(0.4f, -0.4f, 0.4f, 0xff00ffff);
	
	GL_Vert(0.4f, 0.4f, 0.4f, 0xff00ffff);
	GL_Vert(-0.4f, -0.4f, 0.4f, 0xff00ffff);
	GL_Vert(-0.4f, 0.4f, 0.4f, 0xff00ffff);
	
	GL_RenderPrimitives();

	glPopMatrix ();
}


//------------------------------------------------------------------

void RocketAmmo_Update(Object *pObject)
{
	Pickup *pPickup=(Pickup *)pObject;
}

//------------------------------------------------------------------

void RocketAmmo_Render(Object *pObject)
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
