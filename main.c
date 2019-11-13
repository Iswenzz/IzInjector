#include "IzInjector.h"
#include <stdio.h>

void pexit(char *reason, int err_no)
{
	printf(reason);
	exit(err_no);
}

HRESULT wmain(int argc, wchar_t *argv[])
{
	if (argc < 3) 
		pexit("Usage: IzInjector.exe <processName> <dllPath>\n", ERROR_BAD_ARGUMENTS);

	return inject(argv[1], argv[2]);
}
