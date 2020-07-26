#include <Windows.h>

/// <summary>
/// Get the first process occurence by name.
/// </summary>
/// <param name="processName">The process name.</param>
/// <returns>The process ID.</returns>
DWORD getProcessByName(wchar_t *processName);

/// <summary>
/// Inject the specified DLL to a target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="dllpath">The DLL path.</param>
/// <returns>Injection return code.</returns>
HRESULT inject(wchar_t *processName, wchar_t *dllpath);

/// <summary>
/// Eject a specific DLL from the target process.
/// </summary>
/// <param name="processName">The process name.</param>
/// <param name="dllpath">The DLL path.</param>
/// <returns>Ejection return code.</returns>
HRESULT eject(wchar_t* processName, wchar_t* dllpath);
