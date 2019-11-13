#include <Windows.h>

DWORD getProcess(wchar_t *processName);
HRESULT inject(wchar_t *processName, wchar_t *dllpath);
