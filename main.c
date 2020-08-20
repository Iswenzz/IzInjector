#include "IzInjector.h"
#include <stdio.h>
#include <argparse.h>

typedef enum
{
	METHOD_INJECT = 1,
	METHOD_EJECT = 2,
} ProcessMethod;

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
	int pid = -1, verbose = 0;
	char *name = NULL;
	ProcessMethod method = 0;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("IzInjector commands"),
		OPT_BIT(0, "inject", &method, "DLL Injection mode", NULL, METHOD_INJECT),
		OPT_BIT(0, "eject", &method, "DLL Ejection mode", NULL, METHOD_EJECT),
		OPT_GROUP("IzInjector options"),
		OPT_STRING('n', "name", &name, "The target process name"),
		OPT_INTEGER('p', "pid", &pid, "The target process ID"),
		OPT_BOOLEAN('v', "verbose", &verbose, "Log the method process."),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, "\nThis application is intended to allow users to inject a Dynamic-Link Library (DLL) file into another process in memory.", NULL);

	argc = argparse_parse(&argparse, argc, argv);
	// CLI Version
	if (argc != 0)
	{
		if (pid == -1 && name == NULL)
		{
			printf("You must use either --pid or --name option to target a process. Type --help for more informations!");
			return ERROR_BAD_ARGUMENTS;
		}
		for (int i = 0; i < argc; i++)
		{
			switch (method)
			{
				case METHOD_INJECT: return inject(name, argv[i], pid, verbose);
				case METHOD_EJECT: return eject(name, argv[i], pid, verbose);

				default:
					printf("Wrong command! Type --help for more informations.");
					return ERROR_BAD_ARGUMENTS;
			}
		}
	}
	return 0;
}
