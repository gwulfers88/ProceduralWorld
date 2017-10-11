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

#endif