//
// car.h
// craig
//

#ifndef _CAR_H
#define _CAR_H

#include "includeall.h"
#include "object.h"
#include "mesh.h"

#define Friction 0.1f

enum 
{
	CAR_EVO,
	CAR_PORCHE,
	CAR_4X4,
	CAR_DODGE,
	CAR_MAX
};

typedef struct _CarValues
{
	char *name;
	float fMass;
	float fPower;
	float fAccel;
	i32 iNormalAmmo; // NIK: added ammo
	i32 iRocketAmmo; // NIK: added ammo
}CarValues;

typedef struct _Car
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	Model	*pModel;

	float fMass;
	float fAccel;
	float fSpeed;
	float fPower;
	i32 iNormalAmmo; // NIK: added ammo
	i32 iRocketAmmo; // NIK: added ammo

	float fRot;
	float fRotInc;
	float fRotIncMax;

	float fRotWheel;
	float fRotWheelLast;

	Vec3 vecBounce;
	struct _Trail *pTrail[2];

	struct _ColBox *pBox;
}Car;

Car* Car_Create( i32 iType );

inline void Car_Accel(Car *pCar)
{
	ASSERT(pCar->pObject.eType==OBJECT_Car, "not a valid car!");

	pCar->fSpeed += ((pCar->fPower - (Friction * pCar->fSpeed)) / pCar->fMass);
}

inline void Car_Deccel(Car *pCar)
{
	ASSERT(pCar->pObject.eType==OBJECT_Car, "not a valid car!");

	pCar->fSpeed -= ((pCar->fPower - (Friction * pCar->fSpeed)) / pCar->fMass);
}

inline void Car_Rot(Car *pCar, bool bLeft)
{
	ASSERT(pCar->pObject.eType==OBJECT_Car, "not a valid car!");
	float fModif = pCar->fSpeed;
	
	pCar->fRotInc += (pCar->fRotIncMax - pCar->fRotInc)*0.01f;

	if(fModif>1.f)
	{
		fModif = 1.f;
	}
	if(fModif<-1.f)
	{
		fModif = -1.f;
	}
	if(bLeft)
	{
		pCar->fRot -= (pCar->fRotIncMax*fModif);
	}
	else
	{
		pCar->fRot += (pCar->fRotIncMax*fModif);
	}
}

void Car_Fire(Car *pCar);
void Car_Fire_Rocket(Car *pCar);	// NIK: Added separate function for firing rockets
CarValues* Car_GetVal( u32 uVal );

#endif

