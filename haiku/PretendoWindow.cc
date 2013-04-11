
#include "PretendoWindow.h"

#include <iostream>

PretendoWindow::PretendoWindow()
	: BDirectWindow (BRect (0, 0, 0, 0), "Pretendo", B_TITLED_WINDOW, B_NOT_RESIZABLE, 0),
		fView(NULL),
		fMenu(NULL),
		fFileMenu(NULL),
		fEmuMenu(NULL),
		fVideoMenu(NULL),
//		fAudioMenu(NULL),
		fOpenPanel(NULL),
		
//		fPaletteWindow(NULL),
		
		fBitmap(NULL),
		fOverlayBitmap(NULL),
		fBitmapBits(NULL),
		fOverlayBits(NULL),
		fBitsArea(B_ERROR),
		fDirtyArea(B_ERROR),
		fVideoScreen(NULL),
		fAudioStream(NULL)
{
	BRect bounds (Bounds());
	bounds.OffsetTo (B_ORIGIN);
	AddMenu();
	bounds.top = fMenuHeight;
	
	fView = new BView (bounds, "main_view", B_FOLLOW_ALL, 0);
	AddChild (fView);
	
	
	ResizeTo (SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
	BScreen *screen = new BScreen(this);
	float newX = (screen->Frame().Width() - Frame().Width()) / 2;
	float newY = (screen->Frame().Height() - Frame().Height()) / 2;
	MoveTo (newX, newY);
	delete screen;	
	
	fBitmap = new BBitmap (BRect (0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1), B_CMAP8);
	if (! fBitmap || ! fBitmap->IsValid()) {
		(new BAlert ("Error", "Not enough memory for video bitmap.  Quitting.",
			"Bummer", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		OnQuit();
	} else {
		fBitmapBits = reinterpret_cast<uint8 *>(fBitmap->Bits());
		ClearBitmap (false);
	}

	void *areaBits = NULL;
	void *dirtyBits = NULL;
	
	fBitsArea = create_area ("frame buffer", &areaBits, B_ANY_ADDRESS,
					((SCREEN_WIDTH * 2) * (SCREEN_HEIGHT * 2) * 4 + B_PAGE_SIZE-1) & 
					((uint32)-1 ^ (B_PAGE_SIZE-1)), B_NO_LOCK, 
					B_READ_AREA | B_WRITE_AREA);
	
	fDirtyArea = create_area ("dirty buffer", &dirtyBits, B_ANY_ADDRESS,
					((SCREEN_WIDTH * 2) * (SCREEN_HEIGHT * 2) * 4 + B_PAGE_SIZE-1) & 
					((uint32)-1 ^ (B_PAGE_SIZE-1)), B_NO_LOCK, 
					B_READ_AREA | B_WRITE_AREA);
					
	if (fBitsArea < B_OK || fDirtyArea < B_OK) {
		(new BAlert ("Error", "Not enough memory for video buffers.  Quitting.",
			"Bummer", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		OnQuit();
	} else {
		memset (areaBits, 0x00, (SCREEN_WIDTH*2) * (SCREEN_HEIGHT*2) * 4);
		memset (dirtyBits, 0xff, (SCREEN_WIDTH*2) * (SCREEN_HEIGHT*2) * 4);
		
		fBackBuffer.bits = reinterpret_cast<uint8 *>(areaBits);
		fDirtyBuffer.bits = reinterpret_cast<uint8 *>(dirtyBits);
	}					
	
	// crate a BBitmap for overlay framework (checks for overlay support inherently)
	bool overlayOK = false;

	bounds.Set (0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
	fOverlayBitmap = new BBitmap (bounds, B_BITMAP_WILL_OVERLAY, B_YCbCr422);
	overlayOK = fOverlayBitmap && fOverlayBitmap->IsValid();

	if (overlayOK) {
		fVideoMenu->ItemAt(2)->SetEnabled(true);
		fOverlayBits = reinterpret_cast<uint8 *>(fOverlayBitmap->Bits());
		ClearBitmap (true);
	} else {
		fOverlayBits = NULL;
		if (fOverlayBitmap) {
			delete fOverlayBitmap;
		}
		fView->SetViewColor(0, 0, 0);
		ClearBitmap(false);
		fVideoMenu->ItemAt(2)->SetEnabled(false);
		
	}
	
	fDirectConnected = 
	fFullScreen = 
	fFrameworkChanging = false;	
	fFramework = 
	fPrevFramework = NO_FRAMEWORK;
	
	if (overlayOK) {
		ChangeFramework (OVERLAY_FRAMEWORK);
	} else {
		//ChangeFramework(DIRECTWINDOW_FRAMEWORK);
		ChangeFramework(BITMAP_FRAMEWORK);
	}

	// other things we need
	fOpenPanel = new ROMFilePanel;
	fAudioStream = new AudioStream ("Pretendo", 44100.0, 8, 1, 44100.0 / 60);	
	
	fDoubled = false;
	fClear = 0;
	
	fPaused = 
	fReallyPaused = false;
	
	if ((fThread = spawn_thread(threadFunc, "pretendo_thread", B_NORMAL_PRIORITY, 					reinterpret_cast<void *>(this))) < B_OK) {
			std::cout << "failed to spawn thread" << std::endl;
	} else {
		suspend_thread(fThread);
	}
}


PretendoWindow::~PretendoWindow()
{
	if (fOpenPanel->Window()) {
		fOpenPanel->Window()->LockLooper();
		fOpenPanel->Window()->Quit();
		fOpenPanel = NULL;
	}
	
	if (fBitmap->IsValid()) {
		delete fBitmap;
		fBitmap = NULL;
	}
	
	if (fOverlayBitmap->IsValid()) {
		delete fOverlayBitmap;
		fOverlayBitmap = NULL;
	}
	
	delete_area (fBitsArea);
	delete_area (fDirtyArea);
	
	delete fAudioStream;
	
	Hide();
	Sync();	
}


void
PretendoWindow::DirectConnected (direct_buffer_info *info)
{
	fMainLocker.Lock();
	
	switch (info->buffer_state & B_DIRECT_MODE_MASK) {
		case B_DIRECT_START:
			fClear = 5;
			fClipInfo.bounds = info->window_bounds;
			fClipInfo.bounds.top += fMenuHeight + 1;
			
			if (fFramework == DIRECTWINDOW_FRAMEWORK) {
				SetFrontBuffer (reinterpret_cast<uint8 *>(info->bits)
					+ (fClipInfo.bounds.top * info->bytes_per_row), info->pixel_format,
					info->bits_per_pixel / 8, info->bytes_per_row);
			}
		
			fClipInfo.clip_list = NULL;
			fDirectConnected = true;
		// intentional fall through //	
		case B_DIRECT_MODIFY:
			fClear = 5;
			fClipInfo.clip_count = info->clip_list_count;
			fClipInfo.clip_list = 
				reinterpret_cast<clipping_rect *>(realloc(fClipInfo.clip_list, 								fClipInfo.clip_count * sizeof(clipping_rect)));
			memcpy (fClipInfo.clip_list, info->clip_list,
				fClipInfo.clip_count * sizeof(clipping_rect));
			
			for (int32 i = 0; i < fClipInfo.clip_count; i++) {
				if (fClipInfo.clip_list[i].top <= 
					static_cast<int32>(info->window_bounds.top + fMenuHeight)) {
					fClipInfo.clip_list[i].top = info->window_bounds.top + fMenuHeight;
				}
			}
			break;
			
		case B_DIRECT_STOP:
			fDirectConnected = false;
			free (fClipInfo.clip_list);
			break;
	}
	
	fMainLocker.Unlock();
	
	//BDirectWindow::DirectConnected(info);
}


void
PretendoWindow::MessageReceived (BMessage *message)
{
	switch (message->what) {
		case MSG_CHANGE_RENDER:
			ChangeFramework(
				(VIDEO_FRAMEWORK)fVideoMenu->IndexOf(fVideoMenu->FindMarked()));
			break;
		
		case MSG_LEAVE_FULLSCREEN:
			ChangeFramework (fPrevFramework);
		 	break;
			
		case MSG_ROM_LOADED:
			OnLoadCart(message);
			break;
			
		case MSG_SHOW_OPEN:
			fOpenPanel->Show();
			break;
			
		case B_REFS_RECEIVED:
			be_app->PostMessage(message);
			break;

		case MSG_FREE_ROM:
			OnFreeCart();
			break;
			
		case MSG_ABOUT:
			be_app->PostMessage (B_ABOUT_REQUESTED);
			break;
			
		case MSG_QUIT:
			OnQuit();
			break;
			
		case MSG_CPU_RUN:
			OnRun();
			break;
			
		case MSG_CPU_STOP:
			OnStop();
			break;
			
		case MSG_CPU_PAUSE:
			OnPause();
			break;
			
		case MSG_CPU_DEBUG:
			OnDebug();
			break;
			
		case MSG_RST_SOFT:
			OnSoftReset();
			break;
			
		case MSG_RST_HARD:
			OnHardReset();
			break;
			
		case MSG_DRAW_BITMAP:
			// this has to go here, since the window is *guaranteed* to be locked
			fView->DrawBitmap(fBitmap, fView->Bounds());
			break;
			
		case MSG_ADJ_PALETTE:
		//	if (fPaletteWindow != NULL) {
		//		fPaletteWindow->Show();
		//	} else {
		//		fPaletteWindow = new PaletteWindow;
		//	}
			
			break;
			
		default:
			BDirectWindow::MessageReceived (message);
	}
}


void
PretendoWindow::WindowActivated (bool flag)
{	
	#if 0
	if (fPaused == false && fFramework != WINDOWSCREEN_FRAMEWORK) {
		if (flag == false) {
			fMediator->pauseOn();
		} else {
			fMediator->pauseOff();
		}
	}
	#endif
	BDirectWindow::WindowActivated (flag);	
}


void
PretendoWindow::MenusBeginning (void)
{
	#if 0
	if (fPaused == false) {
		fMediator->pauseOn();
	}
	#endif
	
	BMenu *menu = BRecentFilesList::NewFileListMenu ("Load ROM"B_UTF8_ELLIPSIS,
		NULL, NULL, this->PreferredHandler(), 10, false, NULL, 0, 
		"application/x-vnd.scanty-Pretendo");
	
	fFileMenu->AddItem(new BMenuItem(menu, new BMessage(MSG_SHOW_OPEN)), 0);
	
	BDirectWindow::MenusBeginning();
}


void
PretendoWindow::MenusEnded (void)
{
	#if 0
	fReallyPaused = fPaused;
	
	if (fReallyPaused == false) {
		fMediator->pauseOff();	
	}
	#endif
	
	fFileMenu->RemoveItem(0L);
	BDirectWindow::MenusEnded();
}


bool
PretendoWindow::QuitRequested()
{	
	Hide();
	Sync();
	
	be_app->PostMessage (B_QUIT_REQUESTED);
	return true;
}


void
PretendoWindow::ResizeTo (float width, float height)
{
	height += fMenuHeight + 1;
	BDirectWindow::ResizeTo (width, height);
}


void
PretendoWindow::Zoom (BPoint origin, float width, float height)
{
	(void)origin;
	(void)width;
	(void)height;
	
	float w = Frame().right - Frame().left;
	
	if (w <= SCREEN_WIDTH) {
		if (fFramework == DIRECTWINDOW_FRAMEWORK) {
			ResizeTo ((SCREEN_WIDTH*2), (SCREEN_HEIGHT*2));
		} else {
			ResizeTo ((SCREEN_WIDTH*2) - 1, (SCREEN_HEIGHT*2) - 1);
		}
		
		fDoubled = true;
	} else {
		if (fFramework == DIRECTWINDOW_FRAMEWORK) {
			ResizeTo (SCREEN_WIDTH, SCREEN_HEIGHT);
		} else {
			ResizeTo (SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
		}
		
		fDoubled = false;
	}
	
	Hide();
	Show();
	
	//BDirectWindow::Zoom(origin, width, height);
}


void
PretendoWindow::AddMenu (void)
{
	fMenu = new BMenuBar (BRect (0, 0, 0, 0), "menu");
	AddChild (fMenu);
	
	fFileMenu = new BMenu ("File");
	fMenu->AddItem (fFileMenu);
	
	fEmuMenu = new BMenu ("Emulator");
	fMenu->AddItem (fEmuMenu);
	
	fVideoMenu = new BMenu ("Render");
	fMenu->AddItem (fVideoMenu);
	
//	fAudioMenu = new BMenu ("Audio");
//	fMenu->AddItem (fAudioMenu);
	
	fVideoMenu->AddItem(new BMenuItem ("No Output", new BMessage (MSG_CHANGE_RENDER)));
	fVideoMenu->AddItem(new BMenuItem ("BView/BBitmap", new BMessage(MSG_CHANGE_RENDER)));
	fVideoMenu->AddItem(new BMenuItem ("BView/Overlay", new BMessage(MSG_CHANGE_RENDER)));
	fVideoMenu->AddItem(new BMenuItem ("BDirectWindow", new BMessage(MSG_CHANGE_RENDER)));
	fVideoMenu->AddItem(new BMenuItem ("BWindowScreen", 
						new BMessage(MSG_CHANGE_RENDER), 'F'));
	fVideoMenu->SetRadioMode(true);
	fVideoMenu->ItemAt(2)->SetMarked(false);
	fVideoMenu->AddSeparatorItem();
	
	fVideoMenu->AddItem (new BMenuItem ("Adjust Palette"B_UTF8_ELLIPSIS, 
		new BMessage (MSG_ADJ_PALETTE)));

	fFileMenu->AddItem (new BMenuItem ("Free ROM", new BMessage (MSG_FREE_ROM)));
	fFileMenu->AddSeparatorItem();
	fFileMenu->AddItem (new BMenuItem ("About"B_UTF8_ELLIPSIS, new BMessage(MSG_ABOUT)));
	fFileMenu->AddSeparatorItem();
	fFileMenu->AddItem (new BMenuItem ("Quit", new BMessage (MSG_QUIT)));
	
	fEmuMenu->AddItem (new BMenuItem ("Start", new BMessage (MSG_CPU_RUN)));
	fEmuMenu->AddItem (new BMenuItem ("Pause", new BMessage (MSG_CPU_PAUSE)));
	fEmuMenu->AddItem (new BMenuItem ("Stop", new BMessage (MSG_CPU_STOP)));
	fEmuMenu->AddSeparatorItem();
	fEmuMenu->AddItem (new BMenuItem ("Reset (soft)", new BMessage (MSG_RST_SOFT)));
	fEmuMenu->AddItem (new BMenuItem ("Reset (hard)", new BMessage (MSG_RST_HARD)));
	fEmuMenu->AddSeparatorItem();
	fEmuMenu->AddItem (new BMenuItem ("Debug...", new BMessage(MSG_CPU_DEBUG)));
	fMenuHeight = fMenu->Bounds().IntegerHeight();
}


void
PretendoWindow::OnLoadCart (BMessage *message)
{
	BString path;
	if (message->FindString ("path", &path) == B_OK) {
		//fMediator->load (path.String());
		
		if (fFramework == OVERLAY_FRAMEWORK) {
			ClearBitmap (true);
		} else {
			fView->SetViewColor (0, 0, 0);
			fView->Invalidate();
		}
		
	}
}


void
PretendoWindow::OnFreeCart (void)
{
	//fMediator->free();
	
	if (fFramework == OVERLAY_FRAMEWORK) {
		ClearBitmap (true);
	} else {
		fView->SetViewColor (0, 0, 0);
		fView->Invalidate();
	}
	
	fPaused = 
	fReallyPaused = false;
	fEmuMenu->ItemAt(1)->SetMarked(false);
}


void
PretendoWindow::OnQuit (void)
{
	QuitRequested();
}


void
PretendoWindow::OnRun (void)
{
	resume_thread(fThread);
}


void
PretendoWindow::OnStop (void)
{	
//	fMediator->stop();
	
	suspend_thread(fThread);
	
	if (fFramework == OVERLAY_FRAMEWORK) {
		ClearBitmap (true);
	} else {
		fView->SetViewColor (0, 0, 0);
		fView->Invalidate();
	}
	
	fPaused = 
	fReallyPaused = false;
	fEmuMenu->ItemAt(1)->SetMarked(false);
}


void
PretendoWindow::OnPause (void)
{
	fPaused = !fReallyPaused;
	fEmuMenu->ItemAt(1)->SetMarked(fPaused);
}


void
PretendoWindow::OnSoftReset (void)
{
//	fMediator->reset();
}


void
PretendoWindow::OnHardReset (void)
{
//	fMediator->hardReset();
	
//	mInputWindow = new InputWindow;
}


void
PretendoWindow::OnDebug (void)
{
}


void
PretendoWindow::RenderLine8 (uint8 *dest, const uint8 *source, int intensity)
{
	int32 width = SCREEN_WIDTH / 4;
	uint8 *palette = reinterpret_cast<uint8 *>(fMappedPalette[intensity]);
	
	while (width--) {
		*(dest+0) = palette[*source++];
		*(dest+1) = palette[*source++];
		*(dest+2) = palette[*source++];
		*(dest+3) = palette[*source++];
		dest += 4 * sizeof(uint8);
	}	
}


void
PretendoWindow::RenderLine16 (uint8 *dest, const uint8 *source, int intensity)
{
	int32 width = SCREEN_WIDTH / 4;
	uint16 *palette = reinterpret_cast<uint16 *>(fMappedPalette[intensity]);
	
	while (width--) {
		*(uint16 *)(dest+0) = palette[*source++];
		*(uint16 *)(dest+2) = palette[*source++];
		*(uint16 *)(dest+4) = palette[*source++];
		*(uint16 *)(dest+6) = palette[*source++];
		dest += 4 * sizeof(uint16);
	}	
}


void
PretendoWindow::RenderLine32 (uint8 *dest, const uint8 *source, int intensity)
{
	int32 width = SCREEN_WIDTH / 4;
	uint32 const *palette = reinterpret_cast<uint32 *>(fMappedPalette[intensity]);
	
	while (width--) {
		*(uint32 *)(dest+0) = palette[*source++];
		*(uint32 *)(dest+4) = palette[*source++];
		*(uint32 *)(dest+8) = palette[*source++];
		*(uint32 *)(dest+12) = palette[*source++];
		dest += 4 * sizeof(uint32);
	}
}


void
PretendoWindow::ClearDirty (void)
{
	uint32 *start = reinterpret_cast<uint32 *>(fDirtyBuffer.bits);
	uint32 *end = reinterpret_cast<uint32 *>(fDirtyBuffer.bits) + (SCREEN_WIDTH*2) * 				(SCREEN_HEIGHT*2);
	
	if (fClear > 0) {
		while (start < end) {
			*start++ ^= 0xffffffff;
		}
		
		fClear--;
	}
}


void 
PretendoWindow::ClearBitmap (bool overlay)
{
	if (overlay) {
		uint8 *bits = fOverlayBits;
		for (int32 y = 0; y < PretendoWindow::SCREEN_HEIGHT; y++) {
			for (int32 row = 0; row < fOverlayBitmap->BytesPerRow(); row += 2) {
				*(uint16 *)(bits+row) = (128 << 8) | (16 >> 0);
			}
		
			bits += fOverlayBitmap->BytesPerRow();
		}
	} else {
		memset (fBitmapBits, 0x00, fBitmap->BitsLength());
	}
}


void 
PretendoWindow::SetRenderer (color_space cs)
{
	switch (cs) {
		case B_CMAP8:
			for (int32 i = 0; i < 8; i++) {
				fMappedPalette[i] = reinterpret_cast<uint8 *>(&fPalette8[i]);
			}
			
			LineRenderer = &PretendoWindow::RenderLine8;
			break;
			
		case B_RGB15:
			for (int32 i = 0; i < 8; i++) {
				fMappedPalette[i] = reinterpret_cast<uint8 *>(&fPalette15[i]);
			}
			
			LineRenderer = &PretendoWindow::RenderLine16;
			break;
			
		case B_RGB16:
			for (int32 i = 0; i < 8; i++) {
				fMappedPalette[i] = reinterpret_cast<uint8 *>(&fPalette16[i]);
			}
			
			LineRenderer = &PretendoWindow::RenderLine16;
			break;
			
		case B_RGB32:
			for (int32 i = 0; i < 8; i++) {
				fMappedPalette[i] = reinterpret_cast<uint8 *>(&fPalette32[i]);
			}
			
			LineRenderer = &PretendoWindow::RenderLine32;
			break;
			
		default:
			;
	}
}


void
PretendoWindow::SetFrontBuffer (uint8 *bits, color_space cs, int32 pixel_width, 
	int32 row_bytes)
{
	fFrontBuffer.bits = reinterpret_cast<uint8 *>(bits);
	fFrontBuffer.pixel_format = cs;
	fFrontBuffer.pixel_width = pixel_width;
	fFrontBuffer.row_bytes = row_bytes;
	
	if (fFramework == WINDOWSCREEN_FRAMEWORK) {
		memset (fFrontBuffer.bits, 0x00, 480 * fFrontBuffer.row_bytes);
		memset (fDirtyBuffer.bits, 0xff, 480 * fFrontBuffer.row_bytes);
		fFrontBuffer.bits += (640 - SCREEN_WIDTH*2) / 2;
	} else {
		for (int32 y = 0; y < SCREEN_HEIGHT; y++) {
			fLineOffsets[y] = fBackBuffer.bits + y * SCREEN_WIDTH * pixel_width;
		}
	}
	
	SetRenderer (cs);
}


void
PretendoWindow::ChangeFramework (VIDEO_FRAMEWORK fw)
{	
	if (fFramework == fw) {
		return;	
	}
	
	fFrameworkChanging = true;
	fPrevFramework = fFramework;
	fFramework = fw;
	
	fVideoMenu->ItemAt(fFramework)->SetMarked(true);
	
	fVideoLocker.Lock();
	
	// tear down previous framework
	switch (fPrevFramework) {
		case NO_FRAMEWORK:
		case DIRECTWINDOW_FRAMEWORK:
			// nothing to do here
			break;
			
		case BITMAP_FRAMEWORK:
			ClearBitmap (false);
			break;
			
		case OVERLAY_FRAMEWORK:
			ClearBitmap (true);
			fView->ClearViewOverlay();
			fView->SetViewColor(0,0,0);
			fView->Invalidate();
			break;
			
		case WINDOWSCREEN_FRAMEWORK:
			fVideoLocker.Unlock();
			
			if (fVideoScreen->Lock()) {
				fVideoScreen->Quit();
			}
			
			fVideoLocker.Lock();
			break;
	}
	
	// build new framework
	switch (fFramework) {
		case NO_FRAMEWORK:
			// use the bitmap's info as a dummy
			SetFrontBuffer (fBitmapBits, B_CMAP8, 1, fBitmap->BytesPerRow());	
			fView->Invalidate();
			break;
			
		case BITMAP_FRAMEWORK:
			SetFrontBuffer (fBitmapBits, B_CMAP8, 1, fBitmap->BytesPerRow());
			ClearBitmap (false);
			fView->Invalidate();
			break;
			
		case OVERLAY_FRAMEWORK:
			rgb_color key;
			SetFrontBuffer (fOverlayBits, B_RGB16, 2, fOverlayBitmap->BytesPerRow());
			ClearBitmap (true);
			fView->SetViewOverlay (fOverlayBitmap, fOverlayBitmap->Bounds(), 
				fView->Bounds(), &key, B_FOLLOW_ALL, B_OVERLAY_FILTER_HORIZONTAL 
				| B_OVERLAY_FILTER_VERTICAL);
			fView->SetViewColor (key);
			fView->Invalidate();
			break;
			
		case DIRECTWINDOW_FRAMEWORK:
			// front buffer *must* be set in DirectConnected(), not here.
			Hide();
			Show();
			SetRenderer (BScreen().ColorSpace());
			break;
			
		case WINDOWSCREEN_FRAMEWORK:
			fVideoLocker.Unlock();
			fVideoScreen = new VideoScreen (this);
			fVideoScreen->Show();
			fVideoLocker.Lock();
			snooze (1000000); 	// extent of my current creative side
			SetFrontBuffer (fVideoScreen->Bits(), B_CMAP8, 1, fVideoScreen->RowBytes());
			fFullScreen = true;
			break;
	}
	
	fVideoLocker.Unlock();
	fFrameworkChanging = false;
}


void
PretendoWindow::DrawDirect (void)
{
	clipping_rect *clip = fClipInfo.clip_list;
	uint8 *dest;
	uint8 *source;
	uint8 *dirty;
	size_t size;
	
	if (! fDoubled) {
		for (int32 i = 0; i < fClipInfo.clip_count; i++, clip++) {
			int32 x = (clip->left - fClipInfo.bounds.left) * fPixelWidth;
			int32 y = (clip->top - fClipInfo.bounds.top);
			int32 w = clip->right - clip->left + 1;
			int32 h = clip->bottom - clip->top + 1;
			
			dest = fFrontBuffer.bits + y * fFrontBuffer.row_bytes + clip->left 
				* fPixelWidth;
			source = fBackBuffer.bits + y * fBackBuffer.row_bytes + x;
			dirty = fDirtyBuffer.bits + y * fBackBuffer.row_bytes + x;
			size = w * fPixelWidth;
		
			while (h > 0) {
				blit_windowed_dirty_mmx (source, dirty, dest, size, fPixelWidth);
				source += fBackBuffer.row_bytes;
				dirty += fBackBuffer.row_bytes;
				dest += fFrontBuffer.row_bytes;
				h--;
			}
		}
	} else {
		int32 h = fClipInfo.bounds.bottom - fClipInfo.bounds.top + 1;
				
		for (int32 i = 0; i < fClipInfo.clip_count; i++, clip++) {
			int32 x = ((clip->left - fClipInfo.bounds.left) >> 1) * fPixelWidth;
			int32 w = clip->right - clip->left + 1;
		
			for (int32 y = 0; y < h; y += 2) {
				if (clip->top - fClipInfo.bounds.top <= y 
					&& clip->bottom - fClipInfo.bounds.top >= y) {
					
					source = fBackBuffer.bits + (y / 2) * fBackBuffer.row_bytes + x;
					dirty = fDirtyBuffer.bits + (y / 2) * fBackBuffer.row_bytes + x;
					dest = fFrontBuffer.bits + y * fFrontBuffer.row_bytes + clip->left 
						* fPixelWidth;
					size = w * fPixelWidth;						
					
					blit_2x_windowed_dirty_mmx (source, dirty, dest, size, fPixelWidth, fFrontBuffer.row_bytes);									//		fPixelWidth, fFrontBuffer.row_bytes);
				}
			}
		}
	}
}
		


void
PretendoWindow::BlitScreen (void)
{	
	if (fFrameworkChanging  || ! fFramework) {
		return;
	}

	fVideoLocker.Lock();
	uint8 *source;
	uint8 *dest;
	size_t size;
	
	switch (fFramework) {
		case NO_FRAMEWORK:
			; // we should never be here
			
		case BITMAP_FRAMEWORK:
			source = reinterpret_cast<uint8 *>(fBackBuffer.bits);
			dest = reinterpret_cast<uint8 *>(fBitmapBits);
			size = PretendoWindow::SCREEN_WIDTH;
		
			for (int32 y = 0; y < SCREEN_HEIGHT; y++) {
				memcpy(dest,source,size);
				//blit_sse (dest, source, size);
				source += fBackBuffer.row_bytes;
				dest += fBitmap->BytesPerRow();
			}
		
			PostMessage (MSG_DRAW_BITMAP);
			fVideoLocker.Unlock();
			return;
			
		case OVERLAY_FRAMEWORK:
			// blit w/overlay
			source = reinterpret_cast<uint8 *>(fBackBuffer.bits);
			dest = reinterpret_cast<uint8 *>(fOverlayBits);
			size = PretendoWindow::SCREEN_WIDTH / 2;
			
	//		blit_overlay(dest, source, size, fPaletteY, fPaletteYCbCr);
		
			#if 0
			for (int32 y = 0; y < PretendoWindow::SCREEN_HEIGHT; y++) {
				asm volatile ("pushl %%edi\n"
					  		  "pushl %%esi\n"
				  			  "pushl %%ebx\n"
				  			  "pushl %%ebp\n"
				  	
				  			  "movl %0, %%edi\n"
				 	 		  "movl %1, %%esi\n"
				  			  "movl %2, %%ecx\n"
				  			  "movl %3, %%eax\n"
					  		  "movl %4, %%edx\n"
					  
				  		  	  "1:\n"
				  		  	  "movl (%%esi), %%ebx\n"
				  		  	  "shrl $16, %%ebx\n"
				  		  	  "movl (%%eax,%%ebx,4), %%ebx\n"
				  		  	  "shll $16, %%ebx\n"
				  
				  		  	  "movl (%%esi), %%ebp\n"
				  		  	  "andl $0xffff, %%ebp\n"
				  		  	  "movl (%%edx,%%ebp,4), %%ebp\n"
		  
		  				  	  "orl %%ebx, %%ebp\n"
				  
				  		  	  "movl %%ebp, (%%edi)\n"
				  		  	  "addl $4, %%edi\n" 
				  		  	  "addl $4, %%esi\n"
				  
				  		  	  "subl $1, %%ecx\n"
				  		  	  "jnz 1b\n"
				  
				  	  		  "popl %%ebp\n"
				  		  	  "popl %%ebx\n"
				  		  	  "popl %%esi\n"
				  		  	  "popl %%edi\n"
				  		  	  : 
				  		  	  : "D"(dest), "S"(source), "c"(size), 
				  		  	  "a"((uint32 *)fPaletteY), "d" ((uint32 *)fPaletteYCbCr)
				  		  	  : "%ebx", "%ebp"
				);
				
				
			
				source += fBackBuffer.row_bytes;
				dest += fOverlayBitmap->BytesPerRow();
			}
			#endif
		
			fVideoLocker.Unlock();
			return;
			
		case DIRECTWINDOW_FRAMEWORK:
			if (fDirectConnected) {				
				ClearDirty();
				DrawDirect();
			}
			
			fVideoLocker.Unlock();
			return;
			
		case WINDOWSCREEN_FRAMEWORK:
			if (fFullScreen) {
				dest = fFrontBuffer.bits;
				source = fBackBuffer.bits;
				
				uint8 *dirty = fDirtyBuffer.bits;
				uint32 dx = fFrontBuffer.row_bytes;
				uint32 sx = fBackBuffer.row_bytes;
			
				for (int32 y = 0; y < PretendoWindow::SCREEN_HEIGHT; y++) {
					blit_2x_dirty_mmx(dest, source, dirty, dx, SCREEN_WIDTH);
					dest += (dx * 2);
					source += sx;
					dirty += sx;
				}
			}
		
			fVideoLocker.Unlock();
			return;
	}
}


void
PretendoWindow::submit_scanline(int scanline, int intensity, const uint8_t *source)
{
	(this->*LineRenderer)(fLineOffsets[scanline], source, intensity);
}

void 
PretendoWindow::set_palette(const color_emphasis_t *intensity, const rgb_color_t *pal)
{
	int32 i, j;
	rgb_color c;
	
	// rgb palettes
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 64; j++) {
			c.red   = static_cast<uint32>(pal[j].r * intensity[i].r);
			c.green = static_cast<uint32>(pal[j].g * intensity[i].g);
			c.blue  = static_cast<uint32>(pal[j].b * intensity[i].b);
			
			fPalette8[i][j] = BScreen().IndexForColor (c.red, c.green, c.blue);
			
			fPalette15[i][j] = ((c.red & 0xf8) << 7);
			fPalette15[i][j] |= ((c.green & 0xf8) << 2);
			fPalette15[i][j] |= ((c.blue & 0xf8) >> 3);
			
			fPalette16[i][j] = ((c.red & 0xf8) << 8);
			fPalette16[i][j] |= ((c.green & 0xfc) << 3);  
			fPalette16[i][j] |= ((c.blue & 0xf8) >> 3);
			
			fPalette32[i][j] = (c.red << 16) | (c.green << 8) | (c.blue & 0xff);
		}
	}
	
	// luma/chroma palette (for overlay)
	uint8 y;
	uint8 cb[65536];
	uint8 cr[65536];
	
	for (i = 0; i < 65536; i++) {
		// separate r,g,b components
		c.red = ((i & 0xf800) >> 11) << 3;
		c.green = ((i & 0x07e0) >> 5) << 2;
		c.blue = (i & 0x001f) << 3;
		
		// convert to y,cb,cr components
		y = fPaletteY[i] = (uint8)((int16)((double)
			(77.0 / 256.0) * (219 * (c.red / 256.0) + 16) +
			(150.0 / 256.0) * (219 * (c.green / 256.0) + 16) +
			(29.0 / 256.0) * (219 * (c.blue / 256.0) + 16)));
			
		cb[i] = (uint8)((int16)((double)
			(-44.0 / 256.0) * (219 * (c.red / 256.0) + 16) - 
			(87.0 / 256.0) * (219 * (c.green / 256.0) + 16) +
			(131.0 / 256.0) * (219 * (c.blue / 256.0) + 16) + 128.0));
			
		cr[i] = (uint8)((int16)((double)
			(131.0 / 256.0) * (219 * (c.red / 256.0) + 16) -
			(110.0 / 256.0) * (219 * (c.green / 256.0) + 16) -
			(21.0 / 256.0) * (219 * (c.blue / 256.0) + 16) + 128.0));
			
		fPaletteYCbCr[i] = (cr[i] << 24) | (0 << 16) | (cb[i] << 8) | (y & 0xff);
	}		
}


void 
PretendoWindow::start_frame()
{
	fMainLocker.Lock();
	
	if (fDirectConnected) {
		fPixelWidth = fFrontBuffer.pixel_width;
		fBackBuffer.row_bytes = SCREEN_WIDTH * fPixelWidth;
	}
}


void
PretendoWindow::end_frame()
{
	BlitScreen();
	fMainLocker.Unlock();
}


status_t
PretendoWindow::threadFunc (void *data)
{
	while (true) {
		std::cout << "this is how we thread the needle!" << std::endl;
	}
	
	return 0;
}
