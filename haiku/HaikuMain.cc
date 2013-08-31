
#include <ctime>
#include <cstdlib>

#include "PretendoApp.h"

int
main (void)
{
	srand(static_cast<unsigned int>(time(0)));
	PretendoApp *app = new PretendoApp();
	app->Run();
	delete be_app;
	
	return 0;
}
