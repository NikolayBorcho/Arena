//
// mainloop.h
// craig
//

#ifndef _MAINLOOP_H
#define _MAINLOOP_H

#include "includeall.h"

enum MAIN_MODE
{
	MODE_NONE,
	MODE_FRONTEND=1,
	MODE_GAME
};

void MainLoop_Init();
void MainLoop_Exit();
bool MainLoop_Update();
void MainLoop_Render();
void MainLoop_SetMode(MAIN_MODE iNewMode);
void Mainloop_Pause( bool bPause );
void Mainloop_RenderHUD();	// NIK

#endif

