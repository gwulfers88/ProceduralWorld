#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include "../Common/types.h"
#include "vec2.h"
#include <random>

f32 GetNoise(f32 X, f32 Y)
{
	f32 Result = X + Y + (f32)rand();
	return -1.0f + 2.0f * (Result / (f32)RAND_MAX);
}

f32 GetSmoothNoise(f32 X, f32 Y)
{
	f32 Corners = (GetNoise(X - 1, Y - 1) + GetNoise(X + 1, Y - 1) + GetNoise(X - 1, Y + 1) + GetNoise(X + 1, Y + 1)) / 16;
	f32 Sides = (GetNoise(X - 1, Y ) + GetNoise(X + 1, Y) + GetNoise(X, Y - 1) + GetNoise(X, Y + 1)) / 8;
	f32 Center = GetNoise(X, Y) / 4;
	return Corners + Sides + Center;
}

f32 CosLerp(f32 A, f32 t, f32 B)
{
	f32 th = t * Pi32;
	f32 f = (1.0f - cosf(th)) * 0.5f;
	f32 Result = Lerp(A, f, B);
	return Result;
}

f32 Fade(f32 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

f32 GetLerpedNoise(f32 X, f32 Y)
{
	i32 XI32 = floor((i32)X);
	i32 YI32 = floor((i32)Y);
	f32 FractX = X - XI32;
	f32 FractY = Y - YI32;

	f32 ux = Fade(FractX);
	f32 uy = Fade(FractY);

	f32 v1 = GetSmoothNoise(XI32, YI32);
	f32 v2 = GetSmoothNoise(XI32 + 1, YI32);
	f32 v3 = GetSmoothNoise(XI32, YI32 + 1);
	f32 v4 = GetSmoothNoise(XI32 + 1, YI32 + 1);

	f32 i1 = CosLerp(v1, ux, v2);
	f32 i2 = CosLerp(v3, ux, v4);

	return CosLerp(i1, uy, i2);
}

#endif
