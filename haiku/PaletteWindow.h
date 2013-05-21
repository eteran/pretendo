
#ifndef _PALETTE_WINDOW_H_
#define _PALETTE_WINDOW_H_

#include <Window.h>

#include "PaletteView.h"

class PaletteWindow : public BWindow
{	
	public:
			PaletteWindow();
	virtual ~PaletteWindow();
	
	public:
	virtual bool QuitRequested (void);
	virtual void MessageReceived (BMessage *message);
	virtual void MenusEnded (void);
	
	private:
	PaletteView *fPaletteView;
};


#endif //_PALETTE_WINDOW_H_
