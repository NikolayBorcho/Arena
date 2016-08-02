//
// mesh.h
// craig
//

#ifndef _MESH_H
#define _MESH_H

#include "includeall.h"
#include "matrix.h"
#include "opengl.h"
#include "tgaload.h"

#define MAX_MATERIALS 5
typedef struct _Mesh
{
	char cName[20];
	char cTexName[32];

	u32 *p_uColourStart;
	u16	*p_IndexStart;

	float	*p_fVertexStart;
	float	*p_fNormalStart;
	float	*p_fTextureStart;

	u16	iNumVerts;
	u16	iNumIndex;

	float	fSubMeshSize;
	Vec3	vecCenter;

	u32		uTexture;

	Image *pImage;

	void	(*renderCallBackStart)(void *pData, _Mesh *pThis);
	void	(*renderCallBackEnd)(void *pData, _Mesh *pThis);
}Mesh;

typedef struct _Model
{
	u32 *p_uColours;

	float	*p_fVerts;
	float	*p_fNormals;
	float	*p_fUVs;

	u16	*p_iIndexArray;

	Mesh *pMesh[MAX_MATERIALS];
	i32 iMeshes;
	bool bReference;
	char cModelName[64];
	u32 uRenderFrame;

	void *pSendData;
}Model;

typedef struct _SaveMesh
{
	char cName[20];
	char cTexName[32];

	u16	iNumVerts;
	u16	iNumIndex;
	float	fSubMeshSize;
	Vec3	vecCenter;
}SaveMesh;

typedef struct _SaveModel
{
	i16	*p_iVerts;
	i16	*p_iNormals;
	i16	*p_iUVs;
	u32 *p_uColours;

	u16	*p_iIndexArray;

	SaveMesh Mesh[MAX_MATERIALS];
	i32 iMeshes;

	char cModelName[64];
}SaveModel;

void Mesh_Init();
void Mesh_Exit();
void Mesh_Update();
void Mesh_Destroy(Model *pModel);
void Mesh_Render(Model *pModel,RENDER_MODE mode);
Model* LoadMesh3DS(char *p_filename,bool bUseRef=false);
void Mesh_Bounds(Model *pModel, Vec4 &vecBound);
void Mesh_Light(Model *pModel, Matrix &mat);
void Mesh_LightCol(Model *pModel, Matrix &mat, u8 uR,u8 uG,u8 uB);
void Mesh_SetTexture();
float* Mesh_GetLight();
void Mesh_MakeCollision(Model *pModel, struct _CollTri **ppTris, i32 &iNumTris);
void Mesh_Shield(Model *pModel, Vec3 vecPoint,float xTime,u8 uR,u8 uG,u8 uB);
#endif

