#ifndef VEC3_H
#define VEC3_H

#include "vec2.h"

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

vec3 operator*(f32 a, vec3 b)
{
	vec3 result = {};
	result.x = a * b.x;
	result.y = a * b.y;
	result.z = a * b.z;
	return result;
}

vec3 operator*(vec3 b, f32 a)
{
	vec3 result = a*b;
	return result;
}

vec3& operator*=(vec3& b, f32 a)
{
	b = a * b;
	return b;
}

vec3 operator-(vec3 a)
{
	vec3 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}

vec3 operator-(vec3 a, vec3 b)
{
	vec3 result;
	result.x = a.x - b.y;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

vec3 operator+(vec3 a, vec3 b)
{
	vec3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

vec3& operator+=(vec3& a, vec3 b)
{
	a = a + b;
	return a;
}

vec3 Hadamard(vec3 a, vec3 b)
{
	vec3 result = { a.x * b.x, a.y * b.y, a.z * b.z };
	return result;
}

f32 Dot(vec3 a, vec3 b)
{
	f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

f32 LengthSq(vec3 a)
{
	f32 result = Dot(a, a);
	return result;
}

f32 Length(vec3 a)
{
	f32 result = (f32)sqrt(LengthSq(a));
	return result;
}

vec3 Normalize(vec3 A)
{
	vec3 Result = A * (1.0f / Length(A));

	return Result;
}

vec3 Clamp01(vec3 val)
{
	vec3 result;
	result.x = Clamp01(val.x);
	result.y = Clamp01(val.y);
	result.z = Clamp01(val.z);
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
	result.xy = xy;
	result.z = z;
	return result;
}

vec3 Vec3(f32 x, vec2 yz)
{
	vec3 result = {};
	result.x = x;
	result.yz = yz;
	return result;
}

vec3 Vec3(f32 x)
{
	vec3 Result = {};

	Result.x = x;
	Result.y = x;
	Result.z = x;

	return Result;
}

// Mat4x4

struct mat4x4
{
	f32 E[4][4];
};

inline mat4x4
operator*(mat4x4 A, mat4x4 B)
{
	// TODO: Optimize this piece of code to make it better
	mat4x4 R = {};

	for (int r = 0; r <= 3; ++r)
	{
		for (int c = 0; c <= 3; ++c)
		{
			for (int i = 0; i <= 3; ++i)
			{
				R.E[r][c] += A.E[r][i] * B.E[i][c];
			}
		}
	}

	return R;
}

vec3
Transform(mat4x4 A, vec3 P, f32 PW = 1.0f)
{
	vec3 R = {};

	R.x = P.x*A.E[0][0] + P.y*A.E[0][1] + P.z*A.E[0][2] + PW*A.E[0][3];
	R.y = P.x*A.E[1][0] + P.y*A.E[1][1] + P.z*A.E[1][2] + PW*A.E[1][3];
	R.z = P.x*A.E[2][0] + P.y*A.E[2][1] + P.z*A.E[2][2] + PW*A.E[2][3];

	return R;
}

inline vec3
operator*(mat4x4 A, vec3 P)
{
	vec3 R = Transform(A, P, 1.0f);

	return R;
}

mat4x4
Identity(void)
{
	mat4x4 R =
	{
		{ { 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

mat4x4
RotateX(f32 Angle)
{
	f32 c = cos(Angle);
	f32 s = sin(Angle);

	mat4x4 R =
	{
		{ { 1, 0, 0, 0 },
		{ 0, c,-s, 0 },
		{ 0, s, c, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

mat4x4
RotateY(f32 Angle)
{
	f32 c = cos(Angle);
	f32 s = sin(Angle);

	mat4x4 R =
	{
		{ { c, 0, s, 0 },
		{ 0, 1, 0, 0 },
		{ -s, 0, c, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

mat4x4
RotateZ(f32 Angle)
{
	f32 c = cos(Angle);
	f32 s = sin(Angle);

	mat4x4 R =
	{
		{ { c,-s, 0, 0 },
		{ s, c, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

mat4x4
Transpose(mat4x4 A)
{
	mat4x4 R = {};

	for (int j = 0; j <= 3; j++)
	{
		for (int i = 0; i <= 3; i++)
		{
			R.E[j][i] = A.E[i][j];
		}
	}

	return R;
}

mat4x4
PerspectiveProjection(f32 AspectWidthOverHeight, f32 FocalLength)
{
	f32 a = 1.0f;
	f32 b = AspectWidthOverHeight;
	f32 c = FocalLength;

	f32 n = 0.1f; // NOTE: Near Clip plane _distance_
	f32 f = 100.0f; // NOTE: Far Clip plane _distance_

#if 1
					// NOTE: These are the perspective correct terms, for when you divide by -z
	f32 d = (n + f) / (n - f);
	f32 e = (2 * f*n) / (n - f);
#else
					// NOTE: These are the non-perspective corrected terms, for orthographic
	f32 d = 2.0f / (n - f);
	f32 e = (n + f) / (n - f);
#endif

	mat4x4 R =
	{
		{ { a*c,   0,  0,  0 },
		{ 0, b*c,  0,  0 },
		{ 0,   0,  d,  e },
		{ 0,   0, -1,  0 } },
	};

	return R;
}

mat4x4
OrthographicProjection(f32 AspectWidthOverHeight)
{
	f32 a = 1.0f;
	f32 b = AspectWidthOverHeight;

	mat4x4 R =
	{
		{ { a, 0, 0, 0 },
		{ 0, b, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

mat4x4
Column3x3(vec3 X, vec3 Y, vec3 Z)
{
	mat4x4 R =
	{
		{ { X.x, Y.x, Z.x, 0 },
		{ X.y, Y.y, Z.y, 0 },
		{ X.z, Y.z, Z.z, 0 },
		{ 0,   0,   0, 1 } },
	};

	return R;
}

mat4x4
Row3x3(vec3 X, vec3 Y, vec3 Z)
{
	mat4x4 R =
	{
		{ { X.x, X.y, X.z, 0 },
		{ Y.x, Y.y, Y.z, 0 },
		{ Z.x, Z.y, Z.z, 0 },
		{ 0,   0,   0, 1 } },
	};

	return R;
}

mat4x4
Translate(mat4x4 A, vec3 T)
{
	mat4x4 R = A;
	R.E[0][3] += T.x;
	R.E[1][3] += T.y;
	R.E[2][3] += T.z;

	return R;
}

inline vec3
GetColumn(mat4x4 A, u32 C)
{
	vec3 R = { A.E[0][C], A.E[1][C], A.E[2][C] };

	return R;
}

inline vec3
GetRow(mat4x4 A, u32 R)
{
	vec3 Result = { A.E[R][0], A.E[R][1], A.E[R][2] };

	return Result;
}

mat4x4
CameraTransform(vec3 X, vec3 Y, vec3 Z, vec3 P)
{
	mat4x4 R = Row3x3(X, Y, Z);
	R = Translate(R, -(R*P));

	return R;
}

#endif