
#ifndef _CART_INFO_WINDOW_H_
#define _CART_INFO_WINDOW_H_

#include <Window.h>
#include <TabView.h>
#include <ListView.h>
#include <ScrollView.h>
#include <libxml/parser.h>
#include <string>

#include "CartInfoView.h"

using std::string;

class CartInfoScrollView : public BScrollView
{
	public:
	CartInfoScrollView();
	virtual ~CartInfoScrollView();
	
	public:
	virtual void Draw(BRect updateRect);
	virtual void AttachedToWindow (void);
};

class CartInfoWindow : public BWindow
{	
	public:
			CartInfoWindow();
	virtual ~CartInfoWindow();
	
	public:
	virtual void MessageReceived (BMessage *message);
	virtual bool QuitRequested (void);
	
	private:
	CartInfoView *fCartInfoView;
};


#endif //_CART_INFOWINDOW_H_
