#pragma once
#include <Windows.h>

/// <summary>
/// Get the first process occurence by name.
/// </summary>
/// <param name="processName">The process name.</param>
/// <returns>The process ID.</returns>
DWORD GetProcessByName(char *processName);

/// <summary>
/// Inject the specified DLL to a target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <param name="verbose">Log the process.</param>
/// <returns>Ejection return code.</returns>
HRESULT Inject(char* processName, int pid, const char* dllpath, _Bool verbose);

/// <summary>
/// Eject a specific DLL from the target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <param name="verbose">Log the process.</param>
/// <returns>Ejection return code.</returns>
HRESULT Eject(char* processName, int pid, const char* dllpath, _Bool verbose);
