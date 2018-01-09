#include "platformWin32.h"
#include <Windows.h>

//These should get their own header file
PLATFORM_FREE_MEMORY(PlatformFreeMemory)
{
	if (data)
	{
		VirtualFree(data, 0, MEM_RELEASE);
		data = 0;
	}
}

PLATFORM_ALLOC_MEMORY(PlatformAllocMemory)
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
			result.Data = PlatformAllocMemory(size);

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
