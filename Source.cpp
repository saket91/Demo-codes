#include <Windows.h>
#include <gl/GL.h>

#define WIN_WIDTH  800
#define WIN_HEIGHT 600


#pragma comment(lib,"opengl32.lib")

GLint gCountOfBlocks = 10;
GLfloat gblockSize = 0.008;
bool gupKey = false;

GLfloat snakePathX = gblockSize/2.0f;
GLfloat snakePathY = 0.0f;
GLfloat snakePathZ = 0.0f;

// Function for drawing filled triangle
// This function only  draws the triangle doesn't change color
void DrawSnake(GLfloat point1X, GLfloat point1Y, GLfloat point1Z, GLint countOfBlocks, GLfloat blockSize)
{
	glBegin(GL_LINE_LOOP);
	{
		for (int i = 0; i < countOfBlocks; i++)
		{
			glVertex3f(point1X + (i*blockSize), point1Y, point1Z);
		}
		
	}glEnd();
}

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
		TEXT("Testing graphics functionality"),
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

		case VK_UP:
			 gupKey = true;
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
			mi.cbSize = sizeof(MONITORINFO);

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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	//code 
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//Rendering Command

	if(gupKey)
	{
		glTranslatef(snakePathX, snakePathY, snakePathZ);
		//glRotatef(90.0f, 0.0f, 0.0f,1.0f);
		//snakePathX =0.0f; snakePathY = gblockSize/2.0f; snakePathZ = 0.0f;
		gupKey = false;
	}

	glLineWidth(3.0f);

	DrawSnake(-0.5f, -0.5f, 0.0f,gCountOfBlocks, gblockSize);

	
	//glFlush(); -- Commented single buffer api
	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	//code 
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 1000);*/
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