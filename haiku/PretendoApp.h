
#ifndef _PRETENDO_APP_
#define _PRETENDO_APP_

#include <Application.h>
#include "PretendoWindow.h"
#include "AboutWindow.h"

class PretendoApp : public BApplication {
	public:
	PretendoApp();
	
	public:
	virtual void ReadyToRun();
	virtual void AboutRequested (void);
	virtual void RefsReceived (BMessage *message);
	
	public:
	PretendoWindow *Window (void) { return fWindow; };
	
	private:
	PretendoWindow *fWindow;
};


#endif // _PRETENDO_APP
