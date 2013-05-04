
#ifndef _CART_INFO_VIEW_
#define _CART_INFO_VIEW_

#include <OutlineListView.h>


class CartInfoView : public BOutlineListView
{
	public:
	CartInfoView(BRect frame);
	~CartInfoView();
	
	public:
	virtual void AttachedToWindow (void);
	virtual void Draw (BRect frame);
	
	private:
};
	

#endif // _CART_INFO_VIEW
