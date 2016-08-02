//
// Frontend.h
// craig
//

#ifndef _FRONTEND_H
#define _FRONTEND_H

#include "includeall.h"
#include "object.h"
#include "font.h"

enum
{
	FRONTEND_NONE = 0,
	FRONTEND_SELECTCAR,
	FRONTEND_MAX,
};

typedef struct _FrontendMode
{
	void (*init)(void);
	void (*exit)(void);
	void (*update)(void);
	void (*render)(void);
}FrontendMode;

void Frontend_Init();
void Frontend_Exit();
void Frontend_Update();
void Frontend_Render();
void Frontend_NextMode( u32 uNext );
i32 Frontend_GetLevel();
Font *Frontend_GetFont();
void Frontend_SetCar( u32 car );
u32 Frontend_GetCar( );

#endif

