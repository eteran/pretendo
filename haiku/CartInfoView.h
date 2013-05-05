
#ifndef _CART_INFO_VIEW_
#define _CART_INFO_VIEW_

#include <OutlineListView.h>
#include <String.h>
#include <Alert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/uuid/sha1.hpp>
#include <libxml/parser.h>
#include <stdio.h>


class CartInfoView : public BOutlineListView
{
	public:
	typedef struct rom_match {
		xmlNodePtr game;
		xmlNodePtr cart;
	} rom_match_t;
	
	public:
	CartInfoView(BRect frame);
	~CartInfoView();
	
	public:
	virtual void AttachedToWindow (void);
	virtual void Draw (BRect frame);
	
	private:
	void PrintInfo(rom_match *rom);
	rom_match_t *ProcessDatabase(xmlNodePtr root, const xmlChar *search_key, const xmlChar *search_value);
	xmlNodePtr ProcessGame(xmlNodePtr game, const xmlChar *search_key, const xmlChar *search_value);
	std::string SHA1ToString(uint32 hash[5]);
	BString StreamToSHA1 (const uint8 *stream, int32 length);
};
	

#endif // _CART_INFO_VIEW
