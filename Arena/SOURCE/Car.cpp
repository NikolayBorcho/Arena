//
// Car.cpp
// craig
//

#include "camera.h"
#include "Car.h"
#include "level.h"
#include "trail.h"
#include "collision.h"
#include "Projectile.h"
#include "Pickup.h"	// NIK

static CarValues CarVals[CAR_MAX]=
{
	{"Evo",		600.f,	180.f,	1.f, 10, 15}, // NIK: added ammo initial values
	{"Porche",	600.f,	180.f,	1.f, 20, 10},
	{"4X4",		600.f,	180.f,	1.f, 30, 5},
	{"Dodge",	600.f,	180.f,	1.f, 40, 0},
};

void Car_HackWheels( Car *pCar );

//------------------------------------------------------------------

void APIENTRY Car_Exit(Object *pObject)
{
	Car *pCar=(Car *)pObject;

	Mesh_Destroy(pCar->pModel);

	Mem_Free(pCar);
}

//------------------------------------------------------------------

void Car_Light(Car *pCar)
{
	Model *pModel=pCar->pModel;
	Matrix *pMat;

	Object_GetMatrixPtr(&pCar->pObject,&pMat);
	Vec3 *pLight = Level_ClosestLight( &pMat->GetColumn(3) );

	i32 i;
	i32 j;
	u8 *pCol=(u8*)pModel->p_uColours;

	i32 uCol;

	float fCol = 1.f - (*pLight - pMat->GetColumn(3)).Length()/50.f;
	if(fCol>1.f)
		fCol = 1.f;

	if(fCol<0.2f)
		fCol = 0.2f;

	uCol = int(fCol*255.f);
	for(j=0;j<pModel->iMeshes;j++)
	{
		for(i=0;i<pModel->pMesh[j]->iNumVerts;i++)
		{
			pCol[0] += (uCol - (i32)pCol[0])>>3;
			pCol[1] += (uCol - (i32)pCol[0])>>3;
			pCol[2] += (uCol - (i32)pCol[0])>>3;

			pCol+=4;
		}
	}
}

//------------------------------------------------------------------

void APIENTRY Car_Update(Object *pObject)
{
	Car *pCar=(Car *)pObject;
	Matrix mat;
	Matrix *pmat;
	Object_GetMatrix(pObject,&mat);
	Object_GetMatrixPtr(pObject,&pmat);

	Vec3 vecVel(pCar->vecBounce + mat.GetColumn(2) * pCar->fSpeed);

	pCar->vecBounce = pCar->vecBounce *0.5f;
	mat.RotY(pCar->fRot);
	mat.SetColumn(3, vecVel + mat.GetColumn(3));

	pCar->fSpeed *= 0.8f;// friction

	pCar->fRotWheelLast = pCar->fRotWheel;
	pCar->fRotWheel += (pCar->fSpeed*30.f);

	Level_GenerateDraw( &mat.GetColumn(3) );
	Level_GenerateAlphas( &mat.GetColumn(3) );
	
	Car_Light(pCar);

	Collision_UpdateMat(pCar->pBox, &mat);

	float fCol=1.f;

	while(Level_TestBoxCollide( pCar->pBox ))
	{
		ColData Data;
		float fDot;

		Collision_GetColData(&Data);	
		
		fDot = -1.8f * Data.normal.Dot(vecVel);

		pCar->vecBounce = (vecVel + Data.normal * fDot)*fCol;

		pCar->fSpeed = 0.f;

		mat.SetColumn(3, pmat->GetColumn(3) + pCar->vecBounce);

		Collision_UpdateMat(pCar->pBox, &mat);

		vecVel = pCar->vecBounce;
		
		fCol-=0.1f;

		if(fCol<0.f)
		{
			pCar->vecBounce.Set(0.f,0.f,0.f);
			mat.SetColumn(3, pmat->GetColumn(3));

			Collision_UpdateMat(pCar->pBox, &mat);

			if (Level_TestBoxCollide( pCar->pBox ))
			{
				// still colliding issue
				pCar->fSpeed = 1.f;
				break;
			}
			
			//ASSERT(!Level_TestBoxCollide( pCar->pBox ), "still colliding");
		}
	}

	// NIK: pickups collide
	Level_TestPickupsCollide( pCar );

	Object_SetMatrix(pObject, &mat);

	Vec3 vecWidth(mat.GetColumn(0)*0.2f);
	Vec3 vecWheel1(mat.GetColumn(3) - mat.GetColumn(0)*0.6f);
	Vec3 vecWheel2(mat.GetColumn(3) + mat.GetColumn(0)*0.6f);

	vecWheel1.SetY( vecWheel1.GetY() - 0.7f );
	vecWheel2.SetY( vecWheel2.GetY() - 0.7f );
	Trail_AddPoint((Object*)pCar->pTrail[0], vecWheel1, vecWidth);
	Trail_AddPoint((Object*)pCar->pTrail[1], vecWheel2, vecWidth);

}

//------------------------------------------------------------------

void APIENTRY Car_Render(Object *pObject)
{
	Car *pCar=(Car *)pObject;
	Matrix *mat;

	Object_GetInterpMatrix(pObject, &mat);

	glPushMatrix ();

	glMultMatrixf (mat->Getfloat());

	Mesh_Render(pCar->pModel,RENDER_MODE_TEXTURE);

	glPopMatrix ();
}

//------------------------------------------------------------------

Car* Car_Create( i32 iType )
{
	Car *pCar;
	ObjectCreate Create;
	Matrix mat;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Car_Exit;
	Create.renderfunc=Car_Render;
	Create.updatefunc=Car_Update;
	Create.iType=OBJECT_Car;
	Create.pName="Car";

	pCar = (Car*)Object_Create(&Create, sizeof(Car));

	switch(iType)
	{
	// NIK: added ammo intializations
	case CAR_PORCHE:
		pCar->pModel = LoadMesh3DS("data/models/porche.3DS",true);
		pCar->fMass = CarVals[CAR_PORCHE].fMass;
		pCar->fPower = CarVals[CAR_PORCHE].fPower;
		pCar->fAccel = CarVals[CAR_PORCHE].fAccel;
		pCar->iNormalAmmo = CarVals[CAR_PORCHE].iNormalAmmo;
		pCar->iRocketAmmo = CarVals[CAR_PORCHE].iRocketAmmo;
		break;
	case CAR_EVO:
		pCar->pModel = LoadMesh3DS("data/models/evo.3DS",true);
		pCar->fMass = CarVals[CAR_EVO].fMass;
		pCar->fPower = CarVals[CAR_EVO].fPower;
		pCar->fAccel = CarVals[CAR_EVO].fAccel;
		pCar->iNormalAmmo = CarVals[CAR_EVO].iNormalAmmo;
		pCar->iRocketAmmo = CarVals[CAR_EVO].iRocketAmmo;
		break;
	case CAR_DODGE:
		pCar->pModel = LoadMesh3DS("data/models/dodge.3DS",true);
		pCar->fMass = CarVals[CAR_DODGE].fMass;
		pCar->fPower = CarVals[CAR_DODGE].fPower;
		pCar->fAccel = CarVals[CAR_DODGE].fAccel;
		pCar->iNormalAmmo = CarVals[CAR_DODGE].iNormalAmmo;
		pCar->iRocketAmmo = CarVals[CAR_DODGE].iRocketAmmo;
		break;
	case CAR_4X4:
		pCar->pModel = LoadMesh3DS("data/models/four.3DS",true);
		pCar->fMass = CarVals[CAR_4X4].fMass;
		pCar->fPower = CarVals[CAR_4X4].fPower;
		pCar->fAccel = CarVals[CAR_4X4].fAccel;
		pCar->iNormalAmmo = CarVals[CAR_4X4].iNormalAmmo;
		pCar->iRocketAmmo = CarVals[CAR_4X4].iRocketAmmo;
		break;
	}

	pCar->fRotIncMax = 6.f;

	Vec3 vecVel(0,1,0);

	mat.Init();
	mat.SetColumn(3, vecVel);
	Object_SetAllMatrix(&pCar->pObject,&mat);

	Car_HackWheels( pCar );

	pCar->pTrail[0] = Trail_Create( 0.02f, true, 0xff080808, "data/textures/tred.tga" );
	pCar->pTrail[1] = Trail_Create( 0.02f, true, 0xff080808, "data/textures/tred.tga" );

	Object_SetLink((Object *)pCar, (Object *)pCar->pTrail[0]);
	Object_SetLink((Object *)pCar, (Object *)pCar->pTrail[1]);

	Vec4 vecBound;
	Mesh_Bounds(pCar->pModel, vecBound);
	Vec3 vecMax(vecBound*0.5f);
	Vec3 vecMin(vecBound*0.5f);
	vecMin = -vecMin;
	pCar->pBox = Collision_CreateBox(&vecMax, &vecMin, &mat);

	return pCar;
}

//------------------------------------------------------------------
void Car_RenderWheelsEnd(void *pData, Mesh *pThis);
void Car_RenderWheelsStart(void *pData, Mesh *pThis);
void Car_HackWheels( Car *pCar )
{
	Model *pModel=pCar->pModel;
	Mesh *pMesh;
	i32 i,j;

	// change the mesh render function of the wheels so they rotate
	pModel->pSendData = pCar;
	for(j=0;j<pModel->iMeshes;j++)
	{
		pMesh = pModel->pMesh[j];
		if(strstr( pMesh->cName,"wheel"))
		{
			pMesh->renderCallBackStart = Car_RenderWheelsStart;
			pMesh->renderCallBackEnd = Car_RenderWheelsEnd;
			float *pVert=pMesh->p_fVertexStart;

			for(i=0;i<pMesh->iNumVerts;i++)
			{
				pVert[0]-=pMesh->vecCenter.GetX();
				pVert[1]-=pMesh->vecCenter.GetY();
				pVert[2]-=pMesh->vecCenter.GetZ();

				pVert+=3;
			}
		}
	}
}

//------------------------------------------------------------------

void Car_RenderWheelsStart(void *pData, Mesh *pThis)
{
	Matrix mat;
	float fAng;

	ASSERT(pData, "no send data");
	Car *pCar=(Car *)pData;

	fAng = pCar->fRotWheelLast * (1.f - Object_RenderFrac) + pCar->fRotWheel * Object_RenderFrac;
	mat.Init();
	mat.RotX(fAng);
	mat.SetColumn(3, pThis->vecCenter);

	glPushMatrix ();

	glMultMatrixf (mat.Getfloat());
}

//------------------------------------------------------------------

void Car_RenderWheelsEnd(void *pData, Mesh *pThis)
{
	glPopMatrix ();
}

//------------------------------------------------------------------

void Car_Fire(Car *pCar)
{
	ASSERT(pCar->pObject.eType==OBJECT_Car, "not a valid car!");
	
	// NIK: added ammo limit
	if (pCar->iNormalAmmo > 0)
	{
		pCar->iNormalAmmo--;

		Vec4 offset;

		offset.Set(0.8f, 0.f, 0.f, 1.f);

		Projectile_Create( PROJECTILE_BULLET, &pCar->pObject, &offset );

		offset.Set(-0.8f, 0.f, 0.f, 1.f);

		Projectile_Create( PROJECTILE_BULLET, &pCar->pObject, &offset );
	}
}

//------------------------------------------------------------------

// NIK: Ability to fire rockets
// This could instead be done by modifying the Car_Fire function to take a projectile type
// However, task does not specify how to handle ammo amount and rockets
void Car_Fire_Rocket(Car *pCar)
{
	ASSERT(pCar->pObject.eType==OBJECT_Car, "not a valid car!");
	
	if (pCar->iRocketAmmo > 0)
	{
		pCar->iRocketAmmo--;

		Vec4 offset;

		offset.Set(0.8f, 0.f, 0.f, 1.f);

		Projectile_Create( PROJECTILE_ROCKET, &pCar->pObject, &offset );

		offset.Set(-0.8f, 0.f, 0.f, 1.f);

		Projectile_Create( PROJECTILE_ROCKET, &pCar->pObject, &offset );
	}
}

//------------------------------------------------------------------

CarValues* Car_GetVal( u32 uVal )
{
	return &CarVals[uVal];
}
