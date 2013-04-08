
#include <Path.h>

#include "PretendoApp.h"
#include "AboutWindow.h"


PretendoApp::PretendoApp()
	: BApplication("application/x-vnd.scanty-Pretendo") 
{ 	
	//fWindow = new PretendoWindow;
	//fWindow->Show();
	
	AboutWindow *aw = new AboutWindow;
	aw->Show();
}


void 
PretendoApp::RefsReceived(BMessage *message)
{
     switch (message->what) {
     case B_REFS_RECEIVED:
         {
             entry_ref ref;
             if (message->FindRef ("refs", 0, &ref) == B_OK) {
                 BEntry entry;
                 BPath path;
                 entry.SetTo (&ref, true);
                 entry.GetRef (&ref);
                 entry.GetPath (&path);

                 BMessage *msg = new BMessage (MSG_ROM_LOADED);
                 msg->AddString ("path", path.Path());
                 //fWindow->PostMessage (msg);
                 delete msg;
             }
         }
         break;

     default:
         BApplication::RefsReceived (message);
     }
}


void
PretendoApp::ReadyToRun()
{
}


void 
PretendoApp::AboutRequested()
{
     (new AboutWindow)->Show();
}
