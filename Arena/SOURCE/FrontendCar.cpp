//
// FrontendCar.cpp
// craig
//

#include "mainloop.h"
#include "Frontend.h"
#include "FrontendCar.h"
#include "car.h"
#include "font.h"
#include "input.h"
#include "camera.h"

static i32 iSelection;
static Car *cars[CAR_MAX];
static float fRot;

//------------------------------------------------------------------

void FrontendCar_Init()
{
	i32 i;
	Matrix mat;

	iSelection = 0;

	fRot = 0.f;

	Object_Init();
	Camera_Init();

	mat.Init();

	for(i=0;i<CAR_MAX; i++)
	{
		cars[i] = Car_Create( i );

		mat.Init();
		Object_SetAllMatrix((Object *)cars[i],&mat);
		Object_Disable((Object *)cars[i], true);
	}

	Object_Disable((Object *)cars[iSelection], false);

	Camera_SetPos(Vec3(4.f, 2.f, 0.f));
	Camera_LookAt(Vec3(0.f, 0.f, 0.f), true);
}

//------------------------------------------------------------------

void FrontendCar_Exit()
{
	Object_Exit();
}

//------------------------------------------------------------------

void FrontendCar_Update()
{
	Matrix mat;
	i32 iOldSelection = iSelection;

	if(Input_ButtonClicked(BUTTON_LEFT))
	{
		iSelection--;

		if(iSelection<0)
		{
			iSelection = CAR_MAX-1;
		}
	}

	if(Input_ButtonClicked(BUTTON_RIGHT))
	{
		iSelection++;

		if(iSelection>=CAR_MAX)
		{
			iSelection = 0;
		}
	}

	if(iOldSelection != iSelection)
	{
		Object_Disable((Object *)cars[iOldSelection], true);

		Object_Disable((Object *)cars[iSelection], false);

		mat.Init();
		mat.RotY(fRot);
		Object_SetAllMatrix((Object *)cars[iSelection],&mat);
	}

	if(Input_ButtonClicked(BUTTON_FIRE1))
	{
		Frontend_SetCar( iSelection );
		MainLoop_SetMode(MODE_GAME);
	}

	Object_Update();
	Camera_Update();

	mat.Init();
	mat.RotY(fRot);
	Object_SetMatrix((Object *)cars[iSelection],&mat);

	fRot += 1.f;
}

//------------------------------------------------------------------

void FrontendCar_Render()
{
	float fX = 20.f;
	float fY = 50.f;

	glViewport(SWIDTH/2, SHEIGHT/2, SWIDTH/2, SHEIGHT/2);

	Camera_Render();
	Object_Render();

	glViewport(0, 0, SWIDTH, SHEIGHT);

	SetFont(FONT_LEFT,Frontend_GetFont());

	PrintString(fX, fY, 0xffffffff, 0.5f, "Name %s\nMass %0.1f\nPower %0.1f\nAccel %0.1f\nAmmo %i",
		Car_GetVal(iSelection)->name,
		Car_GetVal(iSelection)->fMass,
		Car_GetVal(iSelection)->fPower,
		Car_GetVal(iSelection)->fAccel,
		Car_GetVal(iSelection)->iAmmo); // NIK: added ammo display on car select
}

