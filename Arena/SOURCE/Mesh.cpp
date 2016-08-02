//
// mesh.cpp
// craig
//

#include <windows.h>

#include "mesh.h"
#include "opengl.h"
#include "tgaload.h"
#include "Object.h"

static u32 uModelTexture;
static float LightDir[3]={0.57735f,-0.57735f,-0.57735f};

#define MAX_MODELS 200

static Model *pAllModels[MAX_MODELS];
static i32 iNumModels;

typedef struct _ModelBucket
{
	Model *pModel;
	i32 iNumRef;
}ModelBucket;

static ModelBucket ModelBuckets[MAX_MODELS];
static i32 iNumBuckets; 

void Mesh_Destroy(Model *pModel);

float* Mesh_GetLight()
{
	return LightDir;
}

//------------------------------------------------------------------

void Mesh_Init()
{
	iNumModels=0;
	iNumBuckets=0;
}

//------------------------------------------------------------------

void Mesh_Exit()
{
	while(iNumBuckets)
	{
		ModelBuckets[iNumBuckets-1].iNumRef = 1;

		Mesh_Destroy(ModelBuckets[iNumBuckets-1].pModel);
	}
}

//------------------------------------------------------------------

void Mesh_Update()
{
}

//------------------------------------------------------------------

void Mesh_SetTexture()
{
	GL_SetTexture(uModelTexture);
}

//------------------------------------------------------------------

void Mesh_Render(Model *pModel,RENDER_MODE mode)
{
	i32 i;
	Mesh **ppMesh;
	Mesh *pMesh;

//	GL_SetTexture(uModelTexture);

	GL_RenderMode(mode);

//	glDisable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, pModel->p_fVerts);
	glTexCoordPointer(2, GL_FLOAT, 0, pModel->p_fUVs);

	glColorPointer(4, GL_UNSIGNED_BYTE, 0, pModel->p_uColours);

	ppMesh = pModel->pMesh;
	for(i=0;i<pModel->iMeshes;i++, ppMesh++)
	{
		pMesh = *ppMesh;
		if(pMesh->uTexture)
		{
			GL_SetTexture(pMesh->uTexture);
		}
		else
		{
			GL_SetTexture(0);
		}
		if(pMesh->renderCallBackStart)
		{
			pMesh->renderCallBackStart(pModel->pSendData, pMesh);
		}

		glDrawElements(GL_TRIANGLES,pMesh->iNumIndex,GL_UNSIGNED_SHORT,pMesh->p_IndexStart);

		if(pMesh->renderCallBackEnd)
		{
			pMesh->renderCallBackEnd(pModel->pSendData, pMesh);
		}
#ifdef _DEBUG
	uPolycount+=(pMesh->iNumIndex/3);
#endif
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glColor4f(1.f,1.f,1.f,1.f);


	pModel->uRenderFrame=Object_GetRenderFrame();
/*	i32 j;
	float *pNormal=pModel->p_xNormals;
	float *pVec=pModel->p_xVerts;

	GL_SetTexture(0);
	glBegin(GL_LINES);
	for(j=0;j<pModel->iMeshes;j++)
	{
		for(i=0;i<pModel->pMesh[j]->iNumVerts;i++)
		{
			glVertex3f(unFixed(pVec[0]+pNormal[0])*0.5f,unFixed(pVec[1]+pNormal[1])*0.5f,unFixed(pVec[2]+pNormal[2])*0.5f);
			glVertex3f(unFixed(pVec[0])*0.5f,unFixed(pVec[1])*0.5f,unFixed(pVec[2])*0.5f);
			
			pNormal+=3;
			pVec+=3;
		}
	}
	glEnd();*/
}

//----------------------------------------------------------------------------------

void Mesh_Destroy(Model *pModel)
{
	i32 i;

/*	if(pModel->bReference)
	{
		for(i=0;i<iNumBuckets;i++)
		{
			if(strcmp(ModelBuckets[i].pModel->cModelName,pModel->cModelName)==0)
			{
				ModelBuckets[i].iNumRef--;
				ASSERT(ModelBuckets[i].iNumRef>-1,"freeing to many models");
				if(!ModelBuckets[i].iNumRef)
				{
					iNumBuckets--;
					ModelBuckets[i]=ModelBuckets[iNumBuckets];
					break;
				}
				else
				{
					return;
				}
			}
		}
	}*/

	for(i=0;i<iNumBuckets;i++)
	{
		if(ModelBuckets[i].pModel == pModel)
		{
			iNumBuckets--;
			ModelBuckets[i]=ModelBuckets[iNumBuckets];
			break;
		}
	}

	Mem_Free(pModel->p_fVerts);
	Mem_Free(pModel->p_fNormals);
	Mem_Free(pModel->p_fUVs);
	Mem_Free(pModel->p_iIndexArray);
	Mem_Free(pModel->p_uColours);

	for(i=0;i<pModel->iMeshes;i++)
	{

		Mem_Free(pModel->pMesh[i]);
	}

	Mem_Free(pModel);
}

//----------------------------------------------------------------------------------

void Mesh_LightCol(Model *pModel, Matrix &mat, u8 uR,u8 uG,u8 uB)
{
	i32 i;
	i32 j;
	float *pNormal=pModel->p_fNormals;
	u8 *pCol=(u8*)pModel->p_uColours;
	float xNormal;
	float yNormal;
	float zNormal;
	float xLightDot;
	for(j=0;j<pModel->iMeshes;j++)
	{
		for(i=0;i<pModel->pMesh[j]->iNumVerts;i++)
		{
			// get rotated normal
			xNormal=mat.GetColumn(0).GetX()*pNormal[0] + 
				mat.GetColumn(0).GetY()*pNormal[1] + 
				mat.GetColumn(0).GetZ()*pNormal[2];
			yNormal=mat.GetColumn(1).GetX()*pNormal[0] + 
				mat.GetColumn(1).GetY()*pNormal[1] + 
				mat.GetColumn(1).GetZ()*pNormal[2];
			zNormal=mat.GetColumn(2).GetX()*pNormal[0] + 
				mat.GetColumn(2).GetY()*pNormal[1] + 
				mat.GetColumn(2).GetZ()*pNormal[2];

			// get dot product of normal and light
			xLightDot=xNormal*LightDir[0] + yNormal*LightDir[1] + zNormal*LightDir[2];
			if(xLightDot<0.f)
			{
				pCol[0]=-(u8)(xLightDot*uR);
				pCol[1]=-(u8)(xLightDot*uG);
				pCol[2]=-(u8)(xLightDot*uB);
			}
			else
			{
				pCol[0]=0;
				pCol[1]=0;
				pCol[2]=0;
			}
			
			pNormal+=3;
			pCol+=4;
		}
	}
}

//----------------------------------------------------------------------------------

void Mesh_Light(Model *pModel, Matrix &mat)
{
	i32 i;
	i32 j;
	float *pNormal=pModel->p_fNormals;
	u8 *pCol=(u8*)pModel->p_uColours;
	float xNormal;
	float yNormal;
	float zNormal;
	float xLightDot;
	u8 uCol;
	for(j=0;j<pModel->iMeshes;j++)
	{
		for(i=0;i<pModel->pMesh[j]->iNumVerts;i++)
		{
			// get rotated normal
			xNormal=mat.GetColumn(0).GetX()*pNormal[0] + 
				mat.GetColumn(0).GetY()*pNormal[1] + 
				mat.GetColumn(0).GetZ()*pNormal[2];
			yNormal=mat.GetColumn(1).GetX()*pNormal[0] + 
				mat.GetColumn(1).GetY()*pNormal[1] + 
				mat.GetColumn(1).GetZ()*pNormal[2];
			zNormal=mat.GetColumn(2).GetX()*pNormal[0] + 
				mat.GetColumn(2).GetY()*pNormal[1] + 
				mat.GetColumn(2).GetZ()*pNormal[2];

			// get dot product of normal and light
			xLightDot=xNormal*LightDir[0] + yNormal*LightDir[1] + zNormal*LightDir[2];
			if(xLightDot<0.f)
			{
				uCol=-(u8)(xLightDot*255);
				pCol[0]=uCol;
				pCol[1]=uCol;
				pCol[2]=uCol;
			}
			else
			{
				pCol[0]=0;
				pCol[1]=0;
				pCol[2]=0;
			}
			
			pNormal+=3;
			pCol+=4;
		}
	}
}

//----------------------------------------------------------------------------------

void Mesh_Bounds(Model *pModel, Vec4 &vecBound)
{
	i32 i;
	i32 j;

	Vec3 vecMax(0.f,0.f,0.f);
	Vec3 vecMin(0.f,0.f,0.f);
	float *pVert=pModel->p_fVerts;

	for(j=0;j<pModel->iMeshes;j++)
	{
		for(i=0;i<pModel->pMesh[j]->iNumVerts;i++)
		{
			if(vecMax.GetX()<pVert[0])
			{
				vecMax.SetX(pVert[0]);
			}
			if(vecMax.GetY()<pVert[1])
			{
				vecMax.SetY(pVert[1]);
			}
			if(vecMax.GetZ()<pVert[2])
			{
				vecMax.SetZ(pVert[2]);
			}
			if(vecMin.GetX()>pVert[0])
			{
				vecMin.SetX(pVert[0]);
			}
			if(vecMin.GetY()>pVert[1])
			{
				vecMin.SetY(pVert[1]);
			}
			if(vecMin.GetZ()>pVert[2])
			{
				vecMin.SetZ(pVert[2]);
			}
			pVert+=3;
		}
	}
	vecBound.SetX(vecMax.GetX()-vecMin.GetX());
	vecBound.SetY(vecMax.GetY()-vecMin.GetY());
	vecBound.SetZ(vecMax.GetZ()-vecMin.GetZ());
	vecBound.SetW(vecBound.Length()/2.f);
}

//----------------------------------------------------------------------------------

Model* Mesh_ModelCopy(Model *pModel)
{
	i32 i;
	Model *pModelNew = (Model*)Mem_New(sizeof(Model));

	u16	iNumVerts=0;
	u16	iNumIndex=0;

	memcpy(pModelNew, pModel, sizeof(Model));

	for(i=0;i<pModel->iMeshes;i++)
	{
		iNumVerts+=pModel->pMesh[i]->iNumVerts;
		iNumIndex+=pModel->pMesh[i]->iNumIndex;
	}

	pModelNew->p_uColours=(u32 *)Mem_New(sizeof(u32)*iNumVerts);
	pModelNew->p_iIndexArray=(u16 *)Mem_New(sizeof(u16)*iNumIndex*3);
	pModelNew->p_fVerts=(float *)Mem_New(sizeof(float)*3*iNumVerts);
	pModelNew->p_fNormals=(float *)Mem_New(sizeof(float)*3*iNumVerts);
	pModelNew->p_fUVs=(float *)Mem_New(sizeof(float)*2*iNumVerts);

	float *p_fVertexStart=pModelNew->p_fVerts;
	float *p_fNormalStart=pModelNew->p_fNormals;
	float *p_fTextureStart=pModelNew->p_fUVs;

	u32 *p_uColourStart=pModelNew->p_uColours;
	u16 *p_iIndexStart=pModelNew->p_iIndexArray;

	for(i=0;i<pModel->iMeshes;i++)
	{
		pModelNew->pMesh[i]=(Mesh *)Mem_New(sizeof(Mesh));
		memcpy(pModelNew->pMesh[i], pModel->pMesh[i], sizeof(Mesh));

		pModelNew->pMesh[i]->p_uColourStart=p_uColourStart;
		pModelNew->pMesh[i]->p_IndexStart=p_iIndexStart;

		pModelNew->pMesh[i]->p_fVertexStart=p_fVertexStart;
		pModelNew->pMesh[i]->p_fNormalStart=p_fNormalStart;
		pModelNew->pMesh[i]->p_fTextureStart=p_fTextureStart;

		memcpy(p_fVertexStart, pModel->pMesh[i]->p_fVertexStart, sizeof(float)*(pModel->pMesh[i]->iNumVerts*3));
		memcpy(p_fNormalStart, pModel->pMesh[i]->p_fNormalStart, sizeof(float)*(pModel->pMesh[i]->iNumVerts*3));
		memcpy(p_fTextureStart, pModel->pMesh[i]->p_fTextureStart, sizeof(float)*(pModel->pMesh[i]->iNumVerts*2));
		memcpy(p_uColourStart, pModel->pMesh[i]->p_uColourStart, sizeof(u32)*(pModel->pMesh[i]->iNumVerts));
		memcpy(p_iIndexStart, pModel->pMesh[i]->p_IndexStart, sizeof(u16)*(pModel->pMesh[i]->iNumIndex));

		p_fVertexStart += (pModel->pMesh[i]->iNumVerts*3);
		p_fNormalStart += (pModel->pMesh[i]->iNumVerts*3);
		p_fTextureStart += (pModel->pMesh[i]->iNumVerts*2);
		p_uColourStart += (pModel->pMesh[i]->iNumVerts);
		p_iIndexStart += (pModel->pMesh[i]->iNumIndex);
	}

	return pModelNew;
}

//----------------------------------------------------------------------------------

static void _Save(SaveModel *pModel)
{
	FILE *pFile;
	char cFilename[256];
	i32 i;

	sprintf(cFilename,"models/%s.msh",pModel->cModelName);

	if(!(pFile=fopen(cFilename,"wb")))
		return;

	fwrite(pModel->cModelName,sizeof(char),sizeof(pModel->cModelName),pFile);
	fwrite(&pModel->iMeshes,sizeof(i32),1,pFile);

	u16	iNumVerts=0;
	u16	iNumIndex=0;

	for(i=0;i<pModel->iMeshes;i++)
	{
		fwrite(pModel->Mesh[i].cName,sizeof(char),sizeof(pModel->Mesh[i].cName),pFile);

		iNumVerts+=pModel->Mesh[i].iNumVerts;
		iNumIndex+=pModel->Mesh[i].iNumIndex;

		fwrite(&pModel->Mesh[i].iNumVerts,sizeof(u16),1,pFile);
		fwrite(&pModel->Mesh[i].iNumIndex,sizeof(u16),1,pFile);

		fwrite(&pModel->Mesh[i].fSubMeshSize,sizeof(float),1,pFile);
		fwrite(&pModel->Mesh[i].vecCenter,sizeof(Vec3),1,pFile);

		fwrite(pModel->Mesh[i].cTexName,sizeof(char),sizeof(pModel->Mesh[i].cTexName),pFile);
	}

	fwrite(pModel->p_iVerts,sizeof(i16)*3,iNumVerts,pFile);
	fwrite(pModel->p_iNormals,sizeof(i16)*3,iNumVerts,pFile);
	fwrite(pModel->p_iUVs,sizeof(i16)*2,iNumVerts,pFile);
	fwrite(pModel->p_uColours,sizeof(u32),iNumVerts,pFile);
	fwrite(pModel->p_iIndexArray,sizeof(u16),iNumIndex,pFile);

	fclose(pFile);
}

//----------------------------------------------------------------------------------

static Model* _Load(char *p_filename)
{
	FILE *pFile;
	char cFilename[256];
	i32 i,j;
	SaveModel nModel;

	sprintf(cFilename,"DATA/MODELS/%s.msh",p_filename);

	if(!(pFile=fopen(cFilename,"rb")))
		return NULL;

	fread(nModel.cModelName,sizeof(char),sizeof(nModel.cModelName),pFile);
	fread(&nModel.iMeshes,sizeof(i32),1,pFile);

	u16	iNumVerts=0;
	u16	iNumIndex=0;

	for(i=0;i<nModel.iMeshes;i++)
	{
		fread(nModel.Mesh[i].cName,sizeof(char),sizeof(nModel.Mesh[i].cName),pFile);


		fread(&nModel.Mesh[i].iNumVerts,sizeof(u16),1,pFile);
		fread(&nModel.Mesh[i].iNumIndex,sizeof(u16),1,pFile);

		fread(&nModel.Mesh[i].fSubMeshSize,sizeof(float),1,pFile);
		fread(&nModel.Mesh[i].vecCenter,sizeof(Vec3),1,pFile);

		iNumVerts+=nModel.Mesh[i].iNumVerts;
		iNumIndex+=nModel.Mesh[i].iNumIndex;

		fread(nModel.Mesh[i].cTexName,sizeof(char),sizeof(nModel.Mesh[i].cTexName),pFile);
	}

	nModel.p_iVerts=(i16 *)Mem_New(sizeof(i16)*3*iNumVerts);
	nModel.p_iNormals=(i16 *)Mem_New(sizeof(i16)*3*iNumVerts);
	nModel.p_iUVs=(i16 *)Mem_New(sizeof(i16)*2*iNumVerts);
	nModel.p_uColours=(u32 *)Mem_New(sizeof(u32)*iNumVerts);
	nModel.p_iIndexArray=(u16 *)Mem_New(sizeof(u16)*iNumIndex);

	fread(nModel.p_iVerts,sizeof(i16)*3,iNumVerts,pFile);
	fread(nModel.p_iNormals,sizeof(i16)*3,iNumVerts,pFile);
	fread(nModel.p_iUVs,sizeof(i16)*2,iNumVerts,pFile);
	fread(nModel.p_uColours,sizeof(u32),iNumVerts,pFile);
	fread(nModel.p_iIndexArray,sizeof(u16),iNumIndex,pFile);

	Model *pModel=(Model *)Mem_New(sizeof(Model));
	memset(pModel, 0, sizeof(Model));

	sprintf(pModel->cModelName,"%s",nModel.cModelName);

	pModel->p_uColours=(u32 *)Mem_New(sizeof(u32)*iNumVerts);
	pModel->p_iIndexArray=(u16 *)Mem_New(sizeof(u16)*iNumIndex*3);
	pModel->p_fVerts=(float *)Mem_New(sizeof(float)*3*iNumVerts);
	pModel->p_fNormals=(float *)Mem_New(sizeof(float)*3*iNumVerts);
	pModel->p_fUVs=(float *)Mem_New(sizeof(float)*2*iNumVerts);

	float *p_fVertexStart=pModel->p_fVerts;
	float *p_fNormalStart=pModel->p_fNormals;
	float *p_fTextureStart=pModel->p_fUVs;

	u32 *p_uColourStart=pModel->p_uColours;
	u16 *p_iIndexStart=pModel->p_iIndexArray;

	i16 *piStartVert=nModel.p_iVerts;
	i16 *piStartNormal=nModel.p_iNormals;
	i16 *piStartUV=nModel.p_iUVs;
	u32 *piStartColour=nModel.p_uColours;
	i32 iTotalIndex=0;

	pModel->iMeshes=nModel.iMeshes;

	fclose(pFile);

	for(i=0;i<nModel.iMeshes;i++)
	{
		pModel->pMesh[i]=(Mesh *)Mem_New(sizeof(Mesh));
		memset(pModel->pMesh[i], 0, sizeof(Mesh));

		strcpy(pModel->pMesh[i]->cName,nModel.Mesh[i].cName);
		strcpy(pModel->pMesh[i]->cTexName,nModel.Mesh[i].cTexName);

		pModel->pMesh[i]->p_uColourStart=p_uColourStart;
		pModel->pMesh[i]->p_IndexStart=p_iIndexStart;

		pModel->pMesh[i]->p_fVertexStart=p_fVertexStart;
		pModel->pMesh[i]->p_fNormalStart=p_fNormalStart;
		pModel->pMesh[i]->p_fTextureStart=p_fTextureStart;

		for(j=0;j<nModel.Mesh[i].iNumVerts;j++)
		{
			p_fVertexStart[0]=piStartVert[0]/5000.f;
			p_fVertexStart[1]=piStartVert[1]/5000.f;
			p_fVertexStart[2]=piStartVert[2]/5000.f;
			p_fVertexStart+=3;
			piStartVert+=3;

			p_fNormalStart[0]=piStartNormal[0]/5000.f;
			p_fNormalStart[1]=piStartNormal[1]/5000.f;
			p_fNormalStart[2]=piStartNormal[2]/5000.f;
			p_fNormalStart+=3;
			piStartNormal+=3;

			p_fTextureStart[0]=piStartUV[0]/5000.f;
			p_fTextureStart[1]=piStartUV[1]/5000.f;
			p_fTextureStart+=2;
			piStartUV+=2;

			p_uColourStart[0]=piStartColour[0];
			piStartColour++;
			p_uColourStart++;
		}

		pModel->pMesh[i]->iNumVerts=nModel.Mesh[i].iNumVerts;
		pModel->pMesh[i]->iNumIndex=nModel.Mesh[i].iNumIndex;

		pModel->pMesh[i]->fSubMeshSize=nModel.Mesh[i].fSubMeshSize;
		pModel->pMesh[i]->vecCenter=nModel.Mesh[i].vecCenter;

		if(pModel->pMesh[i]->cTexName[0])
		{
			char cTexName[128];

			sprintf(cTexName,"data/textures/%s",pModel->pMesh[i]->cTexName);
			pModel->pMesh[i]->pImage = LoadTGA(cTexName,true,true);
			pModel->pMesh[i]->uTexture = TgaLoad_genTexture(&pModel->pMesh[i]->pImage);
		}

		iTotalIndex+=nModel.Mesh[i].iNumIndex;
		p_iIndexStart+=nModel.Mesh[i].iNumIndex;
	}
	memcpy(pModel->p_iIndexArray,nModel.p_iIndexArray,sizeof(u16)*iTotalIndex);

	Mem_Free(nModel.p_iVerts);
	Mem_Free(nModel.p_iNormals);
	Mem_Free(nModel.p_iUVs);
	Mem_Free(nModel.p_uColours);
	Mem_Free(nModel.p_iIndexArray);

	return pModel;
}

//----------------------------------------------------------------------------------

Model* LoadMesh3DS(char *p_filename,bool bUseRef)
{
	i32 i;
	char cNameBuff[128];
	char *p_buffPtr=cNameBuff;
	char *p_cname=p_filename;
	Model *pModel;

	while(1)
	{
		*p_buffPtr=*p_cname;
		if(*p_cname=='\\' || *p_cname=='/')
		{
			p_buffPtr=cNameBuff;
		}
		else if(*p_cname=='.')
		{
			*p_buffPtr='\0';
			break;
		}
		else
			p_buffPtr++;
		
		p_cname++;
	}

	for(i=0;i<iNumBuckets;i++)
	{
		if(strcmp(ModelBuckets[i].pModel->cModelName,cNameBuff)==0)
		{
			ASSERT(iNumBuckets<MAX_MODELS,"too many models");
			ModelBuckets[iNumBuckets].pModel = Mesh_ModelCopy(ModelBuckets[i].pModel);
			ModelBuckets[iNumBuckets].iNumRef=1;
			iNumBuckets++; 

			return ModelBuckets[iNumBuckets-1].pModel;
		}
	}

	pModel=_Load(cNameBuff);

	ASSERT(pModel,"no model");

	ASSERT(iNumBuckets<MAX_MODELS,"too many models");
	ModelBuckets[iNumBuckets].pModel=pModel;
	ModelBuckets[iNumBuckets].iNumRef=1;
	iNumBuckets++; 

	pModel->bReference=bUseRef;
	pModel->uRenderFrame=0;

	pAllModels[iNumModels]=pModel;
	ASSERT(iNumModels<MAX_MODELS,"too many models");
	iNumModels++;

	return pModel;
}

//------------------------------------------------------------------
/*
void Mesh_MakeCollision(Model *pModel, CollTri **ppTris, i32 &iNumTris)
{
	i32 i;
	i32 j;

	i32 iFaceCount=0;
	Mesh *pMesh;
	Vec3 vec1;
	Vec3 vec2;
	Vec3 vec3;
	i32 iCount=0;

	for(j=0;j<pModel->iMeshes;j++)
	{
		iFaceCount+=pModel->pMesh[j]->iNumIndex;
	}
	iFaceCount=iFaceCount/3;

	*ppTris=(CollTri*)Mem_New(sizeof(CollTri)*iFaceCount);

	iNumTris=iFaceCount;
	for(j=0;j<pModel->iMeshes;j++)
	{
		pMesh=pModel->pMesh[j];
		for(i=0;i<pMesh->iNumIndex;i+=3)
		{
			vec1.SetX(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 0]*3 + 0]);
			vec1.SetY(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 0]*3 + 1]);
			vec1.SetZ(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 0]*3 + 2]);

			vec2.SetX(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 1]*3 + 0]);
			vec2.SetY(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 1]*3 + 1]);
			vec2.SetZ(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 1]*3 + 2]);

			vec3.SetX(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 2]*3 + 0]);
			vec3.SetY(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 2]*3 + 1]);
			vec3.SetZ(pMesh->p_fVertexStart[pMesh->p_IndexStart[i + 2]*3 + 2]);

			Collisions_MakeTri(vec2,vec3,vec1,&(*ppTris)[iCount]);
			iCount++;
		}
	}
}*/