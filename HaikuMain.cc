
#include <ctime>
#include <cstdlib>

#include "PretendoApp.h"

int
main (void)
{
	srand(static_cast<unsigned int>(time(0)));
	new PretendoApp;
	be_app->Run();
	delete be_app;
	
	return 0;
}
