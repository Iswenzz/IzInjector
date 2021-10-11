#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MessageBox(NULL, L"DLL Injection Succeed!", L"DLL", MB_OK | MB_ICONINFORMATION);
			break;
		case DLL_PROCESS_DETACH:
			MessageBox(NULL, L"DLL Ejection Succeed!", L"DLL", MB_OK | MB_ICONINFORMATION);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
