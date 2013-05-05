
#include "CartInfoWindow.h"

#include <String.h>
#include <Alert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/uuid/sha1.hpp>
#include <libxml/parser.h>
#include <stdio.h>


#include "NES.h"


using nes::cart;


CartInfoWindow::CartInfoWindow()
	: BWindow(BRect(0, 0, 0, 0), "Cart Info", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
	B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fCartInfoView(NULL)
{
	LIBXML_TEST_VERSION;
	
	ResizeTo(640, 320);
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
	switch (message->what) {
		default:
			BWindow::MessageReceived (message);
			break;
		
		case 'OKAY':
			Quit();
			break;
	}
}


bool
CartInfoWindow::QuitRequested (void)
{
	return true;
}


// gcc -I/boot/common/include/libxml2 -I/pretendo/boost_1_50_0 rominfo.cc -o rominfo -lxml2 -lstdc++
