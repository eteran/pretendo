
#ifndef _PRETENDO_APP_
#define _PRETENDO_APP_

#include <Application.h>
#include <Path.h>
#include <Roster.h>

#include "AboutWindow.h"

/*
#include "PretendoWindow.h"
#include "misc.h"
*/

#define MSG_ROM_LOADED 'load'	// temporary

class PretendoApp : public BApplication {
	public:
	PretendoApp();
	
	public:
	virtual void RefsReceived(BMessage *message);
	virtual void ReadyToRun();
	virtual void AboutRequested();
	
	public:
	//PretendoWindow *fWindow { return fWindow; };
	
	private:
	BWindow *fWindow;
	//PretendoWindow *fWindow;
};


#endif // _PRETENDO_APP
