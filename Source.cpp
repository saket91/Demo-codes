#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#define _USE_MATH_DEFINES
#include "math.h"

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")


//Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable decleration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
float gAngle = 0.0f;

//Declaring Matrices here
GLfloat IdentityMatrix [16];
GLfloat TranslationMatrix [16];
GLfloat ScaleMatrix [16];

GLfloat RotateMatrixX [16];
GLfloat RotateMatrixY [16];
GLfloat RotateMatrixZ [16];

//Draw Function
void Update()
{
	gAngle += 1.0f; //increament in degree

	if (gAngle > 360)
	{
		gAngle = 0.0f;
	}
}

void DrawCube()
{
	glBegin(GL_QUADS);
	{
		//Front Face
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f); //top Left front
		glVertex3f(-1.0f, -1.0f, 1.0f); //bottom Left front
		glVertex3f(1.0f, -1.0f, 1.0f); //bottom Right front
		glVertex3f(1.0f, 1.0f, 1.0f); //top Right front

        //Back Face -- make Z negative of front face
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f); 
		glVertex3f(-1.0f, -1.0f, -1.0f); 
		glVertex3f(1.0f, -1.0f, -1.0f); 
		glVertex3f(1.0f, 1.0f, -1.0f); 

		//Left Face
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f); //top Left back
		glVertex3f(-1.0f, -1.0f, -1.0f); //bottom Left back
		glVertex3f(-1.0f, -1.0f, 1.0f); //bottom Left front
		glVertex3f(-1.0f, 1.0f, 1.0f); //top Left front
	     
        //Right Face
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f); 
		glVertex3f(1.0f, -1.0f, -1.0f); 
		glVertex3f(1.0f, -1.0f, 1.0f); 
		glVertex3f(1.0f, 1.0f, 1.0f); 
		
		//Top Face
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f); //top Right front
		glVertex3f(1.0f, 1.0f, -1.0f); //top Right back
		glVertex3f(-1.0f, 1.0f, -1.0f); //top Left back
		glVertex3f(-1.0f, 1.0f, 1.0f); //top Left front
		
		//Bottom Face
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 1.0f); 
		glVertex3f(1.0f, -1.0f, -1.0f); 
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f); 
		
	}glEnd();
}

//Main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);

	//Variable decleration

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;

	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;

	//Code
	//Initilizing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	//Register Class
	RegisterClassEx(&wndclass);

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Fixed Function Pipeline using Native Windowing "),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		600,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	//initialize
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Message Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				Update();
				display();
			}
		}
	}

	uninitialize();

	return ((int)msg.wParam);
}

//Wnd Proc()

LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lparam)
{
	//Function prototype
	void display(void);
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//code 
	switch (imsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

		/*case WM_PAINT:
		display();
		break;
		return(0);*/

		/*case WM_ERASEBKGND:
		return(0);*/

	case WM_SIZE:
		resize(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;

		case 0x46: // F
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, imsg, wParam, lparam));
}
void ToggleFullScreen(void)
{
	//Variable Declerations
	MONITORINFO mi;

	//code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize =  sizeof(MONITORINFO) ;

			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(false);
	}
	else
	{
		//code 
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	//Initialize the matrices
	IdentityMatrix [0] = 1.0f;
	IdentityMatrix [1] = 0.0f;
	IdentityMatrix [2] = 0.0f;
	IdentityMatrix [3] = 0.0f;

	IdentityMatrix [4] = 0.0f;
	IdentityMatrix [5] = 1.0f;
	IdentityMatrix [6] = 0.0f;
	IdentityMatrix [7] = 0.0f;

	IdentityMatrix [8] = 0.0f;
	IdentityMatrix [9] = 0.0f;
	IdentityMatrix [10]= 1.0f;
	IdentityMatrix [11]= 0.0f;

	IdentityMatrix [12]= 0.0f;
	IdentityMatrix [13]= 0.0f;
	IdentityMatrix [14]= 0.0f;
	IdentityMatrix [15]= 1.0f;


    ScaleMatrix [0] = 0.75f;
	ScaleMatrix [1] = 0.0f;
	ScaleMatrix [2] = 0.0f;
	ScaleMatrix [3] = 0.0f;

	ScaleMatrix [4] = 0.0f;
	ScaleMatrix [5] = 0.75f;
	ScaleMatrix [6] = 0.0f;
	ScaleMatrix [7] = 0.0f;
					   
	ScaleMatrix [8] = 0.0f;
	ScaleMatrix [9] = 0.0f;
	ScaleMatrix [10]= 0.75f;
	ScaleMatrix [11]= 0.0f;
					   
	ScaleMatrix [12]= 0.0f;
	ScaleMatrix [13]= 0.0f;
	ScaleMatrix [14]= 0.0f;
	ScaleMatrix [15]= 1.0f;

	TranslationMatrix [0] = 1.0f;
	TranslationMatrix [1] = 0.0f;
	TranslationMatrix [2] = 0.0f;
	TranslationMatrix [3] = 0.0f;
							 
	TranslationMatrix [4] = 0.0f;
	TranslationMatrix [5] = 1.0f;
	TranslationMatrix [6] = 0.0f;
	TranslationMatrix [7] = 0.0f;
							 
	TranslationMatrix [8] = 0.0f;
	TranslationMatrix [9] = 0.0f;
	TranslationMatrix [10]= 1.0f;
	TranslationMatrix [11]= 0.0f;
							 
	TranslationMatrix [12]= 0.0f;
	TranslationMatrix [13]= 0.0f;
	TranslationMatrix [14]= -6.0f;
	TranslationMatrix [15]= 1.0f;

	//function prototypes
	void resize(int, int);

	//variable decleration
	PIXELFORMATDESCRIPTOR pfd;

	int ipixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialize
	pfd.nSize = sizeof(PPIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32; //Changes for 3D window

	ghdc = GetDC(ghwnd);

	ipixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (ipixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, ipixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	GLfloat angleRadian = gAngle *(M_PI /180);

	RotateMatrixX [0] = 1.0f;
	RotateMatrixX [1] = 0.0f;
	RotateMatrixX [2] = 0.0f;
	RotateMatrixX [3] = 0.0f;
		 
	RotateMatrixX [4] = 0.0f;
	RotateMatrixX [5] = cos(angleRadian);
	RotateMatrixX [6] = sin(angleRadian);
	RotateMatrixX [7] = 0.0f;
		 
	RotateMatrixX [8] = 0.0f;
	RotateMatrixX [9] = -sin(angleRadian);
	RotateMatrixX [10]= cos(angleRadian);
	RotateMatrixX [11]= 0.0f;
	
	RotateMatrixX [12]= 0.0f;
	RotateMatrixX [13]= 0.0f;
	RotateMatrixX [14]= 0.0f;
	RotateMatrixX [15]= 1.0f;
	
	
	RotateMatrixY [0] = cos(angleRadian);
	RotateMatrixY [1] = 0.0f;
	RotateMatrixY [2] = sin(angleRadian);
	RotateMatrixY [3] = 0.0f;
	 		
	RotateMatrixY [4] = 0.0f;
	RotateMatrixY [5] = 1.0f;
	RotateMatrixY [6] = 0.0f;
	RotateMatrixY [7] = 0.0f;
		 		
	RotateMatrixY [8] = -sin(angleRadian);
	RotateMatrixY [9] = 0.0f;
	RotateMatrixY [10]= cos(angleRadian);
	RotateMatrixY [11]= 0.0f;
			 	
	RotateMatrixY [12]= 0.0f;
	RotateMatrixY [13]= 0.0f;
	RotateMatrixY [14]= 0.0f;
	RotateMatrixY [15]= 1.0f;
	
	RotateMatrixZ [0] = cos(angleRadian);
	RotateMatrixZ [1] = sin(angleRadian);
	RotateMatrixZ [2] = 0.0f;
	RotateMatrixZ [3] = 0.0f;
			 	
	RotateMatrixZ [4] = -sin(angleRadian);
	RotateMatrixZ [5] = cos(angleRadian);
	RotateMatrixZ [6] = 0.0f;
	RotateMatrixZ [7] = 0.0f;
		 
	RotateMatrixZ [8] = 0.0f;
	RotateMatrixZ [9] = 0.0f;
	RotateMatrixZ [10]= 1.0f;
	RotateMatrixZ [11]= 0.0f;
	 
	RotateMatrixZ [12]= 0.0f;
	RotateMatrixZ [13]= 0.0f;
	RotateMatrixZ [14]= 0.0f;
	RotateMatrixZ [15]= 1.0f;


	//code 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rendering Command
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(IdentityMatrix);   //glLoadIdentity();

	glMultMatrixf(TranslationMatrix); //glTranslatef(0.0f, 0.0f, -6.0f);
	  
	glMultMatrixf(ScaleMatrix);

	glMultMatrixf(RotateMatrixX); //glRotatef(gAngle, 1, 1, 1);
	glMultMatrixf(RotateMatrixY); //glRotatef(gAngle, 1, 1, 1);
	glMultMatrixf(RotateMatrixZ); //glRotatef(gAngle, 1, 1, 1);
	DrawCube();

	//glFlush(); -- Commented single buffer api
	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	//code 
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ((GLfloat)width / (GLfloat)height), 0.1, 100.0);
}

void uninitialize()
{
	//uninitialize code

	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE |
			SWP_NOOWNERZORDER |
			SWP_NOZORDER |
			SWP_NOSIZE
			| SWP_FRAMECHANGED);

		ShowCursor(true);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);

	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
}