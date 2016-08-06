//
// projectile.h
// craig
//

#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "includeall.h"
#include "object.h"

enum PROJECTILE_TYPES
{
	PROJECTILE_BULLET = 0,
	PROJECTILE_ROCKET,
};

typedef struct _Projectile
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	PROJECTILE_TYPES type;
	Vec3	vecVel;
	float	fLife;
	struct _Trail *pTrails[2];
	// NIK: added explosion limit to avoid too many sprites
	i32 iExplosions;

	Object	*pCreator;
}Projectile;

Projectile* Projectile_Create( PROJECTILE_TYPES type, Object *pCreator, Vec4 *pvecOffset );

#endif

