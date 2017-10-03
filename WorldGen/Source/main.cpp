#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

typedef char				i8;
typedef short				i16;
typedef int					i32;
typedef long long			i64;

typedef float				f32;
typedef double				f64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

bool global_isRunning = false;
HGLRC global_openGLRenderContext;

LRESULT CALLBACK MainWinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_CREATE:
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//Flags
			PFD_TYPE_RGBA, // PIXEL FORMAT
			32, //Color depth of framebuffer
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24, //Number of bits in depthbuffer
			8, // Number of bits in stencil buffer
			0, // Number of Aux bits for framebuffer
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC deviceContext = GetDC(window);
		i32 preferedPixelFormat = 0;
		preferedPixelFormat = ChoosePixelFormat(deviceContext, &pfd);
		SetPixelFormat(deviceContext, preferedPixelFormat, &pfd);

		global_openGLRenderContext = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, global_openGLRenderContext);

		MessageBox(window, (i8*)glGetString(GL_VERSION), "OpenGL Version", MB_OK);

	}break;

	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct = {};
		HDC deviceContext = BeginPaint(window, &paintStruct);

		EndPaint(window, &paintStruct);
	}break;

	case WM_CLOSE:
	case WM_DESTROY:
	{
		wglDeleteContext(global_openGLRenderContext);

		PostQuitMessage(0);
		global_isRunning = false;
	}break;

	default:
		result = DefWindowProcA(window, message, wParam, lParam);
	}

	return result;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLn, int cmdShow)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance = hInst;
	windowClass.lpfnWndProc = (WNDPROC)MainWinProc;
	windowClass.lpszClassName = "WorldGen";
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.hCursor = LoadCursor(hInst, IDC_ARROW);

	if (RegisterClass(&windowClass))
	{
		HWND window = CreateWindow(windowClass.lpszClassName, windowClass.lpszClassName,
									WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT, CW_USEDEFAULT,
									800, 600,
									0, 0, hInst, 0);
		if (window)
		{
			UpdateWindow(window);
			ShowWindow(window, SW_SHOW);

			global_isRunning = true;

			while (global_isRunning)
			{
				MSG msg = {};
				while (PeekMessage(&msg, window, 0, 0, PM_REMOVE) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				glViewport(0, 0, 800, 600);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.25f, 0.5f, 1.0f);

				SwapBuffers(GetDC(window));
			}
		}
		DestroyWindow(window);
	}

	
	UnregisterClassA(windowClass.lpszClassName, hInst);
	
	return 0;
}