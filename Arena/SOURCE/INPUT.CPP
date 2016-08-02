//
// input.cpp
// craig
//

#include "input.h"

typedef struct _Button
{
	bool bClicked;
	bool bValue;
	u8 *cKey;
}Button;

static Button buttons[BUTTON_MAX];
static bool bLocked=false;

//------------------------------------------------------------------

void Input_Init(KeyMap *pKeyMap)
{
	i32 i;

	for(i=0;i<BUTTON_MAX;i++)
	{
		buttons[i].bClicked=false;
		buttons[i].bValue=false;
		buttons[pKeyMap[i].iButton].cKey=pKeyMap[i].pKey;
	}
	bLocked=false;
}

//------------------------------------------------------------------

void Input_Exit()
{
}

//------------------------------------------------------------------

void Input_Update()
{
	i32 i;

	for(i=0;i<BUTTON_MAX;i++)
	{
		if(*buttons[i].cKey)
		{
			if(!buttons[i].bValue && !buttons[i].bClicked)
			{
				buttons[i].bClicked=true;
			}
			else if(buttons[i].bClicked)
			{
				buttons[i].bClicked=false;
			}
			buttons[i].bValue=true;	
		}
		else
		{
			buttons[i].bValue=false;
			buttons[i].bClicked=false;
		}
	}
}

//------------------------------------------------------------------

bool Input_ButtonHeld(u32 iButton)
{
	if(bLocked)
		return false;

	return buttons[iButton].bValue;
}

//------------------------------------------------------------------

bool Input_ButtonClicked(u32 iButton)
{
	if(bLocked)
		return false;

	return buttons[iButton].bClicked;
}

//------------------------------------------------------------------

void Input_Lock( bool bLock )
{
	bLocked=bLock;
}
