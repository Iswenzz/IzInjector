#include "Utils.h"
#include <stdio.h>

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
