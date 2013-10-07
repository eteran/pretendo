

#include "AudioView.h"




AudioView::AudioView()
	: BView(Bounds(), "_audio", B_FOLLOW_NONE, B_WILL_DRAW)
{
	
}


AudioView::~AudioView()
{
}


void
AudioView::AttachedToWindow (void)
{
	SetViewColor (0, 0, 0);
}




void 
AudioView::Draw (BRect updateRect)
{
}




