#ifndef VEC4_H
#define VEC4_H

#include "vec3.h"

union vec4
{
	struct
	{
		f32 x, y, z, w;
	};
	struct
	{
		f32 r, g, b, a;
	};
	struct
	{
		vec2 xy;
		f32 ignored0;
		f32 ignored1;
	};
	struct
	{
		f32 ignored2;
		vec2 yz;
		f32 ignored3;
	};
	struct
	{
		f32 ignored4;
		f32 ignored5; 
		vec2 zw;
	};
	struct
	{
		vec3 xyz;
		f32 ignored6;
	};
	f32 E[4];
};

vec4 operator*(f32 a, vec4 b)
{
	vec4 result = {};
	result.x = a * b.x;
	result.y = a * b.y;
	result.z = a * b.z;
	result.w = a * b.w;
	return result;
}

vec4 operator*(vec4 b, f32 a)
{
	vec4 result = a*b;
	return result;
}

vec4& operator*=(vec4& b, f32 a)
{
	b = a * b;
	return b;
}

vec4 operator-(vec4 a)
{
	vec4 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	result.w = -a.w;
	return result;
}

vec4 operator-(vec4 a, vec4 b)
{
	vec4 result;
	result.x = a.x - b.y;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;
	return result;
}

vec4 operator+(vec4 a, vec4 b)
{
	vec4 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

vec4& operator+=(vec4& a, vec4 b)
{
	a = a + b;
	return a;
}

vec4 Hadamard(vec4 a, vec4 b)
{
	vec4 result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
	return result;
}

f32 Dot(vec4 a, vec4 b)
{
	f32 result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
	return result;
}

f32 LengthSq(vec4 a)
{
	f32 result = Dot(a, a);
	return result;
}

f32 Length(vec4 a)
{
	f32 result = (f32)sqrt(LengthSq(a));
	return result;
}

vec4 Normalize(vec4 A)
{
	vec4 Result = A * (1.0f / Length(A));

	return Result;
}

vec4 Clamp01(vec4 val)
{
	vec4 result;
	result.x = Clamp01(val.x);
	result.y = Clamp01(val.y);
	result.z = Clamp01(val.z);
	result.w = Clamp01(val.w);
	return result;
}

vec4 Vec4(f32 x, f32 y, f32 z, f32 w)
{
	vec4 result = {};
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

vec4 Vec4(vec3 xyz, f32 w)
{
	vec4 result = {};
	result.xyz = xyz;
	result.w = w;
	return result;
}

vec4 Vec4(f32 x)
{
	vec4 Result = {};

	Result.x = x;
	Result.y = x;
	Result.z = x;
	Result.w = x;

	return Result;
}

#endif