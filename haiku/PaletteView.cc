
#include <Menu.h>

#include "PaletteView.h"
#include "Palette.h"

PaletteView::PaletteView (BRect frame, int32 numcolors, int32 swatchSize)
	: BView (frame, "palette", B_FOLLOW_NONE, B_WILL_DRAW),
	fSwatchSize(swatchSize),
	fColors(numcolors), fPalette(new rgb_color[fColors])
{
	
	
	rgb_color_t *ntscPalette = Palette::NTSCPalette(
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

	BMenu *mnuHue = new BMenu("Hue");
	BMenu *mnuSat = new BMenu("Saturation");
	BMenu *mnuContrast = new BMenu("Contrast");
	BMenu *mnuBrightness = new BMenu("Brightness");
	BMenu *mnuGamma = new BMenu("Gamma");
	
	BRect r (18, 128, 120, 32);
	fHueMenu = new BMenuField(r, "_hue_menu", "Hue", mnuHue);
	
	r.Set(128, 128, 332, 32);
	fSaturation = new BMenuField(r, "_sat_menu", "Saturation", mnuSat);
	
	r.Set(18, 156, 138, 228);
	fContrast = new BMenuField(r, "_contrast_menu", "Contrast", mnuContrast);
	
	r.Set(158, 156, 302, 402);
	fBrightness = new BMenuField(r, "_bright_menu", "Brightness", mnuBrightness);
	
	AddChild(fHueMenu);
	AddChild (fSaturation);
	AddChild(fContrast);
	AddChild(fBrightness);
}




void
PaletteView::Draw (BRect frame)
{		
	rgb_color_t *ntscPalette = Palette::NTSCPalette(
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
	
	DrawSwatchMatrix (BPoint(16, 16), fSwatchSize, 16, 4);
	
	DrawIndexes();
}


void
PaletteView::SetPalette (rgb_color *palette)
{
	rgb_color_t *ntscPalette = Palette::NTSCPalette(
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
