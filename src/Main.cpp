//#include "tests/Test.h"
//#include "tests/LoadLevelTest.h"
//#include "tests/Deferred.h"

/*
#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
*/

//RUN(Test)
//RUN(LoadLevelTest)
//RUN(Deferred)

/*
#include "tests/Test.h"

void main(int argc, char *argv[])
{
	Test test;
	test.run();
}
*/

//#include "tests/Game.h"
#include "tests/Test.h"

void main(int argc, char *argv[])
{
	Test game;
	game.run();
}

