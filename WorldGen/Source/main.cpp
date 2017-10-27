
#define STB_PERLIN_IMPLEMENTATION 1

#include "Math/vec3.h"
#include "Math/stb_perlin.h"
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

bool global_isRunning = false;
HGLRC global_openGLRenderContext;
i8* global_windowText = "Procedural World Gen\0";
i32 global_width = 1920*0.7f;
i32 global_height = 1080*0.7f;
WINDOWPLACEMENT global_previousPlacement;

struct win32_memory
{
	mem_size persistentStorageSize;
	mem_size transientStorageSize;
	mem_size totalStorageSize;
	void* persistentStorage;
	void* transientStorage;
};

struct win32_dimension
{
	u32 width;
	u32 height;
};

void Win32ToggleFullscreen(HWND Window)
{
	DWORD dwStyle = GetWindowLong(Window, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		MONITORINFO MonitorInfo = { sizeof(MonitorInfo) };
		if (GetWindowPlacement(Window, &global_previousPlacement) &&
			GetMonitorInfo(MonitorFromWindow(Window,
				MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
		{
			SetWindowLong(Window, GWL_STYLE,
				dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(Window, HWND_TOP,
				MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
				MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
				MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else 
	{
		SetWindowLong(Window, GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(Window, &global_previousPlacement);
		SetWindowPos(Window, 0, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

HDC Win32SetPixelFormat(HWND window)
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

	return deviceContext;
}

LRESULT CALLBACK MainWinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_CREATE:
	{
		HDC deviceContext = Win32SetPixelFormat(window);
		global_openGLRenderContext = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, global_openGLRenderContext);

		ReleaseDC(window, deviceContext);

		i8* openGLVersion = (i8*)glGetString(GL_VERSION);
		mem_size bufferSize = (sizeof(i8) * strlen(openGLVersion)) + (sizeof(i8*)*strlen(global_windowText)) + 2;
		void* bufferMem = malloc(bufferSize);
		if (bufferMem)
		{
			ZeroMemory(bufferMem, bufferSize);
			i8* buffer = (i8*)bufferMem;
			buffer = strcat(buffer, global_windowText);
			buffer = strcat(buffer, " ");
			buffer = strcat(buffer, openGLVersion);

			SetWindowText(window, buffer);

			free(bufferMem);
		}
		else
		{
			SetWindowText(window, global_windowText);
		}
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

#define terrain_width 200
#define terrain_height 200
#define terrain_cell_count 150
#define terrain_min_height 1.0f
#define terrain_max_height 200.0f

f32 terrainHeights[terrain_cell_count+1][terrain_cell_count+1];
vec3 terrain_verts[terrain_cell_count + 1][terrain_cell_count + 1];

f32 terrain_highest_peak = F32Min;

vec3 CameraP = Vec3(0.0f, 40.0f, 0.0f);

f32 CameraPitch = 0;// 0.025f*Pi32;
f32 CameraOrbit = 0;
f32 CameraDolly = 0;

i32 current_cell_count = 0;

f32 timer = 0.0f;
f32 delay = 0.5f;

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

			u32 Octaves = 16;
			f32 Persistance = 0.5f;
			f32 Lacunarity = 2;
			f32 NoiseScale = 50;

			for (u32 y = 0; y < terrain_cell_count + 1; y++)
			{
				for (u32 x = 0; x < terrain_cell_count + 1; x++)
				{
					f32 Freq = 1;
					f32 Amp = 1;

					f32 XOff = (f32)(x) / NoiseScale;
					f32 YOff = (f32)(y) / NoiseScale;

					f32 Sample = stb_perlin_fbm_noise3(XOff, YOff, 1.0f, Lacunarity, Persistance, Octaves, 10, 10, 10);

					XOff = (x) / NoiseScale * 0.5f;
					YOff = (y) / NoiseScale * 0.5f;

					Sample += stb_perlin_fbm_noise3(XOff, YOff, 1.0f, Lacunarity, Persistance/2, Octaves/2, 10/2, 10/2, 10/2);
					//Sample *= 50.0f;
					Sample = Maximum(Sample * 50, terrain_min_height);

					if (Sample > terrain_highest_peak)
						terrain_highest_peak = Sample;

					terrainHeights[y][x] = Sample;
				}
			}

			for (u32 y = 0; y < terrain_cell_count; y++)
			{
				for (u32 x = 0; x < terrain_cell_count + 1; x++)
				{
					f32 Sample1 = terrainHeights[y][x];
					f32 Sample2 = terrainHeights[y + 1][x];
					
					vec3 p1 = Vec3((f32)x*(terrain_width / terrain_cell_count), Sample1, (f32)y*(terrain_height / terrain_cell_count));
					vec3 p2 = Vec3((f32)x*(terrain_width / terrain_cell_count), Sample2, ((f32)y + 1) * (terrain_height / terrain_cell_count));

					terrain_verts[y][x] = p1;
					terrain_verts[y + 1][x] = p2;
				}
			}

			if (memory.persistentStorage && memory.transientStorage)
			{
				while (global_isRunning)
				{
					MSG msg = {};
					while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0)
					{
						switch (msg.message)
						{
						case WM_SYSKEYUP:
						case WM_SYSKEYDOWN:
						case WM_KEYUP:
						case WM_KEYDOWN:
						{
							u32 keyCode = (u32)msg.wParam;
							b32 isDown = ((msg.lParam & (1 << 31)) == 0);
							b32 wasDown = ((msg.lParam & (1 << 30)) != 0);
							b32 AltKeyWasDown = (msg.lParam & (1 << 29));

							if (isDown != wasDown)
							{
								if (keyCode == 'w')
								{
									MessageBox(msg.hwnd, "You have pressed the W key", "KeyDown", 0);
								}

								if (isDown)
								{
									if (AltKeyWasDown && keyCode == VK_RETURN)
									{
										if (msg.hwnd)
										{
											Win32ToggleFullscreen(msg.hwnd);
										}
									}
								}
							}
						}break;

						default:
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}break;
						}
					}

					win32_dimension windowDims = {};

					RECT clientRect = {};
					GetClientRect(window, &clientRect);
					windowDims.width = clientRect.right - clientRect.left;
					windowDims.height = clientRect.bottom - clientRect.top;

					glDepthMask(GL_TRUE);
					glDepthFunc(GL_LEQUAL);
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_BLEND);
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

					glEnable(GL_LIGHT0);
					
					glViewport(0, 0, windowDims.width, windowDims.height);

					glClearDepth(1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glClearColor(0.0f, 0.25f, 0.5f, 1.0f);

					f32 focalLen = 0.60f;
					f32 aspect = (f32)(windowDims.width / windowDims.height);
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

					angle += dt * 5.0f;

					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();

					glMatrixMode(GL_PROJECTION);
					f32 WithOverHeight = (f32)windowDims.width/(f32)windowDims.height;
					f32 FocalLength = 0.6f;
					mat4x4 Proj = PerspectiveProjection(WithOverHeight, FocalLength);

					//CameraPitch = 0.075f*Pi32;
					//CameraOrbit += dt;
					//CameraDolly -= 2.0f*dt;

					mat4x4 CameraObject = RotateY(CameraOrbit);// RotateZ(CameraOrbit)*RotateX(CameraPitch);
					vec3 CameraX = GetColumn(CameraObject, 0);
					vec3 CameraY = GetColumn(CameraObject, 1);
					vec3 CameraZ = GetColumn(CameraObject, 2);
					vec3 CameraOt = CameraObject*(CameraP + Vec3(0, 0, CameraDolly));
					mat4x4 CameraT = CameraTransform(CameraX, CameraY, CameraZ, CameraOt);

					glMatrixMode(GL_PROJECTION);

					Proj = Transpose(Proj * CameraT);
					glLoadMatrixf(&Proj.E[0][0]);

					//glRotatef(180, 0.0f, 1.0f, 0);
					glTranslatef(-(terrain_width*0.5f), 0, -(terrain_height*0.5f));
					
					for (u32 y = 0; y < current_cell_count; y++)
					{
						glBegin(GL_TRIANGLE_STRIP);
						for (u32 x = 0; x < current_cell_count + 1; x++)
						{
							vec3 p1 = terrain_verts[y][x];
							vec3 p2 = terrain_verts[y + 1][x];

							vec3 c1 = Vec3(p1.y);
							vec3 c2 = Vec3(p2.y);

							if (p1.y <= 0.0f)
								c1 = Vec3(0, 0.2f, 1.0f);
							if (p1.y > 0.0f && p1.y < terrain_highest_peak * 0.2f)
								c1 = Vec3(0.2f, 1.0f, 0.2f);
							if (p1.y > terrain_highest_peak * 0.2f && p1.y < terrain_highest_peak * 0.5f)
								c1 = Vec3(0.2f, 0.2f, 0.2f);

							
							if (p2.y <= 0.0f)
								c2 = Vec3(0, 0.2f, 1.0f);
							if (p2.y > 0.0f && p2.y < terrain_highest_peak * 0.2f)
								c2 = Vec3(0.2f, 1.0f, 0.2f);
							if (p2.y > terrain_highest_peak * 0.2f && p2.y < terrain_highest_peak * 0.5f)
								c2 = Vec3(0.2f, 0.2f, 0.2f);

							glVertex3fv(p1.E);
							glColor3fv(c1.E);

							glVertex3fv(p2.E);
							glColor3fv(c2.E);

							//glVertex3fv(p3.E);
							//glColor3f(0, (f32)y, 1);

							//glVertex3fv(p4.E);
							//glColor3f(0, 0, 1);
						}
						glEnd();
#if 1
						glLineWidth(2.0f);

						glBegin(GL_LINE_STRIP);
						for (u32 x = 0; x < current_cell_count + 1; x++)
						{
							vec3 p1 = terrain_verts[y][x];
							vec3 p2 = terrain_verts[y + 1][x];

							vec3 c1 = Vec3(0.0f, 0.0f, 0.0f);
							
							//vec3 p1 = Vec3((f32)x*(terrain_width / terrain_cell_count), Sample1, (f32)y*(terrain_height / terrain_cell_count));
							//vec3 p2 = Vec3((f32)x*(terrain_width / terrain_cell_count), Sample2, ((f32)y + 1) * (terrain_height / terrain_cell_count));
							//vec3 p3 = Vec3((f32)x + 1, 1.0f, (f32)y + 1);
							//vec3 p4 = Vec3((f32)x + 1, 1.0f, (f32)y);

							glVertex3fv(p1.E);
							glColor3fv(c1.E);

							glVertex3fv(p2.E);
							glColor3fv(c1.E);

							//glVertex3fv(p3.E);
							//glColor3f(0, (f32)y, 1);

							//glVertex3fv(p4.E);
							//glColor3f(0, 0, 1);
						}
						glEnd();
#endif
					}

					timer += dt;

					if (timer >= delay)
					{
						timer = 0.0f;
						current_cell_count += 1;
					}
					
					if (current_cell_count >= terrain_cell_count)
						current_cell_count = terrain_cell_count;

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