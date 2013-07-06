

#include "PretendoApp.h"
#include <Path.h>


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

void
PretendoApp::AboutRequested (void)
{
	(new AboutWindow)->Show();
}

void
PretendoApp::RefsReceived (BMessage *message)
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
                 fWindow->PostMessage (msg);
                 delete msg;
             }
         }
         break;
	}
	
	BApplication::RefsReceived(message);
}

