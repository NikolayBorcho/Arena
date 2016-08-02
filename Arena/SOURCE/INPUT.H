//
// input.h
// craig
//

#ifndef _INPUT_H
#define _INPUT_H

#include "includeall.h"

typedef struct _KeyMap
{
	u32 iButton;
	u8 *pKey;
}KeyMap;

enum 
{
	BUTTON_FIRE1=0,
	BUTTON_FIRE2,
	BUTTON_CAMRIGHT,
	BUTTON_CAMLEFT,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LSHOULDER,
	BUTTON_RSHOULDER,
	BUTTON_ESC,
	BUTTON_SPACE,

	PEDITOR_OPEN,	
	PEDITOR_CLOSE,
	PEDITOR_NEXTVAL,
	PEDITOR_PREVVAL,
	PEDITOR_INCVAL,
	PEDITOR_DECVAL,
	PEDITOR_NEXT,
	PEDITOR_PREV,
	PEDITOR_INS,
	PEDITOR_DEL,
	PEDITOR_EFFECT,
	PEDITOR_DISCARD,

	BUTTON_MAX
};

void Input_Init(KeyMap *pKeyMap);
void Input_Exit();
void Input_Update();
bool Input_ButtonClicked(u32 iButton);
bool Input_ButtonHeld(u32 iButton);
void Input_Lock( bool bLock );
#endif

