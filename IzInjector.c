#include "IzInjector.h"
#include "utility.h"
#include <stdio.h>
#include <TlHelp32.h>

DWORD getProcessByName(char *processName)
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	DWORD foundPID = -1;

	do
	{
		if (!strcmp((char *)ProcEntry.szExeFile, processName))
		{
			foundPID = ProcEntry.th32ProcessID;
			break;
		}
	} 
	while (Process32Next(hPID, &ProcEntry));

	SafeFreeHandle(hPID);
	return foundPID;
}

HRESULT inject(char *processName, int pid, char *dllpath, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes((LPCSTR)dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY,
		GetLastError(), verbose);

	// Get process ID by name if pid is unknown
	DWORD procID = pid == -1 ? getProcessByName(processName) : pid;
	THROWIF(!procID, GetLastError(), verbose);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ
		| PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, TRUE, procID);
	THROWIF(hndProc == NULL, GetLastError(), verbose);

	// Alloc the dll path string to the target process
	LPVOID pAlloc = NULL;
	if (hndProc)
		pAlloc = VirtualAllocEx(hndProc, NULL, (strlen(dllpath) + 1) * sizeof(char),
			MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	THROWIF(pAlloc == NULL, GetLastError(), verbose);

	if (hndProc && pAlloc)
		THROWIF(!WriteProcessMemory(hndProc, pAlloc, dllpath, (strlen(dllpath) + 1) * sizeof(char), NULL),
			GetLastError(), verbose);

	// Create a remote thread to load the module
	HANDLE thread = NULL;
	if (hndProc && pAlloc)
		thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, 0);
	THROWIF(thread == NULL, GetLastError(), verbose);
	SafeFreeHandle(thread);

	SafeFreeHandle(hndProc);
	return ERROR_SUCCESS;
}

HRESULT eject(char* processName, int pid, char* dllpath, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes((LPCSTR)dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY, 
		GetLastError(), verbose);

	// Get process ID by name if pid is unknown
	DWORD procID = pid == -1 ? getProcessByName(processName) : pid;
	THROWIF(!procID, GetLastError(), verbose);

	// Search the HMODULE library
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);
	MODULEENTRY32 ModEntry = { sizeof(ModEntry) };
	_Bool found = FALSE;
	THROWIF(snapshot == INVALID_HANDLE_VALUE, GetLastError(), verbose);

	for (BOOL bMoreMods = Module32First(snapshot, &ModEntry);
		bMoreMods; 
		bMoreMods = Module32Next(snapshot, &ModEntry))
	{
		found = !strcmp(ModEntry.szModule, dllpath) || !strcmp(ModEntry.szExePath, dllpath);
		if (found) break;
	}
	SafeFreeHandle(snapshot);
	THROWIF(!found, ERROR_MOD_NOT_FOUND, verbose);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD 
		| PROCESS_VM_OPERATION, FALSE, procID);
	THROWIF(!hndProc, GetLastError(), verbose);

	// Create a remote thread to terminate the module
	HANDLE thread = NULL;
	if (hndProc)
		thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, 
			ModEntry.modBaseAddr, 0, 0);
	SafeFreeHandle(hndProc);

	// Wait for the remote thread to terminate
	if (thread)
	{
		WaitForSingleObject(thread, INFINITE);

		// Return thread exit code
		DWORD exitCode;
		THROWIF(!GetExitCodeThread(thread, &exitCode), GetLastError(), verbose);
		SafeFreeHandle(thread);

		THROWIF(exitCode != TRUE, exitCode, verbose);
	}
	THROWIF(thread == NULL, GetLastError(), verbose);
	return ERROR_SUCCESS;
}
