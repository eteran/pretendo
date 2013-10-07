

#include "AudioView.h"

AudioView::AudioView (BRect frame)
	: BView(frame, "_audio", B_FOLLOW_NONE, B_WILL_DRAW)
{
}

AudioViewWindow::~AudioViewWindow()
{
}


AudioView::~AudioView()
{
}

void
AudioView::AttachedToWindow (void)
{
}


AudioViewWindow::AudioViewWindow()
	: BWindow(BRect(0,0,0,0),"Audio View", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fAudioView(NULL)
{
}

void 
AudioView::Draw (BRect updateRect)
{
}

bool 
AudioViewWindow::QuitRequested (void)
{
	return true;
}

void AudioViewWindow::MessageReceived (BMessage *message)
{
}
