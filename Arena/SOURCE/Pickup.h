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
	Pickup_WEAPON_ROCKET,
	Pickup_AMMO,
};

typedef struct _Pickup
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	Pickup_TYPES type;
	Vec3	vecPos;

	struct _ColBox *pBox;
}Pickup;

Pickup* Pickup_Create( Pickup_TYPES type, Vec3 vecPos);

#endif

