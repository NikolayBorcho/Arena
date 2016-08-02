//
// sprite.h
// craig
//

#ifndef _SPRITE_H
#define _SPRITE_H

#include "includeall.h"
#include "object.h"

#define SPR_ADD 0
#define SPR_ALPHA 1

typedef struct _SpriteVertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
	u32 uColour;
}SpriteVertex;

typedef struct _Sprite
{
	Vec3 vecPos;
	Vec4 vecUVs;
	float fSize;
	u32 uColour;
	float fRot;
	i32 iMode;
	bool bYFlat;
	bool bActive;

	_Sprite *pNext;
}Sprite;

Object *Sprite_Create();

Sprite* Sprite_Add(Vec3 &vecPos,Vec4 &vecUVs,float fSize,u32 uColour,i32 iMode);
void Sprite_Remove(Sprite *pSprite);
inline void Sprite_SetYFlat( Sprite *pSprite, bool fFlat )
{
	pSprite->bYFlat=fFlat;
}

inline void Sprite_SetColour( Sprite *pSprite, u32 uColour )
{
	pSprite->uColour = uColour;
}
#endif

