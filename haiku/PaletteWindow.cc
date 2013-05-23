
#include "PaletteWindow.h"


PaletteWindow::PaletteWindow (PretendoWindow *parent)
	: BWindow(BRect(0, 0, 0, 0), "Adjust Palette", B_FLOATING_WINDOW_LOOK, 						B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fPretendoWindow(parent)
{
	
	ResizeTo(356, 292);
	CenterOnScreen();
	
	BView *backView = new BView(Bounds(), "_back_view", B_FOLLOW_ALL, 0);
	backView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild (backView);
	
	fPaletteView = new PaletteView(fPretendoWindow, BRect(8, 8, 343, 296), 64, 16);
	backView->AddChild(fPaletteView);
}


PaletteWindow::~PaletteWindow()
{
}



void
PaletteWindow::MessageReceived (BMessage *message)
{	
	BWindow::MessageReceived(message);
}


void
PaletteWindow::MenusEnded (void)
{
	fPaletteView->Invalidate();	// update palette
}


bool
PaletteWindow::QuitRequested (void)
{
	Hide();
	return false;
}
