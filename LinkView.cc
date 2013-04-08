
#include <Application.h>
#include <Entry.h>

#include "LinkView.h"


uint8 link_cursor[] = { 
	16, 1, 1, 2,
 	// This is the cursor data.
 	0x00, 0x00, 0x38, 0x00, 0x24, 0x00, 0x24, 0x00,
 	0x13, 0xe0, 0x12, 0x5c, 0x09, 0x2a, 0x08, 0x01,
 	0x3c, 0x21, 0x4c, 0x71, 0x42, 0x71, 0x30, 0xf9,
 	0x0c, 0xf9, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00,
 	// This is the cursor mask.
 	0x00, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x3c, 0x00,
 	0x1f, 0xe0, 0x1f, 0xfc, 0x0f, 0xfe, 0x0f, 0xff,
 	0x3f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x3f, 0xff,
 	0x0f, 0xff, 0x03, 0xfe, 0x01, 0xf8, 0x00, 0x00,
};


LinkView::LinkView (BRect frame, char *text, char *link)
	: BStringView(frame, "_linkview", text, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW),
	fText(text), fLink(link)
{
	fLinkCursor = new BCursor(link_cursor);
}

LinkView::~LinkView()
{
}


void
LinkView::AttachedToWindow (void)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetHighColor(0, 0, 255);
	SetFontSize(11.0f);
	
	BStringView::AttachedToWindow();
}

void
LinkView::MouseUp (BPoint point)
{
	be_roster->Launch("text/html", 1, &fLink);
	
	BStringView::MouseUp(point);
}

void
LinkView::MouseMoved (BPoint point, uint32 transit, const BMessage *message)
{
	switch (transit) {
		case B_ENTERED_VIEW:
			//if(GetTextRect().Contains(point)) {
    			be_app->SetCursor(fLinkCursor);
			//}
			break;
			
		case B_EXITED_VIEW:
			be_app->SetCursor(B_HAND_CURSOR);
			break;
	}
	
	BStringView::MouseMoved(point, transit, message);
}

void 
LinkView::Draw (BRect updateRect)
{
	BStringView::Draw(updateRect);
}


BRect LinkView::GetTextRect()
{
 // This function will return a BRect that contains only the text
 // and the underline, so the mouse can change and the link will
 // be activated only when the mouse is over the text itself, not
 // just within the view.
 
 // Note:  We'll use bounding boxes, because they are the most
 //        accurate, and since the user is interacting with the
 //        view, off-by-one-pixel errors look bad.
 	const char *textArray[1];
 	textArray[0]=Text();
 
 	escapement_delta delta;
 	delta.nonspace=0;
 	delta.space=0;
 	escapement_delta escapements[1];
 	escapements[0]=delta;
 
 	BRect returnMe;
 	BRect rectArray[1];
 	rectArray[0]=returnMe;
 
 	BFont font;
 	GetFont(&font);
 	font.GetBoundingBoxesForStrings(textArray, 1, B_SCREEN_METRIC, escapements, 					rectArray);

 	BRect frame=Frame();
 	frame.OffsetTo(B_ORIGIN);
 	returnMe=rectArray[0];
 
 	// Get the height of the current font.
 	font_height height;
 	GetFontHeight(&height);
 	float descent=height.descent;
 
 	// Account for rounding of the floats when drawn to avoid
 	// one-pixel-off errors.
 	float lowerBound=0;
 	if((((int) descent)*2)!=((int) (descent*2))) lowerBound=1;
 
 	// Adjust the bounding box to reflect where the text is in the view.
 	returnMe.bottom+=1;
 	returnMe.OffsetTo(B_ORIGIN);
 	float rectHeight=returnMe.Height();
 	returnMe.bottom=frame.bottom-descent;
 	returnMe.top=returnMe.bottom-rectHeight;
 	returnMe.bottom+=1+1;
 	returnMe.OffsetBy(0.0, -(1+lowerBound));

 return returnMe;
}
