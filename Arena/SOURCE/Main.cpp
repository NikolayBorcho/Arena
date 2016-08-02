//
// main.cpp
// craig
//

#include <windows.h>								// Header File For Windows
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>	
#include <time.h>
#include "includeall.h"
#include "opengl.h"
#include "mainloop.h"
#include "input.h"
#include "font.h"
#include "TgaLoad.h"
#include "Object.h"
#include "mesh.h"
#include "Particles.h"

HGLRC		hRC=NULL;							// Permanent Rendering Context
HDC			hDC=NULL;							// Private GDI Device Context
HWND		hWnd=NULL;							// Holds Our Window Handle
HINSTANCE	hInstance;	

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static float fLastTime=0.f;
static float fMaxTime=(1.f/10.f)*1000.f;
static i32 iRenderFrame;
static i32 iFPS;
static float fSecond;
static char	fullscreen=FALSE;

float MathLib::cosc[DEGREE+1];
float MathLib::sinc[DEGREE+1];

MathLib g_MathLib;

#ifdef _DEBUG
Font *pDebugFont;
#endif

static u8 Keys[256];

#ifndef VK_A
#define  VK_A 0x41 
#endif
#ifndef VK_D
#define  VK_D 0x44 
#endif
#ifndef VK_W
#define  VK_W 0x57 
#endif
#ifndef VK_S
#define  VK_S 0x53 
#endif
#ifndef VK_Q
#define  VK_Q 0x51 
#endif
#ifndef VK_E
#define  VK_E 0x45 
#endif
static KeyMap MainKeys[BUTTON_MAX]=
{
	{ BUTTON_FIRE1,		&Keys[VK_DOWN] },
	{ BUTTON_FIRE2,		&Keys[VK_UP] },
	{ BUTTON_CAMRIGHT,	&Keys[VK_RIGHT] },
	{ BUTTON_CAMLEFT,	&Keys[VK_LEFT] },
	{ BUTTON_LEFT,		&Keys[VK_A] },
	{ BUTTON_RIGHT,		&Keys[VK_D] },
	{ BUTTON_UP,		&Keys[VK_W] },
	{ BUTTON_DOWN,		&Keys[VK_S] },
	{ BUTTON_ESC,		&Keys[VK_ESCAPE] },
	{ BUTTON_SPACE,		&Keys[VK_SPACE] },
	{ BUTTON_LSHOULDER,	&Keys[VK_Q] },
	{ BUTTON_RSHOULDER,	&Keys[VK_E] },

	{ PEDITOR_OPEN,		&Keys[VK_NUMPAD0] },
	{ PEDITOR_CLOSE,	&Keys[VK_DECIMAL] },
	{ PEDITOR_NEXTVAL,	&Keys[VK_NUMPAD2] },
	{ PEDITOR_PREVVAL,	&Keys[VK_NUMPAD8] },
	{ PEDITOR_INCVAL,	&Keys[VK_NUMPAD6] },
	{ PEDITOR_DECVAL,	&Keys[VK_NUMPAD4] },
	{ PEDITOR_NEXT,		&Keys[VK_NEXT] },
	{ PEDITOR_PREV,		&Keys[VK_PRIOR] },
	{ PEDITOR_INS,		&Keys[VK_ADD] },
	{ PEDITOR_DEL,		&Keys[VK_SUBTRACT] },
	{ PEDITOR_EFFECT,	&Keys[VK_RETURN] },
	{ PEDITOR_DISCARD,	&Keys[VK_NUMPAD5] },
};

//------------------------------------------------------------------
struct
{
	__int64		frequency;					// Timer Frequency
	float		resolution;					// Timer Resolution
	unsigned long	mm_timer_start;					// Multimedia Timer Start Value
	unsigned long	mm_timer_elapsed;				// Multimedia Timer Elapsed Time
	BOOL		performance_timer;				// Using The Performance Timer?
	__int64		performance_timer_start;			// Performance Timer Start Value
	__int64		performance_timer_elapsed;			// Performance Timer Elapsed Time
} timer;

void TimerInit(void)
{
	memset(&timer, 0, sizeof(timer));				// Clear Our Timer Structure
	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &timer.frequency))
	{
		// No Performace Counter Available
		timer.performance_timer = FALSE;			// Set Performance Timer To FALSE
		timer.mm_timer_start = timeGetTime();			// Use timeGetTime() To Get Current Time
		timer.resolution = 1.0f/1000.0f;			// Set Our Timer Resolution To .001f
		timer.frequency = 1000;					// Set Our Timer Frequency To 1000
		timer.mm_timer_elapsed = timer.mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &timer.performance_timer_start);
		timer.performance_timer = TRUE;				// Set Performance Timer To TRUE
		
		// Calculate The Timer Resolution Using The Timer Frequency
		timer.resolution = (float) (((double)1.0f)/((double)timer.frequency));
		// Set The Elapsed Time To The Current Time
		timer.performance_timer_elapsed = timer.performance_timer_start;
	}
}

//------------------------------------------------------------------

float TimerGetTime()
{
	__int64 time;							// time Will Hold A 64 Bit Integer
	if (timer.performance_timer)					// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - timer.performance_timer_start) * timer.resolution)*1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - timer.mm_timer_start) * timer.resolution)*1000.0f;
	}
}

//------------------------------------------------------------------

void KillGLWindow(void)					
{
	if (fullscreen)	
	{
		ChangeDisplaySettings(NULL,0);				
		ShowCursor(TRUE);					
	}

	if (hRC){
		
		if (!wglMakeCurrent(NULL,NULL))	{
			
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		
		if (!wglDeleteContext(hRC))	{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;						
	}
	
	if (hDC && !ReleaseDC(hWnd,hDC)){
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;						
	}
	
	if (hWnd && !DestroyWindow(hWnd)){
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;						
	}
	
	if (!UnregisterClass("OpenGL",hInstance)){
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;								
	}
}

//------------------------------------------------------------------

BOOL CreateGLWindow(char* title)
{
	GLuint		PixelFormat;						// Holds The Results After Searching For A Match
	WNDCLASS	wc;							// Windows Class Structure
	DWORD		dwExStyle;						// Window Extended Style
	DWORD		dwStyle;

	PIXELFORMATDESCRIPTOR pfd;

	hInstance			= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;						// No Extra Window Data
	wc.cbWndExtra		= 0;						// No Extra Window Data
	wc.hInstance		= hInstance;					// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;						// No Background Required For GL
	wc.lpszMenuName		= NULL;						// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";					// Set The Class Name
	
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Exit And Return FALSE
	}

#ifndef _DEBUG
	if(MessageBox(NULL,"Run in fullscreen?","Screen Mode",MB_YESNO)==IDYES)
	{
		fullscreen=1;
	}
#endif

	memset(&pfd,0,sizeof(pfd));
	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion=1; 
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER; 
    pfd.iPixelType=PFD_TYPE_RGBA; 
    pfd.cColorBits=32; 
    pfd.cRedBits=0; 
    pfd.cRedShift=0; 
    pfd.cGreenBits=0; 
    pfd.cGreenShift=0; 
    pfd.cBlueBits=0; 
    pfd.cBlueShift=0; 
    pfd.cAlphaBits=0; 
    pfd.cAlphaShift=0; 
    pfd.cAccumBits=0; 
    pfd.cAccumRedBits=0; 
    pfd.cAccumGreenBits=0; 
    pfd.cAccumBlueBits=0; 
    pfd.cAccumAlphaBits=0; 
    pfd.cDepthBits=8; 
    pfd.cStencilBits=0; 
    pfd.cAuxBuffers=0; 
    pfd.iLayerType=PFD_MAIN_PLANE; 
    pfd.bReserved=0; 
    pfd.dwLayerMask=0; 
    pfd.dwVisibleMask=0; 
    pfd.dwDamageMask=0;

	if (fullscreen)								// Attempt Fullscreen Mode?
	{		
		DEVMODE dmScreenSettings;					// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize			=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= 640;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= 480;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		
		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			
			// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;				// Select Windowed Mode (Fullscreen=FALSE)
			}else{
				
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;					// Exit And Return FALSE
			}
		}
	}

	if (fullscreen)	
	{
		dwExStyle=WS_EX_APPWINDOW;					// Window Extended Style
		dwStyle=WS_POPUP;						// Windows Style
		ShowCursor(FALSE);						// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;					// Windows Style
	}

	if (!(hWnd=CreateWindowEx(	dwExStyle/*WS_EX_APPWINDOW | WS_EX_WINDOWEDGE*/,				// Extended Style For The Window
		"OpenGL",				// Class Name
		title,					// Window Title
		dwStyle/*WS_CLIPSIBLINGS |			// Required Window Style
		WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW*/,				// Selected Window Style
		0, 0,					// Window Position
		640,	// Calculate Adjusted Window Width
		480,	// Calculate Adjusted Window Height
		NULL,					// No Parent Window
		NULL,					// No Menu
		hInstance,				// Instance
		NULL)))	
		
	{
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(hDC=GetDC(hWnd)))	{
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	{
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}
//	ChoosePixelFormat(HDC, CONST PIXELFORMATDESCRIPTOR *);

	
	if(!SetPixelFormat(hDC,PixelFormat,&pfd)){
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}
	
	if (!(hRC=wglCreateContext(hDC)))	{
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}
	
	if(!wglMakeCurrent(hDC,hRC)){
		KillGLWindow();							// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}
	
	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);								// Sets Keyboard Focus To The Window

	GL_Init(SWIDTH,SHEIGHT);

	return TRUE;
}

//------------------------------------------------------------------

bool Realtime()
{
	float fTime=TimerGetTime();
	bool bDone=false;

	while(fTime-fLastTime>fMaxTime)
	{
		Object_IncUpdateFrame();
		bDone=MainLoop_Update();

		fSecond+=fMaxTime;
		fLastTime+=fMaxTime;

		// dont go too far
		if(fTime-fLastTime>1000)
		{
			fLastTime=fTime-1000;
		}
	}
	Object_RenderFrac = (fTime-fLastTime)/fMaxTime;
	if(Object_RenderFrac > 1.f)
		Object_RenderFrac = 1.f;

	Object_IncRenderFrame();
	GL_RenderBegin();

	MainLoop_Render();

#ifdef _DEBUG
	char cBuff[256];

	iRenderFrame++;

	if(fSecond>1000)
	{
		iFPS=iRenderFrame;
		iRenderFrame=0;
		fSecond=0.f;
	}
	sprintf(cBuff,"Polys: %d\nMemUsage: %db\nFPS: %d",uPolycount,Memory_GetUsage(),iFPS);
	SetFont(FONT_RIGHT,pDebugFont);
	PrintString(600,400,0xffffffff,1.f,cBuff);
#endif

	SwapBuffers(hDC);

	return bDone;
}

//------------------------------------------------------------------

int WINAPI WinMain(	HINSTANCE	hInstance,				// Instance
				   HINSTANCE	hPrevInstance,				// Previous Instance
				   LPSTR		lpCmdLine,				// Command Line Parameters
				   int		nCmdShow)				// Window Show State
{
	MSG	msg;
	bool done=false;

	// Create Our OpenGL Window
	if (!CreateGLWindow("Arena"))
	{
		return 0;							// Quit If Window Was Not Created
	}

	Memory_Init();
	TgaLoad_Init();
	Mesh_Init();

	TimerInit();

	u32 uTime=(unsigned)time( NULL );

	srand((i32)(10000000.f*TimerGetTime()));
	
	iRenderFrame=0;
	fSecond=0.f;
	Input_Init(MainKeys);

	ASSERT(_CrtCheckMemory(),"bad memory");
#ifdef _DEBUG
	char cBuff[256];
	char *pBuff;

	strcpy(cBuff,lpCmdLine);
	pBuff=strtok(cBuff," ");
	while(pBuff)
	{
		if(strstr(pBuff,"nomusic"))
		{
//			AudioStream_DontPlay();
		}
		pBuff=strtok(NULL," ");
	}
#endif

	ASSERT(_CrtCheckMemory(),"bad memory");

	MainLoop_Init();
	
ASSERT(_CrtCheckMemory(),"bad memory");
#ifdef _DEBUG
	pDebugFont=Font_Load("data/fonts/debug");
#endif

	fLastTime=-99999999.f;
	while(!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				done=TRUE;					// If So done=TRUE
			}
			else
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else
		{
			//run a frame
			done=Realtime();
		}
	}

	MainLoop_Exit();
#ifdef _DEBUG
	Font_Destroy(pDebugFont);
#endif
	TgaLoad_Exit();
	Mesh_Exit();

	KillGLWindow();

	Memory_Exit();

	return (int)(msg.wParam);
}

//------------------------------------------------------------------

LRESULT CALLBACK WndProc(	HWND	hWnd,					// Handle For This Window
						 UINT	uMsg,					// Message For This Window
						 WPARAM	wParam,					// Additional Message Information
						 LPARAM	lParam)					// Additional Message Information
{
	switch(uMsg)
	{
	case WM_SIZE:
		{          
			WORD    wWidth = LOWORD(lParam);    //width of rectangle   
			WORD    wHeight = HIWORD(lParam);   //height of rectangle  

			GL_Resize(wWidth,wHeight);
		}
		break;
	case WM_CLOSE:							// Did We Receive A Close Message?
		PostQuitMessage(0);					// Send A Quit Message
		return 0;
	case WM_KEYDOWN:						
			Keys[wParam] = TRUE;	
			return 0;				
		
	case WM_KEYUP:						
			Keys[wParam] = FALSE;		
			return 0;
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam)) 
		{	
//			Network_Shutdown();
			return 0;
		}
		switch (WSAGETSELECTEVENT(lParam)) 
		{		// Differentiate between the events
		case FD_READ:
			//Network_Recieve();
			break;
		case FD_WRITE:
			// Write data
			break;
		case FD_CONNECT:
			// connect to server
			//Network_Connected();
			break;
		case FD_ACCEPT:
			//Network_AcceptClient();
			break;
		}
		break;
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//------------------------------------------------------------------

u32 StringKey( const char *pString )
{
	u32 uKey=0;

	while(pString)
	{
		uKey+=*pString;
		uKey+=255;
		pString++;
	}

	return uKey;
}

//float random()
//{
//	return ((float)rand()/(float)RAND_MAX);
//}
