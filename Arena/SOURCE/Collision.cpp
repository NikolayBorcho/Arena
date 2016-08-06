//
// Collision.cpp
// craig
//

#include "Collision.h"
#include "opengl.h"

#define MAX_BOXES 1000

static ColBox *pBoxes[MAX_BOXES];
static i32 iBox;
static ColData thisColData;

//------------------------------------------------------------------

void APIENTRY Collision_Exit(Object *pObject)
{
	Collision *pCollision=(Collision *)pObject;
	iBox = 0;	// NIK: do not allow to pile up over time with game restarts
	Mem_Free(pCollision);
}

//------------------------------------------------------------------

void APIENTRY Collision_Update(Object *pObject)
{
	Collision *pCollision=(Collision *)pObject;
}

//------------------------------------------------------------------

void APIENTRY Collision_Render(Object *pObject)
{
	Collision *pCollision=(Collision *)pObject;
	i32 i,j;
	Vec4 vec;
	Vec3 vecNorm;

	GL_SetTexture(0);
	GL_RenderMode(RENDER_MODE_FLAT);

	GL_PrimitiveStart(PRIM_TYPE_LINELIST, RENDER_COLOUR);

	for(i=0;i<iBox;i++)
	{
		for(j=0;j<6;j++)
		{
			vec = pBoxes[i]->mat * pBoxes[i]->center[j];
			vecNorm = pBoxes[i]->normals[j];
			vecNorm = pBoxes[i]->mat * vecNorm; 
			GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

			GL_Vert(vec.GetX() + vecNorm.GetX(),
				vec.GetY() + vecNorm.GetY(), 
				vec.GetZ() + vecNorm.GetZ(), 0xffffffff);
		}

		vec.Set(pBoxes[i]->Points[0].GetX(),pBoxes[i]->Points[0].GetY(),pBoxes[i]->Points[0].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[1].GetX(),pBoxes[i]->Points[1].GetY(),pBoxes[i]->Points[1].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[5].GetX(),pBoxes[i]->Points[5].GetY(),pBoxes[i]->Points[5].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[3].GetX(),pBoxes[i]->Points[3].GetY(),pBoxes[i]->Points[3].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[0].GetX(),pBoxes[i]->Points[0].GetY(),pBoxes[i]->Points[0].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[2].GetX(),pBoxes[i]->Points[2].GetY(),pBoxes[i]->Points[2].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[1].GetX(),pBoxes[i]->Points[1].GetY(),pBoxes[i]->Points[1].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[4].GetX(),pBoxes[i]->Points[4].GetY(),pBoxes[i]->Points[4].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[3].GetX(),pBoxes[i]->Points[3].GetY(),pBoxes[i]->Points[3].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[6].GetX(),pBoxes[i]->Points[6].GetY(),pBoxes[i]->Points[6].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[5].GetX(),pBoxes[i]->Points[5].GetY(),pBoxes[i]->Points[5].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);

		vec.Set(pBoxes[i]->Points[7].GetX(),pBoxes[i]->Points[7].GetY(),pBoxes[i]->Points[7].GetZ(),1.f);
		vec = pBoxes[i]->mat * vec;

		GL_Vert(vec.GetX(),
				vec.GetY(), 
				vec.GetZ(), 0xffff0000);
	}

	GL_RenderPrimitives();
}

//------------------------------------------------------------------

Collision* Collision_Create( )
{
	Collision *pCollision;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Collision_Exit;
//	Create.renderfunc=Collision_Render;
//	Create.updatefunc=Collision_Update;
	Create.iType=OBJECT_Collision;
	Create.pName="Collision";

	pCollision = (Collision*)Object_Create(&Create, sizeof(Collision));

	return pCollision;
}

//------------------------------------------------------------------

ColBox* Collision_CreateBox(Vec3 *pvecMax, Vec3 *pvecMin, Matrix *pMat)
{
	ColBox *pBox = (ColBox*)Mem_New(sizeof(ColBox));

	pBox->mat = *pMat;
	pBox->mat.Inverse(&pBox->matInv);

	pBox->Points[0].Set(pvecMax->GetX(), pvecMax->GetY(), pvecMax->GetZ(), 1.f);

	pBox->Points[1].Set(pvecMin->GetX(), pvecMax->GetY(), pvecMax->GetZ(), 1.f);

	pBox->Points[2].Set(pvecMax->GetX(), pvecMin->GetY(), pvecMax->GetZ(), 1.f);

	pBox->Points[3].Set(pvecMax->GetX(), pvecMax->GetY(), pvecMin->GetZ(), 1.f);

	pBox->Points[4].Set(pvecMin->GetX(), pvecMin->GetY(), pvecMax->GetZ(), 1.f);

	pBox->Points[5].Set(pvecMin->GetX(), pvecMax->GetY(), pvecMin->GetZ(), 1.f);

	pBox->Points[6].Set(pvecMax->GetX(), pvecMin->GetY(), pvecMin->GetZ(), 1.f);

	pBox->Points[7].Set(pvecMin->GetX(), pvecMin->GetY(), pvecMin->GetZ(), 1.f);


	pBox->normals[0].CalcNormal(pBox->Points[1],pBox->Points[0],pBox->Points[2]);
	pBox->normals[0].SetW( -pBox->normals[0].Dot(pBox->Points[0]));
	pBox->center[0] = (pBox->Points[0] + pBox->Points[1] + pBox->Points[2])/3.f;
	pBox->center[0].SetW(1.f);

	pBox->normals[1].CalcNormal(pBox->Points[0],pBox->Points[3],pBox->Points[6]);
	pBox->normals[1].SetW( -pBox->normals[1].Dot(pBox->Points[3]));
	pBox->center[1] = (pBox->Points[3] + pBox->Points[0] + pBox->Points[6])/3.f;
	pBox->center[1].SetW(1.f);

	pBox->normals[2].CalcNormal(pBox->Points[3],pBox->Points[5],pBox->Points[7]);
	pBox->normals[2].SetW( -pBox->normals[2].Dot(pBox->Points[5]));
	pBox->center[2] = (pBox->Points[5] + pBox->Points[3] + pBox->Points[7])/3.f;
	pBox->center[2].SetW(1.f);

	pBox->normals[3].CalcNormal(pBox->Points[5],pBox->Points[1],pBox->Points[4]);
	pBox->normals[3].SetW( -pBox->normals[3].Dot(pBox->Points[1]));
	pBox->center[3] = (pBox->Points[1] + pBox->Points[5] + pBox->Points[4])/3.f;
	pBox->center[3].SetW(1.f);

	pBox->normals[4].CalcNormal(pBox->Points[5],pBox->Points[3],pBox->Points[0]);
	pBox->normals[4].SetW( -pBox->normals[4].Dot(pBox->Points[3]));
	pBox->center[4] = (pBox->Points[3] + pBox->Points[5] + pBox->Points[0])/3.f;
	pBox->center[4].SetW(1.f);

	pBox->normals[5].CalcNormal(pBox->Points[6],pBox->Points[7],pBox->Points[2]);
	pBox->normals[5].SetW( -pBox->normals[5].Dot(pBox->Points[6]));
	pBox->center[5] = (pBox->Points[6] + pBox->Points[7] + pBox->Points[2])/3.f;
	pBox->center[5].SetW(1.f);

	pBox->fRadius = (*pvecMax - *pvecMin).Length()/2.f;

	pBox->vecMax = *pvecMax;
	pBox->vecMin = *pvecMin;
	ASSERT(iBox<MAX_BOXES, "too many collision boxes");
	pBoxes[iBox] = pBox;
	iBox++;

	return pBox;
}

//------------------------------------------------------------------

void Collision_UpdateMat(ColBox *pBox, Matrix *pMat)
{
	pBox->mat = *pMat;
	pBox->mat.Inverse(&pBox->matInv);
}

//------------------------------------------------------------------

void Collision_DeleteBox(ColBox *pBox)
{
	Mem_Free(pBox);
}

//------------------------------------------------------------------

bool Collision_BoxPointTest(ColBox *pBoxThis, Vec4 &VecPoint)
{
	Vec4 VecTestPoint(pBoxThis->matInv * VecPoint);

	if(pBoxThis->fRadius<VecTestPoint.Length())
	{
		return false;
	}

	if(pBoxThis->vecMax.GetX()<VecTestPoint.GetX() ||
		pBoxThis->vecMax.GetY()<VecTestPoint.GetY() ||
		pBoxThis->vecMax.GetZ()<VecTestPoint.GetZ())
	{
		return false;
	}

	if(pBoxThis->vecMin.GetX()>VecTestPoint.GetX() ||
		pBoxThis->vecMin.GetY()>VecTestPoint.GetY() ||
		pBoxThis->vecMin.GetZ()>VecTestPoint.GetZ())
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------

bool _BoxPointTest(ColBox *pBoxThis, Vec3 &VecPoint)
{
	if(pBoxThis->fRadius<VecPoint.Length())
	{
		return false;
	}

	if(pBoxThis->vecMax.GetX()<VecPoint.GetX() ||
		pBoxThis->vecMax.GetY()<VecPoint.GetY() ||
		pBoxThis->vecMax.GetZ()<VecPoint.GetZ())
	{
		return false;
	}

	if(pBoxThis->vecMin.GetX()>VecPoint.GetX() ||
		pBoxThis->vecMin.GetY()>VecPoint.GetY() ||
		pBoxThis->vecMin.GetZ()>VecPoint.GetZ())
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------

bool Collision_BoxLineTest(ColBox *pBoxThis, Vec4 &VecStart, Vec4 &VecEnd)
{
	i32 i;
	Vec4 VecTestStart(pBoxThis->matInv * VecStart);
	Vec4 VecTestEnd(pBoxThis->matInv * VecEnd);

	Vec3 vecLine(VecTestEnd - VecTestStart);
	Vec3 vecLineNorm(vecLine);
	float fLineLen = vecLineNorm.Normalise();

	Vec4 *pNormals = pBoxThis->normals;
	float fDotStart;
	float fDotEnd;
	float fDotLine;

	for(i=0;i<6;i++)
	{
		if((fDotLine=pNormals->Dot(vecLine))<0)
		{
			fDotStart = pNormals->GetW() + pNormals->Dot(VecTestStart);
			fDotEnd = pNormals->GetW() + pNormals->Dot(VecTestEnd);

			if(fDotStart>0.f && fDotEnd<0.f)
			{
				float frac = -fDotStart/fDotLine;

				Vec3 CollPoint(VecTestStart + vecLineNorm*(fLineLen * frac));
				if(_BoxPointTest(pBoxThis, CollPoint))
				{
					Vec4 vecCol;
					vecCol.Set(CollPoint.GetX(), CollPoint.GetY(), CollPoint.GetZ(), 1.f);
					thisColData.normal = *pNormals;
					thisColData.normal = pBoxThis->mat * thisColData.normal;
					thisColData.vecPoint = pBoxThis->mat * vecCol;
					return true;
				}
			}
		}
		pNormals++;
	}

	return false;
}

//------------------------------------------------------------------

bool _BoxLineTest(ColBox *pBoxThis, Vec4 &VecTestStart, Vec4 &VecTestEnd)
{
	i32 i;

	Vec3 vecLine(VecTestEnd - VecTestStart);
	Vec3 vecLineNorm(vecLine);
	float fLineLen = vecLineNorm.Normalise();

	Vec4 *pNormals = pBoxThis->normals;
	float fDotStart;
	float fDotEnd;
	float fDotLine;
	bool	bCollide = false;
	Vec3 vecNormal;
	float fNormalCount=0.f;

	thisColData.normal.Set(0.f, 0.f, 0.f);
	for(i=0;i<6;i++)
	{
		if((fDotLine=pNormals->Dot(vecLine))<0)
		{
			fDotStart = pNormals->GetW() + pNormals->Dot(VecTestStart);
			fDotEnd = pNormals->GetW() + pNormals->Dot(VecTestEnd);

			if(fDotStart>0.f && fDotEnd<0.f)
			{
				float frac = -fDotStart/fDotLine;

				Vec3 CollPoint(VecTestStart + vecLineNorm*(fLineLen * frac));
				if(_BoxPointTest(pBoxThis, CollPoint))
				{
					Vec4 vecCol;
					vecCol.Set(CollPoint.GetX(), CollPoint.GetY(), CollPoint.GetZ(), 1.f);
					vecNormal = *pNormals;
					thisColData.normal += pBoxThis->mat * vecNormal;
					thisColData.vecPoint = pBoxThis->mat * vecCol;
					fNormalCount++;
					bCollide = true;
				}
			}
		}
		pNormals++;
	}

	if(bCollide)
	{
		thisColData.normal = thisColData.normal/fNormalCount;
	}

	return bCollide;
}

//------------------------------------------------------------------

bool Collision_BoxBoxTest(ColBox *pBoxThis,ColBox *pBoxTest)
{
	Matrix TestMat(pBoxTest->mat * pBoxThis->matInv);

	if(pBoxThis->fRadius + pBoxTest->fRadius<TestMat.GetColumn(3).Length())
	{
		return false;
	}

	Vec4 VecTestStart(pBoxTest->mat * pBoxTest->Points[0]);
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[3]);
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[5]);
	VecTestStart = pBoxThis->matInv * VecTestStart;
//	VecTestEnd = (TestMat * pBoxTest->Points[6]);

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[1]);
	VecTestStart = pBoxThis->matInv * VecTestStart;
//	VecTestEnd = (TestMat * pBoxTest->Points[7]);

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}



	VecTestStart = pBoxTest->mat * pBoxTest->Points[4];
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[2]);
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[6]);
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}

	VecTestStart = (pBoxTest->mat * pBoxTest->Points[7]);
	VecTestStart = pBoxThis->matInv * VecTestStart;

	if(_BoxLineTest(pBoxThis, TestMat.GetColumn(3), VecTestStart))
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------

void Collision_GetColData(ColData *pData)
{
	*pData = thisColData;
}