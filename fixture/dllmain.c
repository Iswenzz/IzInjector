#include <Windows.h>

/// <summary>
/// DLL Entry point.
/// </summary>
/// <param name="hModule">DLL module handle.</param>
/// <param name="ul_reason_for_call">Reason for calling this function.</param>
/// <param name="lpReserved">Indicates whether the DLL is being loaded statically or dynamically.</param>
/// <returns></returns>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MessageBox(NULL, "DLL Injection Succeed!", "DLL", MB_OK | MB_ICONINFORMATION);
			break;
		case DLL_PROCESS_DETACH:
			MessageBox(NULL, "DLL Ejection Succeed!", "DLL", MB_OK | MB_ICONINFORMATION);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
