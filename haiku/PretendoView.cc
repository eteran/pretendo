// PretendoView.cc

#include <Alert.h>
#include <Entry.h>
#include <Path.h>

#include "PretendoView.h"

class PretendoWindow;

PretendoView::PretendoView (BRect frame, PretendoWindow *parent)
	: BView (frame, "_pretendo_view_", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
	fParent(parent)
{
}


PretendoView::~PretendoView()
{
}


void
PretendoView::DrawBitmap (BBitmap *bmp)
{
	BView::DrawBitmap(bmp);
}

void 
PretendoView::MessageReceived (BMessage *msg)
{
	if (msg->WasDropped()) {
		entry_ref ref;
		
		if (msg->FindRef ("refs", 0, &ref) == B_OK) {
			BEntry entry;
			BPath path;
			BMessage *msg;
			
			entry.SetTo(&ref, true);
			entry.GetPath(&path);
			
			msg = new BMessage(MSG_ROM_LOADED);
			msg->AddString("path", path.Path());
			fParent->PostMessage(msg);
			
			delete msg;
		}
	}
;

	
	BView::MessageReceived (msg);
}

