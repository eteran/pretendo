
#ifndef _CART_INFO_WINDOW_H_
#define _CART_INFO_WINDOW_H_

#include <Window.h>
#include <TabView.h>
#include <OutlineListView.h>
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
	
	BOutlineListView *fGameInfoOutline;
	BOutlineListView *fCartInfoOutline;
	BOutlineListView *fPeripheralsOutline;
	
	BView *fMainView;
	BTabView *fTabView;
	
	public:
	BOutlineListView *GameInfoOutline (void) { return fGameInfoOutline; }
	BOutlineListView *CartInfoOutline (void) { return fCartInfoOutline; }
	BOutlineListView *PeripheralsOutline (void) { return fPeripheralsOutline; }
	
	BTabView *TabView (void) { return fTabView; }
	
	private:
	void print_info(rom_match *rom);
	rom_match *process_database(xmlNodePtr root, const xmlChar *search_key, const xmlChar *search_value);
	xmlNodePtr process_game(xmlNodePtr game, const xmlChar *search_key, const xmlChar *search_value);
	std::string sha1_to_string(uint32 hash[5]);
	BString StreamToSHA1 (const char *stream, int32 length);
};


#endif //_CART_INFOWINDOW_H_
