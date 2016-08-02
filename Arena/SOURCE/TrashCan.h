//
// TrashCan.h
// craig
//

#ifndef _TRASHCAN_H
#define _TRASHCAN_H

#include "includeall.h"

void TrashCan_Init();
void TrashCan_Exit();
void TrashCan_Update();
void TrashCan_Render();
void TrashCan_DeleteObject( struct _Object *pObj );
#endif

