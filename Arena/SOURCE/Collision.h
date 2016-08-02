//
// collision.h
// craig
//

#ifndef _COLLISION_H
#define _COLLISION_H

#include "includeall.h"
#include "object.h"

typedef struct _ColBox
{
	Matrix mat;
	Matrix matInv;

	float	fRadius;
	Vec3 vecMax;
	Vec3 vecMin;
	Vec4 Points[8];
	Vec4 normals[6];
	Vec4 center[6];
}ColBox;

typedef struct _ColData
{
	Vec3 normal;
	Vec3 vecPoint;
}ColData;

typedef struct _Collision
{
	// nothing before this!!!
	Object pObject;

	// put stuff after here
}Collision;

Collision* Collision_Create( );

ColBox* Collision_CreateBox(Vec3 *pvecMax, Vec3 *pvecMin, Matrix *pMat);
void Collision_DeleteBox(ColBox *pBox);
void Collision_UpdateMat(ColBox *pBox, Matrix *pMat);
bool Collision_BoxBoxTest(ColBox *pBoxThis,ColBox *pBoxTest);
bool Collision_BoxPointTest(ColBox *pBoxThis, Vec4 &VecPoint);	
bool Collision_BoxLineTest(ColBox *pBoxThis, Vec4 &VecStart, Vec4 &VecEnd);
void Collision_GetColData(ColData *pData);
#endif

