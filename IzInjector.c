#include "IzInjector.h"
#include <stdio.h>
#include <TlHelp32.h>

DWORD getProcess(wchar_t *processName)
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);

	do
	{
		if (!wcscmp(ProcEntry.szExeFile, processName))
		{
			DWORD dwPID = ProcEntry.th32ProcessID;
			CloseHandle(hPID);
			return dwPID;
		}
	} 
	while (Process32Next(hPID, &ProcEntry));
	return 0;
}

HRESULT inject(wchar_t *processName, wchar_t *dllpath)
{
	DWORD dwAttrib = GetFileAttributes(dllpath);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
		return GetLastError();

	DWORD procID = getProcess(processName);
	if (!procID)
		return GetLastError();

	HANDLE hndProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ
		| PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, TRUE, procID);
	if (hndProc == NULL)
		return GetLastError();

	LPVOID pAlloc = VirtualAllocEx(hndProc, NULL, (wcslen(dllpath) + 1) * sizeof(wchar_t),
		MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pAlloc == NULL) 
		return GetLastError();

	if (!WriteProcessMemory(hndProc, pAlloc, dllpath, (wcslen(dllpath) + 1) * sizeof(wchar_t), NULL))
		return GetLastError();

	if (CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, 0) == NULL)
		 return GetLastError();

	CloseHandle(hndProc);
	return ERROR_SUCCESS;
}
