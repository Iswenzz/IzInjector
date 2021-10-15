#pragma warning(disable: 6335)
#include "IzInjector.h"
#include "Utils.h"

#include <greatest.h>
#include <Windows.h>

PROCESS_INFORMATION OpenNotepad()
{
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    CreateProcess("C:\\Windows\\notepad.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    return pi;
}

void CloseNotepad(PROCESS_INFORMATION pi)
{
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void SyncDestroyDialog()
{
    while (TRUE)
    {
        HWND iHandle = FindWindow(NULL, "DLL");
        if (iHandle)
        {
            SendMessage(iHandle, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;
        }
        Sleep(200);
    }
}

TEST test_inject() 
{
    LPPROCESS_INFORMATION proc = Inject("notepad.exe", 0, "IzInjectorFixture.dll", TRUE);
    SyncDestroyDialog();

    ASSERT(proc);
    FreeProcInfo(proc);
}

TEST test_eject()
{
    LPPROCESS_INFORMATION proc = Eject("notepad.exe", 0, "IzInjectorFixture.dll", FALSE, TRUE);
    SyncDestroyDialog();
    
    ASSERT(proc);
    FreeProcInfo(proc);
}

SUITE(Suite_IzInjector)
{
    // Fixture DLL location and notepad for testing it
    SetCurrentDirectory("bin");
    PROCESS_INFORMATION process = OpenNotepad();

    RUN_TEST(test_inject);
    RUN_TEST(test_eject);

    CloseNotepad(process);
}
