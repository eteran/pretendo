
#ifndef _PALETTEVIEW_H_
#define _PALETTEVIEW_H_

#include <View.h>
#include <MenuField.h>

class PaletteView : public BView
{
	public:
	PaletteView (BRect frame, int32 numcolors, int32 swatchSize);
	virtual ~PaletteView();
	
	public:
	virtual void AttachedToWindow (void);
	virtual void Draw (BRect updateRect);
	
	private:
	void  DrawSwatchRow (BPoint start, int32 size, int32 rowlen);
	void SetPalette (rgb_color *palette);
	void DrawSwatch (BPoint where, rgb_color fill);
	void DrawSwatchMatrix (BPoint start, int32 size, int32 ncols, int32 nrows);
	void DrawIndexes (void);
	
	int32 fSwatchSize;
	int32 fColors;
	int32 fNumColors;
	rgb_color *fPalette;
	
	private:
	BMenuField *fHueMenu;
	BMenuField *fSaturation;
	BMenuField *fContrast;
	BMenuField *fBrightness;
	BMenuField *fGamma;

};

#endif
