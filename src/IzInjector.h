#pragma once
#include <Windows.h>

/// <summary>
/// Throw the function with a HRESULT code if the condition match.
/// </summary>
#define THROWIF(condition, code, verbose)		\
if (ThrowIf(condition, code, verbose) != S_OK)	\
	return NULL									\

/// <summary>
/// Print a message to stdout if verbose flag is set.
/// </summary>
#define VPRINTF(...) \
if (verbose) printf(__VA_ARGS__)

/// <summary>
/// Get the first process occurence by name.
/// </summary>
/// <param name="processName">The process name.</param>
/// <returns>The process ID.</returns>
DWORD GetProcessByName(char *processName);

/// <summary>
/// Free a LPPROCESS_INFORMATION.
/// </summary>
/// <param name="procInfo">The proc info to free.</param>
void FreeProcInfo(LPPROCESS_INFORMATION procInfo);

/// <summary>
/// Inject the specified DLL to a target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <param name="verbose">Log the process.</param>
/// <returns>DLL Process.</returns>
LPPROCESS_INFORMATION Inject(const char* processName, int pid, const char* dllpath, _Bool verbose);

/// <summary>
/// Eject a specific DLL from the target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="pid">The process ID.</param>
/// <param name="dllpath">The DLL path.</param>
/// <param name="waitForExit">Wait for the thread to terminate.</param>
/// <param name="verbose">Log the process.</param>
/// <returns>DLL Process.</returns>
LPPROCESS_INFORMATION Eject(const char* processName, int pid, const char* dllpath,
	_Bool waitForExit, _Bool verbose);

/// <summary>
/// Throw the error code if the condition match.
/// </summary>
/// <param name="condition">The condition.</param>
/// <param name="code">The error code.</param>
/// <param name="verbose">Log the error code message.</param>
/// <returns>The error code.</returns>
HRESULT ThrowIf(_Bool condition, HRESULT code, _Bool verbose);
