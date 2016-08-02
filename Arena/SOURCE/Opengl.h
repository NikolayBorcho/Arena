// 
// opengl.h
// craig
//

#ifndef _OPENGL_H_
#define _OPENGL_H_

#include <windows.h>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>	

typedef struct _Vertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
	u32 colour;
}Vertex;

enum	RENDER_MODE
{
	RENDER_MODE_NONE,
	RENDER_MODE_FLAT,
	RENDER_MODE_ADD,
	RENDER_MODE_ALPHA,
	RENDER_MODE_TEXTURE_ADD,
	RENDER_MODE_TEXTURE,
	RENDER_MODE_ALPHATEXTURE,
	RENDER_MODE_SUBTRACTTEXTURE,
};

enum RENDER_TYPE
{
	PRIM_TYPE_LINELIST,
	PRIM_TYPE_TRIANGLELIST,
	PRIM_TYPE_TRIANGLESTRIP,
	PRIM_TYPE_QUADLIST,
};

#define RENDER_UVs		0x00000001
#define RENDER_COLOUR	0x00000002

#define ARGB(a,r,g,b) ((((u8)(a*255.f))<<24) + (((u8)(b*255.f))<<16) + (((u8)(g*255.f))<<8) + ((u8)(r*255.f)))

void GL_Init(int iWidth,int Height);
void GL_RenderBegin();
void GL_SetTexture(GLuint tex);
void GL_RenderMode(RENDER_MODE mode);
void GL_Resize(i32 iWidth,i32 iHeight);
void GL_SetViewAngle(float fViewAngle);

void GL_PrimitiveStart(RENDER_TYPE type, u32 uFlags);
void GL_RenderPrimitives(void);

void GL_RenderVerts(RENDER_TYPE rType, u32 uFlags, Vertex *pVerts, i32 iNum);

void GL_Vert(float x,float y, float z, float u1,float v1);
void GL_Vert(float x,float y, float z, float u1,float v1, u32 colour);
void GL_Vert(float x,float y, float z, u32 colour);
void GL_Quad(float x,float y, float z, float w, float h, u32 colour);
void GL_Quad(float x,float y, float z, float w, float h,float u1,float v1, float u2, float v2, u32 colour);

void GL_Ortho(i32 iWidth,i32 iHeight);
void GL_Projection(i32 iWidth,i32 iHeight);

u32  GL_ARGB(u8 a, u8 r, u8 g, u8 b);
u32  GL_ARGB(i32 a, i32 r, i32 g, i32 b);

Matrix *GL_GetProjection();

typedef void (APIENTRY *LOCKARRAYS_PROC)(int first, int count);
typedef void (APIENTRY *UNLOCKARRAYS_PROC)(void);

extern LOCKARRAYS_PROC lpfnLockArrays;
extern UNLOCKARRAYS_PROC lpfnUnlockArrays;

extern u32 uPolycount;
#endif //_OPENGL_H_