
#ifndef _CART_INFO_WINDOW_H_
#define _CART_INFO_WINDOW_H_

#include <Window.h>
#include <TabView.h>
#include <ListView.h>
#include <libxml/parser.h>
#include <string>

using std::string;

class CartInfoWindow : public BWindow
{
	public:
	typedef struct rom_match {
		xmlNodePtr game;
		xmlNodePtr cart;
	} rom_match_t;
	
	public:
			CartInfoWindow();
	virtual ~CartInfoWindow();
	
	public:
	virtual void MessageReceived (BMessage *message);
	virtual bool QuitRequested (void);
	
	private:
	
	BListView *fGameInfoList;
	BListView *fCartInfoList;
	BListView *fPeripheralInfoList;
	BListView *fPRGInfoList;
	BListView *fCHRInfoList;
	BListView *fWRAMInfoList;
	BListView *fMapperInfoList;
	BListView *fCICInfoList;
		
	BView *fMainView;
	BTabView *fTabView;
	
	public:
	BListView *GameInfoList (void) { return fGameInfoList; }
	BListView *CartInfoList (void) { return fCartInfoList; }
	BListView *PeripheralsList(void) { return fPeripheralInfoList; }
	BListView *PRGInfoList (void) { return fPRGInfoList; }
	BListView *CHRInfoList (void) { return fCHRInfoList; }
	BListView *WRAMInfoList (void) { return fWRAMInfoList; }
	BListView *MapperInfoList (void) { return fMapperInfoList; }
	BListView *CICInfoList (void) { return fCICInfoList; }
	BTabView *TabView (void) { return fTabView; }
	
	private:
	void print_info(rom_match *rom);
	rom_match *process_database(xmlNodePtr root, const xmlChar *search_key, const xmlChar *search_value);
	xmlNodePtr process_game(xmlNodePtr game, const xmlChar *search_key, const xmlChar *search_value);
	std::string sha1_to_string(uint32 hash[5]);
	BString StreamToSHA1 (const uint8 *stream, int32 length);
};


#endif //_CART_INFOWINDOW_H_
