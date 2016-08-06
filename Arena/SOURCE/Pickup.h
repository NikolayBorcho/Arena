//
// Pickup.h
// nikolay
//

#ifndef _PICKUP_H
#define _PICKUP_H

#include "includeall.h"
#include "object.h"

enum Pickup_TYPES
{
	Pickup_NONE = 0,
	Pickup_NORMAL_AMMO,
	Pickup_ROCKET_AMMO
};

typedef struct _Pickup
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	Pickup_TYPES type;
	Vec3	vecPos;
	float fRotationAngle;

	struct _ColBox *pBox;
}Pickup;

Pickup* Pickup_Create( Pickup_TYPES type, Vec3 vecPos);
void NormalAmmo_Update(Object *pObject);
void RocketAmmo_Update(Object *pObject);
void NormalAmmo_Render(Object *pObject);
void RocketAmmo_Render(Object *pObject);

#endif

