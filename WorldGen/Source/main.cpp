
#define STB_PERLIN_IMPLEMENTATION 1

#include "Math/vec3.h"
#include "Math/stb_perlin.h"
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")

global_variable bool global_isRunning = false;
global_variable HGLRC global_openGLRenderContext;
global_variable i8* global_windowText = "Procedural World Gen\0";
global_variable i32 global_width = (i32)(1920*0.9f);
global_variable i32 global_height = (i32)(1080*0.9f);
global_variable WINDOWPLACEMENT global_previousPlacement;
global_variable LARGE_INTEGER GlobalPerfCounterFreq;

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

f32 terrain_highest_peak = F32Min;

vec3 CameraX;
vec3 CameraY;
vec3 CameraZ;
vec2 LastMouseP;

f32 CameraPitch = 0;// 0.025f*Pi32;
f32 CameraOrbit = 0;
f32 CameraZDolly = 0;
f32 CameraYDolly = 0;
f32 CameraXDolly = 0;

struct camera
{
	vec3 P;
	f32 FocalLen;
	f32 WidthOverHeight;
};

struct terrain_chunk
{
	vec3 ChunkPos;
	vec2 ChunkDims;
	u32 cellCount;
	u32 VertCount;
	vec3* Verts;
};

struct terrain
{
	f32 MinHeight;
	u32 ChunksPerW;
	u32 ChunksPerL;
	terrain_chunk* Chunks;
};

struct memory_arena
{
	void* Base;
	mem_size Used;
	mem_size TotalSize;
};

memory_arena InitializeArena(void* Base, mem_size Size)
{
	memory_arena Arena = {};
	Arena.Base = Base;
	Arena.Used = 0;
	Arena.TotalSize = Size;

	return Arena;
}

struct game_button
{
	b32 EndedDown;
	u32 HalfTransitionCount;
};

struct game_controller
{
	union
	{
		game_button Buttons[12];
		struct
		{
			game_button MoveUp;
			game_button MoveDown;
			game_button MoveLeft;
			game_button MoveRight;

			game_button ActionUp;
			game_button ActionDown;
			game_button ActionLeft;
			game_button ActionRight;

			game_button Start;
			game_button Back;

			game_button LeftBumper;
			game_button RightBumber;

			// NOTE: All new buttons need to be added before this line
			game_button ButtonCount;
		};
	};
};

#define MAX_CONTROLLER_COUNT 5
enum platform_mouse_button
{
	PlatformMouseButton_Left,
	PlatformMouseButton_Middle,
	PlatformMouseButton_Right,

	PlatformMouseButton_Count,
};

struct game_input
{
	game_controller GameControllers[MAX_CONTROLLER_COUNT];
	game_button MouseButtons[PlatformMouseButton_Count];
	f32 dtForFrame;
	f32 MouseX;
	f32 MouseY;

	b32 AltDown;
	b32 ShiftDown;
	b32 ControlDown;
};

#define PushStruct(Arena, Size, Type) (Type*)PushMemory(Arena, sizeof(Type)*Size)
#define PushArray(Arena, CountW, CountH, Type) (Type*)PushMemory(Arena, sizeof(Type)*(CountW*CountH))
void* PushMemory(memory_arena* Arena, mem_size Size)
{
	void* Base = 0;
	if (Arena->Used + Size < Arena->TotalSize)
	{
		Base = ((u8*)Arena->Base + Arena->Used + Size);
		Arena->Used += Size;
	}
	return Base;
}

void Win32ProcessKeyboardMessage(game_button *NewState, b32 IsDown)
{
	if (NewState->EndedDown != IsDown)
	{
		NewState->EndedDown = IsDown;
		++NewState->HalfTransitionCount;
	}
}

inline game_controller *
GetController(game_input *Input, u32 ControllerIndex)
{
	Assert((ControllerIndex < ArrayCount(Input->GameControllers)));

	game_controller *Result = Input->GameControllers + ControllerIndex;
	return Result;
}

inline b32
WasPressed(game_button Button)
{
	b32 Result = (Button.HalfTransitionCount > 1 ||
		Button.HalfTransitionCount == 1 && (Button.EndedDown));
	return Result;
}

inline b32
IsDown(game_button Button)
{
	b32 Result = (Button.EndedDown);
	return Result;
}

win32_dimension Win32GetWindowDimensions(HWND window)
{
	win32_dimension Result = {};
	RECT clientRect = {};
	GetClientRect(window, &clientRect);
	Result.width = clientRect.right - clientRect.left;
	Result.height = clientRect.bottom - clientRect.top;

	return Result;
}

LARGE_INTEGER Win32GetClock()
{
	LARGE_INTEGER perfCounter = {};
	QueryPerformanceCounter(&perfCounter);
	return perfCounter;
}

f64 Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	return (((f64)End.QuadPart - (f64)Start.QuadPart) / (f64)GlobalPerfCounterFreq.QuadPart);
}

void Win32ProcessPendingMessages(game_controller* KeyboardController)
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
				if (keyCode == 'W')
				{
					Win32ProcessKeyboardMessage(&KeyboardController->MoveUp, isDown);
				}
				if (keyCode == 'S')
				{
					Win32ProcessKeyboardMessage(&KeyboardController->MoveDown, isDown);
				}
				if (keyCode == 'A')
				{
					Win32ProcessKeyboardMessage(&KeyboardController->MoveLeft, isDown);
				}
				if (keyCode == 'D')
				{
					Win32ProcessKeyboardMessage(&KeyboardController->MoveRight, isDown);
				}
				if (keyCode == VK_UP)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->ActionUp, isDown);
				}
				if (keyCode == VK_DOWN)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->ActionDown, isDown);
				}
				if (keyCode == VK_LEFT)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->ActionLeft, isDown);
				}
				if (keyCode == VK_RIGHT)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->ActionRight, isDown);
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
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLn, int cmdShow)
{
	QueryPerformanceFrequency(&GlobalPerfCounterFreq);

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

			HDC dc = GetDC(window);
			i32 MonitorRefreshHz = GetDeviceCaps(dc, VREFRESH);
			f32 TargetFrameTime = 1.0f / (f32)MonitorRefreshHz;
			ReleaseDC(window, dc);

			UINT DesiredSchedulerMS = 1;
			b32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);

			win32_memory memory = {};
			memory.persistentStorageSize = Megabyte(64);
			memory.transientStorageSize = Megabyte(64);
			memory.totalStorageSize = memory.persistentStorageSize + memory.transientStorageSize;
			
			memory.persistentStorage = VirtualAlloc(0, memory.persistentStorageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			memory.transientStorage = VirtualAlloc(0, memory.transientStorageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			if (memory.persistentStorage && memory.transientStorage)
			{
				LARGE_INTEGER startCounter = Win32GetClock();
				DWORD startCycles = (DWORD)__rdtsc();

				game_input Input[2] = {};
				game_input *NewInput = &Input[0];
				game_input *OldInput = &Input[1];

				game_controller *OldKeyboardController = GetController(OldInput, 0);
				game_controller *NewKeyboardController = GetController(NewInput, 0);
				*NewKeyboardController = {};
				for (u32 ButtonIndex = 0;
					ButtonIndex < ArrayCount(NewKeyboardController->Buttons);
					++ButtonIndex)
				{
					NewKeyboardController->Buttons[ButtonIndex].EndedDown =
						OldKeyboardController->Buttons[ButtonIndex].EndedDown;
				}

				memory_arena WorldArena = InitializeArena(memory.persistentStorage, memory.persistentStorageSize);

				terrain Terrain = {};
				Terrain.ChunksPerW = 8;
				Terrain.ChunksPerL = 8;
				Terrain.Chunks = PushArray(&WorldArena, Terrain.ChunksPerW, Terrain.ChunksPerL, terrain_chunk);
				Terrain.MinHeight = -20.0f;

				u32 Octaves = 8;
				f32 Persistance = 0.4f;
				f32 Lacunarity = 2.5f;
				f32 NoiseScale = 200;

				for (u32 ChunkY = 0; ChunkY < Terrain.ChunksPerL; ChunkY++)
				{
					for (u32 ChunkX = 0; ChunkX < Terrain.ChunksPerW; ChunkX++)
					{
						terrain_chunk* Chunk = &Terrain.Chunks[ChunkY * Terrain.ChunksPerW + ChunkX];
						Chunk->ChunkDims = Vec2(50, 50);
						Chunk->ChunkPos = Vec3((ChunkX * Chunk->ChunkDims.x), 0, (ChunkY * Chunk->ChunkDims.y));
						Chunk->cellCount = (u32)Chunk->ChunkDims.x - 1;
						Chunk->VertCount = Chunk->cellCount + 1;
						Chunk->Verts = PushArray(&WorldArena, Chunk->VertCount, Chunk->VertCount, vec3);

						for (u32 y = 0; y < Chunk->cellCount + 1; y++)
						{
							for (u32 x = 0; x < Chunk->cellCount + 1; x++)
							{
								vec3 p1 = Vec3((f32)x*(Chunk->ChunkDims.x / Chunk->cellCount), 1.0f, (f32)y*(Chunk->ChunkDims.y / Chunk->cellCount)) + (Chunk->ChunkPos);
								vec3 p2 = Vec3((f32)x*(Chunk->ChunkDims.x / Chunk->cellCount), 1.0f, ((f32)y + 1) * (Chunk->ChunkDims.y / Chunk->cellCount)) + (Chunk->ChunkPos);

								f32 Sample = stb_perlin_fbm_noise3((p1.x) / NoiseScale,
									(p1.y) / NoiseScale,
									(p1.z) / NoiseScale, Lacunarity, Persistance, Octaves, MAXINT32, MAXINT32, MAXINT32);

								f32 Sample2 = stb_perlin_fbm_noise3((p2.x) / NoiseScale,
									(p2.y) / NoiseScale,
									(p2.z) / NoiseScale, Lacunarity, Persistance, Octaves, MAXINT32, MAXINT32, MAXINT32);

								Sample = Maximum(Sample * 50, Terrain.MinHeight);
								Sample2 = Maximum(Sample2 * 50, Terrain.MinHeight);

								if (Sample > terrain_highest_peak)
									terrain_highest_peak = Sample;

								if (Sample2 > terrain_highest_peak)
									terrain_highest_peak = Sample2;

								p1.y = Sample;
								p2.y = Sample2;

								Chunk->Verts[y * Chunk->VertCount + x] = p1;
								Chunk->Verts[(y + 1) * Chunk->VertCount + x] = p1;
							}
						}
					}
				}

				mem_size PermStorageRemaining = WorldArena.TotalSize - WorldArena.Used;

				camera Camera = {};
				Camera.P = Vec3(Terrain.ChunksPerW*(Terrain.Chunks[0].ChunkDims.x*0.5f), 20.0f, Terrain.ChunksPerL*(Terrain.Chunks[0].ChunkDims.y*0.5f));

				while (global_isRunning)
				{
					Win32ProcessPendingMessages(NewKeyboardController);

					POINT MouseP = {};
					GetCursorPos(&MouseP);
					ScreenToClient(window, &MouseP);
					f32 MouseX = (f32)MouseP.x;
					f32 MouseY = (f32)MouseP.y;

					NewInput->MouseX = MouseX;
					NewInput->MouseY = MouseY;

					win32_dimension windowDims = Win32GetWindowDimensions(window);

					NewInput->ShiftDown = (GetKeyState(VK_SHIFT) & (1 << 15));
					NewInput->AltDown = (GetKeyState(VK_MENU) & (1 << 15));
					NewInput->ControlDown = (GetKeyState(VK_CONTROL) & (1 << 15));

					DWORD Win32Buttons[] =
					{
						VK_LBUTTON,
						VK_MBUTTON,
						VK_RBUTTON,
					};
					for (u32 ButtonIndex = 0;
						ButtonIndex < 3;
						ButtonIndex++)
					{
						NewInput->MouseButtons[ButtonIndex] = OldInput->MouseButtons[ButtonIndex];
						NewInput->MouseButtons[ButtonIndex].HalfTransitionCount = 0;
						Win32ProcessKeyboardMessage(&NewInput->MouseButtons[ButtonIndex], (GetKeyState(Win32Buttons[ButtonIndex]) & (1 << 15)));
					}

					NewInput->dtForFrame = TargetFrameTime;

					// GameUpdateAndRender(&GameMemory, NewInput, &Commands);

					game_controller* Controller = GetController(NewInput, 0);
					if (Controller)
					{
						f32 offsetY = 0;
						if (IsDown(Controller->MoveUp))
						{
							offsetY = 1;
						}
						else if (IsDown(Controller->MoveDown))
						{
							offsetY = -1;
						}

						f32 offsetX = 0;
						if (IsDown(Controller->MoveLeft))
						{
							offsetX = -1;
						}
						else if (IsDown(Controller->MoveRight))
						{
							offsetX = 1;
						}

						f32 RotY = 0;
						if (IsDown(Controller->ActionLeft))
						{
							RotY = 1;
						}
						else if (IsDown(Controller->ActionRight))
						{
							RotY = -1;
						}

						f32 CameraSpeed = 20.0f;
						CameraOrbit += RotY * NewInput->dtForFrame;
						CameraXDolly += offsetX * CameraSpeed * NewInput->dtForFrame;
						CameraYDolly += offsetY * CameraSpeed * NewInput->dtForFrame;
						
						vec2 MouseP = {};
						
						MouseP.x = Input->MouseX;
						MouseP.y = Input->MouseY;
						vec2 dMouseP = MouseP - LastMouseP;

						vec3 CameraOffset = Camera.P;

						if (Input->MouseButtons[PlatformMouseButton_Left].EndedDown)
						{
							f32 RotationSpeed = 0.001f*Pi32;
							//CameraOrbit += RotationSpeed*dMouseP.x;
							CameraPitch += RotationSpeed*dMouseP.y;
						}
						else if (Input->MouseButtons[PlatformMouseButton_Middle].EndedDown)
						{
							f32 ZoomSpeed = (CameraOffset.z + CameraZDolly)*0.005f;
							CameraZDolly -= ZoomSpeed*dMouseP.y;
						}
							
						LastMouseP = MouseP;
					}

					glDepthMask(GL_TRUE);
					glDepthFunc(GL_LEQUAL);
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_BLEND);
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

					glEnable(GL_LIGHT0);
					
					glViewport(0, 0, windowDims.width, windowDims.height);

					glClearDepth(1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glClearColor(0.2f, 0.5f, 0.9f, 1.0f);

					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();

					glMatrixMode(GL_PROJECTION);
					Camera.WidthOverHeight = (f32)windowDims.width/(f32)windowDims.height;
					Camera.FocalLen = 0.6f;
					mat4x4 Proj = PerspectiveProjection(Camera.WidthOverHeight, Camera.FocalLen, 0.01f, 10000.0f);

					mat4x4 CameraObject = RotateY(CameraOrbit)*RotateX(CameraPitch);// RotateZ(CameraOrbit);
					CameraX = GetColumn(CameraObject, 0);
					CameraY = GetColumn(CameraObject, 1);
					CameraZ = GetColumn(CameraObject, 2);
					vec3 CameraOt = (Camera.P + Vec3(CameraXDolly, CameraYDolly, CameraZDolly))*CameraObject;
					mat4x4 CameraT = CameraTransform(CameraX, CameraY, CameraZ, CameraOt);
					
					Proj = Transpose(Proj*CameraT);
					glLoadMatrixf(&Proj.E[0][0]);

					for (u32 ChunkY = 0; ChunkY < Terrain.ChunksPerL; ChunkY++)
					{
						for (u32 ChunkX = 0; ChunkX < Terrain.ChunksPerW; ChunkX++)
						{
							terrain_chunk* Chunk = &Terrain.Chunks[ChunkY * Terrain.ChunksPerW + ChunkX];
							Assert(Chunk);

							for (u32 y = 0; y < Chunk->cellCount; y++)
							{
								glBegin(GL_TRIANGLE_STRIP);
								for (u32 x = 0; x < Chunk->cellCount + 1; x++)
								{
									vec3 p1 = Chunk->Verts[y * Chunk->VertCount + x];
									vec3 p2 = Chunk->Verts[(y + 1) * Chunk->VertCount + x];

									vec3 c1 = Vec3(0.9f);
									vec3 c2 = Vec3(0.9f);

									if (p1.y <= Terrain.MinHeight)
										c1 = Vec3(0, 0.2f, 1.0f);
									if (p1.y > Terrain.MinHeight && p1.y < (Terrain.MinHeight - terrain_highest_peak) * 0.2f)
										c1 = Vec3(0.2f, 1.0f, 0.2f);
									if (p1.y >(Terrain.MinHeight - terrain_highest_peak) * 0.2f && p1.y < (terrain_highest_peak) * 0.5f)
										c1 = Vec3(0.3f, 0.3f, 0.3f);

									if (p2.y <= Terrain.MinHeight)
										c2 = Vec3(0, 0.2f, 1.0f);
									if (p2.y > Terrain.MinHeight && p2.y < (Terrain.MinHeight - terrain_highest_peak) * 0.2f)
										c2 = Vec3(0.2f, 1.0f, 0.2f);
									if (p2.y >(Terrain.MinHeight - terrain_highest_peak) * 0.2f && p2.y < (terrain_highest_peak) * 0.5f)
										c2 = Vec3(0.3f, 0.3f, 0.3f);

									glVertex3fv(p1.E);
									glColor3fv(c1.E);

									glVertex3fv(p2.E);
									glColor3fv(c2.E);
								}
								glEnd();
#if 1
								glLineWidth(2.0f);

								glBegin(GL_LINE_STRIP);
								for (u32 x = 0; x < Chunk->cellCount + 1; x++)
								{
									vec3 p1 = Chunk->Verts[y * Chunk->VertCount + x];
									vec3 p2 = Chunk->Verts[(y + 1) * Chunk->VertCount + x];

									vec3 c1 = Vec3(0.0f);
									
									if (x == 0 || y == 0)
									{
										c1 = Vec3(1.0f, 0.0f, 0.0f);
									}

									glVertex3fv(p1.E);
									glColor3fv(c1.E);

									glVertex3fv(p2.E);
									glColor3fv(c1.E);
								}
								glEnd();
#endif
							}
						}
					}
					HDC dc = GetDC(window);
					//glFlush();
					SwapBuffers(dc);
					ReleaseDC(window, dc);

					LARGE_INTEGER WorkCounter = Win32GetClock();
					f32 WorkSecondsElapsed = (f32)Win32GetSecondsElapsed(startCounter, WorkCounter);

					// TODO(casey): NOT TESTED YET!  PROBABLY BUGGY!!!!!
					f32 SecondsElapsedForFrame = WorkSecondsElapsed;
					if (SecondsElapsedForFrame < TargetFrameTime)
					{
						if (SleepIsGranular)
						{
							DWORD SleepMS = (DWORD)(1000.0f * (TargetFrameTime - SecondsElapsedForFrame));
							if (SleepMS > 0)
							{
								Sleep(SleepMS);
							}
						}

						f32 TestSecondsElapsedForFrame = (f32)Win32GetSecondsElapsed(startCounter, Win32GetClock());
						if (TestSecondsElapsedForFrame < TargetFrameTime)
						{
							// TODO(casey): LOG MISSED SLEEP HERE
						}

						while (SecondsElapsedForFrame < TargetFrameTime)
						{
							SecondsElapsedForFrame = (f32)Win32GetSecondsElapsed(startCounter, Win32GetClock());
						}
					}
					else
					{
						// TODO(casey): MISSED FRAME RATE!
						// TODO(casey): Logging
					}

					game_input *Temp = NewInput;
					NewInput = OldInput;
					OldInput = Temp;

					LARGE_INTEGER endCounter = Win32GetClock();
					f64 MSPerFrame = 1000.0f * Win32GetSecondsElapsed(startCounter, endCounter);

					DWORD endCycles = (DWORD)__rdtsc();

					u64 cyclesElapsed = (endCycles - startCycles);

					f64 FPS = 0.0;
					f64 MCPF = (f64)cyclesElapsed / (1000.0f * 1000.0f);

					startCycles = endCycles;
					startCounter = endCounter;

					i8 buffer[256];
					sprintf(buffer, "%.02fms, %.02ff/s, %.02fmc/f\n", MSPerFrame, FPS, MCPF);
					OutputDebugString(buffer);
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