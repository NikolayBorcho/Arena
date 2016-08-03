//
// Player.cpp
// craig
//

#include "input.h"
#include "camera.h"
#include "Player.h"
#include "Mainloop.h"
#include "Pickup.h"

//------------------------------------------------------------------

void APIENTRY Player_Exit(Object *pObject)
{
	Player *pPlayer=(Player *)pObject;

	Mem_Free(pPlayer);
}

//------------------------------------------------------------------

void APIENTRY Player_Update(Object *pObject)
{
	Player *pPlayer=(Player *)pObject;

	if(!pPlayer->bRemote)
	{
		if(Input_ButtonHeld(BUTTON_LEFT))
		{
			Car_Rot(pPlayer->pCar, true);
		}
		if(Input_ButtonHeld(BUTTON_RIGHT))
		{
			Car_Rot(pPlayer->pCar, false);
		}
		if(Input_ButtonHeld(BUTTON_UP))
		{
			Car_Accel(pPlayer->pCar);
		}
		if(Input_ButtonHeld(BUTTON_DOWN))
		{
			Car_Deccel(pPlayer->pCar);
		}
		if(Input_ButtonHeld(BUTTON_FIRE1))
		{
			Car_Fire(pPlayer->pCar);
		}
		if(Input_ButtonClicked(BUTTON_FIRE2))	// NIK: Clicked instead of held - too many explosions cause too many sprites
		{
			Car_Fire_Rocket(pPlayer->pCar);

			// NIK: Create a Pickup
			/*
			Vec4 offset;

			offset.Set(0.8f, 0.f, 0.f, 1.f);

			Pickup_Create( Pickup_WEAPON_ROCKET, &pPlayer->pCar->pObject, &offset );
			*/
		}

		if(Input_ButtonHeld(BUTTON_CAMLEFT))
			Camera_SetLook( CAM_LEFT );
		else if(Input_ButtonHeld(BUTTON_CAMRIGHT))
			Camera_SetLook( CAM_RIGHT );
		else
			Camera_SetLook( CAM_BACK );
	}
}
 
//------------------------------------------------------------------

Player* Player_Create( bool bRemote, i32 iCarType )
{
	Player *pPlayer;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc = Player_Exit;
	Create.updatefunc = Player_Update;
	Create.iType = OBJECT_Player;
	Create.pName = "Player";

	pPlayer = (Player*)Object_Create(&Create, sizeof(Player));

	pPlayer->bRemote = bRemote;

	pPlayer->pCar = Car_Create(iCarType);

	return pPlayer;
}