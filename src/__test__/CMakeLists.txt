if(BUILD_TESTING)
	target_sources(IzInjector.Tests PRIVATE
		IzInjector.test.c
		main.test.c)
endif()
