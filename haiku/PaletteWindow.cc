


#include <String.h>
#include <Alert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>

#include "PaletteWindow.h"
#include "PaletteView.h"


PaletteWindow::PaletteWindow()
	: BWindow(BRect(0, 0, 0, 0), "Palette Info", B_FLOATING_WINDOW_LOOK, 						B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
{
	
	ResizeTo(640, 320);
	CenterOnScreen();
	
	
	fPaletteView = new PaletteView(BRect(8, 8, 343, 296), 64, 16);
	AddChild(fPaletteView);
}


PaletteWindow::~PaletteWindow()
{
}




void
PaletteWindow::MessageReceived (BMessage *message)
{
	
	BWindow::MessageReceived(message);
}


bool
PaletteWindow::QuitRequested (void)
{
	return true;
}
