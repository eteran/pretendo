
#include <libxml2/libxml/parser.h>

#include "CartInfoWindow.h"


CartInfoWindow::CartInfoWindow()
	: BWindow(BRect(0, 0, 0, 0), "Cart Info", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
	B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fCartInfoView(NULL)
{
	ResizeTo(720, 320);
	CenterOnScreen();
	
	BRect r = Bounds();
	r.right -= 16;
	fCartInfoView = new CartInfoView(r);
	BScrollView *sv = new BScrollView("_cart_info_sv", fCartInfoView, B_FOLLOW_ALL, 0, false, true);
	AddChild(sv);
}


CartInfoWindow::~CartInfoWindow()
{
}




void
CartInfoWindow::MessageReceived (BMessage *message)
{
	BWindow::MessageReceived (message);
}


bool
CartInfoWindow::QuitRequested (void)
{
	Hide();
	return false;
}
