//
// trail.h
// craig
//

#ifndef _TRAIL_H
#define _TRAIL_H

#include "includeall.h"
#include "object.h"
#include "tgaload.h"

#define MAX_TRAIL_POINTS 60
#define MAX_TRAIL_ADD_BUFFER 10

typedef struct _TrailPointAdd
{
	Vec3 vecPoint;
	Vec3 vecDir;
}TrailPointAdd;

typedef struct _TrailPoint
{
	Vec3 vecPoint[2];
	float	fAge;
	_TrailPoint *pNext;
}TrailPoint;

typedef struct _Trail
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
	TrailPoint vecpoints[MAX_TRAIL_POINTS];
	i32		iLastPoint;
	float	fDecAge;
	
	float	fR;
	float	fG;
	float	fB;
	float	fA;

	bool	bAlpha;
	bool	bDelete;

	TrailPoint *pList;
	Image	*pImage;
	u32		uTex;

	Vertex	vertBuff[MAX_TRAIL_POINTS*2];
	i32		iVertCount;

	i32		iAddPoints;
	TrailPointAdd AddBuffer[MAX_TRAIL_ADD_BUFFER];

	void (*leadingPointCallback)(void *pData, Vec3 *pVec);
	void *pSendData;
}Trail;

Trail* Trail_Create( float fDecay, bool bAlpha, u32 uColour, char *pTexture );
void Trail_AddPoint(Object *pObject, Vec3 &vecPoint, Vec3 &vecDir);
void Trail_SetDelete(Object *pObject, bool bDelete);
void Trail_SetLeadingPointCallback(Object *pObject, void (*leadingPointCallback)(void *pData, Vec3 *pVec));

#endif

