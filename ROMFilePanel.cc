
#include	"ROMFilePanel.h"

ROMFilePanel::ROMFilePanel ()
	: BFilePanel (B_OPEN_PANEL, NULL, NULL, 0, false, NULL, NULL, true, true)
{
	Window()->Lock();
	
	SetRefFilter (new ROMFilter);
	
	// FIXME: hack.
	//SetPanelDirectory ("/void/roms/nes");
	//SetPanelDirectory ("/the dump/roms/nes");
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
	#if 0
	BWindow *window = this->Window();
	BView	*view = window->ChildAt (0);
	BView	*files = view->FindView ("PoseView");
	BView	*count = view->FindView ("CountVw");
	BView	*hscr = view->FindView ("HScrollBar");
	BView	*vscr = view->FindView ("VScrollBar");

	window->SetTitle ("Pretendo: Choose a ROM Image...");
	files->ResizeBy (0, -200);
	count->MoveBy (0, -200);
	hscr->MoveBy (0, -200);
	vscr->ResizeBy (0, -200);
	SetButtonLabel (B_DEFAULT_BUTTON, "Load");
	
	BRect viewRect (count->Frame().left,
					count->Frame().bottom+16,
					vscr->Frame().right,
					count->Frame().bottom+16+180);
	fROMInfoView = new ROMInfoView (viewRect);					
	
	view->AddChild (fROMInfoView);
	#endif
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
