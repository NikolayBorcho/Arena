//
// level.h
// craig
//

#ifndef _LEVEL_H
#define _LEVEL_H

#include "includeall.h"
#include "object.h"
#include "tgaload.h"
#include "Car.h"

typedef struct _level
{
	Object Object;

	Image *pImage[12];
}Level;

Level* Level_Create( );

void Level_Start( i32 iLevelNum );
void Level_GenerateDraw( Vec3 *pPos );
void Level_GenerateAlphas( Vec3 *pPos );
Vec3* Level_ClosestLight( Vec3 *pPos );
bool Level_TestCollide( Vec3 *pPos );
bool Level_TestLineCollide( Vec4 &VecStart, Vec4 &VecEnd );
bool Level_TestBoxCollide( struct _ColBox *pBoxThis );
enum Pickup_TYPES Level_TestPickupsCollide( struct _Car *pCarThis );	// NIK
void Level_AddLevelEdgesCols(); // NIK
#endif

