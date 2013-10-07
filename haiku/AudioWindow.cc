
#include <View.h>


AudioWindow::AudioWindow()
	: BWindow(BRect(0,0,0,0),"Audio View", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fAudioView(NULL)
{
}



AudiowWindow::~AudioWindow()
{
}


bool 
AudiowWindow::QuitRequested (void)
{
	return true;
}

void 
AudioView::MessageReceived (BMessage *message)
{
}
