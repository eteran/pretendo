
#include "VideoScreen.h"


static status_t error;


VideoScreen::VideoScreen (PretendoWindow *owner)
	: BWindowScreen ("Pretendo Fullscreen", B_8_BIT_640x480, &error),
	fOwner (owner)
{
	if (error < B_OK) {
		PostMessage (B_QUIT_REQUESTED, this);
	}
}


VideoScreen::~VideoScreen()
{
	Hide();
	Sync();
}


void
VideoScreen::MessageReceived (BMessage *message)
{
	switch (message->what) {
		case B_KEY_DOWN:
			int8 key;
			
			if ((message->FindInt8 ("byte", 0, &key) == B_OK) && key == B_ESCAPE) {
				fOwner->PostMessage (MSG_LEAVE_FULLSCREEN);
			}
			break;
			
		default:
			BWindowScreen::MessageReceived (message);
	}
}


bool
VideoScreen::QuitRequested (void)
{
	return true;
}


void
VideoScreen::ScreenConnected (bool connected)
{
	if (connected == true) {
		status_t error;
		error = SetSpace (B_8_BIT_640x480);
		
		if (error != B_OK) {
			PostMessage (B_QUIT_REQUESTED, this);
		}
		
		fRowBytes = CardInfo()->bytes_per_row;
		fBits = reinterpret_cast<uint8 *>(CardInfo()->frame_buffer);
	}
}
