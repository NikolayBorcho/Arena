//
// mainloop.cpp
// craig
//

#include "mainloop.h"
#include "tgaload.h"
#include "input.h"
#include "opengl.h"
#include "camera.h"
#include "font.h"
#include "level.h"
#include "Car.h"
#include "player.h"
#include "Collision.h"
#include "TrashCan.h"
#include "frontend.h"
#include "sprite.h"
#include "Particles.h"

//------------------------------------------------------------------

static u32 iNextMode;
static u32 iMode;
static i32 iMenu;
static bool bPaused = false;
static float fPulseDir=0.1f;
static float fPulse=0.f;
static u32 PulseColour;
Font *pMainFont;

void MainLoop_UpdatePauseMenu();
void MainLoop_RenderPauseMenu();

Player *pTestCar;

//------------------------------------------------------------------

static void _ExitGame()
{

}

//------------------------------------------------------------------

void MainLoop_Init()
{
	Font_Init();
	
	// cache meshes
	LoadMesh3DS("data/models/porche.msh",true);
	LoadMesh3DS("models/evo.3DS",true);
	LoadMesh3DS("models/dodge.3DS",true);
	LoadMesh3DS("models/four.3DS",true);
	pMainFont = Font_Load("data/fonts/frontend");

	iNextMode = MODE_FRONTEND;
	iMode = 0;
}

//------------------------------------------------------------------

void MainLoop_ExitMode()
{
	switch(iMode)
	{
	case MODE_GAME:
		Object_Exit();
		TrashCan_Exit();
		Particles_FreeDefinitions();
		break;

	case MODE_FRONTEND:
		Frontend_Exit();
		break;
	}
}

//------------------------------------------------------------------

void MainLoop_Exit()
{
	MainLoop_ExitMode();
	Font_Destroy(pMainFont);
}

//------------------------------------------------------------------

void MainLoop_InitMode()
{
	switch(iMode)
	{
	case MODE_GAME:
		Object_Init();
		TrashCan_Init();
		Sprite_Create();
		Level_Create( );
		Camera_Init();
		Collision_Create( );
		Particles_LoadDefinitions();
		Level_Start( Frontend_GetLevel() );
		pTestCar = Player_Create(false, Frontend_GetCar( ) );
		Camera_TargetObject((Object*)pTestCar->pCar);
		break;

	case MODE_FRONTEND:
		Frontend_Init();
		break;
	}
}

//------------------------------------------------------------------

bool MainLoop_Update()
{
	Input_Update();

	if(iNextMode != iMode)
	{
		MainLoop_ExitMode();

		iMode = iNextMode;

		MainLoop_InitMode();
	}

	switch(iMode)
	{
	case MODE_GAME:
		Object_Update();
		Camera_Update();
		TrashCan_Update();
			
		break;

	case MODE_FRONTEND:
		Frontend_Update();
		break;
	}

	return false;
}

//------------------------------------------------------------------

void MainLoop_Render()
{
	switch(iMode)
	{
	case MODE_GAME:
		Camera_Render();
		Object_Render();
		break;

	case MODE_FRONTEND:
		Frontend_Render();
		break;
	}
}

//------------------------------------------------------------------

void MainLoop_SetMode(MAIN_MODE iNewMode)
{
	iNextMode=iNewMode;
}

//------------------------------------------------------------------

void MainLoop_UpdatePauseMenu()
{
	if(Input_ButtonClicked(BUTTON_UP))
		iMenu--;
	if(Input_ButtonClicked(BUTTON_DOWN))
		iMenu++;
	if(iMenu<0)
		iMenu=1;
	if(iMenu>1)
		iMenu=0;

	fPulse+=fPulseDir;
	if(fPulse>1.f)
	{
		fPulse=1.f;
		fPulseDir=-fPulseDir;
	}
	if(fPulse<0.f)
	{
		fPulse=0.f;
		fPulseDir=-fPulseDir;
	}
	PulseColour=0xff000000 + (127+((i32)(fPulse*127))) + ((50+((i32)(fPulse*100)))<<8) + (10<<16);

	if(Input_ButtonClicked(BUTTON_FIRE1))
	{
		if(iMenu==1)
			iNextMode = MODE_FRONTEND;

		Mainloop_Pause( false );
	}
}

//------------------------------------------------------------------

void MainLoop_RenderPauseMenu()
{
	/*u8 fade = 200;

	GL_Ortho(SWIDTH,SHEIGHT);

	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	GL_SetTexture(0);
	GL_RenderMode(RENDER_MODE_ALPHA);

	GL_PrimitiveStart(PRIM_TYPE_QUADLIST, RENDER_COLOUR);

	GL_Quad(320 - 90, 240 - 60, 0.f, 180, 120, GL_ARGB(fade, 0, 0, 0));

	GL_RenderPrimitives();

	SetFont(FONT_MIDDLE,pMainFont);
	PrintString(320, 240 - 50,SWITCHCOL(0xffFBBC14),1.f,"PAUSED");

	u32 uColour=0xffffffff;

	if(iMenu==0)
		uColour=PulseColour;
	PrintString(320,240 - 10,uColour,0.5f,"Resume");

	uColour=0xffffffff;
	if(iMenu==1)
		uColour=PulseColour;
	PrintString(320,240 + 10,uColour,0.5f,"Quit");

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	GL_Projection(SWIDTH,SHEIGHT);*/
}

//------------------------------------------------------------------

void Mainloop_Pause( bool bPause )
{
	bPaused = bPause;
}