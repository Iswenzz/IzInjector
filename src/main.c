#include "IzInjector.h"

#include <argtable3.h>
#include <stdio.h>

#define PROGRAM_NAME "IzInjector"
#define PROGRAM_DESCRIPTION                                                                                            \
	"This application is intended to allow users to inject a Dynamic-Link Library (DLL) file into another process in " \
	"memory."
#define PROGRAM_USAGE PROGRAM_NAME " --mode INJECT --name notepad.exe [DLLs Paths]\n"

/// <summary>
/// Disaplay the help message.
/// </summary>
/// <param name="argtable">CLI arg table.</param>
void DisplayHelp(void* argtable)
{
	printf("\nUsage: %s\n", PROGRAM_USAGE);
	arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
	printf("\n");
}

/// <summary>
/// Entry point of the program.
/// </summary>
/// <param name="argc">CLI arg count.</param>
/// <param name="argv">CLI args.</param>
/// <returns>Injection return code.</returns>
int main(int argc, const char** argv)
{
	// Arguments
	arg_lit_t* help = arg_litn(NULL, "help", 0, 1, "Display the program help message");
	arg_lit_t* version = arg_litn(NULL, "version", 0, 1, "Display the program version");
	arg_lit_t* verb = arg_litn("v", "verbose", 0, 1, "Log the command process");

	arg_str_t* mode = arg_strn("m", "mode", "", 1, 1, "DLL Injection mode [INJECT or EJECT]");
	arg_str_t* name = arg_strn("n", "name", "", 0, 1, "The target process name [notepad.exe]");

	arg_int_t* pid = arg_intn("p", "pid", "", 0, 1, "The target process ID");
	arg_file_t* files = arg_filen(NULL, NULL, "<files>", 1, 20, "DLL Files");
	arg_end_t* end = arg_end(20);

	void* argtable[] = { help, version, verb, mode, name, pid, files, end };
	int nerrors = arg_parse(argc, argv, argtable);
	int PIDValue = pid->count ? *pid->ival : 0;
	_Bool verbose = verb->count;

	// Display help or error message
	if (nerrors || help->count)
	{
		DisplayHelp(argtable);
		if (nerrors)
		{
			arg_print_errors(stdout, end, PROGRAM_NAME);
			return ERROR_BAD_ARGUMENTS;
		}
		return 0;
	}

	// Check if either proc pid or name is defined
	if (!pid->count && !name->count)
	{
		DisplayHelp(argtable);
		printf("You must use either --pid or --name option to target a process.\n");
		return ERROR_BAD_ARGUMENTS;
	}
	if (pid->count)
		VPRINTF("Name: %s\n", *name->sval);
	else if (name->count)
		VPRINTF("PID: %d\n", *pid->ival);

	// Process all files
	for (int i = 0; i < files->count; i++)
	{
		VPRINTF("\n[INFO] %s %s\n", *mode->sval, *files[i].filename);

		if (!stricmp(*mode->sval, "INJECT"))
		{
			LPPROCESS_INFORMATION proc = Inject(*name->sval, PIDValue, *files[i].filename, verbose);
			if (!proc)
				return 1;
			FreeProcInfo(proc);
		}
		else if (!stricmp(*mode->sval, "EJECT"))
		{
			LPPROCESS_INFORMATION proc = Eject(*name->sval, PIDValue, *files[i].filename, TRUE, verbose);
			if (!proc)
				return 1;
			FreeProcInfo(proc);
		}
	}
	return 0;
}
