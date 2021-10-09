#include "IzInjector.h"
#include "Utility.h"

#include <stdio.h>
#include <TlHelp32.h>

DWORD GetProcessByName(char *processName)
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	DWORD foundPID = 0;

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

HRESULT Inject(char *processName, int pid, const char *dllpath, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes(dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY,
		GetLastError(), verbose);
	VPRINTF("[INFO] Valid file attribute %d\n", dwAttrib);

	// Get process ID by name if pid is unknown
	DWORD procID = !pid ? GetProcessByName(processName) : pid;
	THROWIF(!procID, ERROR_PROC_NOT_FOUND, verbose);
	VPRINTF("[INFO] Found procID %d\n", procID);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ
		| PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, TRUE, procID);
	THROWIF(hndProc == NULL, GetLastError(), verbose);
	VPRINTF("[INFO] Target process handle %p\n", hndProc);

	// Alloc the dll path string to the target process
	LPVOID pAlloc = NULL;
	if (hndProc)
		pAlloc = VirtualAllocEx(hndProc, NULL, (strlen(dllpath) + 1) * sizeof(char),
			MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	THROWIF(pAlloc == NULL, GetLastError(), verbose);
	VPRINTF("[INFO] Library path alloc %p\n", pAlloc);
	if (hndProc && pAlloc)
		THROWIF(!WriteProcessMemory(hndProc, pAlloc, dllpath, (strlen(dllpath) + 1) * sizeof(char), NULL),
			GetLastError(), verbose);

	// Create a remote thread to load the module
	HANDLE thread = NULL;
	if (hndProc && pAlloc)
		thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, 0);
	THROWIF(thread == NULL, GetLastError(), verbose);
	VPRINTF("[INFO] Created thread handle %p\n", thread);
	SafeFreeHandle(thread);

	SafeFreeHandle(hndProc);
	return ERROR_SUCCESS;
}

HRESULT Eject(char* processName, int pid, const char* dllpath, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes(dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY, 
		GetLastError(), verbose);
	VPRINTF("[INFO] Valid file attribute %d\n", dwAttrib);

	// Get process ID by name if pid is unknown
	DWORD procID = pid == -1 ? GetProcessByName(processName) : pid;
	THROWIF(!procID, GetLastError(), verbose);
	VPRINTF("[INFO] Found procID %d\n", procID);

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
	VPRINTF("[INFO] Found module handle %p\n", ModEntry.modBaseAddr);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD 
		| PROCESS_VM_OPERATION, FALSE, procID);
	THROWIF(!hndProc, GetLastError(), verbose);
	VPRINTF("[INFO] Target process handle %p\n", hndProc);

	// Create a remote thread to terminate the module
	HANDLE thread = NULL;
	if (hndProc)
		thread = CreateRemoteThread(hndProc, 0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, 
			ModEntry.modBaseAddr, 0, 0);
	SafeFreeHandle(hndProc);
	VPRINTF("[INFO] Thread handle %p\n", thread);

	// Wait for the remote thread to terminate
	if (thread)
	{
		VPRINTF("[INFO] Wait for the remote thread to terminate the module...\n");
		WaitForSingleObject(thread, INFINITE);

		// Return thread exit code
		DWORD exitCode;
		THROWIF(!GetExitCodeThread(thread, &exitCode), GetLastError(), verbose);
		SafeFreeHandle(thread);
		VPRINTF("[INFO] Target exit code %d\n", exitCode);

		THROWIF(exitCode != TRUE, exitCode, verbose);
	}
	THROWIF(thread == NULL, GetLastError(), verbose);
	return ERROR_SUCCESS;
}
