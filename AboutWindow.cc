
#include "AboutWindow.h"

#include <Roster.h>
#include <TranslationUtils.h>
#include <View.h>
#include <Resources.h>
#include <Entry.h>
#include <Path.h>
#include <TextView.h>
#include <cstdio>
#include <cstring>


AboutView::AboutView(BRect frame)
	: BView (frame, "_about_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	fIcon = BTranslationUtils::GetBitmap('bits', "About Icon");

	// FIXME: we don't know why this does not work
	//fBanner = BTranslationUtils::GetBitmap('bits', "About Banner");
	//fBanner = BTranslationUtils::GetBitmap('bits', 3);
	
	
	
}


AboutView::~AboutView()
{
}


void
AboutView::AttachedToWindow (void)
{
	SetViewColor (ui_color(B_PANEL_BACKGROUND_COLOR));
	
	
	BRect r (0, 0, 0, 0);
	BButton *button = new BButton (r, "_okay", "Okay ", new BMessage (0x6532));
	button->ResizeToPreferred();
	button->MakeDefault(true);
	
	r = Bounds();
	button->MoveTo ((r.Width() - button->Frame().Width()) / 2 , 
					(r.bottom - button->Frame().Height() - 12)
	);
	
	r.Set(53, 50, r.right - 10, r.bottom - 50);
	BTextView *textview = new BTextView (r, "_textview", 
		BRect(3, 3, r.Width() - 3, r.Height() - 3), 
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
		
	BFont f = be_plain_font;
	f.SetSize(11.0f);
	textview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	textview->SetFontAndColor(&f);	
	
	char const *text = "A freeware, portable Nintendo NES emulator\n\n"
	"Version:\t000000\n"
	"Built:\t" __DATE__ " " __TIME__
	"\n\nWritten by: Eli Dayan and Evan Teran\n\n"
	"\"Nintendo\" and \"Nintendo Entertainment System\" are registered trademarks of "
	"Nintendo Co., Ltd";
	textview->SetText(text, strlen(text));
	
	AddChild(textview);
	AddChild(button);
}


void
AboutView::Draw (BRect updateRect)
{
	(void)updateRect;
	BRect r = Bounds(); 
    r.right = 30;
    SetHighColor(tint_color(ViewColor(), B_DARKEN_1_TINT)); 
  	FillRect (r); 
    SetDrawingMode(B_OP_OVER); 
    DrawBitmap (fIcon, BPoint(18, 6));
    r = Bounds();
    r.left = 53.0;
    r.right = Bounds().right;
    DrawBitmap (fBanner, BPoint( (r.Width() - 53) / 2, 11));
    
    SetDrawingMode(B_OP_COPY); 
}


AboutWindow::AboutWindow()
	: BWindow (BRect (0,0,0,0), "About Window", B_MODAL_WINDOW, 
		B_NOT_CLOSABLE | B_NOT_RESIZABLE)
{
	ResizeTo (340, 280);
	MoveTo ((BScreen().Frame().Width() - Frame().Width()) / 2,
		     (BScreen().Frame().Height() - Frame().Height()) / 2);
	
	fAboutView = new AboutView(Bounds());
	AddChild(fAboutView);
}


AboutWindow::~AboutWindow()
{
}


bool
AboutWindow::QuitRequested (void)
{
	return true;
}


void
AboutWindow::MessageReceived (BMessage *message)
{
	if (message->what == 0x6532) {
		PostMessage(B_QUIT_REQUESTED);
	} else {
		BWindow::MessageReceived (message);
	}
}

