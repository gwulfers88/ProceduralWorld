
#define STB_PERLIN_IMPLEMENTATION 1

#include <Windows.h>
#include "Math/vec4.h"
#include "Math/stb_perlin.h"
#include "OpenGL/openGL.h"
#include <iostream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")

global_variable bool global_isRunning = false;
global_variable HGLRC global_openGLRenderContext;
global_variable i8* global_windowText = "Procedural World Gen\0";
global_variable i32 global_width = (i32)(1920*0.5f);
global_variable i32 global_height = (i32)(1080*0.5f);
global_variable WINDOWPLACEMENT global_previousPlacement;
global_variable LARGE_INTEGER GlobalPerfCounterFreq;

global_variable opengl OpenGL;

typedef char GLchar;

// NOTE: MSAA Functions
typedef void WINAPI type_glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void WINAPI type_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void WINAPI type_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void WINAPI type_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum WINAPI type_glCheckFramebufferStatus(GLenum target);

// NOTE: Shader Functions
typedef void WINAPI type_glAttachShader(GLuint program, GLuint shader);
typedef GLuint WINAPI type_glCreateProgram(void);
typedef GLuint WINAPI type_glCreateShader(GLenum type);
typedef void WINAPI type_glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void WINAPI type_glCompileShader(GLuint shader);
typedef void WINAPI type_glLinkProgram(GLuint program);
typedef void WINAPI type_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void WINAPI type_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void WINAPI type_glValidateProgram(GLuint program);
typedef void WINAPI type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef GLint WINAPI type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef GLint WINAPI type_glGetAttribLocation(GLuint program, const GLchar *name);
typedef void WINAPI type_glDisableVertexAttribArray(GLuint index);
typedef void WINAPI type_glEnableVertexAttribArray(GLuint index);
typedef void WINAPI type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void WINAPI type_glBindVertexArray(GLuint array);
typedef void WINAPI type_glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
typedef void WINAPI type_glGenVertexArrays(GLsizei n, GLuint *arrays);

typedef void WINAPI type_glUniform4iv(GLint location, GLsizei count, const GLint *value);
typedef void WINAPI type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void WINAPI type_glUseProgram(GLuint program);
typedef void WINAPI type_glUniform1i(GLint location, GLint v0);
typedef void WINAPI type_glUniform1f(GLint location, GLfloat v0);
typedef void WINAPI type_glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
typedef void WINAPI type_glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
typedef void WINAPI type_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void WINAPI type_glPatchParameteri(GLenum pname, GLint value);
typedef void WINAPI type_glActiveTexture(GLenum texture);
typedef void WINAPI type_glGenerateMipmap(GLenum target);

typedef BOOL WINAPI type_wglSwapIntervalEXT(int interval);
typedef HGLRC WINAPI type_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);

#define Win32GlobalOpenGLFunction(Name) global_variable type_##Name *Name
// NOTE: MSAA Functions (Multi Sample Anti Aliasing)
Win32GlobalOpenGLFunction(glTexImage2DMultisample);
Win32GlobalOpenGLFunction(glBindFramebuffer);
Win32GlobalOpenGLFunction(glGenFramebuffers);
Win32GlobalOpenGLFunction(glFramebufferTexture2D);
Win32GlobalOpenGLFunction(glCheckFramebufferStatus);

// NOTE: Shader Functions
Win32GlobalOpenGLFunction(glLinkProgram);
Win32GlobalOpenGLFunction(glCreateProgram);
Win32GlobalOpenGLFunction(glShaderSource);
Win32GlobalOpenGLFunction(glCompileShader);
Win32GlobalOpenGLFunction(glCreateShader);
Win32GlobalOpenGLFunction(glAttachShader);

Win32GlobalOpenGLFunction(glGetProgramInfoLog);
Win32GlobalOpenGLFunction(glGetShaderInfoLog);
Win32GlobalOpenGLFunction(glValidateProgram);
Win32GlobalOpenGLFunction(glGetProgramiv);
Win32GlobalOpenGLFunction(glGetUniformLocation);
Win32GlobalOpenGLFunction(glGetAttribLocation);
Win32GlobalOpenGLFunction(glDisableVertexAttribArray);
Win32GlobalOpenGLFunction(glEnableVertexAttribArray);
Win32GlobalOpenGLFunction(glVertexAttribPointer);
Win32GlobalOpenGLFunction(glUniform4iv);
Win32GlobalOpenGLFunction(glUniformMatrix4fv);
Win32GlobalOpenGLFunction(glUseProgram);
Win32GlobalOpenGLFunction(glUniform1i);
Win32GlobalOpenGLFunction(glUniform1f);
Win32GlobalOpenGLFunction(glUniform2fv);
Win32GlobalOpenGLFunction(glUniform3fv);
Win32GlobalOpenGLFunction(glDrawArraysInstanced);
Win32GlobalOpenGLFunction(glPatchParameteri);
Win32GlobalOpenGLFunction(glBindVertexArray);
Win32GlobalOpenGLFunction(glDeleteVertexArrays);
Win32GlobalOpenGLFunction(glGenVertexArrays);
Win32GlobalOpenGLFunction(glActiveTexture);
Win32GlobalOpenGLFunction(glGenerateMipmap);

Win32GlobalOpenGLFunction(wglSwapIntervalEXT);
Win32GlobalOpenGLFunction(wglCreateContextAttribsARB);

int Win32OpenGLAttribs[] =
{
	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	WGL_CONTEXT_MINOR_VERSION_ARB, 0,
	WGL_CONTEXT_FLAGS_ARB, 0// NOTE: Enable for testing WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if WULFENGINE_INTERNAL
	| WGL_CONTEXT_DEBUG_BIT_ARB
#endif
	,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	0,
};

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

struct read_file_result
{
	i32 Size;
	void *Data;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) read_file_result name(i8* Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);
#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(i8* filename, read_file_result fileResult)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#define PLATFORM_FREE_MEMORY(name) void name(void* data)
typedef PLATFORM_FREE_MEMORY(platform_free_memory);
#define PLATFORM_ALLOC_MEMORY(name) void* name(i32 size)
typedef PLATFORM_ALLOC_MEMORY(platform_alloc_memory);

//These should get their own header file
PLATFORM_FREE_MEMORY(FreeMemory)
{
	if (data)
	{
		VirtualFree(data, 0, MEM_RELEASE);
		data = 0;
	}
}

PLATFORM_ALLOC_MEMORY(AllocMemory)
{
	return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DebugPlatformReadEntireFile)
{
	read_file_result result = {};

	HANDLE filehandle = CreateFile(Filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

	if (filehandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER filesize;

		if (GetFileSizeEx(filehandle, &filesize))
		{
			u32 size = filesize.QuadPart;
			result.Data = AllocMemory(size);

			if (result.Data)
			{
				DWORD bytesRead = 0;

				if (ReadFile(filehandle, result.Data, size, &bytesRead, 0) && size == bytesRead)
				{
					result.Size = bytesRead;
				}
			}
		}

		CloseHandle(filehandle);
	}

	return result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DebugPlatformWriteEntireFile)
{
	HANDLE filehandle = CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

	if (filehandle != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten = 0;
		WriteFile(filehandle, fileResult.Data, fileResult.Size, &bytesWritten, 0);
		CloseHandle(filehandle);
	}
	else
	{
		// LOGGING
		return false;
	}
	return true;
}

GLuint OpenGLCreateProgram(char *HeaderCode, char *VertexCode, char *FragmentCode)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLchar *VertexShaderCode[] =
	{
		HeaderCode,
		VertexCode,
	};
	glShaderSource(VertexShaderID, ArrayCount(VertexShaderCode), VertexShaderCode, 0);
	glCompileShader(VertexShaderID);

	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar *FragmentShaderCode[] =
	{
		HeaderCode,
		FragmentCode,
	};
	glShaderSource(FragmentShaderID, ArrayCount(FragmentShaderCode), FragmentShaderCode, 0);
	glCompileShader(FragmentShaderID);

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glValidateProgram(ProgramID);
	GLint Linked = false;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Linked);
	if (!(Linked))
	{
		GLsizei Ignored;
		char VertexErrors[4096];
		char FragmentErrors[4096];
		char ProgramErrors[4096];
		glGetShaderInfoLog(VertexShaderID, sizeof(VertexErrors), &Ignored, VertexErrors);
		glGetShaderInfoLog(FragmentShaderID, sizeof(FragmentErrors), &Ignored, FragmentErrors);
		glGetProgramInfoLog(ProgramID, sizeof(ProgramErrors), &Ignored, ProgramErrors);

		Assert(!"Shader Validation Failed");
	}

	return ProgramID;
}

GLuint OpenGLCreateProgram(char *HeaderCode, char *VertexCode, char* TessControlCode, char* TessEvalCode, char *FragmentCode)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLchar *VertexShaderCode[] =
	{
		HeaderCode,
		VertexCode,
	};
	glShaderSource(VertexShaderID, ArrayCount(VertexShaderCode), VertexShaderCode, 0);
	glCompileShader(VertexShaderID);

	GLuint TessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLchar *TessControlShaderCode[] =
	{
		HeaderCode,
		TessControlCode,
	};
	glShaderSource(TessControlShaderID, ArrayCount(TessControlShaderCode), TessControlShaderCode, 0);
	glCompileShader(TessControlShaderID);

	GLuint TessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLchar *TessEvalShaderCode[] =
	{
		HeaderCode,
		TessEvalCode,
	};
	glShaderSource(TessEvalShaderID, ArrayCount(TessEvalShaderCode), TessEvalShaderCode, 0);
	glCompileShader(TessEvalShaderID);

	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar *FragmentShaderCode[] =
	{
		HeaderCode,
		FragmentCode,
	};
	glShaderSource(FragmentShaderID, ArrayCount(FragmentShaderCode), FragmentShaderCode, 0);
	glCompileShader(FragmentShaderID);

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, TessControlShaderID);
	glAttachShader(ProgramID, TessEvalShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glValidateProgram(ProgramID);
	GLint Linked = false;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Linked);
	if (!(Linked))
	{
		GLsizei Ignored;
		char VertexErrors[4096];
		char TessControlErrors[4096];
		char TessEvalErrors[4096];
		char FragmentErrors[4096];
		char ProgramErrors[4096];
		glGetShaderInfoLog(VertexShaderID, sizeof(VertexErrors), &Ignored, VertexErrors);
		glGetShaderInfoLog(TessControlShaderID, sizeof(TessControlErrors), &Ignored, TessControlErrors);
		glGetShaderInfoLog(TessEvalShaderID, sizeof(TessEvalErrors), &Ignored, TessEvalErrors);
		glGetShaderInfoLog(FragmentShaderID, sizeof(FragmentErrors), &Ignored, FragmentErrors);
		glGetProgramInfoLog(ProgramID, sizeof(ProgramErrors), &Ignored, ProgramErrors);

		Assert(!"Shader Validation Failed");
	}

	return ProgramID;
}

GLuint OpenGLCreateProgram(char *HeaderCode, char *VertexCode, char* TessControlCode, char* TessEvalCode, char* GeoCode, char *FragmentCode)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLchar *VertexShaderCode[] =
	{
		HeaderCode,
		VertexCode,
	};
	glShaderSource(VertexShaderID, ArrayCount(VertexShaderCode), VertexShaderCode, 0);
	glCompileShader(VertexShaderID);

	GLuint TessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLchar *TessControlShaderCode[] =
	{
		HeaderCode,
		TessControlCode,
	};
	glShaderSource(TessControlShaderID, ArrayCount(TessControlShaderCode), TessControlShaderCode, 0);
	glCompileShader(TessControlShaderID);

	GLuint TessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLchar *TessEvalShaderCode[] = 
	{
		HeaderCode,
		TessEvalCode,
	};
	glShaderSource(TessEvalShaderID, ArrayCount(TessEvalShaderCode), TessEvalShaderCode, 0);
	glCompileShader(TessEvalShaderID);

	GLuint GeoShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	GLchar *GeoShaderCode[] =
	{
		HeaderCode,
		GeoCode,
	};
	glShaderSource(GeoShaderID, ArrayCount(GeoShaderCode), GeoShaderCode, 0);
	glCompileShader(GeoShaderID);

	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar *FragmentShaderCode[] =
	{
		HeaderCode,
		FragmentCode,
	};
	glShaderSource(FragmentShaderID, ArrayCount(FragmentShaderCode), FragmentShaderCode, 0);
	glCompileShader(FragmentShaderID);

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, TessControlShaderID);
	glAttachShader(ProgramID, TessEvalShaderID);
	glAttachShader(ProgramID, GeoShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glValidateProgram(ProgramID);
	GLint Linked = false;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Linked);
	if (!(Linked))
	{
		GLsizei Ignored;
		char VertexErrors[4096];
		char TessControlErrors[4096];
		char TessEvalErrors[4096];
		char GeoErrors[4096];
		char FragmentErrors[4096];
		char ProgramErrors[4096];
		glGetShaderInfoLog(VertexShaderID, sizeof(VertexErrors), &Ignored, VertexErrors);
		glGetShaderInfoLog(TessControlShaderID, sizeof(TessControlErrors), &Ignored, TessControlErrors);
		glGetShaderInfoLog(TessEvalShaderID, sizeof(TessEvalErrors), &Ignored, TessEvalErrors);
		glGetShaderInfoLog(GeoShaderID, sizeof(GeoErrors), &Ignored, GeoErrors);
		glGetShaderInfoLog(FragmentShaderID, sizeof(FragmentErrors), &Ignored, FragmentErrors);
		glGetProgramInfoLog(ProgramID, sizeof(ProgramErrors), &Ignored, ProgramErrors);

		Assert(!"Shader Validation Failed");
	}

	return ProgramID;
}

inline void
OpenGLInit(opengl_info Info)
{
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &OpenGL.MaxMultiSampleCount);
	if (OpenGL.MaxMultiSampleCount > 16)
	{
		OpenGL.MaxMultiSampleCount = 16;
	}

	OpenGL.DefaultSpriteTextureFormat = GL_RGBA8;
	OpenGL.DefaultFramebufferTextureFormat = GL_RGBA8;
	if (Info.GL_EXT_texture_sRGB)
	{
		OpenGL.DefaultSpriteTextureFormat = GL_SRGB8_ALPHA8;
	}

	if (Info.GL_EXT_framebuffer_sRGB)
	{
		GLuint TestTexture;
		glGenTextures(1, &TestTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TestTexture);
		glGetError();
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, OpenGL.MaxMultiSampleCount,
			GL_SRGB8_ALPHA8,
			1920, 1080,
			GL_FALSE);
		if (glGetError() == GL_NO_ERROR)
		{
			OpenGL.DefaultFramebufferTextureFormat = GL_SRGB8_ALPHA8;
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		glDeleteTextures(1, &TestTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	char *HeaderCode = R"FOO(
    #version 450
    // Header Code
    )FOO";

	// Default Shader Program
	char *DefaultVertexCode = R"FOO(
	// Vertex Code
	uniform mat4x4 Transform;
	in vec3 Position;
	in vec4 Color;
	out vec4 Color_Out;
	void main(void)
	{
		gl_Position = Transform*vec4(Position, 1);
		Color_Out = Color;
	}
	)FOO";

	char *DefaultFragmentCode = R"FOO(
	// Fragment Code
	in vec4 Color_Out;
	out vec4 FragColor;

	void main(void)
	{
		FragColor = vec4(1, 1, 1, 1);//Color_Out;
	}
	)FOO";

	OpenGL.DefaultShaderProgram = OpenGLCreateProgram(HeaderCode, DefaultVertexCode, DefaultFragmentCode);
	OpenGL.DefaultTransformID = glGetUniformLocation(OpenGL.DefaultShaderProgram, "Transform");
	OpenGL.DefaultPositionID = glGetAttribLocation(OpenGL.DefaultShaderProgram, "Position");

	// TERRAIN SHADER PROGRAM SETUP
	read_file_result TerrainVertShader = DebugPlatformReadEntireFile("data/shaders/terrain/terrain.vs");
	read_file_result TerrainTessCtrlShader = DebugPlatformReadEntireFile("data/shaders/terrain/terrain.tc");
	read_file_result TerrainTessEvalShader = DebugPlatformReadEntireFile("data/shaders/terrain/terrain.te");
	read_file_result TerrainFragShader = DebugPlatformReadEntireFile("data/shaders/terrain/terrain.fs");

	Assert(TerrainVertShader.Data);
	Assert(TerrainTessCtrlShader.Data);
	Assert(TerrainTessEvalShader.Data);
	Assert(TerrainFragShader.Data);

	char* VertexCode = (char*)TerrainVertShader.Data;
	char* TessControlCode = (char*)TerrainTessCtrlShader.Data;
	char* TessEvalCode = (char*)TerrainTessEvalShader.Data;
	char* FragmentCode = (char*)TerrainFragShader.Data;

	OpenGL.TerrainShaderProgram = OpenGLCreateProgram(HeaderCode, VertexCode, TessControlCode, TessEvalCode, FragmentCode);
	OpenGL.TerrainMVMat4ID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "mv_matrix");
	OpenGL.TerrainProjMat4ID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "proj_matrix");
	OpenGL.TerrainMVPMat4ID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "mvp_matrix");
	OpenGL.TerrainViewMat4ID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "view_matrix");
	OpenGL.TerrainDmapDepthID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "dmap_depth");
	OpenGL.TerrainEnableFogID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "enable_fog");
	OpenGL.TerrainDispMapID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "tex_displacement");
	
	OpenGL.TerrainTexMapID[0] = glGetUniformLocation(OpenGL.TerrainShaderProgram, "tex_dirt");
	OpenGL.TerrainTexMapID[1] = glGetUniformLocation(OpenGL.TerrainShaderProgram, "tex_grass");
	OpenGL.TerrainTexMapID[2] = glGetUniformLocation(OpenGL.TerrainShaderProgram, "tex_rock");
	OpenGL.TerrainTexMapID[3] = glGetUniformLocation(OpenGL.TerrainShaderProgram, "tex_snow");

	OpenGL.TerrainOffsetID = glGetUniformLocation(OpenGL.TerrainShaderProgram, "terrain_offset");

	FreeMemory(TerrainVertShader.Data);
	FreeMemory(TerrainTessCtrlShader.Data);
	FreeMemory(TerrainTessEvalShader.Data);
	FreeMemory(TerrainFragShader.Data);
}

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

void Win32SetWindowTitle(HWND Window, opengl_info Info)
{
	i8* modern = "ModernGL: ";
	size_t len = strlen(Info.Vendor);
	len += strlen(Info.Version);
	len += strlen(Info.ShadingLanguageVersion);
	len += strlen(Info.Renderer);
	len += strlen(modern);

	mem_size bufferSize = (sizeof(i8) * len) + (sizeof(i8*)*strlen(global_windowText)) + 11;
	void* bufferMem = malloc(bufferSize);
	if (bufferMem)
	{
		ZeroMemory(bufferMem, bufferSize);
		i8* buffer = (i8*)bufferMem;
		buffer = strcat(buffer, global_windowText);
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, Info.Version);
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, Info.Vendor);
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, Info.ShadingLanguageVersion);
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, Info.Renderer);
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, modern);
		buffer = strcat(buffer, Info.ModernContext == false ? "false" : "true");

		SetWindowText(Window, buffer);

		free(bufferMem);
	}
	else
	{
		SetWindowText(Window, global_windowText);
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

void Win32InitOpenGL(HWND Window)
{
	HDC WindowDC = Win32SetPixelFormat(Window);

	HGLRC OpenGLRC = wglCreateContext(WindowDC);
	if (wglMakeCurrent(WindowDC, OpenGLRC))
	{
		// TODO: Call here wglChoosePixelFormat if we want to use it.
		// NOTE: This must be called after we have created an initial OpenGL Context
		b32 ModernContext = false;

#define Win32GetOpenGLFunction(Name) (type_##Name *)wglGetProcAddress(#Name)
		wglCreateContextAttribsARB = Win32GetOpenGLFunction(wglCreateContextAttribsARB);

		if (wglCreateContextAttribsARB)
		{
			// NOTE: This is an modern version of OpenGL
			HGLRC ShareContext = 0;
			HGLRC ModernGLRC = wglCreateContextAttribsARB(WindowDC, ShareContext, Win32OpenGLAttribs);
			if (ModernGLRC)
			{
				if (wglMakeCurrent(WindowDC, ModernGLRC))
				{
					ModernContext = true;
					wglDeleteContext(OpenGLRC);
					OpenGLRC = ModernGLRC;
				}
			}
		}
		else
		{
			// NOTE: Antiquated version of OpenGL
		}
		opengl_info Info = OpenGLGetInfo(ModernContext);
		Win32SetWindowTitle(Window, Info);

		// NOTE: Linking MSAA Functions
		glTexImage2DMultisample = Win32GetOpenGLFunction(glTexImage2DMultisample);

		// NOTE: Linking Shader Functions
		glLinkProgram = Win32GetOpenGLFunction(glLinkProgram);
		glCreateProgram = Win32GetOpenGLFunction(glCreateProgram);
		glShaderSource = Win32GetOpenGLFunction(glShaderSource);
		glCompileShader = Win32GetOpenGLFunction(glCompileShader);
		glCreateShader = Win32GetOpenGLFunction(glCreateShader);
		glAttachShader = Win32GetOpenGLFunction(glAttachShader);
		glGetProgramInfoLog = Win32GetOpenGLFunction(glGetProgramInfoLog);
		glGetShaderInfoLog = Win32GetOpenGLFunction(glGetShaderInfoLog);
		glValidateProgram = Win32GetOpenGLFunction(glValidateProgram);
		glGetProgramiv = Win32GetOpenGLFunction(glGetProgramiv);
		glGetUniformLocation = Win32GetOpenGLFunction(glGetUniformLocation);
		glGetAttribLocation = Win32GetOpenGLFunction(glGetAttribLocation);
		glDisableVertexAttribArray = Win32GetOpenGLFunction(glDisableVertexAttribArray);
		glEnableVertexAttribArray = Win32GetOpenGLFunction(glEnableVertexAttribArray);
		glVertexAttribPointer = Win32GetOpenGLFunction(glVertexAttribPointer);
		glBindVertexArray = Win32GetOpenGLFunction(glBindVertexArray);
		glDeleteVertexArrays = Win32GetOpenGLFunction(glDeleteVertexArrays);
		glGenVertexArrays = Win32GetOpenGLFunction(glGenVertexArrays);
		glActiveTexture = Win32GetOpenGLFunction(glActiveTexture);
		glGenerateMipmap = Win32GetOpenGLFunction(glGenerateMipmap);

		glUniform4iv = Win32GetOpenGLFunction(glUniform4iv);
		glUniformMatrix4fv = Win32GetOpenGLFunction(glUniformMatrix4fv);
		glUseProgram = Win32GetOpenGLFunction(glUseProgram);
		glUniform1i = Win32GetOpenGLFunction(glUniform1i);
		glUniform1f = Win32GetOpenGLFunction(glUniform1f);
		glUniform2fv = Win32GetOpenGLFunction(glUniform2fv);
		glUniform3fv = Win32GetOpenGLFunction(glUniform3fv);
		glDrawArraysInstanced = Win32GetOpenGLFunction(glDrawArraysInstanced);
		glPatchParameteri = Win32GetOpenGLFunction(glPatchParameteri);

		wglSwapIntervalEXT = Win32GetOpenGLFunction(wglSwapIntervalEXT);
		if (wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(1);
		}

		OpenGLInit(Info);
	}
	else
	{
		// InvalidCodePath;
		// TODO: Diagnostic
	}

	ReleaseDC(Window, WindowDC);
	global_openGLRenderContext = OpenGLRC;
}

LRESULT CALLBACK MainWinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_CREATE:
	{
		Win32InitOpenGL(window);
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

vec3 CameraX = Vec3(1, 0, 0);
vec3 CameraY = Vec3(0, 1, 0);
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
				if (keyCode == VK_NUMPAD1)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->LeftBumper, isDown);
				}
				if (keyCode == VK_NUMPAD2)
				{
					Win32ProcessKeyboardMessage(&KeyboardController->RightBumber, isDown);
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

struct terrain
{
	vec3 P;
	vec3 Dims;
};

u32 ImageChannels = 4;
u32 ImageWidth = 1024;
u32 ImageHeight = 1024;
u32 ImageSize = ImageWidth * ImageHeight * ImageChannels;
void* ImagePixels = nullptr;

b32 wireFrame = true;

// BITMAP
#pragma pack(push, 1)
struct BitmapHeader
{
	u16 FileType;           /*  File type always 4D42h ("BM")   */
	u32 FileSize;           /*  Size of the file in bytes   */
	u16 Reserved1;          /*  Always 0    */
	u16 Reserved2;          /*  Always 0    */
	u32 BitmapOffset;       /*  Starting position of image data in bytes    */

	u32 Size;               /*  Size of this header in bytes    */
	i32 Width;              /*  Image width in pixels   */
	i32 Height;             /*  Image Height in pixels  */
	u16 Planes;             /*  Number of color planes  */
	u16 BitsPerPixel;       /*  Number of bits per pixel    */

							/* Fields added for Windows 3.x follow this line */
	u32 Compression;        /*  Compression methods used    */
	u32 SizeOfBitmap;       /*  Size of bitmap in bytes */
	i32 HorzResolution;     /*  Horizontal resolution in pixels per meter   */
	i32 VertResolution;     /*  Vertical resolution in pixels per meter */
	u32 ColorsUsed;         /*  Number of colors in the image   */
	u32 ColorsImportant;    /*  Minimum number of important colors  */

	u32 RedMask;            /*  Mask identifying bits of red component  */
	u32 GreenMask;          /*  Mask identifying bits of green component    */
	u32 BlueMask;           /*  Mask identifying bits of blue component */
};
#pragma pack(pop)

inline vec4
SRGB255ToLinear1(vec4 C)
{
	vec4 Result;

	f32 Inv255 = 1.0f / 255.0f;

	Result.r = Square(Inv255 * C.r);
	Result.g = Square(Inv255 * C.g);
	Result.b = Square(Inv255 * C.b);
	Result.a = Inv255 * C.a;

	return Result;
}

inline vec4
Linear1ToSRGB255(vec4 C)
{
	vec4 Result;

	f32 One255 = 255.0f;

	Result.r = One255 * SquareRoot(C.r);
	Result.g = One255 * SquareRoot(C.g);
	Result.b = One255 * SquareRoot(C.b);
	Result.a = 255.0f * C.a;

	return Result;
}

vec4 UnpackColor32(u32 Packed)
{
	vec4 Result = Vec4((f32)((Packed >> 16) & 0xFF),
		(f32)((Packed >> 8) & 0xFF),
		(f32)((Packed >> 0) & 0xFF),
		(f32)((Packed >> 24) & 0xFF));
	return Result;
}

u32 PackColor4f(vec4 Unpacked)
{
	u32 Result = (((u32)(Unpacked.a) << 24) |
		((u32)(Unpacked.r) << 16) |
		((u32)(Unpacked.g) << 8) |
		((u32)(Unpacked.b) << 0));

	return Result;
}

struct bit_scan_result
{
	b32 Found;
	u32 Index;
};

inline bit_scan_result
FindLeastSignificantSetBit(u32 Value)
{
	bit_scan_result Result = {};

#if COMPILER_MSVC
	Result.Found = _BitScanForward((unsigned long *)&Result.Index, Value);
#else
	for (u32 Test = 0;
		Test < 32;
		++Test)
	{
		if (Value & (1 << Test))
		{
			Result.Index = Test;
			Result.Found = true;
			break;
		}
	}
#endif

	return(Result);
}

struct bitmap_buffer
{
	i32 Width;
	i32 Height;
	u32 *Pixels;
	u32 BytesPerPixel;
	i32 Pitch;
};

/*
internal bitmap_buffer
MakeEmptyBitmap(memory_arena *Arena, i32 Width, i32 Height, b32 ZeroFill = true)
{
bitmap_buffer Result = {};

Result.Width = Width;
Result.Height = Height;
Result.BytesPerPixel = 4;
Result.Pitch = Align16(Result.Width * Result.BytesPerPixel);

Result.Pixels = PushArray(Arena, Width*Height, u32);

return Result;
}
*/

bitmap_buffer
DebugLoadBitmap(i8 *Filename)
{
	bitmap_buffer Result = {};

	read_file_result FileResult = DebugPlatformReadEntireFile(Filename);

	if (FileResult.Size != 0)
	{
		BitmapHeader *Header = (BitmapHeader *)FileResult.Data;

		Assert((Header->Compression == 3));

		u32 *Pixels = (u32 *)((u8 *)FileResult.Data + Header->BitmapOffset);
		Result.Pixels = Pixels;
		Result.Width = Header->Width;
		Result.Height = Header->Height;
		Result.BytesPerPixel = 4;

		u32 RedMask = Header->RedMask;
		u32 GreenMask = Header->GreenMask;
		u32 BlueMask = Header->BlueMask;
		u32 AlphaMask = ~(RedMask | GreenMask | BlueMask);

		bit_scan_result RedScan = FindLeastSignificantSetBit(RedMask);
		bit_scan_result GreenScan = FindLeastSignificantSetBit(GreenMask);
		bit_scan_result BlueScan = FindLeastSignificantSetBit(BlueMask);
		bit_scan_result AlphaScan = FindLeastSignificantSetBit(AlphaMask);

		Assert(RedScan.Found);
		Assert(GreenScan.Found);
		Assert(BlueScan.Found);
		Assert(AlphaScan.Found);

		i32 RedShiftDown = (i32)RedScan.Index;
		i32 GreenShiftDown = (i32)GreenScan.Index;
		i32 BlueShiftDown = (i32)BlueScan.Index;
		i32 AlphaShiftDown = (i32)AlphaScan.Index;

		u32 *SourceDest = Pixels;
		for (i32 Y = 0;
			Y < Header->Height;
			++Y)
		{
			for (i32 X = 0;
				X < Header->Width;
				++X)
			{
				u32 C = *SourceDest;

				vec4 Texel = Vec4((f32)((C & RedMask) >> RedShiftDown),
					(f32)((C & GreenMask) >> GreenShiftDown),
					(f32)((C & BlueMask) >> BlueShiftDown),
					(f32)((C & AlphaMask) >> AlphaShiftDown));

				//Texel = SRGB255ToLinear1(Texel);
#if 0
				Texel.r *= Texel.a;
				Texel.g *= Texel.a;
				Texel.b *= Texel.a;
#endif
				//Texel = Linear1ToSRGB255(Texel);

				*SourceDest++ = (((u32)(Texel.a + 0.5f) << 24) |
					((u32)(Texel.r + 0.5f) << 16) |
					((u32)(Texel.g + 0.5f) << 8) |
					((u32)(Texel.b + 0.5f) << 0));
			}
		}
	}

	Result.Pitch = Align16(Result.Width * Result.BytesPerPixel);

#if 0
	Result.Pixels = (u32 *)((u8 *)Result.Pixels - Result.Pitch*(Result.Height - 1));
	Result.Pitch = -Result.Pitch;
#endif

	return Result;
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
				Terrain.P = Vec3(0, 0, 0);
				Terrain.Dims = Vec3(64, 0, 64);

				u32 Octaves = 8;
				f32 Persistance = 0.4f;
				f32 Lacunarity = 2.0f;
				f32 NoiseScale = 400;
				f32 NoiseScaleInv = 1.0f / NoiseScale;

				// Generate Perlin Noise Texture
				/*ImagePixels = PushArray(&WorldArena, ImageWidth, ImageHeight, u32);
				if (ImagePixels)
				{
					u32 *Pixels = (u32*)ImagePixels;

					for (u32 y = 0; y < ImageHeight; y++)
					{
						for (u32 x = 0; x < ImageWidth; x++)
						{
							vec3 p1 = Vec3((f32)x, 1.0f, (f32)y);

							f32 Sample = stb_perlin_fbm_noise3((p1.x) * NoiseScaleInv, (p1.y) * NoiseScaleInv, (p1.z) * NoiseScaleInv, Lacunarity, Persistance, Octaves, MAXINT32, MAXINT32, MAXINT32);
							Sample += stb_perlin_fbm_noise3((p1.x) * NoiseScaleInv, (p1.y) * NoiseScaleInv, (p1.z) * NoiseScaleInv, Lacunarity, Persistance, Octaves, MAXINT32, MAXINT32, MAXINT32) / 2;
							
							Sample = abs(Sample);
							Sample *= 255;
							Sample = Minimum(Sample, 255);
							
							u32 PackedColor = ((u32)Sample << 0 |
												(u32)Sample << 8 |
												(u32)Sample << 16 |
												(u32)255 << 24);

							*Pixels++ = PackedColor;
						}
					}
				}*/

				mem_size PermStorageRemaining = WorldArena.TotalSize - WorldArena.Used;

				camera Camera = {};
				Camera.P = Vec3(0.0f, 20.0f, 0.0f);

				GLuint vao = 0;
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glPatchParameteri(GL_PATCH_VERTICES, 4);
/*
				GLuint textureID = 0;
				glActiveTexture(GL_TEXTURE0);
				glGenTextures(1, &textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, OpenGL.DefaultSpriteTextureFormat, ImageWidth, ImageHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ImagePixels);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glUniform1i(OpenGL.TerrainDispMapID, 0);
*/
				bitmap_buffer terrainTex[4] = {};
				
				terrainTex[0] = DebugLoadBitmap("data/textures/dirt_d.bmp");
				terrainTex[1] = DebugLoadBitmap("data/textures/grass_d.bmp");
				terrainTex[2] = DebugLoadBitmap("data/textures/rock_d.bmp");
				terrainTex[3] = DebugLoadBitmap("data/textures/snow_d.bmp");

				GLuint textureMapID[4] = {};

				glActiveTexture(GL_TEXTURE1);
				glGenTextures(1, &textureMapID[0]);
				glBindTexture(GL_TEXTURE_2D, textureMapID[0]);
				glTexImage2D(GL_TEXTURE_2D, 0, OpenGL.DefaultSpriteTextureFormat, terrainTex[0].Width, terrainTex[0].Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, terrainTex[0].Pixels);

				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glUniform1i(OpenGL.TerrainTexMapID[0], 1);

				FreeMemory(terrainTex[0].Pixels);

				// ---------------

				glActiveTexture(GL_TEXTURE2);
				glGenTextures(1, &textureMapID[1]);
				glBindTexture(GL_TEXTURE_2D, textureMapID[1]);
				glTexImage2D(GL_TEXTURE_2D, 0, OpenGL.DefaultSpriteTextureFormat, terrainTex[1].Width, terrainTex[1].Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, terrainTex[1].Pixels);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glUniform1i(OpenGL.TerrainTexMapID[1], 1);

				FreeMemory(terrainTex[1].Pixels);

				// -----------------------

				glActiveTexture(GL_TEXTURE3);
				glGenTextures(1, &textureMapID[2]);
				glBindTexture(GL_TEXTURE_2D, textureMapID[2]);
				glTexImage2D(GL_TEXTURE_2D, 0, OpenGL.DefaultSpriteTextureFormat, terrainTex[2].Width, terrainTex[2].Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, terrainTex[2].Pixels);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glUniform1i(OpenGL.TerrainTexMapID[2], 1);

				FreeMemory(terrainTex[2].Pixels);

				// ---------------------------------

				glActiveTexture(GL_TEXTURE4);
				glGenTextures(1, &textureMapID[3]);
				glBindTexture(GL_TEXTURE_2D, textureMapID[3]);
				glTexImage2D(GL_TEXTURE_2D, 0, OpenGL.DefaultSpriteTextureFormat, terrainTex[3].Width, terrainTex[3].Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, terrainTex[3].Pixels);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glUniform1i(OpenGL.TerrainTexMapID[3], 1);

				FreeMemory(terrainTex[3].Pixels);

				glEnable(GL_CULL_FACE);

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
						if (IsDown(Controller->LeftBumper))
						{
							wireFrame = false;
						}
						else if (IsDown(Controller->RightBumber))
						{
							wireFrame = true;
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
						
						f32 CameraSpeed = 10.0f;
						vec3 CameraOffset = {};

						if (IsDown(Controller->MoveUp))
						{
							CameraOffset.z -= 1.0f;
						}
						else if (IsDown(Controller->MoveDown))
						{
							CameraOffset.z += 1.0f;
						}

						if (IsDown(Controller->MoveLeft))
						{
							CameraOffset.x = -1;
						}
						else if (IsDown(Controller->MoveRight))
						{
							CameraOffset.x = 1;
						}

						if (IsDown(Controller->ActionUp))
						{
							CameraOffset.y = 1;
						}
						else if (IsDown(Controller->ActionDown))
						{
							CameraOffset.y = -1;
						}

						if (Input->ShiftDown)
						{
							CameraSpeed = 40.0f;
						}

						Camera.P += CameraX * CameraOffset.x * CameraSpeed * NewInput->dtForFrame;
						Camera.P += CameraY * CameraOffset.y * CameraSpeed * NewInput->dtForFrame;
						Camera.P += CameraZ * CameraOffset.z * CameraSpeed * Input->dtForFrame;
						CameraOrbit += RotY * NewInput->dtForFrame;

						vec2 MouseP = {};
						
						MouseP.x = Input->MouseX;
						MouseP.y = Input->MouseY;
						vec2 dMouseP = LastMouseP-MouseP;

						CameraOffset = Camera.P;
						
						if (Input->MouseButtons[PlatformMouseButton_Left].EndedDown)
						{
							f32 RotationSpeed = 0.001f*Pi32;
							//CameraOrbit += RotationSpeed*dMouseP.x;
							CameraPitch += RotationSpeed*dMouseP.y;							
						}
						else if (Input->MouseButtons[PlatformMouseButton_Middle].EndedDown)
						{
							f32 ZoomSpeed = (abs(CameraOffset.z) + CameraZDolly)*0.005f;
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

					Camera.WidthOverHeight = (f32)windowDims.width/(f32)windowDims.height;
					Camera.FocalLen = 0.6f;
					mat4x4 Proj = PerspectiveProjection(Camera.WidthOverHeight, Camera.FocalLen, 0.01f, 10000.0f);

					mat4x4 CameraObject = RotateZ(0.0f) * RotateY(CameraOrbit) * RotateX(CameraPitch);
					CameraX = GetColumn(CameraObject, 0);
					CameraY = GetColumn(CameraObject, 1);
					CameraZ = GetColumn(CameraObject, 2);
					
					mat4x4 viewMatrix = LookAt(Camera.P, Camera.P + Vec3(0, 0, 5), Vec3(0, 1, 0));
					
					vec3 CameraOt = (Camera.P);
					mat4x4 CameraT = CameraTransform(CameraX, CameraY, CameraZ, CameraOt);
					
#if _DEBUG			// DEBUG output
					{
						char Buffer[256];
						sprintf(Buffer, "Camera Pos: %0.02fx, %0.02fy, %0.02fz\n", Camera.P.x, Camera.P.y, Camera.P.z);
						sprintf(Buffer, "Terrain Cell: %0dx, %0dz\n", (i32)(Camera.P.x / 64), (i32)(Camera.P.z / 64));
						OutputDebugString(Buffer);
					}
#endif
					mat4x4 mvp = Transpose(Proj*CameraT);
					Proj = Transpose(Proj);
					CameraT = Transpose(CameraT);
					viewMatrix = Transpose(viewMatrix);

					b32 enable_fog = true;

					// TODO: Octree Implementation ? for camera culling and terrain streaming system.
					// Terrain #1
					f32 OneOverTerrainDim = 1/Terrain.Dims.x;
					vec3 CameraCellP = (Camera.P * OneOverTerrainDim);
					for (i32 CellZ = (i32)CameraCellP.z - 1; CellZ < (i32)CameraCellP.z + 1; CellZ++)
					{
						for (i32 CellX = (i32)CameraCellP.x - 1; CellX < (i32)CameraCellP.x + 1; CellX++)
						{
							Terrain.P = Vec3(CellX*Terrain.Dims.x, 0, CellZ*Terrain.Dims.z);
							glUseProgram(OpenGL.TerrainShaderProgram);
							glUniformMatrix4fv(OpenGL.TerrainProjMat4ID, 1, GL_FALSE, &Proj.E[0][0]);
							glUniformMatrix4fv(OpenGL.TerrainMVMat4ID, 1, GL_FALSE, &CameraT.E[0][0]);
							glUniformMatrix4fv(OpenGL.TerrainMVPMat4ID, 1, GL_FALSE, &mvp.E[0][0]);
							glUniformMatrix4fv(OpenGL.TerrainViewMat4ID, 1, GL_FALSE, &viewMatrix.E[0][0]);
							glUniform1f(OpenGL.TerrainDmapDepthID, 25.0f);
							glUniform1i(OpenGL.TerrainEnableFogID, enable_fog);
							glUniform3fv(OpenGL.TerrainOffsetID, 1, Terrain.P.E);
							glPolygonMode(GL_FRONT_AND_BACK, wireFrame == false ? GL_FILL : GL_LINE);
							glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
							glUseProgram(0);
						}
					}

					HDC dc = GetDC(window);
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
					// 60FPS = 16.666ms 30FPS = 33.333ms
					i8 buffer[256];
					if (MSPerFrame <= 33.333)
						FPS = 30;
					else if (MSPerFrame <= 16.666)
						FPS = 60;
					else
						FPS = 5;

					sprintf(buffer, "%.02fms, %.02ff/s, %.02fmc/f\n", MSPerFrame, FPS, MCPF);
					OutputDebugString(buffer);
				}

				glDeleteVertexArrays(1, &vao);

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