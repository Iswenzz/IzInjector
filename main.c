#include "IzInjector.h"
#include <stdio.h>

/// <summary>
/// Entry point of the program.
/// </summary>
/// <returns>Injection return code.</returns>
int main()
{
	LPWSTR *argv = NULL;
	int argc;
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argv == NULL || argc < 3)
	{
		printf("Usage: IzInjector.exe <processName> <dllPath>\n");
		return ERROR_BAD_ARGUMENTS;
	}

	DWORD injectCode = argv ? inject(argv[1], argv[2]) : -1;
	if (injectCode != ERROR_SUCCESS)
		printf("ERROR: %d", injectCode);
	return injectCode;
}
