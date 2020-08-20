#include "IzInjector.h"
#include "utility.h"
#include <stdio.h>
#include <argparse.h>

typedef enum
{
	COMMAND_INJECT = 1,
	COMMAND_EJECT = 2,
} ProcessCommand;

/// <summary>
/// The CLI usage message.
/// </summary>
static const char *usage[] = {
	"IzInjector <commands> [[--] args] <dlls>",
	NULL
};

/// <summary>
/// Entry point of the program.
/// </summary>
/// <param name="argc">CLI arg count.</param>
/// <param name="argv">CLI arg strings.</param>
/// <returns>Injection return code.</returns>
int main(int argc, const char **argv)
{
	HRESULT exitCode = 0;
	int pid = -1, verbose = 0;
	char *name = NULL;
	ProcessCommand command = 0;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("IzInjector commands"),
		OPT_BIT(0, "inject", &command, "DLL Injection mode", NULL, COMMAND_INJECT),
		OPT_BIT(0, "eject", &command, "DLL Ejection mode", NULL, COMMAND_EJECT),
		OPT_GROUP("IzInjector options"),
		OPT_STRING('n', "name", &name, "The target process name"),
		OPT_INTEGER('p', "pid", &pid, "The target process ID"),
		OPT_BOOLEAN('v', "verbose", &verbose, "Log the command process."),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, "\nThis application is intended to allow users to inject a Dynamic-Link Library (DLL) file into another process in memory.", NULL);
	argc = argparse_parse(&argparse, argc, argv);

	// CLI Version
	if (argc != 0)
	{
		// Check if either proc pid or name is defined
		if (pid == -1 && name == NULL)
		{
			printf("You must use either --pid or --name option to target a process. Type --help for more informations!\n");
			return ERROR_BAD_ARGUMENTS;
		}
		VPRINTF("\nCommand: %d\n", command);
		if (pid == -1) VPRINTF("Name: %s", name);
		if (name == NULL) VPRINTF("PID: %d", pid);

		// Process all files
		for (int i = 0; i < argc; i++)
		{
			VPRINTF("\nDLLs[%d]: %s\n", i, argv[i]);
			switch (command)
			{
				case COMMAND_INJECT:
					exitCode = inject(name, pid, (char *)argv[i], verbose);	
					break;
				case COMMAND_EJECT:
					exitCode = eject(name, pid, (char*)argv[i], verbose);
					break;

				default:
					printf("Wrong command! Type --help for more informations.\n");
					return ERROR_BAD_ARGUMENTS;
			}
		}
	}
	return exitCode;
}
