#include "IzInjector.h"

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

	CloseHandle(hPID);
	return foundPID;
}

void FreeProcInfo(LPPROCESS_INFORMATION procInfo)
{
	CloseHandle(procInfo->hThread);
	CloseHandle(procInfo->hProcess);
	free(procInfo);
}

LPPROCESS_INFORMATION Inject(const char *processName, int pid, const char *dllpath, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes(dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY,
		GetLastError(), verbose);
	VPRINTF("[INFO] Valid file attribute %d\n", dwAttrib);

	// Get absolute path
	TCHAR absolutePath[MAX_PATH];
	GetFullPathName(dllpath, MAX_PATH, absolutePath, NULL);

	// Get process ID by name if pid is unknown
	DWORD procID = !pid ? GetProcessByName((char*)processName) : pid;
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
	{
		pAlloc = VirtualAllocEx(hndProc, NULL, (strlen(absolutePath) + 1) * sizeof(char),
			MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	ThrowIf(pAlloc == NULL, GetLastError(), verbose);
	VPRINTF("[INFO] Library path alloc %p\n", pAlloc);
	if (hndProc && pAlloc)
	{
		THROWIF(!WriteProcessMemory(hndProc, pAlloc, absolutePath,
			(strlen(absolutePath) + 1) * sizeof(char), NULL),
			GetLastError(), verbose);
	}

	// Create a remote thread to load the module
	HANDLE thread = NULL;
	DWORD threadID = 0;
	if (hndProc && pAlloc)
	{
		thread = CreateRemoteThread(hndProc, NULL, 0,
			(LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, &threadID);
	}
	THROWIF(thread == NULL, GetLastError(), verbose);
	VPRINTF("[INFO] Created thread handle %p\n", thread);

	// Create a process info
	LPPROCESS_INFORMATION procInfo = (LPPROCESS_INFORMATION)malloc(sizeof(PROCESS_INFORMATION));
	procInfo->hProcess = hndProc;
	procInfo->dwProcessId = procID;
	procInfo->hThread = thread;
	procInfo->dwThreadId = threadID;
	return procInfo;
}

LPPROCESS_INFORMATION Eject(const char* processName, int pid, const char* dllpath,
	_Bool waitForExit, _Bool verbose)
{
	// Check if DLL path is valid
	DWORD dwAttrib = GetFileAttributes(dllpath);
	THROWIF(dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY,
		GetLastError(), verbose);
	VPRINTF("[INFO] Valid file attribute %d\n", dwAttrib);

	// Get absolute path
	TCHAR absolutePath[MAX_PATH];
	GetFullPathName(dllpath, MAX_PATH, absolutePath, NULL);

	// Get process ID by name if pid is unknown
	DWORD procID = !pid ? GetProcessByName((char*)processName) : pid;
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
		found = !strcmp(ModEntry.szModule, absolutePath) || !strcmp(ModEntry.szExePath, absolutePath);
		if (found) break;
	}
	if (snapshot) CloseHandle(snapshot);
	THROWIF(!found, ERROR_MOD_NOT_FOUND, verbose);
	VPRINTF("[INFO] Found module handle %p\n", ModEntry.modBaseAddr);

	// Open target process
	HANDLE hndProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD
		| PROCESS_VM_OPERATION, FALSE, procID);
	THROWIF(!hndProc, GetLastError(), verbose);
	VPRINTF("[INFO] Target process handle %p\n", hndProc);

	// Create a remote thread to terminate the module
	HANDLE thread = NULL;
	DWORD threadID = 0;
	if (hndProc)
	{
		thread = CreateRemoteThread(hndProc, NULL, 0,
			(LPTHREAD_START_ROUTINE)FreeLibrary, ModEntry.modBaseAddr, 0, &threadID);
	}
	VPRINTF("[INFO] Thread handle %p\n", thread);

	// Wait for the remote thread to terminate
	if (waitForExit && thread)
	{
		VPRINTF("[INFO] Wait for the remote thread to terminate the module...\n");
		WaitForSingleObject(thread, INFINITE);

		// Return thread exit code
		DWORD exitCode;
		THROWIF(!GetExitCodeThread(thread, &exitCode), GetLastError(), verbose);
		VPRINTF("[INFO] Target exit code %d\n", exitCode);

		THROWIF(exitCode != TRUE, exitCode, verbose);
	}
	THROWIF(thread == NULL, GetLastError(), verbose);

	// Create a process info
	LPPROCESS_INFORMATION procInfo = (LPPROCESS_INFORMATION)malloc(sizeof(PROCESS_INFORMATION));
	procInfo->hProcess = hndProc;
	procInfo->dwProcessId = procID;
	procInfo->hThread = thread;
	procInfo->dwThreadId = threadID;
	return procInfo;
}

HRESULT ThrowIf(_Bool condition, HRESULT code, _Bool verbose)
{
	if (condition)
	{
		// Get the error message from HRESULT
		if (verbose)
		{
			LPSTR wszMsgBuff = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&wszMsgBuff, 0, NULL);
			printf("ERROR[%d]: %s\n", code, wszMsgBuff);
			LocalFree(wszMsgBuff);
		}
		return code;
	}
	return S_OK;
}
