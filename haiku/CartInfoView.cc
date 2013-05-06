
#include "CartInfoView.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/uuid/sha1.hpp>
#include <libxml/parser.h>
#include <String.h>
#include "NES.h"

using nes::cart;

CartInfoView::CartInfoView(BRect frame)
	: BOutlineListView(frame, "_cart_info_view")
{
	std::vector<uint8_t> image = cart.raw_image();
    BString stringSHA1 = StreamToSHA1 (&image[0], image.size());

    if(xmlDoc *const file = xmlParseFile("/boot/home/Desktop/nescarts.xml")) {
        // get the root element it should be <database>
		if(const xmlNodePtr root = xmlDocGetRootElement(file)) {
			if (xmlStrcmp(root->name, 
				reinterpret_cast<const xmlChar *>("database")) == 0) {
				if(rom_match *const rom = ProcessDatabase(root, 
					reinterpret_cast<const xmlChar *>("sha1"), 
					reinterpret_cast<const xmlChar *>(stringSHA1.String()))) {
					// goto work!
					PrintInfo(rom);
				}
			}
		}
		
		xmlFreeDoc(file);
	}
        
	xmlCleanupParser();	
}




CartInfoView::~CartInfoView()
{
}


void
CartInfoView::AttachedToWindow (void)
{	
	SetFont(be_fixed_font);
	SetFontSize(10.0f);
	
	BOutlineListView::AttachedToWindow();
}


void
CartInfoView::Draw (BRect updateRect)
{
	BOutlineListView::Draw(updateRect);
}


//------------------------------------------------------------------------------
// Name: process_game
// Desc: iterates the <document> childrent of a <cartridge> node.
// Returns: NULL or a pointer to a the <cartridge> node which had a matching 
//          property/value pair
//------------------------------------------------------------------------------
xmlNodePtr 
CartInfoView::ProcessGame(xmlNodePtr game, const xmlChar *search_key, const xmlChar *search_value) {
	// get the list of children, this should be text nodes and <cartridge> nodes
	for(xmlNodePtr cartridge = game->children; cartridge; cartridge = cartridge->next) {
		if (xmlStrcmp(cartridge->name, reinterpret_cast<const xmlChar *>("cartridge")) == 0) {
			// ok we are looking at a cart, let's look at the attributes

			if(xmlChar *const value = xmlGetProp(cartridge, search_key)) {
				if (xmlStrcmp(value, search_value) == 0) {
					BString buffer;
					buffer << "Process Game: " << " " << reinterpret_cast<char *>(value) << " " << reinterpret_cast<const char *>(search_value);
					BListItem *processGame = new BStringItem(buffer);
					AddItem (processGame);
					return cartridge;
				}
			}
		}
	}
	
	return NULL;
}

//------------------------------------------------------------------------------
// Name: process_database
// Desc: iterates the <game> childrent of a <document> node.
// Returns: NULL or a pointer to a rom_match object (statically allocated, 
//          no need to free it
//------------------------------------------------------------------------------

CartInfoView::rom_match_t*
CartInfoView::ProcessDatabase(xmlNodePtr root, const xmlChar *search_key, const xmlChar *search_value) {

	static rom_match match;

	// get the list of children, this should be text nodes and <game> nodes
	for(xmlNodePtr game = root->children; game; game = game->next) {
		if (xmlStrcmp(game->name, reinterpret_cast<const xmlChar *>("game")) == 0) {
			if(xmlNodePtr node = ProcessGame(game, search_key, search_value)) {
				match.game = game;
				match.cart = node;
				return &match;
			}
		}
	}
	
	return NULL;
}

//------------------------------------------------------------------------------
// Name: print_info
// Desc: prints the info associated with a given game/cart
//------------------------------------------------------------------------------
void
CartInfoView::PrintInfo(rom_match *rom)
{	
	char buffer[1024];
	BListItem *gameInfo = new BStringItem("Game Info");
	AddItem (gameInfo);
	for(xmlAttr *properties = rom->game->properties; properties; properties = properties->next) {
		snprintf(buffer, sizeof(buffer), "%-15s: %s", properties->name, xmlGetProp(rom->game, properties->name));
		BListItem *name = new BStringItem(buffer);
		AddUnder(name, gameInfo);
	}

	BListItem *cartInfo = new BStringItem("Cart Info");
	AddItem(cartInfo);
	for(xmlAttr *properties = rom->cart->properties; properties; properties = properties->next) {
		snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(rom->cart, properties->name));
		BListItem *info = new BStringItem(buffer);
		AddUnder(info, cartInfo);
	}
	
	// get the peripherals
	for(xmlNodePtr node = rom->game->children; node; node = node->next) {		
		if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("peripherals")) == 0) {
			BListItem *periphInfo = new BStringItem("Peripheral Info");
			AddItem(periphInfo);
			
			for(xmlNodePtr device = node->children; device; device = device->next) {
				for(xmlAttr *properties = device->properties; properties; properties = properties->next) {
					snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(device, properties->name));
					BListItem *info = new BStringItem(buffer);
					AddUnder(info, periphInfo);
				}
			}
		}
	}
	
	// get the board info
	for(xmlNodePtr board = rom->cart->children; board; board = board->next) {
		if (xmlStrcmp(board->name, reinterpret_cast<const xmlChar *>("board")) == 0) {
			for(xmlNodePtr node = board->children; node; node = node->next) {
				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("prg")) == 0) {
					
					BListItem *prgInfo = new BStringItem("PRG Info");
					AddItem(prgInfo);
					
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(node, properties->name));
						BListItem *info = new BStringItem(buffer);
						AddUnder(info, prgInfo);
					}
					
					Collapse(prgInfo);
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("chr")) == 0) {
					BListItem *chrInfo = new BStringItem("CHR Info");
					AddItem(chrInfo);
					
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(node, properties->name));
						BListItem *info = new BStringItem(buffer);
						AddUnder(info, chrInfo);
					}
					
					Collapse(chrInfo);
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("wram")) == 0) {
					BListItem *wramInfo = new BStringItem("WRAM");
					AddItem(wramInfo);
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(node, properties->name));
						BListItem *info = new BStringItem(buffer);
						AddUnder(info, wramInfo);
					}
					
					Collapse(wramInfo);
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("chip")) == 0) {
					BListItem *chipInfo = new BStringItem("Chip Info");
					AddItem(chipInfo);

					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(node, properties->name));
						BListItem *info =  new BStringItem(buffer);
						AddUnder(info, chipInfo);
					}
					
					Collapse(chipInfo);
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("cic")) == 0) {
					BListItem *cicInfo = new BStringItem ("CIC Info");
					AddItem(cicInfo);

					for (xmlAttr *properties = node->properties; properties; properties = properties->next) {
						snprintf(buffer, sizeof(buffer), "%-15s : %s", properties->name, xmlGetProp(node, properties->name));
						BListItem *info = new BStringItem(buffer);
						AddUnder(info, cicInfo);
					}
					
					Collapse(cicInfo);
				}
			}
			
		}
	}
}


std::string 
CartInfoView::SHA1ToString(uint32 hash[5]) 
{
	static const char hexchars[] = "0123456789ABCDEF";
	char buf[40];

	for(int i = 0; i < 8; ++i) {
		buf[i + 0x00] = hexchars[(hash[0] & 0xf0000000) >> 28];
		buf[i + 0x08] = hexchars[(hash[1] & 0xf0000000) >> 28];
		buf[i + 0x10] = hexchars[(hash[2] & 0xf0000000) >> 28];
		buf[i + 0x18] = hexchars[(hash[3] & 0xf0000000) >> 28];
		buf[i + 0x20] = hexchars[(hash[4] & 0xf0000000) >> 28];

		hash[0] <<= 4;
		hash[1] <<= 4;
		hash[2] <<= 4;
		hash[3] <<= 4;
		hash[4] <<= 4;
	}
	
	return std::string(buf, sizeof(buf));
}


BString
CartInfoView::StreamToSHA1 (const uint8 *stream, int32 length)
{

	boost::uuids::detail::sha1 sha1;
  	uint32_t hash[5];
 	sha1.process_bytes(stream, length);
 	sha1.get_digest(hash);

  	char buffer[32*4];
  	
  	sprintf(buffer, "%08X%08X%08X%08X%08X", hash[0], hash[1], hash[2], hash[3], hash[4]);
  	
  	BString bstHash;
  	bstHash += buffer;
  	return bstHash;
}
