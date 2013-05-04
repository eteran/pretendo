
#include "CartInfoView.h"

#include <iostream>

using std::cout;
using std::endl;

CartInfoView::CartInfoView(BRect frame)
	: BOutlineListView(frame, "_cart_info_view")
{
	std::cout << "CONSTRUCTOR" << std::endl;
}


CartInfoView::~CartInfoView()
{
}


void
CartInfoView::AttachedToWindow (void)
{	
	BOutlineListView::AttachedToWindow();
}


void
CartInfoView::Draw (BRect updateRect)
{
	BOutlineListView::Draw(updateRect);
}
