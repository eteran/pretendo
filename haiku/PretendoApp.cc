

#include "PretendoApp.h"


PretendoApp::PretendoApp()
	: BApplication("application/x-vnd.scanty-Pretendo") 
{ 	
}

void
PretendoApp::ReadyToRun()
{
	fWindow = new PretendoWindow();
	fWindow->Show();
}


