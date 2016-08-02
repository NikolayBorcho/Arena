//
// opengl.cpp
// craig
//

#include "includeall.h"
#include "opengl.h"

#define	MAX_VERTICES	200

#define MAX_INDEX 132
unsigned short QuadIndexs[]=
{
	0 ,1 ,2 ,2 ,3 ,0,
	4 ,5 ,6 ,6 ,7 ,4,
	8 ,9,10,10,11,8,
	12,13,14,14,15,12,
	16,17,18,18,19,16,
	20,21,22,22,23,20,
	24,25,26,26,27,24,
	28,29,30,30,31,28,
	32,33,34,34,35,32,
	36,37,38,38,39,36,
	40,41,42,42,43,40,
	44,45,46,46,47,44,
	48,49,50,50,51,48,
	52,53,54,54,55,52,
	56,57,58,58,59,56,
	60,61,62,62,63,60,
	64,65,66,66,67,64,
	68,69,70,70,71,68,
	72,73,74,74,75,72,
	76,77,78,78,79,76,
	80,81,82,82,83,80,
	84,85,86,86,87,84,
};

static Vertex vertices[MAX_VERTICES];
static Vertex *CurrentVert;
static i32 iNumVerts;
static RENDER_TYPE rType;
static u32 RENDER_Flags;
static GLuint iTexture=0;
static float fglView;
static Matrix Projection;

LOCKARRAYS_PROC lpfnLockArrays;
UNLOCKARRAYS_PROC lpfnUnlockArrays;

#ifdef _DEBUG

u32 uPolycount;
#endif
//------------------------------------------------------------------

void GL_Ortho(i32 iWidth,i32 iHeight)
{
	// setup an inverse projection matrix
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();

	float fmat[4][4];

	fmat[0][0]=2.f/(iWidth-0);
	fmat[0][1]=0;
	fmat[0][2]=0;
	fmat[0][3]=0;

	fmat[1][0]=0;
	fmat[1][1]=-2.f/(iHeight-0);
	fmat[1][2]=0;
	fmat[1][3]=0;

	fmat[2][0]=0;
	fmat[2][1]=0;
	fmat[2][2]=-2.f/(1-(-1));
	fmat[2][3]=0;

	fmat[3][0]=-(iWidth+0)/(float)(iWidth-0);
	fmat[3][1]=(iHeight+0)/(float)(iHeight-0);
	fmat[3][2]=(1+(-1))/(1-(-1));
	fmat[3][3]=1;
	glMultMatrixf(&fmat[0][0]);
	glMatrixMode(GL_MODELVIEW);	
}

//------------------------------------------------------------------

void GL_Projection(i32 iWidth,i32 iHeight)
{
	// setup an inverse projection matrix
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();

	float fmat[4][4];

	fmat[0][0]=2.f/(iWidth-0);
	fmat[0][1]=0;
	fmat[0][2]=0;
	fmat[0][3]=0;

	fmat[1][0]=0;
	fmat[1][1]=-2.f/(iHeight-0);
	fmat[1][2]=0;
	fmat[1][3]=0;

	fmat[2][0]=0;
	fmat[2][1]=0;
	fmat[2][2]=-2.f/(1-(-1));
	fmat[2][3]=0;

	fmat[3][0]=-(iWidth+0)/(float)(iWidth-0);
	fmat[3][1]=(iHeight+0)/(float)(iHeight-0);
	fmat[3][2]=(1+(-1))/(1-(-1));
	fmat[3][3]=1;
	gluPerspective(fglView,(GLfloat)iWidth/(GLfloat)iHeight,1.f,1000.f);
	//glMultMatrixf(&fmat[0][0]);

	glGetFloatv(GL_PROJECTION_MATRIX,&fmat[0][0]);

/*	Projection.SetVal(0,0, Fixed(fmat[0][0]));
	Projection.SetVal(0,1, Fixed(fmat[0][1]));
	Projection.SetVal(0,2, Fixed(fmat[0][2]));
	Projection.SetVal(0,3, Fixed(fmat[0][3]));

	Projection.SetVal(1,0, Fixed(fmat[1][0]));
	Projection.SetVal(1,1, Fixed(fmat[1][1]));
	Projection.SetVal(1,2, Fixed(fmat[1][2]));
	Projection.SetVal(1,3, Fixed(fmat[1][3]));

	Projection.SetVal(2,0, Fixed(fmat[2][0]));
	Projection.SetVal(2,1, Fixed(fmat[2][1]));
	Projection.SetVal(2,2, Fixed(fmat[2][2]));
	Projection.SetVal(2,3, Fixed(fmat[2][3]));

	Projection.SetVal(3,0, Fixed(fmat[3][0]));
	Projection.SetVal(3,1, Fixed(fmat[3][1]));
	Projection.SetVal(3,2, Fixed(fmat[3][2]));
	Projection.SetVal(3,3, Fixed(fmat[3][3]));*/
	Projection.SetVal(0,0, fmat[0][0]);
	Projection.SetVal(0,1, fmat[1][0]);
	Projection.SetVal(0,2, fmat[2][0]);
	Projection.SetVal(0,3, fmat[3][0]);

	Projection.SetVal(1,0, fmat[0][1]);
	Projection.SetVal(1,1, fmat[1][1]);
	Projection.SetVal(1,2, fmat[2][1]);
	Projection.SetVal(1,3, fmat[3][1]);

	Projection.SetVal(2,0, fmat[0][2]);
	Projection.SetVal(2,1, fmat[1][2]);
	Projection.SetVal(2,2, fmat[2][2]);
	Projection.SetVal(2,3, fmat[3][2]);

	Projection.SetVal(3,0, fmat[0][3]);
	Projection.SetVal(3,1, fmat[1][3]);
	Projection.SetVal(3,2, fmat[2][3]);
	Projection.SetVal(3,3, fmat[3][3]);

//	glLoadIdentity();
//	fmat[0][0]=-fmat[0][0];
//	fmat[2][2]=-fmat[2][2];
//	fmat[2][3]=-fmat[2][3];

//	glMultMatrixf(&fmat[0][0]);

	glMatrixMode(GL_MODELVIEW);	
}

//------------------------------------------------------------------

Matrix *GL_GetProjection()
{
	return &Projection;
}

//------------------------------------------------------------------

void GL_SetViewAngle(float fViewAngle)
{
	fglView=fViewAngle;
}

//------------------------------------------------------------------

void GL_Resize(i32 iWidth,i32 iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	GL_Projection(iWidth, iHeight);
}

//------------------------------------------------------------------

void GL_Init(i32 iWidth,i32 iHeight)
{
	glClearDepth(1.0f);	
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	fglView=80.f;
	// setup an inverse projection matrix
/*	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();		
	
	float fmat[4][4];

	fmat[0][0]=2.f/(iWidth-0);
	fmat[0][1]=0;
	fmat[0][2]=0;
	fmat[0][3]=0;

	fmat[1][0]=0;
	fmat[1][1]=-2.f/(Height-0);
	fmat[1][2]=0;
	fmat[1][3]=0;

	fmat[2][0]=0;
	fmat[2][1]=0;
	fmat[2][2]=-2.f/(1-(-1));
	fmat[2][3]=0;

	fmat[3][0]=-(iWidth+0)/(float)(iWidth-0);
	fmat[3][1]=(Height+0)/(float)(Height-0);
	fmat[3][2]=(1+(-1))/(1-(-1));
	fmat[3][3]=1;
	gluPerspective(50.0f,(GLfloat)iWidth/(GLfloat)Height,6.f,6000.f);
	//glMultMatrixf(&fmat[0][0]);

	glGetFloatv(GL_PROJECTION_MATRIX,&fmat[0][0]);
	glLoadIdentity();
	fmat[1][1]=-fmat[1][1];
	glMultMatrixf(&fmat[0][0]);
*/
	GL_Projection(iWidth, iHeight);

	glShadeModel(GL_SMOOTH);							
	glClearColor(0.f, 0.f, 0.0f, 0.0f);			
						
	glDepthFunc(GL_LESS);					
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_MODELVIEW);					
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	
	lpfnLockArrays = (LOCKARRAYS_PROC) wglGetProcAddress("glLockArraysEXT");

	lpfnUnlockArrays = (UNLOCKARRAYS_PROC) wglGetProcAddress("glUnlockArraysEXT");

	//set fastest gl modes
	glHint(GL_FOG_HINT,GL_FASTEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_FASTEST);

//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	
}

//------------------------------------------------------------------

void GL_SetTexture(GLuint tex)
{
	if(!tex)
	{
		iTexture=tex;
		glDisable(GL_TEXTURE_2D);
	}
	else if(tex!=iTexture)
	{
		iTexture=tex;

		glBindTexture(GL_TEXTURE_2D, iTexture);
		glEnable(GL_TEXTURE_2D);
	}
}

//------------------------------------------------------------------

void GL_RenderMode(RENDER_MODE mode)
{
	switch(mode)
	{
	case RENDER_MODE_ALPHATEXTURE:
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		break;
	case RENDER_MODE_ALPHA:
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		break;
	case RENDER_MODE_SUBTRACTTEXTURE:
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
		glEnable(GL_BLEND);
		break;
	case RENDER_MODE_TEXTURE:
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		break;
	case RENDER_MODE_FLAT:
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		break;
	case RENDER_MODE_TEXTURE_ADD:
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE,GL_ONE);
		glEnable(GL_BLEND);
		break;
	case RENDER_MODE_ADD:
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE,GL_ONE);
		glEnable(GL_BLEND);
		break;
	}
}

//------------------------------------------------------------------

void GL_PrimitiveStart(RENDER_TYPE type, u32 uFlags)
{
	iNumVerts=0;
	CurrentVert=vertices;
	rType=type;
	RENDER_Flags=uFlags;
}

//------------------------------------------------------------------

void GL_Quad(float x,float y, float z, float w, float h, u32 colour)
{
	CurrentVert->x=x;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x;
	CurrentVert->y=y+h;
	CurrentVert->z=z;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x+w;
	CurrentVert->y=y+h;
	CurrentVert->z=z;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x+w;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->colour=colour;

	CurrentVert++;

	iNumVerts+=4;

	if(rType==PRIM_TYPE_QUADLIST && (3*(iNumVerts>>1))>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}

	ASSERT(iNumVerts<MAX_VERTICES,"Too many verts!!");
}

//------------------------------------------------------------------

void GL_Quad(float x,float y, float z, float w, float h,float u1,float v1, float u2, float v2, u32 colour)
{
	CurrentVert->x=x;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->u=u1;
	CurrentVert->v=v1;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x;
	CurrentVert->y=y+h;
	CurrentVert->z=z;

	CurrentVert->u=u1;
	CurrentVert->v=v2;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x+w;
	CurrentVert->y=y+h;
	CurrentVert->z=z;

	CurrentVert->u=u2;
	CurrentVert->v=v2;

	CurrentVert->colour=colour;

	CurrentVert++;
	CurrentVert->x=x+w;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->u=u2;
	CurrentVert->v=v1;

	CurrentVert->colour=colour;

	CurrentVert++;

	iNumVerts+=4;

	if(rType==PRIM_TYPE_QUADLIST && (3*(iNumVerts>>1))>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}

	ASSERT(iNumVerts<MAX_VERTICES,"Too many verts!!");
}

//------------------------------------------------------------------

void GL_Vert(float x,float y, float z, float u1,float v1)
{
	CurrentVert->x=x;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->u=u1;
	CurrentVert->v=v1;

	CurrentVert++;
	iNumVerts++;

	if(rType==PRIM_TYPE_QUADLIST && (3*(iNumVerts>>1))>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}

	ASSERT(iNumVerts<MAX_VERTICES,"Too many verts!!");
}

//------------------------------------------------------------------

void GL_Vert(float x,float y, float z, float u1,float v1, u32 colour)
{
	CurrentVert->x=x;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->u=u1;
	CurrentVert->v=v1;

	CurrentVert->colour=colour;

	CurrentVert++;
	iNumVerts++;

	if(rType==PRIM_TYPE_QUADLIST && (3*(iNumVerts>>1))>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}

	ASSERT(iNumVerts<MAX_VERTICES,"Too many verts!!");
}

//------------------------------------------------------------------

void GL_Vert(float x,float y, float z, u32 colour)
{
	CurrentVert->x=x;
	CurrentVert->y=y;
	CurrentVert->z=z;

	CurrentVert->colour=colour;

	CurrentVert++;
	iNumVerts++;

	if(rType==PRIM_TYPE_QUADLIST && (3*(iNumVerts>>1))>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}
	else if(/*rType==PRIM_TYPE_TRIANGLELIST && */iNumVerts>=MAX_INDEX)
	{
		GL_RenderPrimitives();
	}

	ASSERT(iNumVerts<MAX_VERTICES,"Too many verts!!");
}

//------------------------------------------------------------------

void GL_RenderPrimitives(void)
{
	if	(!iNumVerts)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].x);

	if(RENDER_UVs & RENDER_Flags)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);
	}

	if(RENDER_COLOUR & RENDER_Flags)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vertices[0].colour);
	}
	else
	{
		glColor4f(1.f,1.f,1.f,1.f);
	}

	switch	(rType)
	{
	case	PRIM_TYPE_LINELIST:
		glDrawArrays(GL_LINES,0,iNumVerts);
		break;
	case	PRIM_TYPE_TRIANGLELIST:
		glDrawArrays(GL_TRIANGLES,0,iNumVerts);
		break;
	case	PRIM_TYPE_QUADLIST:
		glDrawElements(GL_TRIANGLES,3*(iNumVerts>>1),GL_UNSIGNED_SHORT,QuadIndexs);
		break;
	case	PRIM_TYPE_TRIANGLESTRIP:
		glDrawArrays(GL_TRIANGLE_STRIP,0,iNumVerts);
		break;
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
#ifdef _DEBUG
	uPolycount+=(3*(iNumVerts>>1));
#endif
	iNumVerts=0;
	CurrentVert=vertices;
}

//------------------------------------------------------------------


void GL_RenderBegin()
{
#ifdef _DEBUG
	uPolycount=0;
#endif
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);			

	glLoadIdentity();
}

//------------------------------------------------------------------

void GL_RenderVerts(RENDER_TYPE rType, u32 uFlags, Vertex *pVerts, i32 iNum)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &pVerts->x);

	if(RENDER_UVs & uFlags)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &pVerts->u);
	}

	if(RENDER_COLOUR & uFlags)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &pVerts->colour);
	}
	else
	{
		glColor4f(1.f,1.f,1.f,1.f);
	}

	switch	(rType)
	{
	case	PRIM_TYPE_LINELIST:
		glDrawArrays(GL_LINES,0,iNum);
		break;
	case	PRIM_TYPE_TRIANGLELIST:
		glDrawArrays(GL_TRIANGLES,0,iNum);
		break;
	case	PRIM_TYPE_QUADLIST:
		glDrawElements(GL_TRIANGLES,3*(iNum>>1),GL_UNSIGNED_SHORT,QuadIndexs);
		break;
	case	PRIM_TYPE_TRIANGLESTRIP:
		glDrawArrays(GL_TRIANGLE_STRIP,0,iNum);
		break;
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
#ifdef _DEBUG
	uPolycount+=(3*(iNum>>1));
#endif
}

//------------------------------------------------------------------

u32  GL_ARGB(u8 a, u8 r, u8 g, u8 b)
{
	return r | (g << 8) | (b << 16) | (a << 24);
}

//------------------------------------------------------------------

u32  GL_ARGB(i32 a, i32 r, i32 g, i32 b)
{
	return r | (g << 8) | (b << 16) | (a << 24);
}
