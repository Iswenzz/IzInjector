#include "IzInjector.h"
#include <stdio.h>

void pexit(const char *reason, int err_no)
{
	printf(reason);
	exit(err_no);
}

int main()
{
	LPWSTR *argv;
	int argc;
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argv == NULL || argc < 3) 
		pexit("Usage: IzInjector.exe <processName> <dllPath>\n", ERROR_BAD_ARGUMENTS);

	return inject(argv[1], argv[2]);
}
