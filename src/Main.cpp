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

//#include "tests/Test.h"
//#include "tests/Game.h"
#include "tests/Deferred.h"

void main(int argc, char *argv[])
{
	//Test test;
	//Game test;
	Deferred test;
	test.run();
}

