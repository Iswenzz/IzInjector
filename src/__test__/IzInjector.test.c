#include "IzInjector.h"

#include <Windows.h>
#include <greatest.h>

PROCESS_INFORMATION OpenNotepad()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };

	CreateProcess("C:\\Windows\\notepad.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	return pi;
}

void CloseNotepad(PROCESS_INFORMATION pi)
{
	TerminateProcess(pi.hProcess, 0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void CloseFixtureDialog()
{
	while (TRUE)
	{
		HWND handle = FindWindow(NULL, "DLL");
		if (handle)
		{
			SendMessage(handle, WM_SYSCOMMAND, SC_CLOSE, 0);
			break;
		}
		Sleep(200);
	}
}

TEST test_inject()
{
	PROCESS_INFORMATION proc = Inject("notepad.exe", 0, "IzInjector.Fixture.dll");
	CloseFixtureDialog();

	ASSERT(proc.dwProcessId);
	PASS();
}

TEST test_eject()
{
	PROCESS_INFORMATION proc = Eject("notepad.exe", 0, "IzInjector.Fixture.dll", FALSE);

	ASSERT(proc.dwProcessId);
	PASS();
}

SUITE(Suite_IzInjector)
{
	SetCurrentDirectory("fixture/Debug");
	PROCESS_INFORMATION process = OpenNotepad();

	RUN_TEST(test_inject);
	RUN_TEST(test_eject);

	CloseNotepad(process);
}
