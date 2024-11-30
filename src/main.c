#include "IzInjector.h"

#include <argtable3.h>
#include <stdio.h>

#define PROGRAM_NAME "IzInjector"
#define PROGRAM_VERSION "1.0.5"
#define PROGRAM_USAGE PROGRAM_NAME " --mode INJECT --name notepad.exe [DLLs Paths]\n"

/// <summary>
/// Display the help message.
/// </summary>
/// <param name="argtable">CLI arg table.</param>
void DisplayHelp(void* argtable)
{
	IZ_PRINTF("\nUsage: %s", PROGRAM_USAGE);
	arg_print_glossary(stdout, argtable, "\t%-25s %s\n");
	IZ_PRINT("");
}

/// <summary>
/// Entry point of the program.
/// </summary>
/// <param name="argc">CLI arg count.</param>
/// <param name="argv">CLI args.</param>
/// <returns></returns>
int main(int argc, const char** argv)
{
	arg_lit_t* help = arg_litn(NULL, "help", 0, 1, "Display the program help message");
	arg_lit_t* version = arg_litn(NULL, "version", 0, 1, "Display the program version");
	arg_str_t* mode = arg_strn("m", "mode", "", 1, 1, "DLL injection mode [INJECT or EJECT]");
	arg_str_t* name = arg_strn("n", "name", "", 0, 1, "The target process name [notepad.exe]");
	arg_int_t* pid = arg_intn("p", "pid", "", 0, 1, "The target process ID");
	arg_file_t* files = arg_filen(NULL, NULL, "<files>", 1, 20, "DLL Files");
	arg_end_t* end = arg_end(20);

	void* argtable[] = { help, version, mode, name, pid, files, end };
	int errors = arg_parse(argc, argv, argtable);
	int processID = pid->count ? *pid->ival : 0;
	PROCESS_INFORMATION info = { 0 };

	// Conditions
	if (help->count)
	{
		DisplayHelp(argtable);
		return 0;
	}
	if (version->count)
	{
		IZ_PRINT(PROGRAM_NAME " " PROGRAM_VERSION);
		return 0;
	}
	if (!mode->count || !files->count)
	{
		IZ_PRINT("\nYou must use --mode INJECT or --mode EJECT and <files>.");
		DisplayHelp(argtable);
		return ERROR_BAD_ARGUMENTS;
	}
	if (!pid->count && !name->count)
	{
		IZ_PRINT("\nYou must use either --pid or --name option to target a process.");
		DisplayHelp(argtable);
		return ERROR_BAD_ARGUMENTS;
	}
	// Process all files
	for (int i = 0; i < files->count; i++)
	{
		if (!stricmp(*mode->sval, "INJECT"))
			info = Inject(*name->sval, processID, *files[i].filename);
		if (!stricmp(*mode->sval, "EJECT"))
			info = Eject(*name->sval, processID, *files[i].filename, TRUE);

		if (!info.hProcess)
			return 1;
	}
	return 0;
}
