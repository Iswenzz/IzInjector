#pragma once
#include <Windows.h>

#include <TlHelp32.h>
#include <assert.h>
#include <stdio.h>

#define IZ_PRINT(message) printf(message "\n")
#define IZ_PRINTF(message, ...) printf(message "\n", __VA_ARGS__)
#define IZ_INFO(message, ...) IZ_PRINTF("[INFO] " message, __VA_ARGS__)

#define IZ_ASSERT(condition, ...)         \
	if (!condition)                       \
	{                                     \
		IZ_PRINTF("[ERROR] "__VA_ARGS__); \
		assert(condition);                \
		exit(1);                          \
	}

PROCESS_INFORMATION Inject(const char* processName, int pid, const char* dllpath);
PROCESS_INFORMATION Eject(const char* processName, int pid, const char* dllpath, _Bool waitForExit);

PROCESSENTRY32 GetProcessEntry(char* processName);
MODULEENTRY32 GetModuleEntry(int process, char* moduleName);
