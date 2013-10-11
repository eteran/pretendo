

#include "AudioView.h"




AudioView::AudioView()
	: BView(BRect(0,0,0,0), "_audio", B_FOLLOW_NONE, B_WILL_DRAW)
{
}


AudioView::~AudioView()
{
}


void
AudioView::AttachedToWindow (void)
{
	ResizeTo(400, 300);
	SetViewColor (0, 0, 0);
	
}




void 
AudioView::Draw (BRect updateRect)
{
	BView::Draw(updateRect);
}




