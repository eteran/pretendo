
#include	"ROMFilePanel.h"

ROMFilePanel::ROMFilePanel ()
	: BFilePanel (B_OPEN_PANEL, NULL, NULL, 0, false, NULL, NULL, true, true)
{
	Window()->Lock();
	
	SetRefFilter (new ROMFilter);
	
	// FIXME: hack.
	//SetPanelDirectory ("/void/roms/nes");
	//SetPanelDirectory ("/the dump/roms/nes");
	SetPanelDirectory ("/pretendo/roms");
	//fTempCart = new NESCart;
	
	Customize();
	
	Window()->Unlock();
}


ROMFilePanel::~ROMFilePanel()
{
}


void
ROMFilePanel::Customize (void)
{
	BWindow *window = this->Window();
	BView	*view = window->ChildAt (0);
	BView	*files = view->FindView ("PoseView");
	BView	*count = view->FindView ("CountVw");
	BView	*hscr = view->FindView ("HScrollBar");
	BView	*vscr = view->FindView ("VScrollBar");

	window->SetTitle ("Pretendo: Choose a ROM Image...");
	SetButtonLabel (B_DEFAULT_BUTTON, "Load");
}


void
ROMFilePanel::SelectionChanged (void)
{
	BFilePanel::SelectionChanged();
	#if 0
	entry_ref	ref;
	
	Rewind();
	
	if (GetNextSelectedRef (&ref) == B_OK) {
		BEntry entry (&ref);
		
		if (entry.IsDirectory() == false) {
			if (fPrevRef == ref) {
				return;
			}
			
			BPath path;
			entry.GetPath (&path);
			
			fTempCart->Load (path.Path());
			fROMInfoView->Refresh (fTempCart);
			fTempCart->Free();
			
			fPrevRef = ref;
		}
	}
	#endif
}
