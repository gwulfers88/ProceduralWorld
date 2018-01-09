#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

#include "../Common/types.h"

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

#define PLATFORM_FREE_MEMORY(name) void name(void* data)
#define PLATFORM_ALLOC_MEMORY(name) void* name(i32 size)

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) read_file_result name(i8* Filename)
#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(i8* filename, read_file_result fileResult)

typedef PLATFORM_FREE_MEMORY(platform_free_memory);
typedef PLATFORM_ALLOC_MEMORY(platform_alloc_memory);

typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

PLATFORM_FREE_MEMORY(PlatformFreeMemory);
PLATFORM_ALLOC_MEMORY(PlatformAllocMemory);
DEBUG_PLATFORM_READ_ENTIRE_FILE(DebugPlatformReadEntireFile);
DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DebugPlatformWriteEntireFile);

#endif