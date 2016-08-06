//
// Particles.cpp
// craig
//

#include "Particles.h"
#include "font.h"
#include "Input.h"
#include "opengl.h"
#include "Camera.h"

#define MAX_PARTICLES 32
#define MAX_EFFECTS 10	// NIK

typedef struct _Particles
{
	Object pObject;

	Sprite *pSprites[MAX_PARTICLES];
	Vec3 vecPos[MAX_PARTICLES];
	Vec3 vecVel[MAX_PARTICLES];

	float fParticleSize[MAX_PARTICLES];
	u32 uColours[MAX_PARTICLES];
	float fParticleTime[MAX_PARTICLES];
	float fRotSpeed[MAX_PARTICLES];

	u8 uR1;
	u8 uG1;
	u8 uB1;
	u8 uA1;

	u8 uR2;
	u8 uG2;
	u8 uB2;
	u8 uA2;

	Vec4 vecUVs;
	float fSystemTime;
	float fSystemMaxTime;
	float fParticleMaxTime;
	float fParticleRate;
	float fGenerateTime;
	float fMaxSpeed;
	float fMinSpeed;
	float fMinSize;
	float fMaxSize;
	float fSpread;
	float fGrowSpeed;
	float fFriction;
	
	i32 iMaxParticles;
	bool bAdditive;
	bool bActive;
	bool bGravity;
	bool bBounce;
	bool bRot;
	bool bOneShot;
	bool bYFlat;

	u32 uRenderFrame;
	bool bSpritesFreed;
}Particles;

static ParticleCreate *pDefs;
static u32 iNumDefs;
static i32 iNumEffects = 0;	// NIK: keep track of how many particle effects are on at the same time

//------------------------------------------------------------------

void Particles_LoadDefinitions()
{
	FILE *pFile;

	pFile=fopen("data/particles/particledefs", "rb");

	pDefs=0;
	iNumDefs=0;

	u32 iSizeDef;
	u32 i;

	if(pFile)
	{
		fread(&iNumDefs,sizeof(u32),1,pFile);
		fread(&iSizeDef,sizeof(u32),1,pFile);

		pDefs=(ParticleCreate*)Mem_New(sizeof(ParticleCreate)*iNumDefs);
		memset(pDefs,0,sizeof(ParticleCreate)*iNumDefs);

		for(i=0;i<iNumDefs;i++)
		{
			fread(&pDefs[i],sizeof(char),iSizeDef,pFile);
		}

		fclose(pFile);
	}
}

//------------------------------------------------------------------

void Particles_FreeDefinitions()
{
	if(pDefs)
	{
		Mem_Free(pDefs);
		pDefs=NULL;
	}
}

//------------------------------------------------------------------

void APIENTRY Particles_Exit(Object *pObject)
{
	Particles *pParticles=(Particles *)pObject;

	Mem_Free(pParticles);
}

//------------------------------------------------------------------

static void _NewParticle(Particles *pParticles,i32 i,float fTimeInterp)
{
	float fAngle;
	float fAngle2;
	float fSpeed;
	
	u8 uR;
	u8 uG;
	u8 uB;
	u8 uA;

	uR=(u8)( fTimeInterp*pParticles->uR1 );
	uG=(u8)( fTimeInterp*pParticles->uG1 );
	uB=(u8)( fTimeInterp*pParticles->uB1 );
	uA=(u8)( fTimeInterp*pParticles->uA1 );

	Matrix *pMat;
	Object_GetMatrixPtr((Object *)pParticles, &pMat);

	if(!pParticles->bSpritesFreed)
	{
		if(pParticles->bRot)
		{
			pParticles->pSprites[i]->fRot+=pParticles->fRotSpeed[i];
		}
		pParticles->vecPos[i]=pMat->GetColumn(3);
		pParticles->fParticleSize[i]=pParticles->fMinSize + (pParticles->fMaxSize-pParticles->fMinSize)*random();
		pParticles->uColours[i]=(uA<<24) | (uB<<16) | (uG<<8) | (uR);
	}
	pParticles->fParticleTime[i]=0.f;

	fSpeed=pParticles->fMinSpeed + (pParticles->fMaxSpeed-pParticles->fMinSpeed)*random();
	fAngle=(360.f)*random();
	fAngle2=pParticles->fSpread*random();

	pParticles->vecVel[i].SetX(g_MathLib.Sin(fAngle)*g_MathLib.Sin(fAngle2)*fSpeed);
	pParticles->vecVel[i].SetY(g_MathLib.Cos(fAngle2)*fSpeed);
	pParticles->vecVel[i].SetZ(g_MathLib.Cos(fAngle)*g_MathLib.Sin(fAngle2)*fSpeed);

	pParticles->vecVel[i]=*pMat*pParticles->vecVel[i];
	pParticles->fParticleTime[i]=0.0001f;
}

//------------------------------------------------------------------

void APIENTRY Particles_Update(Object *pObject)
{
	Particles *pParticles=(Particles *)pObject;
	i32 i;

	float fTimeInterp=1.f-(pParticles->fSystemTime/pParticles->fSystemMaxTime);
	float fTimeInterpParticle;

	u8 uR;
	u8 uG;
	u8 uB;
	u8 uA;
	i32 iCreate = (i32)pParticles->fGenerateTime;

	if(fTimeInterp<0.f)
	{
		fTimeInterp=0.f;
		iCreate=0;
	}

	if(pParticles->bOneShot)
	{
		iCreate=0;
	}

	if(!pParticles->bSpritesFreed)
	{
		for(i=0;i<pParticles->iMaxParticles;i++)
		{
			if(pParticles->fParticleTime[i])
			{
				if(pParticles->fParticleTime[i]<pParticles->fParticleMaxTime)
				{
					pParticles->vecPos[i]+=pParticles->vecVel[i];

					if(pParticles->bGravity)
					{
						pParticles->vecVel[i].SetY(pParticles->vecVel[i].GetY()-0.02f);
					}
					if(pParticles->bBounce)
					{
						if(pParticles->vecPos[i].GetY()<0.f)
						{
							pParticles->vecVel[i].SetY(-pParticles->vecVel[i].GetY()*0.7f);
							pParticles->vecVel[i]=pParticles->vecVel[i]*(0.5f);
							pParticles->vecPos[i]+=pParticles->vecVel[i];	
						}
					}
					
					if(pParticles->fFriction>0.f)
					{
						pParticles->vecVel[i]=pParticles->vecVel[i]*pParticles->fFriction;
					}
					
					pParticles->fParticleSize[i]+=pParticles->fGrowSpeed;

					fTimeInterpParticle=pParticles->fParticleTime[i]/pParticles->fParticleMaxTime;

					if(pParticles->bAdditive)
					{
						if(fTimeInterpParticle<0.5f)
						{
							fTimeInterpParticle=fTimeInterpParticle*2.f;
							uR=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uR2 + (1.f-fTimeInterpParticle)*pParticles->uR1) );
							uG=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uG2 + (1.f-fTimeInterpParticle)*pParticles->uG1) );
							uB=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uB2 + (1.f-fTimeInterpParticle)*pParticles->uB1) );
							uA=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uA2 + (1.f-fTimeInterpParticle)*pParticles->uA1) );
						}
						else
						{
							fTimeInterpParticle=1.f - (fTimeInterpParticle-0.5f)*2.f;
							uR=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uR2) );
							uG=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uG2) );
							uB=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uB2) );
							uA=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uA2) );
						}
					}
					else
					{
						if(fTimeInterpParticle<0.5f)
						{
							fTimeInterpParticle=fTimeInterpParticle*2.f;
							uR=(u8)( (fTimeInterpParticle*pParticles->uR2 + (1.f-fTimeInterpParticle)*pParticles->uR1) );
							uG=(u8)( (fTimeInterpParticle*pParticles->uG2 + (1.f-fTimeInterpParticle)*pParticles->uG1) );
							uB=(u8)( (fTimeInterpParticle*pParticles->uB2 + (1.f-fTimeInterpParticle)*pParticles->uB1) );
							uA=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uA2 + (1.f-fTimeInterpParticle)*pParticles->uA1) );
						}
						else
						{
							fTimeInterpParticle=1.f - (fTimeInterpParticle-0.5f)*2.f;
							uR=(u8)( (fTimeInterpParticle*pParticles->uR2) );
							uG=(u8)( (fTimeInterpParticle*pParticles->uG2) );
							uB=(u8)( (fTimeInterpParticle*pParticles->uB2) );
							uA=(u8)( fTimeInterp*(fTimeInterpParticle*pParticles->uA2) );
						}

					}

					
					pParticles->uColours[i]=(uA<<24) | (uB<<16) | (uG<<8) | (uR);
					
					pParticles->fParticleTime[i]+=0.05f;
				}
				else
				{
					pParticles->uColours[i]=0;
					pParticles->fParticleTime[i]=0.f;
				}

				if(pParticles->bRot)
				{
					pParticles->pSprites[i]->fRot+=pParticles->fRotSpeed[i];
				}
				pParticles->pSprites[i]->vecPos=pParticles->vecPos[i];
				pParticles->pSprites[i]->fSize=pParticles->fParticleSize[i];
				pParticles->pSprites[i]->uColour=pParticles->uColours[i];
			}
			else if(iCreate>0)
			{
				_NewParticle(pParticles,i,fTimeInterp);
				iCreate--;
				pParticles->fGenerateTime=0.f;
			}
		}
	}

	if(pParticles->fSystemMaxTime<95.f)
		pParticles->fSystemTime+=0.05f;
//	else
	{
		if(!pParticles->bSpritesFreed && pParticles->uRenderFrame<Object_GetRenderFrame()-10)
		{
			for(i=0;i<pParticles->iMaxParticles;i++)
			{
				ASSERT(pParticles->pSprites[i]->bActive,"sprite removed");
				Sprite_Remove(pParticles->pSprites[i]);
			}
			pParticles->bSpritesFreed=true;
		}
		else if(pParticles->bSpritesFreed && pParticles->uRenderFrame>=Object_GetRenderFrame()-1)
		{
			i32 iMode;

			if(pParticles->bAdditive)
			{
				iMode=SPR_ADD;
			}
			else
			{
				iMode=SPR_ALPHA;
			}

			for(i=0;i<pParticles->iMaxParticles;i++)
			{
				pParticles->pSprites[i]=Sprite_Add(pParticles->vecPos[i],
					pParticles->vecUVs,
					pParticles->fParticleSize[i],
					pParticles->uColours[i],
					iMode);

				Sprite_SetYFlat( pParticles->pSprites[i], pParticles->bYFlat );
			}
			pParticles->bSpritesFreed=false;
		}
	}

	pParticles->fGenerateTime+=pParticles->fParticleRate;

	if(pParticles->fSystemTime>pParticles->fSystemMaxTime)
	{
		if(!pParticles->bSpritesFreed)
		{
			for(i=0;i<pParticles->iMaxParticles;i++)
			{
				ASSERT(pParticles->pSprites[i]->bActive,"sprite removed");
				Sprite_Remove(pParticles->pSprites[i]);
			}
			pParticles->bSpritesFreed=true;
		}
		TrashCan_DeleteObject(pObject);
		iNumEffects--;
	}
}

//------------------------------------------------------------------

void APIENTRY Particles_Render(Object *pObject)
{
	Particles *pParticles=(Particles *)pObject;

	pParticles->uRenderFrame = Object_GetRenderFrame();
}

static void _Setup(ParticleCreate *pCreate,Particles *pParticles)
{
	i32 i;

	Vec4 vecX;
	Vec4 vecY;
	Vec4 vecZ;

	vecZ=pCreate->vecDir;
	if(qfabs(vecZ.GetY())>(0.9999f))
	{
		vecX.Set(1.f, 0.f, 0.f, 0.f);
		vecY=vecZ.CrossProd(vecX);
		vecY.Normalise();
		
		vecX=vecZ.CrossProd(vecY);
		vecX.Normalise();
	}
	else
	{
		vecY.Set(0.f, 1.f, 0.f, 0.f);
		vecX=vecY.CrossProd(vecZ);
		vecX.Normalise();
		
		vecY=vecZ.CrossProd(vecX);
		vecY.Normalise();
	}
	vecX.SetW(0.f);
	vecY.SetW(0.f);
	vecZ.SetW(0.f);

	Matrix Mat;
	

	Mat.SetColumn(0, vecX);
	Mat.SetColumn(1, vecZ);
	Mat.SetColumn(2, vecY);
	Mat.SetColumn(3, pCreate->vecPos);
	Mat.SetVal(3, 3, 1.f);
	Object_SetAllMatrix((Object *)pParticles, &Mat);

	float fAngle;
	float fAngle2;
	float fSpeed;
	i32 iMode;

	if(pCreate->bAdditive)
	{
		iMode=SPR_ADD;
	}
	else
	{
		iMode=SPR_ALPHA;
	}

	i32 iGenerate;

	if(pCreate->iNumParticles == (i32)pCreate->fParticleRate)
	{
		iGenerate=pCreate->iNumParticles;
	}
	else
	{
		iGenerate=1 + (i32)pCreate->fParticleRate;
	}

	switch(pCreate->ParticleType)
	{
	case PARTICLE_BLOB:
		pParticles->vecUVs.Set(0.f, 0.f, 16.f/128.f ,16.f/128.f);
		break;
	case PARTICLE_SPARK:
		pParticles->vecUVs.Set(0,16.f/128.f,16.f/128.f,32.f/128.f);
		break;
	case PARTICLE_SMOKE:
		pParticles->vecUVs.Set((64.f/256.f),(0.f),(128.f/256.f),(64.f/256.f));
		break;
	case PARTICLE_FLAME:
	//	pParticles->vecUVs.Set(Fixed(0),Fixed(128.f/256.f),Fixed(64.f/256.f),Fixed(191.f/256.f));
		pParticles->vecUVs.Set((0),(64.f/256.f),(64.f/256.f),(128.f/256.f));
		break;
	case PARTICLE_RING:
		pParticles->vecUVs.Set((0),(192.f/256.f),(64.f/256.f),(256.f/256.f));
		break;
	case PARTICLE_RINGS:
		pParticles->vecUVs.Set((0),(128.f/256.f),(64.f/256.f),(192.f/256.f));
		break;
	case PARTICLE_FLAME2:
		pParticles->vecUVs.Set((64.f/256.f),(64.5f/256.f),(128.f/256.f),(128.f/256.f));
		break;
	}

	for(i=0;i<iGenerate;i++)
	{
		pParticles->vecPos[i]=pCreate->vecPos;
		pParticles->fParticleSize[i]=pCreate->fMinSize + (pCreate->fMaxSize-pCreate->fMinSize)*random();
		pParticles->uColours[i]=0;

		if(Object_GetUpdateFrame()>0)
		{
			pParticles->pSprites[i]=Sprite_Add(pCreate->vecPos,
				pParticles->vecUVs,
				pParticles->fParticleSize[i]
				,0/*pCreate->uColour1*/, 
				iMode);

				Sprite_SetYFlat( pParticles->pSprites[i], pCreate->bYFlat );
		}

		pParticles->vecPos[i]=pCreate->vecPos;
		pParticles->fParticleTime[i]=0.f;

		fSpeed=pCreate->fMinSpeed + (pCreate->fMaxSpeed-pCreate->fMinSpeed)*random();
		fAngle=(360.f)*random();
		fAngle2=pCreate->fSpread*random();

		pParticles->vecVel[i].SetX(g_MathLib.Sin(fAngle)*g_MathLib.Sin(fAngle2)*fSpeed);
		pParticles->vecVel[i].SetY(g_MathLib.Cos(fAngle2)*fSpeed);
		pParticles->vecVel[i].SetZ(g_MathLib.Cos(fAngle)*g_MathLib.Sin(fAngle2)*fSpeed);

		pParticles->vecVel[i]=Mat*pParticles->vecVel[i];
		pParticles->fParticleTime[i]=0.0001f;

		if(pCreate->fRotSpeed>0.f)
		{
			if(Object_GetUpdateFrame()>0)
				pParticles->pSprites[i]->fRot=random()*(M_PI*2.f);
			pParticles->fRotSpeed[i]=pCreate->fRotSpeed*random()-0.5f;
		}
	}

	for(;i<pCreate->iNumParticles;i++)
	{
		pParticles->vecPos[i]=pCreate->vecPos;
		pParticles->fParticleSize[i]=pCreate->fMinSize + (pCreate->fMaxSize-pCreate->fMinSize)*random();
		pParticles->uColours[i]=0;

		if(Object_GetUpdateFrame()>0)
		{
			pParticles->pSprites[i]=Sprite_Add(pCreate->vecPos,
				pParticles->vecUVs,
				pParticles->fParticleSize[i]
				,0, 
				iMode);

			Sprite_SetYFlat( pParticles->pSprites[i], pCreate->bYFlat );
		}

		pParticles->fParticleTime[i]=0.f;

		if(pCreate->fRotSpeed>0.f)
		{
			if(Object_GetUpdateFrame()>0)
				pParticles->pSprites[i]->fRot=random()*(M_PI*2.f);
			pParticles->fRotSpeed[i]=random()-0.5f;
		}
	}

	pParticles->uR1=(pCreate->uColour1&0x00ff0000)>>16;
	pParticles->uG1=(pCreate->uColour1&0x0000ff00)>>8;
	pParticles->uB1=(pCreate->uColour1&0x000000ff);
	pParticles->uA1=(pCreate->uColour1&0xff000000)>>24;

	pParticles->uR2=(pCreate->uColour2&0x00ff0000)>>16;
	pParticles->uG2=(pCreate->uColour2&0x0000ff00)>>8;
	pParticles->uB2=(pCreate->uColour2&0x000000ff);
	pParticles->uA2=(pCreate->uColour2&0xff000000)>>24;

	pParticles->fSystemTime=0.f;
	pParticles->iMaxParticles=pCreate->iNumParticles;
	pParticles->fSystemMaxTime=pCreate->fSystemTime;
	pParticles->fParticleMaxTime=pCreate->fParticleTime;
	pParticles->fParticleRate=pCreate->fParticleRate;
	pParticles->fGenerateTime=0.f;
	pParticles->fMaxSpeed=pCreate->fMaxSpeed;
	pParticles->fMinSpeed=pCreate->fMinSpeed;
	pParticles->fMinSize=pCreate->fMinSize;
	pParticles->fMaxSize=pCreate->fMaxSize;
	pParticles->fSpread=pCreate->fSpread;
	pParticles->bGravity=pCreate->bGravity;
	pParticles->bBounce=pCreate->bBounce;
	pParticles->bRot=pCreate->fRotSpeed>0.f;
	pParticles->fGrowSpeed=pCreate->fGrowSpeed;
	pParticles->fFriction=pCreate->fFriction;
	pParticles->bOneShot=pCreate->iNumParticles == (i32)pCreate->fParticleRate;
	pParticles->bYFlat=pCreate->bYFlat;
	pParticles->bAdditive=pCreate->bAdditive;

	if(Object_GetUpdateFrame()==0)
	{
		pParticles->bSpritesFreed=true;
	}
	else
	{
		pParticles->bSpritesFreed=false;
	}
}

//------------------------------------------------------------------

void Particles_Create( ParticleCreate *pCreate )
{
	Particles *pParticles;
	ObjectCreate Create;

	memset(&Create,0,sizeof(Create));
	Create.exitfunc=Particles_Exit;
	Create.renderfunc=Particles_Render;
	Create.updatefunc=Particles_Update;
	Create.iType=OBJECT_Particle;

	Create.pName="Particles";

	pParticles = (Particles*)Object_Create(&Create, sizeof(Particles));

	pParticles->uRenderFrame=0;

	_Setup(pCreate,pParticles);

//	return pParticles;
}

//------------------------------------------------------------------

void Particles_Create( i32 iID, Vec3 &vecPos )
{
	// NIK: do not create a new effect if exceeded limit
	if (iNumEffects < MAX_EFFECTS)
		iNumEffects++;
	else
		return;

	ParticleCreate *pDef;
	i32 iNumDefs2=iNumDefs;
	if(iID>(i32)iNumDefs)
	{
		pDef=&pDefs[iNumDefs];
		while(iNumDefs2)
		{
			pDef--;
			iNumDefs2--;
			if(pDef->iID==iID)
			{
				break;
			}
		}
	}
	else
	{
		pDef=&pDefs[iID];
		if(pDef->iID>iID)
		{
			while(true)
			{
				pDef--;
				if(!pDef->iID || pDef->iID==iID)
				{
					break;
				}
			}
		}
		else if(pDef->iID<iID)
		{
			while(true)
			{
				pDef++;
				if(!pDef->iID || pDef->iID==iID)
				{
					break;
				}
			}
		}
	}

	pDef->vecPos=vecPos;
	Particles_Create( pDef );
}
