

#include <Application.h>
#include <Roster.h>
#include <TranslationUtils.h>
#include <View.h>
#include <TextView.h>

#include "AboutWindow.h"
#include "LinkView.h"


AboutView::AboutView(BRect frame)
	: BView (frame, "_about_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	fIcon = BTranslationUtils::GetBitmap('bits', "About Icon");
	fLogo = BTranslationUtils::GetBitmap('bits', "Pretendo Logo");
}


AboutView::~AboutView()
{
}


void
AboutView::AttachedToWindow (void)
{
	SetViewColor (ui_color(B_PANEL_BACKGROUND_COLOR));
	
	
	BRect r (0, 0, 0, 0);
	BButton *button = new BButton (r, "_okay", "Okay ", new BMessage ('OKAY'));
	button->ResizeToPreferred();
	button->MakeDefault(true);
	
	r = Bounds();
	button->MoveTo ((r.Width() - button->Frame().Width()) / 2 , 270);
	
	r.Set(53, 50, r.right - 10, r.bottom - 100);
	BTextView *textview = new BTextView (r, "_textview", 
		BRect(3, 3, r.Width() - 3, r.Height() - 3), 
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
		
	BFont f = be_plain_font;
	f.SetSize(11.0f);
	textview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	textview->SetFontAndColor(&f);	
	textview->MakeEditable(false);
	textview->MakeSelectable(false);
	
	char const *text = "A freeware, portable Nintendo NES emulator\n\n"
	"Version:\t\t000000\n"
	"Built on:\t\t" __DATE__ " " __TIME__
	"\nWritten by: \tEli Dayan and Evan Teran\n\n"
	"\"Nintendo\" and \"Nintendo Entertainment System\" are registered trademarks of "
	"Nintendo Co., Ltd";
	textview->SetText(text);
	textview->ResizeToPreferred();
	
	AddChild(textview);
	textview->ResizeBy(0,-20);
	button->MoveBy(0, -10);
	AddChild(button);
	
	BView::AttachedToWindow();
}


void
AboutView::Draw (BRect updateRect)
{
	BRect r = Bounds(); 
    r.right = 30;
    SetHighColor(tint_color(ViewColor(), B_DARKEN_1_TINT)); 
  	FillRect (r); 
    SetDrawingMode(B_OP_OVER); 
    DrawBitmap (fIcon, BPoint(18, 6));
    DrawBitmap(fLogo, BPoint((Bounds().Width() - 196) / 2, 11));
    
    BView::Draw(updateRect);
}


AboutWindow::AboutWindow()
	: BWindow (BRect (0,0,0,0), "About Window", B_MODAL_WINDOW, 
		B_NOT_CLOSABLE | B_NOT_RESIZABLE)
{
	ResizeTo (340, 300);
	MoveTo ((BScreen().Frame().Width() - Frame().Width()) / 2,
		     (BScreen().Frame().Height() - Frame().Height()) / 2);
	
	fAboutView = new AboutView(Bounds());
	AddChild(fAboutView);
	
	BRect r;
	r.Set(53, 185, 194, 200);
	fAboutView->AddChild(new LinkView(r, const_cast<char *>("Pretendo on google code"), const_cast<char *>("http://code.google.com/p/pretendo/")));
	
	r.Set(53, 205, 124, 220);
	fAboutView->AddChild(new LinkView(r, const_cast<char *>("Eli's website"), const_cast<char *>("http://shell.reverse.net/~eli")));
	
	r.Set(53, 225, 140, 240);
	fAboutView->AddChild(new LinkView(r, const_cast<char *>("Evan's website"), const_cast<char *>("http://www.codef00.com")));
	
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
	if (message->what == 'OKAY') {
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
	
	BWindow::MessageReceived (message);
}

