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

#include <math.h>

f32 Square(f32 val)
{
	f32 result = val * val;
	return result;
}

f32 Lerp(f32 a, f32 t, f32 b)
{
	f32 result = (1.0f - t)*a + t*b;
	return result;
}

f32 Clamp(f32 min, f32 val, f32 max)
{
	f32 result = val;
	if (val < min)
	{
		result = min;
	}
	else if (val > max)
	{
		result = max;
	}
	return result;
}

f32 Clamp01(f32 val)
{
	f32 result = Clamp(0.0f, val, 1.0f);
	return result;
}

f32 Clamp01MapToRange(f32 min, f32 t, f32 max)
{
	f32 result = 0.0f;
	f32 range = max - min;
	if (range != 0.0f)
	{
		result = Clamp01((t - min) / range);
	}
	return result;
}

union vec2
{
	struct
	{
		f32 x, y;
	};

	f32 E[2];
};

vec2 operator*(f32 a, vec2 b)
{
	vec2 result = {};
	result.x = a * b.x;
	result.y = a * b.y;
	return result;
}

vec2 operator*(vec2 b, f32 a)
{
	vec2 result = a*b;
	return result;
}

vec2& operator*=(vec2& b, f32 a)
{
	b = a * b;
	return b;
}

vec2 operator-(vec2 a)
{
	vec2 result;
	result.x = -a.x;
	result.y = -a.y;
	return result;
}

vec2 operator-(vec2 a, vec2 b)
{
	vec2 result;
	result.x = a.x - b.y;
	result.y = a.y - b.y;
	return result;
}

vec2 operator+(vec2 a, vec2 b)
{
	vec2 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

vec2& operator+=(vec2& a, vec2 b)
{
	a = a + b;
	return a;
}

vec2 Hadamard(vec2 a, vec2 b)
{
	vec2 result = { a.x * b.x, a.y * b.y };
	return result;
}

f32 Dot(vec2 a, vec2 b)
{
	f32 result = a.x*b.x + a.y*b.y;
	return result;
}

f32 LengthSq(vec2 a)
{
	f32 result = Dot(a, a);
	return result;
}

f32 Length(vec2 a)
{
	f32 result = sqrt(LengthSq(a));
	return result;
}

vec2 Perp(vec2 a)
{
	vec2 result = {-a.y, a.x};
	return result;
}

vec2 Clamp01(vec2 val)
{
	vec2 result;
	result.x = Clamp01(val.x);
	result.y = Clamp01(val.y);
	return result;
}

union vec3
{
	struct
	{
		f32 x, y, z;
	};
	struct
	{
		f32 r, g, b;
	};
	struct
	{
		vec2 xy;
		f32 ignored0;
	};
	struct
	{
		f32 ignored1;
		vec2 yz;
	};
	f32 E[3];
};

vec2 Vec2(f32 x, f32 y)
{
	vec2 result = {};
	result.x = x;
	result.y = y;

	return result;
}

vec3 Vec3(f32 x, f32 y, f32 z)
{
	vec3 result = {};
	result.x = x;
	result.y = y;
	result.z = z;

	return result;
}

vec3 Vec3(vec2 xy, f32 z)
{
	vec3 result = {};
	result.x = xy.x;
	result.y = xy.y;
	result.z = z;

	return result;
}

vec3 Vec3(f32 x, vec2 yz)
{
	vec3 result = {};
	result.x = x; 
	result.y = yz.x;
	result.z = yz.y;
	
	return result;
}

bool global_isRunning = false;
HGLRC global_openGLRenderContext;
i32 width = 800;
i32 height = 600;

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
									width, height,
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

				glViewport(0, 0, width, height);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.25f, 0.5f, 1.0f);

				f32 focalLen = 0.60f;
				f32 aspect = (f32)(width / height);
				f32 n = 0.1f;
				f32 f = 1000.0f;

				f32 proj[] = 
				{
					1*focalLen,	0,			0,					0,
					0,	aspect*focalLen,	0,					0,
					0,	0,			((n + f)/(n - f)), -1,
					0,	0,			((2*f*n)/(n-f)),	0,
				};

				angle += dt * 10.0f;

				glLoadIdentity();
				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf(&proj[0]);

				glMatrixMode(GL_MODELVIEW);

				glRotatef(angle, 0.0f, 1.0f, 0.0f);

				glBegin(GL_TRIANGLES);
				{
					vec3 pos = Vec3(0, 0, -2.0f);
					glVertex3f(pos.x, pos.y + 0.5f, pos.z);
					glColor3f(pos.x, 0, 0);

					glVertex3f(pos.x + 0.5f, pos.y - 0.5f, pos.z);
					glColor3f(0, pos.y, 0);

					glVertex3f(pos.x - 0.5f, pos.y - 0.5f, pos.z);
					glColor3f(0, 0, pos.z*-1);
				}
				glEnd();

				SwapBuffers(GetDC(window));
			}
		}
		DestroyWindow(window);
	}

	
	UnregisterClassA(windowClass.lpszClassName, hInst);
	
	return 0;
}