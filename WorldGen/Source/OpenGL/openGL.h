#ifndef OPENGL_H
#define OPENGL_H

#include <gl/GL.h>

#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_VALIDATE_STATUS                0x8B83
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_BGRA                           0x80E1
#define GL_BGR                            0x80E0

#define GL_FRAMEBUFFER                    0x8D40
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E

// NOTE: Windows-Specific
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

struct opengl_info
{
	b32 ModernContext;

	char *Vendor;
	char *Renderer;
	char *Version;
	char *ShadingLanguageVersion;
	char *Extensions;

	b32 GL_EXT_texture_sRGB;
	b32 GL_EXT_framebuffer_sRGB;
};

struct opengl
{
	GLint MaxMultiSampleCount;
	b32 sRGBSupport;
	b32 SupportSRGBFramebuffer;
	GLuint DefaultSpriteTextureFormat;
	GLuint DefaultFramebufferTextureFormat;

	GLuint ReserveBlitTexture;

	// Default Shader
	GLuint DefaultShaderProgram;
	GLuint DefaultTransformID;
	GLuint DefaultPositionID;
	GLuint DefaultColorID;

	// Terrain Shader
	GLuint TerrainShaderProgram;
	GLuint TerrainMVMat4ID;
	GLuint TerrainProjMat4ID;
	GLuint TerrainMVPMat4ID;
	GLuint TerrainViewMat4ID;
	GLuint TerrainDmapDepthID;
	GLuint TerrainEnableFogID;
	GLuint TerrainDispMapID;
	GLuint TerrainTexMapID[4];
	GLuint TerrainOffsetID;
};

opengl_info OpenGLGetInfo(b32 ModernContext)
{
	opengl_info Result = {};

	Result.ModernContext = ModernContext;
	Result.Vendor = (char *)glGetString(GL_VENDOR);
	Result.Renderer = (char *)glGetString(GL_RENDERER);
	Result.Version = (char *)glGetString(GL_VERSION);
	if (Result.ModernContext)
	{
		Result.ShadingLanguageVersion = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	}
	else
	{
		Result.ShadingLanguageVersion = "(NONE)";
	}
	Result.Extensions = (char *)glGetString(GL_EXTENSIONS);
	/*
	char *At = Result.Extensions;
	while (*At)
	{
	while (IsWhitespace(*At)) { ++At; }
	char *End = At;
	while (*End && !IsWhitespace(*End)) { ++End; }

	umm Count = End - At;

	if (0) {}
	else if (StringsAreEqual(Count, At, "GL_EXT_texture_sRGB")) { Result.GL_EXT_texture_sRGB = true; }
	else if (StringsAreEqual(Count, At, "GL_EXT_framebuffer_sRGB")) { Result.GL_EXT_framebuffer_sRGB = true; }

	At = End;
	}
	*/
	return Result;
}

#endif
