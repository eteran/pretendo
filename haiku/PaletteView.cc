

#include <MenuItem.h>
#include <Box.h>

#include "Palette.h"
#include "PaletteView.h"



PaletteView::PaletteView (PretendoWindow *parent, BRect frame, int32 numcolors, int32 swatchSize)
	: BView (frame, "palette", B_FOLLOW_NONE, B_WILL_DRAW),
	fSwatchSize(swatchSize),
	fPalette(new rgb_color[numcolors]),
	fPretendoWindow(parent)
{
	
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
	fHueMenu->SetTargetForItems(this);
	
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
	fSatMenu->SetTargetForItems(this);
	
	fContrastMenu->AddItem(new BMenuItem("0.5 (reduced)", new BMessage('CN05')));
	fContrastMenu->AddItem(new BMenuItem("0.92", new BMessage('CN92')));
	fContrastMenu->AddItem(new BMenuItem("0.94", new BMessage('CN94')));
	fContrastMenu->AddItem(new BMenuItem("1.0 (normal)", new BMessage('CN10')));;
	fContrastMenu->AddItem(new BMenuItem("1.5", new BMessage('CN15')));
	fContrastMenu->AddItem(new BMenuItem("2.0", new BMessage('CN20')));
	fContrastMenu->ItemAt(3)->SetMarked(true);
	fContrastMenu->SetRadioMode(true);
	fContrastMenu->SetTargetForItems(this);
	
	fBrightnessMenu->AddItem(new BMenuItem("0.5 (reduced)", new BMessage('BR05')));
	fBrightnessMenu->AddItem(new BMenuItem("1.0 (normal)", new BMessage('BR10')));
	fBrightnessMenu->AddItem(new BMenuItem("1.07", new BMessage('B107')));
	fBrightnessMenu->AddItem(new BMenuItem("1.08", new BMessage('B108')));
	fBrightnessMenu->AddItem(new BMenuItem("1.5", new BMessage('BR15')));
	fBrightnessMenu->AddItem(new BMenuItem("2.0", new BMessage('BR20')));
	fBrightnessMenu->ItemAt(1)->SetMarked(true);
	fBrightnessMenu->SetRadioMode(true);
	fBrightnessMenu->SetTargetForItems(this);
	
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
	fGammaMenu->SetTargetForItems(this);
	
	BBox *box = new BBox(BRect(163, 126, 164, 256));
	AddChild(box);
	
	BButton *defaultButton = new BButton(BRect(208, 128, 320, 156), "_default button", "Defaults", new BMessage('DFLT'));
	AddChild(defaultButton);
	defaultButton->SetTarget(this);
	
	BButton *usePalette = new BButton(BRect(208, 172, 320, 200), "_use_palette", "Use Palette", new BMessage('UPAL'));
	AddChild (usePalette);
	usePalette->SetTarget(this);
	
	BButton *cancel = new BButton(BRect(208, 216, 320, 244), "_cancel", "Cancel", new BMessage('CNCL'));
	AddChild (cancel);
	cancel->SetTarget(this);
	
	// first try to read the palette from the config file.  if we can't
	// then we'll use the defaults
	
	// if (......) {
	//		read from config
	// } else {
		SetDefaultPalette();
	//}
}


void
PaletteView::MessageReceived (BMessage *message)
{
	switch (message->what) {
		case 'HUM1':	fCurrentHue = -1.0f;	break;
		case 'HU08':	fCurrentHue = -0.081f;	break;
		case 'HUE0':	fCurrentHue = 0.0f;		break;
		case 'HUP1':	fCurrentHue = +1.0f;	break;
		
		case 'SAT0':	fCurrentSaturation = 0.0f;	break;
		case 'ST05':	fCurrentSaturation = 0.5f;	break;
		case 'SAT1':	fCurrentSaturation = 1.0f;	break;
		case 'ST12':	fCurrentSaturation = 1.2f;	break;
		case 'ST15':	fCurrentSaturation = 1.5f;	break;
		case 'S153':	fCurrentSaturation = 1.53f; break;
		case 'S154':	fCurrentSaturation = 1.54f;	break;
		case 'ST20':	fCurrentSaturation = 2.0f;	break;
		case 'ST30':	fCurrentSaturation = 3.0f;	break;
		case 'ST40':	fCurrentSaturation = 4.0f;	break;
		case 'ST50':	fCurrentSaturation = 5.0f;	break;
		
		case 'CN05':	fCurrentContrast = 0.5f; SetPalette();	break;
		case 'CN92':	fCurrentContrast = 0.92f;	break;
		case 'CN94':	fCurrentContrast = 0.94f;	break;
		case 'CN10':	fCurrentContrast = 1.0f;	break;
		case 'CN15':	fCurrentContrast = 1.5f;	break;
		case 'CN20':	fCurrentContrast = 2.0f;	break;
		
		case 'BR05':	fCurrentBrightness = 0.5f;	break;
		case 'BR10':	fCurrentBrightness = 1.0f;	break;
		case 'B107':	fCurrentBrightness = 1.07f;	break;
		case 'B108':	fCurrentBrightness = 1.08f;	break;
		case 'BR15':	fCurrentBrightness = 1.5f;	break;
		case 'BR20':	fCurrentBrightness = 2.0f;	break;
		
		case 'GM10':	fCurrentGamma = 1.0f;	break;
		case 'GM15':	fCurrentGamma = 1.5f;	break;
		case 'GM17':	fCurrentGamma = 1.7f;	break;
		case 'GM18':	fCurrentGamma = 1.8f;	break;
		case 'G199':	fCurrentGamma = 1.99f;	break;
		case 'GM20':	fCurrentGamma = 2.0f;	break;
		case 'GM21':	fCurrentGamma = 2.1f;	break;
		case 'GM22':	fCurrentGamma = 2.2f;	break;
		case 'GM25':	fCurrentGamma = 2.5f;	break;
		
		case 'DFLT':	SetDefaultPalette();	break;
		case 'UPAL':							break;
		case 'CNCL':							break;	
		
		default:
			BView::MessageReceived (message);
	}
}



void
PaletteView::Draw (BRect frame)
{		
	// palette is redrawn by Parent::MenusEnded()
	(void)frame;
	
	const rgb_color_t *ntscPalette = Palette::NTSC(
					fCurrentSaturation,
					fCurrentHue,
					fCurrentContrast,
					fCurrentBrightness,
					fCurrentGamma);
	
	for (int32 i = 0; i < 64; i++) {
		fPalette[i].red = ntscPalette[i].r;
		fPalette[i].green = ntscPalette[i].g;
		fPalette[i].blue = ntscPalette[i].b;
	}
	
	fWorkPalette = fPalette;	
	DrawSwatchMatrix (BPoint(16, 16), fSwatchSize, 16, 4);
	DrawIndexes();
}


void
PaletteView::SetDefaultPalette (void)
{
	fCurrentSaturation = Palette::default_saturation;
	fCurrentHue = Palette::default_hue;
	fCurrentContrast = Palette::default_contrast;
	fCurrentBrightness = Palette::default_brightness;
	fCurrentGamma = Palette::default_gamma;
	
	
	fPretendoWindow->set_palette(Palette::intensity,
		Palette::NTSC(
			fCurrentSaturation,
			fCurrentHue,
			fCurrentContrast,
			fCurrentBrightness,
			fCurrentGamma));
			
	fHueMenu->ItemAt(2)->SetMarked(true);
	fSatMenu->ItemAt(2)->SetMarked(true);
	fContrastMenu->ItemAt(3)->SetMarked(true);
	fBrightnessMenu->ItemAt(1)->SetMarked(true);
	fGammaMenu->ItemAt(5)->SetMarked(true);
		
	fWorkPalette = fPalette;
	Invalidate();
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
	if (fPalette == NULL || fWorkPalette == NULL || size <= 0 || rowlen <= 0) {
		return;
	}
	
	for (int32 i = 0; i < rowlen; i++) {
		DrawSwatch (start, fWorkPalette[i]);
		start.x += size+4;
	}
		
}


void
PaletteView::DrawSwatchMatrix (BPoint start, int32 size, int32 ncols, int32 nrows)
{
	if (nrows <= 0 || size <= 0 || fPalette == NULL || fWorkPalette == NULL) {
		return;
	}
	
	for (int32 y = 0; y < nrows; y++) {
		DrawSwatchRow (start, size, ncols);
		start.y += size+4;
		fWorkPalette += nrows * sizeof(rgb_color);
	}
}


void
PaletteView::DrawIndexes (void)
{
	char const nybbles[] = "0123456789ABCDEF";
	
	// y 
	BPoint p(4, 28);
	for (int32 i = 0; i < 4; i++) {
		SetHighColor(0,0,0);
		SetFont(be_fixed_font);
		DrawChar(nybbles[i], p);
		p.y += (fSwatchSize+4);
	}

	// x
	p.Set(20, 110);
	for (int32 i = 0; i < 16; i++) {
		SetHighColor (0, 0, 0);
		SetFont (be_fixed_font);
		DrawChar(nybbles[i], p);
		p.x += fSwatchSize+4;
	}
}

void 
PaletteView::SetPalette (void)
{
	fPretendoWindow->set_palette(Palette::intensity, Palette::NTSC(
		fCurrentSaturation,
		fCurrentHue,
		fCurrentContrast,
		fCurrentBrightness,
		fCurrentGamma));
}
