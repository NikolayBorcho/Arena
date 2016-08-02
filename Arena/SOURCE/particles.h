//
// particles.h
// craig
//

#ifndef _PARTICLES_H
#define _PARTICLES_H

#include "includeall.h"
#include "object.h"

#include "sprite.h"

enum PARTICLE_GFX
{
	PARTICLE_BLOB,
	PARTICLE_SPARK,
	PARTICLE_SMOKE,
	PARTICLE_FLAME,
	PARTICLE_RING,
	PARTICLE_RINGS,
	PARTICLE_FLAME2,
	PARTICLE_MAX,
};

typedef struct _ParticleCreate
{
	i32 iID;
	Vec3 vecPos;
	Vec3 vecDir;
	Object *pOwner;
	i32 iNumParticles;
	i32 ParticleType; 
	float fSystemTime;
	float fParticleTime;// particle life in seconds
	float fParticleRate;// particles to generate per frame
	float fSpread;
	float fMaxSpeed;
	float fMinSpeed;
	float fMinSize;
	float fMaxSize;
	float fRotSpeed;
	float fGrowSpeed;
	float fFriction;
	u32 uColour1;
	u32 uColour2;
	bool bAdditive;
	bool bGravity;
	bool bBounce;
	bool bYFlat;
}ParticleCreate;

// Create new particle effect 
void Particles_Create( ParticleCreate *pCreate );

// Create loaded particle effect
//  0 = Explosion
//	1 = Spark
void Particles_Create( i32 iID, Vec3 &vecPos );
void Particles_LoadDefinitions();
void Particles_FreeDefinitions();

#endif

