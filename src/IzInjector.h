#pragma once
#include <Windows.h>

#include <TlHelp32.h>
#include <assert.h>
#include <stdio.h>

/// <summary>
/// Print message.
/// </summary>
#define IZ_PRINT(message) printf(message "\n")

/// <summary>
/// Print formatted message.
/// </summary>
#define IZ_PRINTF(message, ...) printf(message "\n", __VA_ARGS__)

/// <summary>
/// Print info message.
/// </summary>
#define IZ_INFO(message, ...) IZ_PRINTF("[INFO] " message, __VA_ARGS__)

/// <summary>
/// Assert message.
/// </summary>
#define IZ_ASSERT(condition, ...)         \
	if (!condition)                       \
	{                                     \
		IZ_PRINTF("[ERROR] "__VA_ARGS__); \
		assert(condition);                \
		exit(1);                          \
	}

/// <summary>
/// Inject the specified DLL to a target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <returns>Process info.</returns>
PROCESS_INFORMATION Inject(const char* processName, int pid, const char* dllpath);

/// <summary>
/// Eject the specified DLL from the target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <param name="waitForExit">Wait for the thread to terminate.</param>
/// <returns>Process info.</returns>
PROCESS_INFORMATION Eject(const char* processName, int pid, const char* dllpath, _Bool waitForExit);

/// <summary>
/// Get process entry by name.
/// </summary>
/// <param name="processName">The process name.</param>
/// <returns>The process entry.</returns>
PROCESSENTRY32 GetProcessEntry(char* processName);

/// <summary>
/// Get module entry by name.
/// </summary>
/// <param name="process">The process id.</param>
/// <param name="moduleName">The module name or file path.</param>
/// <returns>The module entry.</returns>
MODULEENTRY32 GetModuleEntry(int process, char* moduleName);
