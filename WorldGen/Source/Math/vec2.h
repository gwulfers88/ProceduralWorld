#ifndef VEC2_H
#define VEC2_H

#include "../Common/types.h"
#include <math.h>

/// Scalar Operations

inline f32
SquareRoot(f32 A)
{
	f32 Result = (f32)sqrt(A);
	return Result;
}

inline f32
Pow(f32 A, f32 B)
{
	f32 Result = (f32)pow(A, B);
	return Result;
}

inline i32
RoundReal32ToInt32(f32 Real32)
{
	i32 Result = (i32)roundf(Real32);
	return(Result);
}

inline u32
RoundReal32ToUInt32(f32 Real32)
{
	u32 Result = (u32)roundf(Real32);
	return(Result);
}

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

/// Vector 2 Operations
union vec2
{
	struct
	{
		f32 x, y;
	};

	f32 E[2];
};

vec2 Vec2(f32 x, f32 y)
{
	vec2 result = {};
	result.x = x;
	result.y = y;
	return result;
}

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
	f32 result = (f32)sqrt(LengthSq(a));
	return result;
}

vec2 Perp(vec2 a)
{
	vec2 result = { -a.y, a.x };
	return result;
}

vec2 Clamp01(vec2 val)
{
	vec2 result;
	result.x = Clamp01(val.x);
	result.y = Clamp01(val.y);
	return result;
}


#endif
