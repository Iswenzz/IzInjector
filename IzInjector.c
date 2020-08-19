#include "IzInjector.h"
#include <stdio.h>
#include <TlHelp32.h>

#define FreeHandle(handle)	\
if (handle)					\
{							\
	CloseHandle(handle);	\
	handle = NULL;			\
}

#define THROW(condition, code)	\
if (condition)					\
	return code;

DWORD getProcessByName(wchar_t *processName)
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	DWORD foundPID = -1;

	do
	{
		if (!wcscmp((wchar_t *)ProcEntry.szExeFile, processName))
		{
			foundPID = ProcEntry.th32ProcessID;
			break;
		}
	} 
	while (Process32Next(hPID, &ProcEntry));

	FreeHandle(hPID);
	return foundPID;
}

HRESULT inject(wchar_t *processName, wchar_t *dllpath)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes((LPCSTR)dllpath);
	THROW(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY, GetLastError());

	// Get process ID by name
	DWORD procID = getProcessByName(processName);
	THROW(!procID, GetLastError());

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ
		| PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, TRUE, procID);
	THROW(hndProc == NULL, GetLastError());

	// Alloc the dll path string to the target process
	LPVOID pAlloc = VirtualAllocEx(hndProc, NULL, (wcslen(dllpath) + 1) * sizeof(wchar_t),
		MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	THROW(pAlloc == NULL, GetLastError());
	THROW(!WriteProcessMemory(hndProc, pAlloc, dllpath, (wcslen(dllpath) + 1) * sizeof(wchar_t), NULL),
		GetLastError());

	// Create a remote thread to load the module
	HANDLE thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, 0);
	THROW(thread == NULL, GetLastError());
	FreeHandle(thread);

	FreeHandle(hndProc);
	return ERROR_SUCCESS;
}

HRESULT eject(wchar_t* processName, wchar_t* dllpath)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes((LPCSTR)dllpath);
	THROW(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY, GetLastError());

	// Get process ID by name
	DWORD procID = getProcessByName(processName);
	THROW(!procID, GetLastError());

	// Search the HMODULE library
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);
	MODULEENTRY32W ModEntry = { sizeof(ModEntry) };
	_Bool found = FALSE;
	THROW(snapshot == INVALID_HANDLE_VALUE, GetLastError());

	for (BOOL bMoreMods = Module32FirstW(snapshot, &ModEntry);
		bMoreMods; 
		bMoreMods = Module32NextW(snapshot, &ModEntry))
	{
		found = !wcscmp(ModEntry.szModule, dllpath) || !wcscmp(ModEntry.szExePath, dllpath);
		if (found) break;
	}
	FreeHandle(snapshot);
	THROW(!found, ERROR_MOD_NOT_FOUND);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD 
		| PROCESS_VM_OPERATION, FALSE, procID);
	THROW(!hndProc, GetLastError());

	// Create a remote thread to terminate the module
	HANDLE thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, 
		ModEntry.modBaseAddr, 0, 0);
	THROW(thread == NULL, GetLastError());
	FreeHandle(hndProc);

	// Wait for the remote thread to terminate
	WaitForSingleObject(thread, INFINITE);

	// Return thread exit code
	DWORD exitCode;
	THROW(!GetExitCodeThread(thread, &exitCode), GetLastError());
	FreeHandle(thread);

	THROW(exitCode != TRUE, exitCode);
	return ERROR_SUCCESS;
}
