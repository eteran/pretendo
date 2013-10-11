
#include "AudioWindow.h"
#include "AudioView.h"

class AudioWindow;


AudioWindow::AudioWindow(PretendoWindow *parent)
	: BWindow(BRect(0,0,0,0),"Audio View", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fAudioView(NULL)
{
	ResizeTo(400, 100);
	CenterOnScreen();
	
	fAudioView = new AudioView();
	AddChild(fAudioView);
	fAudioView->Invalidate();
}



AudioWindow::~AudioWindow()
{
}


bool 
AudioWindow::QuitRequested (void)
{
	Hide();
	return false;
}

void 
AudioWindow::MessageReceived (BMessage *message)
{
	BWindow::MessageReceived(message);
}
