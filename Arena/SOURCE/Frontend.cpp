//
// Frontend.cpp
// craig
//

#include "Frontend.h"
#include "FrontendCar.h"
#include "opengl.h"

FrontendMode FrontendModes[FRONTEND_MAX]=
{
	// init					exit					update					render
	{NULL,					NULL,					NULL,					NULL,},
	{FrontendCar_Init,		FrontendCar_Exit,		FrontendCar_Update,		FrontendCar_Render,},
};

enum 
{
	FRONTEND_NORMAL =0,
	FRONTEND_FADINGOUT,
	FRONTEND_FADINGIN
};

static u32 Frontend_Mode;
static u32 Frontend_ModeNext;
static i32 Frontend_Level = 0;
static u32 Frontend_theCar = 0;
static float fFade;
static float fFadeDir = 0.1f;
static u32 Frontend_UpdateMode;
 
static Font *pFrontendFont;

//------------------------------------------------------------------

void Frontend_Init()
{
	pFrontendFont = Font_Load("data/fonts/frontend");

	Frontend_Mode = FRONTEND_NONE;
	Frontend_ModeNext = FRONTEND_SELECTCAR;//FRONTEND_NONE;

	fFade = 1.f;
	Frontend_UpdateMode = FRONTEND_FADINGOUT;
}

//------------------------------------------------------------------

void Frontend_Exit()
{
	if(FrontendModes[Frontend_Mode].exit)
	{
		FrontendModes[Frontend_Mode].exit();
	}

	Font_Destroy(pFrontendFont);
}

//------------------------------------------------------------------

void Frontend_Update()
{
	switch(Frontend_UpdateMode)
	{
	case FRONTEND_NORMAL:

		if(FrontendModes[Frontend_Mode].update)
		{
			FrontendModes[Frontend_Mode].update();
		}
		break;

	case FRONTEND_FADINGOUT:
		fFade += fFadeDir;

		if(fFade>1.f)
		{
			Frontend_UpdateMode = FRONTEND_FADINGIN;
			fFade = 1.f;

			if(FrontendModes[Frontend_Mode].exit)
			{
				FrontendModes[Frontend_Mode].exit();
			}

			Frontend_Mode = Frontend_ModeNext;

			if(FrontendModes[Frontend_Mode].init)
			{
				FrontendModes[Frontend_Mode].init();
			}
		}
		break;

	case FRONTEND_FADINGIN:
		fFade -= fFadeDir;
		if(fFade<0.f)
		{
			Frontend_UpdateMode = FRONTEND_NORMAL;
			fFade = 0.f;
		}
		break;
	}
}

//------------------------------------------------------------------

void Frontend_Render()
{
	GL_Ortho(SWIDTH,SHEIGHT);

	glDisable(GL_DEPTH_TEST);

	GL_RenderMode(RENDER_MODE_FLAT);
	GL_SetTexture(0);
	GL_PrimitiveStart(PRIM_TYPE_TRIANGLELIST, RENDER_COLOUR);

	// clear the screen

	GL_Vert(0.f, 0.f, 0.f, 0);
	GL_Vert(0.f, 480.f, 0.f, 0);
	GL_Vert(640.f, 0.f, 0.f, 0);
	

	GL_Vert(640.f, 0.f, 0.f, 0);
	GL_Vert(0.f, 480.f, 0.f, 0);
	GL_Vert(640.f, 480.f, 0.f, 0);

	GL_RenderPrimitives();

	glEnable(GL_DEPTH_TEST);

	GL_Projection(SWIDTH,SHEIGHT);

	if(FrontendModes[Frontend_Mode].render)
	{
		FrontendModes[Frontend_Mode].render();
	}

	if(fFade>0.f)
	{
		GL_Ortho(SWIDTH,SHEIGHT);

		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);

		GL_RenderMode(RENDER_MODE_ALPHA);
		GL_SetTexture(0);
		GL_PrimitiveStart(PRIM_TYPE_TRIANGLELIST, RENDER_COLOUR);

		// render fade

		u32 uCol = (u8)(fFade*255.f)<<24;
		GL_Vert(0.f, 0.f, 0.f, uCol);
		GL_Vert(0.f, 480.f, 0.f, uCol);
		GL_Vert(640.f, 0.f, 0.f, uCol);
		

		GL_Vert(640.f, 0.f, 0.f, uCol);
		GL_Vert(0.f, 480.f, 0.f, uCol);
		GL_Vert(640.f, 480.f, 0.f, uCol);

		GL_RenderPrimitives();

		glEnable(GL_DEPTH_TEST);

		GL_Projection(SWIDTH,SHEIGHT);
	}

	
}

//------------------------------------------------------------------

void Frontend_NextMode( u32 uNext )
{
	Frontend_ModeNext = uNext;
	Frontend_UpdateMode = FRONTEND_FADINGOUT;
}

//------------------------------------------------------------------

i32 Frontend_GetLevel()
{
	return Frontend_Level;
}

//------------------------------------------------------------------

void Frontend_SetCar( u32 car )
{
	Frontend_theCar = car;
}

//------------------------------------------------------------------

u32 Frontend_GetCar( )
{
	return Frontend_theCar;
}

//------------------------------------------------------------------

Font *Frontend_GetFont()
{
	return pFrontendFont;
}