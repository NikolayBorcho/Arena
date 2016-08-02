//
// Camera.cpp
// craig
//

#include "vector.h"
#include "Camera.h"
#include "opengl.h"
#include "object.h"

static Camera *pCurrentCamera;
static Camera *pMainCamera;
static float xZoom;
static Matrix matInv;
static u32 iInvFrame;
static i32 iLook;
static Vec3 vecLookAngle(0.f,-0.307f,0.807f);

static Vec3 vecLooks[4];

//------------------------------------------------------------------

void Camera_Init()
{
	Matrix *pMat;

	pMainCamera = Camera_Create();

	Object_GetMatrixPtr((Object*)pMainCamera,&pMat);

	pMainCamera->pObjTarget=NULL;
	xZoom=10.8f;
	pCurrentCamera=pMainCamera;
	pCurrentCamera->vecLast=pMat->GetColumn(3);
	iInvFrame=0;

	vecLooks[CAM_FRONT].Set(0.f, 0.307f, 0.807f);
	vecLooks[CAM_BACK].Set(0.f, 0.307f, -0.807f);
	vecLooks[CAM_LEFT].Set(0.807f, 0.307f,0.f);
	vecLooks[CAM_RIGHT].Set(-0.807f, 0.307f,0.f);

	iLook = CAM_BACK;
}

//------------------------------------------------------------------

void APIENTRY Camera_Exit(Object *pObject)
{
	Camera *pCamera=(Camera *)pObject;

	Mem_Free(pCamera);
}

//------------------------------------------------------------------

void Camera_Update()
{
	Matrix *pCamMat;
	Matrix NewCamMat;
	Matrix InterpMat;
	
	Object_GetMatrixPtr((Object*)pCurrentCamera,&pCamMat);
	Vec3 vec = pCamMat->GetColumn(3);

	if(!pCurrentCamera)
		return;

	if(pCurrentCamera->pObjTarget)
	{
		Matrix *pMat;

		Object_GetMatrixPtr(pCurrentCamera->pObjTarget, &pMat);

		Vec3 vecPos(*pMat*(vecLooks[iLook]*xZoom));

		vec = vecPos + pMat->GetColumn(3);
		NewCamMat.LookAt(vec, pMat->GetColumn(3));

		InterpMat = pCamMat->Interp(0.3f, NewCamMat);
		Object_SetMatrix((Object*)pCurrentCamera, &InterpMat);
		vec = InterpMat.GetColumn(3);
	}
	else
	{
		NewCamMat = *pCamMat;
		Object_SetMatrix((Object*)pCurrentCamera, &NewCamMat);
	}

	pCurrentCamera->vecLast = vec;
}

//------------------------------------------------------------------

void Camera_Render()
{
	
	if(!pCurrentCamera)
		return;

	Matrix *mat;

	Object_GetInterpMatrix((Object*)pCurrentCamera, &mat);
	mat->Orth();

	float fmat[4][4];
	fmat[0][0]=-mat->GetVal(0,0);
	fmat[1][0]=-mat->GetVal(0,1);
	fmat[2][0]=-mat->GetVal(0,2);
	fmat[3][0]=0;

	fmat[0][1]=mat->GetVal(1,0);
	fmat[1][1]=mat->GetVal(1,1);
	fmat[2][1]=mat->GetVal(1,2);
	fmat[3][1]=0;

	fmat[0][2]=-mat->GetVal(2,0);
	fmat[1][2]=-mat->GetVal(2,1);
	fmat[2][2]=-mat->GetVal(2,2);
	fmat[3][2]=0;

	fmat[0][3]=0;
	fmat[1][3]=0;
	fmat[2][3]=0;
	fmat[3][3]=1.f;

	glMultMatrixf(&fmat[0][0]);
	
	glTranslatef(-mat->GetVal(3,0),
		-mat->GetVal(3,1),
		-mat->GetVal(3,2));

/*	int i;


	GL_SetTexture(0);
	glBegin(GL_LINES);
	glColor4f(1.f,1.f,1.f,1.f);

	for(i=0;i<10;i++)
	{
		glVertex3f(-1000,0,(float)(i-5)*200);
		glVertex3f(1000,0,(float)(i-5)*200);

		glVertex3f((float)(i-5)*200,0,-1000);
		glVertex3f((float)(i-5)*200,0,1000);
	}

	glEnd();
*/
}

//------------------------------------------------------------------

void Camera_Set(Camera *pCamera)
{
	pCurrentCamera=pCamera;
}

//------------------------------------------------------------------

Camera* Camera_Current()
{
	return pCurrentCamera;
}

//------------------------------------------------------------------

void Camera_SetMatrix(Matrix *pMat)
{
	ASSERT(pCurrentCamera,"No camera!!");

	Object_SetAllMatrix((Object*)pCurrentCamera,pMat);
}

//------------------------------------------------------------------

void Camera_SetPos(Vec3 &vecPos)
{
	Matrix mat;

	ASSERT(pCurrentCamera,"No camera!!");

	Object_GetMatrix((Object*)pCurrentCamera,&mat);

	mat.SetColumn(3, vecPos);

	Object_SetAllMatrix((Object*)pCurrentCamera,&mat);
}

//----------------------------------------------------------------------------------

void Camera_LookAt(Vec3 &vecPos, bool bSnap)
{
	Matrix matLook;
	Matrix *pMat;

	Object_GetMatrixPtr((Object*)pCurrentCamera, &pMat);

	matLook.LookAt(pMat->GetColumn(3), vecPos);
	if(bSnap)
		Object_SetAllMatrix((Object*)pCurrentCamera,&matLook);
	else
		Object_SetMatrix((Object*)pCurrentCamera, &matLook);
}

//----------------------------------------------------------------------------------

void Camera_TargetObject(Object *Target)
{
	Matrix *pMat;
	Matrix mat;
	pCurrentCamera->pObjTarget=Target;

	Object_GetMatrixPtr(pCurrentCamera->pObjTarget, &pMat);
	Object_GetMatrix((Object*)pCurrentCamera, &mat);

	Vec3 vecPos=(*pMat*vecLookAngle);
	vecPos=vecPos*xZoom;
	mat.SetColumn(3, pMat->GetColumn(3)-vecPos);
	Object_SetMatrix((Object*)pCurrentCamera, &mat);
}

//----------------------------------------------------------------------------------

void Camera_IncZoom()
{
	xZoom+=0.3f;

	if(pCurrentCamera->pObjTarget)
	{
		Matrix *pMat;

		Object_GetMatrixPtr(pCurrentCamera->pObjTarget, &pMat);

		Vec3 vecPos=(*pMat*vecLooks[iLook]);
		vecPos=vecPos*xZoom;
	}
}

//----------------------------------------------------------------------------------

void Camera_DecZoom()
{
	xZoom-=0.3f;

	if(pCurrentCamera->pObjTarget)
	{
		Matrix *pMat;

		Object_GetMatrixPtr(pCurrentCamera->pObjTarget, &pMat);

		Vec3 vecPos=(*pMat*vecLooks[iLook]);
		vecPos=vecPos*xZoom;
	}
}

//----------------------------------------------------------------------------------

Matrix* Camera_Inverse()
{
	if(iInvFrame<Object_GetRenderFrame())
	{
		Matrix *pMat;

		Object_GetMatrixPtr((Object*)pCurrentCamera, &pMat);

		pMat->Inverse(&matInv);
	}

	return &matInv;
}

//----------------------------------------------------------------------------------

void Camera_SetLook( i32 iNewLook )
{
	iLook = iNewLook;
}

//----------------------------------------------------------------------------------

Matrix* Camera_GetRenderMatrix()
{
	static Matrix *pMat;

	Object_GetInterpMatrix((Object*)pCurrentCamera, &pMat);
	pMat->Orth();

	return pMat;
}

//------------------------------------------------------------------

Camera *Camera_Create()
{
	Camera *pCamera;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Camera_Exit;
	Create.iType=OBJECT_Camera;
	Create.pName="main Camera";

	pCamera = (Camera*)Object_Create(&Create, sizeof(Camera));

	return pCamera;
}
