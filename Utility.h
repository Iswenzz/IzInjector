#pragma once
#include <Windows.h>

/// <summary>
/// Throw the function with a HRESULT code if the condition match.
/// </summary>
#define THROWIF(condition, code, verbose)				\
if (ThrowIf(condition, code, verbose) != ERROR_SUCCESS)	\
	return code;										\

/// <summary>
/// Print a message to stdout if verbose flag is set.
/// </summary>
#define VPRINTF(...) \
if (verbose) printf(__VA_ARGS__);

/// <summary>
/// Safely free a HANDLE.
/// </summary>
/// <param name="handle">The HANDLE to free.</param>
void SafeFreeHandle(HANDLE handle);

/// <summary>
/// Throw the error code if the condition match.
/// </summary>
/// <param name="condition">The condition.</param>
/// <param name="code">The error code.</param>
/// <param name="verbose">Log the error code message.</param>
/// <returns>The error code.</returns>
HRESULT ThrowIf(_Bool condition, HRESULT code, _Bool verbose);
