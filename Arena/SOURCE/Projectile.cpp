//
// Projectile.cpp
// craig
//

#include "Projectile.h"
#include "opengl.h"
#include "level.h"
#include "trail.h"
#include "Particles.h"
#include "collision.h"

static void Bullet_Update(Object *pObject);
static void Bullet_Render(Object *pObject);

static void Rocket_Update(Object *pObject);
static void Rocket_Render(Object *pObject);

static void Rocket_GetRenderTrans(void *pData, Vec3 *pVec);

//------------------------------------------------------------------

void APIENTRY Projectile_Exit(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;

	Mem_Free(pProjectile);
}

//------------------------------------------------------------------

void APIENTRY Projectile_Update(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;
	
	switch(pProjectile->type)
	{
	case PROJECTILE_BULLET:
		Bullet_Update(pObject);
		break;

	case PROJECTILE_ROCKET:
		Rocket_Update(pObject);
		break;
	}

	pProjectile->fLife -= 0.1f;

	if(pProjectile->fLife<0.f)
	{
		if(pProjectile->pTrails[0])
		{
			Trail_SetDelete( (Object*)pProjectile->pTrails[0], true );
		}
		if(pProjectile->pTrails[1])
		{
			Trail_SetDelete( (Object*)pProjectile->pTrails[1], true );
		}
		TrashCan_DeleteObject( (Object*)pProjectile );
	}
}

//------------------------------------------------------------------

void APIENTRY Projectile_Render(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;

	switch(pProjectile->type)
	{
	case PROJECTILE_BULLET:
		Bullet_Render(pObject);
		break;

	case PROJECTILE_ROCKET:
		Rocket_Render(pObject);
		break;
	}
}

//------------------------------------------------------------------

Projectile* Projectile_Create( PROJECTILE_TYPES type, Object *pCreator, Vec4 *pvecOffset )
{
	Projectile *pProjectile;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Projectile_Exit;
	Create.renderfunc=Projectile_Render;
	Create.updatefunc=Projectile_Update;
	Create.iType=OBJECT_Projectile;
	Create.pName="Projectile";

	pProjectile = (Projectile*)Object_Create(&Create, sizeof(Projectile));

	pProjectile->type = type;
	pProjectile->pCreator = pCreator;

	Matrix *pMat;
	Matrix Mat;

	Object_GetMatrixPtr((Object *)pCreator, &pMat);

	Mat = *pMat;
	Mat.SetColumn(3, *pMat * (*pvecOffset));
	Object_SetAllMatrix((Object *)pProjectile, &Mat);

	Vec3 vecDir;

	switch(type)
	{
	case PROJECTILE_BULLET:
		pProjectile->fLife = 1.f;
		pProjectile->vecVel = Mat.GetColumn(2) * 7.f; 
		break;

	case PROJECTILE_ROCKET:
		pProjectile->fLife = 3.f;
		pProjectile->vecVel = Mat.GetColumn(2) * 5.f; 

		pProjectile->pTrails[0] = Trail_Create( 0.08f, false, SWITCHCOL(0x1faaaaaa), "data/textures/stream.tga" );
		pProjectile->pTrails[1] = Trail_Create( 0.08f, false, SWITCHCOL(0x1faaaaaa), "data/textures/stream.tga" );

		vecDir = (Mat.GetColumn(0)*0.25f);

		Trail_AddPoint((Object*)pProjectile->pTrails[0], Mat.GetColumn(3), vecDir);

		vecDir = (Mat.GetColumn(1)*0.25f);
		Trail_AddPoint((Object*)pProjectile->pTrails[1], Mat.GetColumn(3), vecDir);

		pProjectile->pTrails[0]->leadingPointCallback = Rocket_GetRenderTrans;
		pProjectile->pTrails[0]->pSendData = pProjectile;

		pProjectile->pTrails[1]->leadingPointCallback = Rocket_GetRenderTrans;
		pProjectile->pTrails[1]->pSendData = pProjectile;
		break;
	}

	return pProjectile;
}

//------------------------------------------------------------------

void Bullet_Update(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;

	Matrix mat;
	Matrix *pLast;

	Object_GetMatrix(pObject,&mat);
	mat.SetColumn(3, mat.GetColumn(3) + pProjectile->vecVel);

	Object_GetMatrixPtrLast(pObject,&pLast);

	if(Level_TestLineCollide( pLast->GetColumn(3), mat.GetColumn(3) ))
	{
		ColData Data;

		// Get the collision data
		Collision_GetColData(&Data);

		// Create particle effect at collision position
		Particles_Create( 1, Data.vecPoint );

		pProjectile->fLife = -1.f;
	}

	Object_SetMatrix(pObject, &mat);
}

//------------------------------------------------------------------

void Bullet_Render(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;

	Matrix *mat;

	if(!Object_bTransparent)
	{
		Object_AddTransparent(pObject);
		return;
	}

	Object_GetInterpMatrix(pObject, &mat);

	glPushMatrix ();

	glMultMatrixf (mat->Getfloat());

	glDisable(GL_CULL_FACE);

	GL_SetTexture(0);
	GL_RenderMode(RENDER_MODE_ADD);
	
	GL_PrimitiveStart(PRIM_TYPE_TRIANGLELIST, RENDER_COLOUR);

	GL_Vert(-0.1f, 0.f, 0.f, 0);
	GL_Vert( 0.1f, 0.f, 0.f, 0);
	GL_Vert(-0.1f, 0.f, 2.f, SWITCHCOL(0xffe59619));

	GL_Vert(-0.1f, 0.f, 2.f, SWITCHCOL(0xffe59619));
	GL_Vert( 0.1f, 0.f, 0.f, 0);
	GL_Vert( 0.1f, 0.f, 2.f, SWITCHCOL(0xffe59619));

	GL_Vert(0.f, -0.1f, 0.f, 0);
	GL_Vert(0.f,  0.1f, 0.f, 0);
	GL_Vert(0.f, -0.1f, 2.f, SWITCHCOL(0xffe59619));

	GL_Vert(0.f, -0.1f, 2.f, SWITCHCOL(0xffe59619));
	GL_Vert( 0.f, 0.1f, 0.f, 0);
	GL_Vert( 0.f, 0.1f, 2.f, SWITCHCOL(0xffe59619));

	GL_RenderPrimitives();

	glEnable(GL_CULL_FACE);

	glPopMatrix ();
}

//------------------------------------------------------------------

void Rocket_Update(Object *pObject)
{
	Projectile *pProjectile=(Projectile *)pObject;

	Matrix mat;
	Matrix *pLast;

	Object_GetMatrix(pObject,&mat);
	mat.SetColumn(3, mat.GetColumn(3) + pProjectile->vecVel);

	Object_GetMatrixPtrLast(pObject,&pLast);

	Vec3 vecDir(mat.GetColumn(0)*0.25f);

	Trail_AddPoint((Object*)pProjectile->pTrails[0], pLast->GetColumn(3), vecDir);

	vecDir = (mat.GetColumn(1)*0.25f);
	Trail_AddPoint((Object*)pProjectile->pTrails[1], pLast->GetColumn(3), vecDir);

	if(Level_TestLineCollide( pLast->GetColumn(3), mat.GetColumn(3) ))
	{
		pProjectile->fLife = -1.f;

		ColData Data;

		// Get the collision data
		Collision_GetColData(&Data);

		vecDir = (mat.GetColumn(0)*0.25f);

		Trail_AddPoint((Object*)pProjectile->pTrails[0], Data.vecPoint, vecDir);

		vecDir = (mat.GetColumn(1)*0.25f);
		Trail_AddPoint((Object*)pProjectile->pTrails[1], Data.vecPoint, vecDir);

		// NIK: Create particle effect at collision position
		Particles_Create( 0, Data.vecPoint );
	}

	Object_SetMatrix(pObject, &mat);
}

//------------------------------------------------------------------

void Rocket_Render(Object *pObject)
{
}

//------------------------------------------------------------------

void Rocket_GetRenderTrans(void *pData, Vec3 *pVec)
{
	Object *pObj = (Object*)pData;
	Matrix *pMat;

	Object_GetInterpMatrix(pObj, &pMat);
	*pVec = pMat->GetColumn(3);
}