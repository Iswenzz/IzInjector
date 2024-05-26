#include <greatest.h>
GREATEST_MAIN_DEFS();

extern SUITE(Suite_IzInjector);

int main(int argc, char** argv)
{
	GREATEST_MAIN_BEGIN();

	RUN_SUITE(Suite_IzInjector);

	GREATEST_MAIN_END();
}
