
#include "CartInfoWindow.h"

#include <String.h>
#include <Alert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/uuid/sha1.hpp>
#include <libxml/parser.h>
#include <stdio.h>

#include "NES.h"


using nes::cart;

CartInfoWindow::CartInfoWindow()
	: BWindow(BRect(0, 0, 0, 0), "Cart Info", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
	B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	fCartInfoView(NULL)
{
	LIBXML_TEST_VERSION;
	
	ResizeTo(800, 500);
	CenterOnScreen();
	
	fCartInfoView = new CartInfoView(Bounds());
	AddChild(fCartInfoView);
	
	std::vector<uint8_t> image = cart.raw_image();
    BString stringSHA1 = StreamToSHA1 (&image[0], image.size());

    if(xmlDoc *const file = xmlParseFile("/boot/home/Desktop/nescarts.xml")) {
        // get the root element it should be <database>
		if(const xmlNodePtr root = xmlDocGetRootElement(file)) {
			if (xmlStrcmp(root->name, 
				reinterpret_cast<const xmlChar *>("database")) == 0) {
				if(rom_match *const rom = process_database(root, 
					reinterpret_cast<const xmlChar *>("sha1"), 
					reinterpret_cast<const xmlChar *>(stringSHA1.String()))) {
					// goto work!
					print_info(rom);
				}
			}
		}
		
		xmlFreeDoc(file);
	}
        
	xmlCleanupParser();	
}


CartInfoWindow::~CartInfoWindow()
{
}

//------------------------------------------------------------------------------
// Name: process_game
// Desc: iterates the <document> childrent of a <cartridge> node.
// Returns: NULL or a pointer to a the <cartridge> node which had a matching 
//          property/value pair
//------------------------------------------------------------------------------
xmlNodePtr CartInfoWindow::process_game(xmlNodePtr game, const xmlChar *search_key, const xmlChar *search_value) {
	// get the list of children, this should be text nodes and <cartridge> nodes
	for(xmlNodePtr cartridge = game->children; cartridge; cartridge = cartridge->next) {
		if (xmlStrcmp(cartridge->name, reinterpret_cast<const xmlChar *>("cartridge")) == 0) {
			// ok we are looking at a cart, let's look at the attributes

			if(xmlChar *const value = xmlGetProp(cartridge, search_key)) {
				if (xmlStrcmp(value, search_value) == 0) {
					printf("process game: %s, %s\n", value, search_value);
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

CartInfoWindow::rom_match *
CartInfoWindow::process_database(xmlNodePtr root, const xmlChar *search_key, const xmlChar *search_value) {

	static rom_match match;

	// get the list of children, this should be text nodes and <game> nodes
	for(xmlNodePtr game = root->children; game; game = game->next) {
		if (xmlStrcmp(game->name, reinterpret_cast<const xmlChar *>("game")) == 0) {
			if(xmlNodePtr node = process_game(game, search_key, search_value)) {
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
CartInfoWindow::print_info(rom_match *rom) {

	printf("GAME INFO\n");
	for(xmlAttr *properties = rom->game->properties; properties; properties = properties->next) {
		printf("%-15s : %s\n", properties->name, xmlGetProp(rom->game, properties->name));
		
	}

	printf("\n");
	printf("CART INFO\n");
	for(xmlAttr *properties = rom->cart->properties; properties; properties = properties->next) {
		printf("%-15s : %s\n", properties->name, xmlGetProp(rom->cart, properties->name));
	}
	
	// get the peripherals
	for(xmlNodePtr node = rom->game->children; node; node = node->next) {		
		if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("peripherals")) == 0) {
			printf("\n");
			printf("PERIPHERAL INFO\n");

			for(xmlNodePtr device = node->children; device; device = device->next) {
				for(xmlAttr *properties = device->properties; properties; properties = properties->next) {
					printf("%-15s : %s\n", properties->name, xmlGetProp(device, properties->name));
				}
			}
		}
	}
	
	// get the board info
	for(xmlNodePtr board = rom->cart->children; board; board = board->next) {
		if (xmlStrcmp(board->name, reinterpret_cast<const xmlChar *>("board")) == 0) {
			for(xmlNodePtr node = board->children; node; node = node->next) {
				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("prg")) == 0) {
					printf("\n");
					printf("PRG INFO\n");
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						printf("%-15s : %s\n", properties->name, xmlGetProp(node, properties->name));
					}
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("chr")) == 0) {
					printf("\n");
					printf("CHR INFO\n");
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						printf("%-15s : %s\n", properties->name, xmlGetProp(node, properties->name));
					}
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("wram")) == 0) {
					printf("\n");
					printf("WRAM INFO\n");
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						printf("%-15s : %s\n", properties->name, xmlGetProp(node, properties->name));
					}
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("chip")) == 0) {
					printf("\n");
					printf("CHIP INFO\n");
					for(xmlAttr *properties = node->properties; properties; properties = properties->next) {
						printf("%-15s : %s\n", properties->name, xmlGetProp(node, properties->name));
					}
				}

				if (xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("cic")) == 0) {
					printf("\n");
					printf("CIC INFO\n");
					for (xmlAttr *properties = node->properties; properties; properties = properties->next) {
						printf("%-15s : %s\n", properties->name, xmlGetProp(node, properties->name));
					}
				}
			}
		}
	}
}

std::string 
CartInfoWindow::SHA1ToString(uint32 hash[5]) 
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
CartInfoWindow::StreamToSHA1 (const uint8 *stream, int32 length)
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


void
CartInfoWindow::MessageReceived (BMessage *message)
{
	switch (message->what) {
		default:
			BWindow::MessageReceived (message);
			break;
		
		case 'OKAY':
			Quit();
			break;
	}
}


bool
CartInfoWindow::QuitRequested (void)
{
	return true;
}

// gcc -I/boot/common/include/libxml2 -I/pretendo/boost_1_50_0 rominfo.cc -o rominfo -lxml2 -lstdc++
