/*
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
*/

#include "tests/Test.h"
#include "tests/Game.h"
#include "tests/Deferred1.h"
#include "tests/Deferred2.h"

void main(int argc, char *argv[])
{
	//Test test;
	//Game test;
	//Deferred1 test;
	Deferred2 test;
	test.run();
}

