
#ifndef _PALETTEVIEW_H_
#define _PALETTEVIEW_H_

#include <View.h>
#include <MenuField.h>
#include <Menu.h>
#include <Button.h>

class PaletteView : public BView
{
	public:
	PaletteView (BRect frame, int32 numcolors, int32 swatchSize);
	virtual ~PaletteView();
	
	public:
	virtual void AttachedToWindow (void);
	virtual void Draw (BRect updateRect);
	virtual void MessageReceived (BMessage *message);
	
	private:
	void DrawSwatchRow (BPoint start, int32 size, int32 rowlen);
	void SetDefaultPalette (void);
	void DrawSwatch (BPoint where, rgb_color fill);
	void DrawSwatchMatrix (BPoint start, int32 size, int32 ncols, int32 nrows);
	void DrawIndexes (void);
	
	int32 fSwatchSize;
	rgb_color *const fPalette;
	rgb_color *fWorkPalette;
	
	private:
	BMenuField *fHueField;
	BMenuField *fSaturationField;
	BMenuField *fContrastField;
	BMenuField *fBrightnessField;
	BMenuField *fGammaField;
	
	BMenu *fHueMenu;
	BMenu *fSatMenu;
	BMenu *fContrastMenu;
	BMenu *fBrightnessMenu;
	BMenu *fGammaMenu;
	
	float fCurrentHue;
	float fCurrentSaturation;
	float fCurrentContrast;
	float fCurrentBrightness;
	float fCurrentGamma;

};

#endif
