#include "IzInjector.h"

PROCESS_INFORMATION Inject(const char *processName, int pid, const char *dllpath)
{
	// Check file path
	int attributes = GetFileAttributes(dllpath);
	IZ_ASSERT(attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY), "File not found");

	// Get absolute path
	char path[MAX_PATH];
	GetFullPathName(dllpath, MAX_PATH, path, NULL);

	// Get process entry
	PROCESSENTRY32 entry = GetProcessEntry((char *)processName);
	int processID = pid ? pid : entry.th32ProcessID;
	IZ_ASSERT(processID, "Process not found");
	IZ_INFO("Process %d", processID);

	// Open process
	HANDLE process = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
		TRUE, processID);
	IZ_ASSERT(process, "Cannot open process");
	IZ_INFO("Process handle %p", process);

	// Alloc the dll path to the target process
	LPVOID pAlloc = VirtualAllocEx(process, NULL, (strlen(path) + 1) * sizeof(char), MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);
	IZ_ASSERT(pAlloc, "Cannot allocate memory to process");
	WriteProcessMemory(process, pAlloc, path, (strlen(path) + 1) * sizeof(char), NULL);
	IZ_INFO("Library path alloc %p", pAlloc);

	// Create a remote thread to load the module
	int threadID = 0;
	HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pAlloc, 0, &threadID);
	IZ_ASSERT(thread, "Cannot create a remote thread to process");
	IZ_INFO("Thread handle %p", thread);

	// Create a process info
	PROCESS_INFORMATION info = { 0 };
	info.hProcess = process;
	info.dwProcessId = processID;
	info.hThread = thread;
	info.dwThreadId = threadID;
	return info;
}

PROCESS_INFORMATION Eject(const char *processName, int pid, const char *dllpath, _Bool waitForExit)
{
	// Check file path
	int attributes = GetFileAttributes(dllpath);
	IZ_ASSERT(attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY), "File not found");

	// Get absolute path
	char path[MAX_PATH];
	GetFullPathName(dllpath, MAX_PATH, path, NULL);

	// Get process entry
	PROCESSENTRY32 entry = GetProcessEntry((char *)processName);
	int processID = pid ? pid : entry.th32ProcessID;
	IZ_ASSERT(processID, "Process not found");
	IZ_INFO("Process %d", processID);

	// Get module entry
	MODULEENTRY32 module = GetModuleEntry(processID, path);
	IZ_ASSERT(module.th32ModuleID, "Module not found");
	IZ_INFO("Module handle %p", module.modBaseAddr);

	// Open process
	HANDLE process =
		OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, processID);
	IZ_ASSERT(process, "Cannot open process");
	IZ_INFO("Process handle %p", process);

	// Create a remote thread to terminate the module
	int threadID = 0;
	HANDLE thread =
		CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, module.modBaseAddr, 0, &threadID);
	IZ_ASSERT(thread, "Cannot create a remote thread to process");
	IZ_INFO("Thread handle %p", thread);

	// Wait for the remote thread to terminate
	if (waitForExit)
	{
		IZ_INFO("Waiting for the remote thread to terminate...");
		WaitForSingleObject(thread, INFINITE);

		int code = 0;
		GetExitCodeThread(thread, &code);
		IZ_INFO("Thread exit code %d", code);
	}

	// Create a process info
	PROCESS_INFORMATION info = { 0 };
	info.hProcess = process;
	info.dwProcessId = processID;
	info.hThread = thread;
	info.dwThreadId = threadID;
	return info;
}

PROCESSENTRY32 GetProcessEntry(char *processName)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry = { sizeof(entry) };
	PROCESSENTRY32 fail = { 0 };

	do
	{
		if (!strcmp(entry.szExeFile, processName))
		{
			CloseHandle(snapshot);
			return entry;
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);
	return fail;
}

MODULEENTRY32 GetModuleEntry(int process, char *moduleName)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process);
	MODULEENTRY32 entry = { sizeof(entry) };
	MODULEENTRY32 fail = { 0 };

	do
	{
		if (!strcmp(entry.szModule, moduleName) || !strcmp(entry.szExePath, moduleName))
		{
			CloseHandle(snapshot);
			return entry;
		}
	} while (Module32Next(snapshot, &entry));

	CloseHandle(snapshot);
	return fail;
}
