#include "Math/vec3.h"
#include <iostream>
#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

#define Align16(value) ((value + 15) & ~15)
#define Kilobyte(value) (value * 1024)
#define Megabyte(value) (Kilobyte(value) * 1024)
#define Gigabyte(value) (Megabyte(value) * 1024)

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))
#define Maximum(a, b) (a > b ? a : b)
#define Minimum(a, b) (a < b ? a : b)

bool global_isRunning = false;
HGLRC global_openGLRenderContext;
i32 global_width = 1920 * 0.5f;
i32 global_height = 1080 * 0.5f;

struct win32_memory
{
	mem_size persistentStorageSize;
	mem_size transientStorageSize;
	mem_size totalStorageSize;
	void* persistentStorage;
	void* transientStorage;
};

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

f32 dt = 0.016f;
f32 angle = 0;

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
									global_width, global_height,
									0, 0, hInst, 0);
		if (window)
		{
			UpdateWindow(window);
			ShowWindow(window, SW_SHOW);

			global_isRunning = true;

			win32_memory memory = {};
			memory.persistentStorageSize = Megabyte(64);
			memory.transientStorageSize = Megabyte(64);
			memory.totalStorageSize = memory.persistentStorageSize + memory.transientStorageSize;
			
			memory.persistentStorage = VirtualAlloc(0, memory.persistentStorageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			memory.transientStorage = VirtualAlloc(0, memory.transientStorageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			if (memory.persistentStorage && memory.transientStorage)
			{
				while (global_isRunning)
				{
					MSG msg = {};
					while (PeekMessage(&msg, window, 0, 0, PM_REMOVE) > 0)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}

					glViewport(0, 0, global_width, global_height);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glClearColor(0.0f, 0.25f, 0.5f, 1.0f);

					f32 focalLen = 0.60f;
					f32 aspect = (f32)(global_width / global_height);
					f32 n = 0.1f;
					f32 f = 1000.0f;

					f32 a = 1.0f;
					f32 b = aspect;
					f32 c = focalLen;

					f32 d = (n + f) / (n - f);
					f32 e = (2 * f*n) / (n - f);

					f32 proj[] =
					{
						a*c,	  0,	0,	0,
						  0,	b*c,	0,	0,
						  0,	  0,	d, -1,
						  0,	  0,	e,	0,
					};

					angle += dt * 10.0f;

					glLoadIdentity();
					glMatrixMode(GL_PROJECTION);
					glLoadMatrixf(&proj[0]);

					glMatrixMode(GL_MODELVIEW);

					glTranslatef(-5, -5, -10);

					glBegin(GL_QUADS);
					{
						for (u32 y = 0; y < 10; y++)
						{
							for (u32 x = 0; x < 10; x++)
							{
								vec3 p1 = Vec3((f32)x, 1.0f, (f32)y);
								vec3 p2 = Vec3((f32)x, 1.0f, (f32)y + 1);
								vec3 p3 = Vec3((f32)x + 1, 1.0f, (f32)y + 1);
								vec3 p4 = Vec3((f32)x + 1, 1.0f, (f32)y);

								glVertex3fv(p1.E);
								glColor3f((f32)x, 0, 0);

								glVertex3fv(p2.E);
								glColor3f(0, (f32)y, 0);

								glVertex3fv(p3.E);
								glColor3f(0, (f32)y, 1);

								glVertex3fv(p4.E);
								glColor3f(0, 0, 1);
							}
						}
					}
					glEnd();

					SwapBuffers(GetDC(window));
				}

				VirtualFree(memory.persistentStorage, 0, MEM_RELEASE);
				memory.persistentStorage = 0;

				VirtualFree(memory.transientStorage, 0, MEM_RELEASE);
				memory.transientStorage = 0;
			}
			else
			{
				// LOG
				MessageBox(window, "Unable to allocate memory, please check to see if you have enough.", "Error", MB_OK | MB_ICONERROR);
			}
		}
		else
		{
			MessageBox(window, "Unable to create window.", "Error", MB_OK | MB_ICONERROR);
		}

		DestroyWindow(window);
	}

	UnregisterClassA(windowClass.lpszClassName, hInst);
	
	return 0;
}