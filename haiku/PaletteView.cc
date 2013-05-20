

#include <MenuItem.h>
#include <Box.h>
#include <Button.h>

#include "PaletteView.h"
#include "Palette.h"

PaletteView::PaletteView (BRect frame, int32 numcolors, int32 swatchSize)
	: BView (frame, "palette", B_FOLLOW_NONE, B_WILL_DRAW),
	fSwatchSize(swatchSize), fPalette(new rgb_color[numcolors])
{
	
	
	rgb_color_t *ntscPalette = Palette::NTSC(
					Palette::default_saturation,
					Palette::default_hue,
					Palette::default_contrast,
					Palette::default_brightness,
					Palette::default_gamma);
	for (int32 i = 0; i < 64; i++) {
		fPalette[i].red = ntscPalette[i].r;
		fPalette[i].green = ntscPalette[i].g;
		fPalette[i].blue = ntscPalette[i].b;
	}
	
	fWorkPalette = const_cast<rgb_color *>(fPalette);
}


PaletteView::~PaletteView()
{
	delete[] fPalette;
}


void
PaletteView::AttachedToWindow (void)
{
	SetViewColor (ui_color(B_PANEL_BACKGROUND_COLOR));

	fHueMenu = new BMenu("Hue");
	fSatMenu = new BMenu("Saturation");
	fContrastMenu = new BMenu("Contrast");
	fBrightnessMenu = new BMenu("Brightness");
	fGammaMenu = new BMenu("Gamma");
	
	BRect r (8, 128, 120, 32);
	fHueField = new BMenuField(r, "_hue_menu", B_EMPTY_STRING, fHueMenu, true);
	fHueField->SetDivider(0);
	
	r.Set(8, 156, 120, 172);
	fSaturationField = new BMenuField(r, "_sat_menu", "Saturation", fSatMenu, true);
	fSaturationField->SetDivider(0);
	
	r.Set(8, 184, 120, 312);
	fContrastField = new BMenuField(r, "_contrast_menu", "Contrast", fContrastMenu, true);
	fContrastField->SetDivider(0);
	
	r.Set(8, 212, 120, 452);
	fBrightnessField = new BMenuField(r, "_bright_menu", "Brightness", fBrightnessMenu, true);
	fBrightnessField->SetDivider(0);
	
	r.Set(8, 240, 120, 592);
	fGammaField = new BMenuField(r, "_gamma_menu", "Gamma", fGammaMenu, true);
	fGammaField->SetDivider(0);
	
	AddChild(fHueField);
	AddChild (fSaturationField);
	AddChild(fContrastField);
	AddChild(fBrightnessField);
	AddChild(fGammaField);
	
	fHueMenu->AddItem(new BMenuItem("-1 (-30°)", new BMessage('HUM1')));
	fHueMenu->AddItem(new BMenuItem("-.081 (-2.43°)", new BMessage('HU08')));
	fHueMenu->AddItem(new BMenuItem("0 (0°)", new BMessage('HUE0')));
	fHueMenu->AddItem(new BMenuItem("+1 (30°)", new BMessage('HUP1')));
	fHueMenu->ItemAt(2)->SetMarked(true);
	fHueMenu->SetRadioMode(true);
	
	fSatMenu->AddItem(new BMenuItem("0.0 (grayscale)", new BMessage('SAT0')));
	fSatMenu->AddItem(new BMenuItem("0.5", new BMessage('ST05')));
	fSatMenu->AddItem(new BMenuItem("1.0", new BMessage('SAT1')));
	fSatMenu->AddItem(new BMenuItem("1.2", new BMessage('ST12')));
	fSatMenu->AddItem(new BMenuItem("1.5", new BMessage('ST15')));
	fSatMenu->AddItem(new BMenuItem("1.53", new BMessage('S153')));
	fSatMenu->AddItem(new BMenuItem("1.54", new BMessage('S154')));
	fSatMenu->AddItem(new BMenuItem("2.0", new BMessage('ST20')));
	fSatMenu->AddItem(new BMenuItem("3.0", new BMessage('ST30')));
	fSatMenu->AddItem(new BMenuItem("4.0", new BMessage('ST40')));
	fSatMenu->AddItem(new BMenuItem("5.0", new BMessage('ST50')));
	fSatMenu->ItemAt(2)->SetMarked(true);
	fSatMenu->SetRadioMode(true);
	
	fContrastMenu->AddItem(new BMenuItem("0.5 (reduced)", new BMessage('CN05')));
	fContrastMenu->AddItem(new BMenuItem("0.92", new BMessage('CN92')));
	fContrastMenu->AddItem(new BMenuItem("0.94", new BMessage('CN94')));
	fContrastMenu->AddItem(new BMenuItem("1.0 (normal)", new BMessage('CN10')));;
	fContrastMenu->AddItem(new BMenuItem("1.5", new BMessage('CN15')));
	fContrastMenu->AddItem(new BMenuItem("2.0", new BMessage('CN20')));
	fContrastMenu->ItemAt(3)->SetMarked(true);
	fContrastMenu->SetRadioMode(true);
	
	fBrightnessMenu->AddItem(new BMenuItem("0.5 (reduced)", new BMessage('BR05')));
	fBrightnessMenu->AddItem(new BMenuItem("1.0 (normal)", new BMessage('BR05')));
	fBrightnessMenu->AddItem(new BMenuItem("1.07", new BMessage('B107')));
	fBrightnessMenu->AddItem(new BMenuItem("1.08", new BMessage('B108')));
	fBrightnessMenu->AddItem(new BMenuItem("1.5", new BMessage('BR15')));
	fBrightnessMenu->AddItem(new BMenuItem("2.0", new BMessage('BR20')));
	fBrightnessMenu->ItemAt(1)->SetMarked(true);
	fBrightnessMenu->SetRadioMode(true);
	
	fGammaMenu->AddItem(new BMenuItem("1.0", new BMessage('GM10')));
	fGammaMenu->AddItem(new BMenuItem("1.5", new BMessage('GM15')));
	fGammaMenu->AddItem(new BMenuItem("1.7", new BMessage('GM17')));
	fGammaMenu->AddItem(new BMenuItem("1.8", new BMessage('GM18')));	
	fGammaMenu->AddItem(new BMenuItem("1.99", new BMessage('G199')));
	fGammaMenu->AddItem(new BMenuItem("2.0", new BMessage('GM20')));
	fGammaMenu->AddItem(new BMenuItem("2.1", new BMessage('GM21')));
	fGammaMenu->AddItem(new BMenuItem("2.2", new BMessage('GM22')));
	fGammaMenu->AddItem(new BMenuItem("2.5", new BMessage('GM25')));
	fGammaMenu->ItemAt(5)->SetMarked(true);
	fGammaMenu->SetRadioMode(true);
	
	BBox *box = new BBox(BRect(163, 126, 164, 256));
	AddChild(box);
	
	BButton *defaultButton = new BButton(BRect(208, 128, 320, 156), "_default button", "Defaults", new BMessage('DFLT'));
	AddChild(defaultButton);
	
	BButton *usePalette = new BButton(BRect(208, 172, 320, 200), "_use_palette", "Use Palette", new BMessage('UPAL'));
	AddChild (usePalette);
	
	BButton *cancel = new BButton(BRect(208, 216, 320, 244), "_cancel", "Cancel", new BMessage('CNCL'));
	AddChild (cancel);
}




void
PaletteView::Draw (BRect frame)
{		
	(void)frame;
	
	rgb_color_t *ntscPalette = Palette::NTSC(
					Palette::default_saturation,
					Palette::default_hue,
					Palette::default_contrast,
					Palette::default_brightness,
					Palette::default_gamma);
	for (int32 i = 0; i < 64; i++) {
		fPalette[i].red = ntscPalette[i].r;
		fPalette[i].green = ntscPalette[i].g;
		fPalette[i].blue = ntscPalette[i].b;
	}
	
	fWorkPalette = const_cast<rgb_color *>(fPalette);	
	DrawSwatchMatrix (BPoint(16, 16), fSwatchSize, 16, 4);
	DrawIndexes();
}


void
PaletteView::SetPalette (rgb_color *palette)
{
	rgb_color_t *ntscPalette = Palette::NTSC(
					Palette::default_saturation,
					Palette::default_hue,
					Palette::default_contrast,
					Palette::default_brightness,
					Palette::default_gamma);
	for (int32 i = 0; i < 64; i++) {
		palette[i].red = ntscPalette[i].r;
		palette[i].green = ntscPalette[i].g;
		palette[i].blue = ntscPalette[i].b;
	}
	
	fWorkPalette = const_cast<rgb_color *>(fPalette);
}


void
PaletteView::DrawSwatch (BPoint where, rgb_color fill)
{
	rgb_color const no_tint = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color const lightenmax = tint_color(no_tint, B_LIGHTEN_MAX_TINT);
	rgb_color const darken1 = tint_color(no_tint, B_DARKEN_1_TINT); 
	rgb_color const darken4 = tint_color(no_tint, B_DARKEN_4_TINT);
	
	BRect rect (where.x, where.y, where.x+fSwatchSize, where.y+fSwatchSize);
	
	SetHighColor (darken1); 
	StrokeLine (rect.LeftBottom(), rect.LeftTop()); 
	StrokeLine (rect.LeftTop(), rect.RightTop()); 
	SetHighColor (lightenmax); 
	StrokeLine (BPoint(rect.left + 1.0f, rect.bottom), rect.RightBottom()); 
	StrokeLine (rect.RightBottom(), BPoint(rect.right, rect.top + 1.0f)); 
	rect.InsetBy (1, 1);
	
	SetHighColor (darken4); 
	StrokeLine (rect.LeftBottom(), rect.LeftTop()); 
	StrokeLine (rect.LeftTop(), rect.RightTop()); 
	SetHighColor (no_tint); 
	StrokeLine (BPoint(rect.left + 1.0f, rect.bottom), rect.RightBottom()); 
	StrokeLine (rect.RightBottom(), BPoint(rect.right, rect.top + 1.0f)); 
	
	rect.InsetBy (1,1);
		
	SetHighColor (fill);
	FillRect (rect);		
}


void 
PaletteView::DrawSwatchRow (BPoint start, int32 size, int32 rowlen)
{
	if (fPalette == NULL || size <= 0 || rowlen <= 0) {
		return;
	}
	
	for (int32_t i = 0; i < rowlen; i++) {
		DrawSwatch (start, fWorkPalette[i]);
		start.x += size+4;
	}
		
}

#include <stdio.h>

void
PaletteView::DrawSwatchMatrix (BPoint start, int32 size, int32 ncols, int32 nrows)
{
	if (nrows <= 0 || size <= 0 || fPalette == NULL) {
		return;
	}
	
	for (int32_t y = 0; y < nrows; y++) {
		DrawSwatchRow (start, size, ncols);
		start.y += size+4;
		fWorkPalette += nrows * sizeof(rgb_color);
		//fPalette += nrows * sizeof(rgb_color);
	}
}


void
PaletteView::DrawIndexes (void)
{
	char const nybbles[] = "0123456789ABCDEF";
	
	// y 
	BPoint p(4, 28);
	for (int32_t i = 0; i < 4; i++) {
		SetHighColor(0,0,0);
		SetFont(be_fixed_font);
		DrawChar(nybbles[i], p);
		p.y += (fSwatchSize+4);
	}

	// x
	p.Set(20, 110);
	for (int32_t i = 0; i < 16; i++) {
		SetHighColor (0, 0, 0);
		SetFont (be_fixed_font);
		DrawChar(nybbles[i], p);
		p.x += fSwatchSize+4;
	}
}
