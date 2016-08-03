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
	Pickup_WEAPON_ROCKET = 0,
	Pickup_AMMO,
};

typedef struct _Pickup
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	Pickup_TYPES type;
	Vec3	vecPos;

	Object	*pCreator;
	struct _ColBox *pBox;
}Pickup;

Pickup* Pickup_Create( Pickup_TYPES type, Object *pCreator, Vec4 *pvecOffset );

#endif

