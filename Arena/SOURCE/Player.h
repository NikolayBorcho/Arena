//
// player.h
// craig
//

#ifndef _PLAYER_H
#define _PLAYER_H

#include "includeall.h"
#include "object.h"
#include "car.h"

typedef struct _Player
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	bool	bRemote;
	Car *pCar;
}Player;

Player* Player_Create( bool bRemote, i32 iCarType );

#endif

