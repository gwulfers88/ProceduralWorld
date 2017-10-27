#ifndef TYPES_H
#define TYPES_H

typedef char				i8;
typedef short				i16;
typedef int					i32;
typedef long long			i64;

typedef float				f32;
typedef double				f64;

typedef i32					b32;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef size_t				mem_size;

#define F32Max FLT_MAX
#define F32Min -FLT_MAX
#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Align16(value) ((value + 15) & ~15)
#define Kilobyte(value) (value * 1024)
#define Megabyte(value) (Kilobyte(value) * 1024)
#define Gigabyte(value) (Megabyte(value) * 1024)

#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))
#define Maximum(a, b) (a > b ? a : b)
#define Minimum(a, b) (a < b ? a : b)

#endif
