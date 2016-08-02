//
// camera.h
// craig
//

#ifndef _CAMERA_H
#define _CAMERA_H

#include "includeall.h"
#include "matrix.h"
#include "object.h"

#define CAM_FRONT 0
#define CAM_BACK 1
#define CAM_LEFT 2
#define CAM_RIGHT 3

typedef struct _Camera
{
	// nothing before this!!!
	Object pObject;

//	Matrix mat;
	Vec3 vecLast;
	Object *pObjTarget;
}Camera;

void Camera_Init();
void Camera_Exit();
void Camera_Update();
void Camera_Render();

void Camera_Set(Camera *pCamera);
Camera* Camera_Current();
void Camera_SetMatrix(Matrix *pMat);
void Camera_SetPos(Vec3 &vecPos);
void Camera_LookAt(Vec3 &vecPos, bool bSnap);
void Camera_TargetObject(Object *Target);
void Camera_IncZoom();
void Camera_DecZoom();
Matrix* Camera_Inverse();
void Camera_SetLook( i32 iNewLook );
Matrix* Camera_GetRenderMatrix();
Camera *Camera_Create();

#endif

