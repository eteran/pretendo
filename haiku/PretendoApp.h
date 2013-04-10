
#ifndef _PRETENDO_APP_
#define _PRETENDO_APP_

#include <Application.h>
#include "PretendoWindow.h"


class PretendoApp : public BApplication {
	public:
	PretendoApp();
	
	public:

	virtual void ReadyToRun();
	
	public:
	PretendoWindow *Window (void) { return fWindow; };
	
	private:
	PretendoWindow *fWindow;
};


#endif // _PRETENDO_APP
