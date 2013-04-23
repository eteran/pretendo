/*
Copyright (C) 2000 - 2011 Evan Teran
                          eteran@alum.rit.edu

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

#include "load_unif.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define SAFE_FREE(x) do { free(x); (x) = 0; } while(0)

/*-----------------------------------------------------------------------------
// name: load_file_UNIF(const char *filename, unif_cart_t *cart)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE load_file_UNIF(const char *filename, unif_cart_t *cart) {
	FILE *unif_file = 0;
	UNIF_RETURN_CODE ret_val;

	assert(cart != 0);
	assert(filename != 0);

	/* clear out out structure we use to store file info */
	memset(cart, 0, sizeof(unif_cart_t));

	/* open the file */
	ret_val = open_UNIF(filename, &unif_file, UNIF_OPEN_READ);

	if(ret_val == UNIF_OK) {

		/* read the header */
		/* automaticaly sets file pointer to begining of file */
		ret_val = read_header_UNIF(unif_file, &cart->header);
		if(ret_val != UNIF_OK) {
			close_UNIF(unif_file);
			return ret_val;
		}

		/* do a check to see if the header is valid */
		ret_val = check_header_UNIF(&(cart->header));
		if(ret_val != UNIF_OK) {
			close_UNIF(unif_file);
			return ret_val;
		}

		/* as long as we dont have an error, proccess tags */
		while(ret_val == UNIF_OK) {
			unif_chunk_t chunk_hdr = { "", 0 };

			union {
				void     *ptr;
				uint8_t  *ptr8;
				uint32_t *ptr32;
				char     *str;
			} chunk_data = { 0 };

			ret_val = read_chunk_UNIF(unif_file, &chunk_hdr, &chunk_data.ptr);

			/* did we hit the end of the file? */
			if(ret_val == UNIF_END_OF_FILE)
				break;

			/* if the read was ok, store it */
			if(ret_val == UNIF_OK) {

				if(memcmp(chunk_hdr.id, "DINF", 4) == 0) {
					cart->dumper_info = chunk_data.ptr;
				} else if(memcmp(chunk_hdr.id, "MAPR", 4) == 0) {
					cart->mapr_name = chunk_data.str;
				} else if(memcmp(chunk_hdr.id, "READ", 4) == 0) {
					cart->read_text = chunk_data.str;
				} else if(memcmp(chunk_hdr.id, "NAME", 4) == 0) {
					cart->name = chunk_data.str;
				} else if(memcmp(chunk_hdr.id, "TVCI", 4) == 0) {
					cart->tvci_byte = chunk_data.ptr8;
				} else if(memcmp(chunk_hdr.id, "CTRL", 4) == 0) {
					cart->ctrl_byte = chunk_data.ptr8;
				} else if(memcmp(chunk_hdr.id, "BATR", 4) == 0) {
					cart->batr_byte = chunk_data.ptr8;
				} else if(memcmp(chunk_hdr.id, "VROR", 4) == 0) {
					cart->vror_byte = chunk_data.ptr8;
				} else if(memcmp(chunk_hdr.id, "MIRR", 4) == 0) {
					cart->mirr_data = chunk_data.ptr8;

				} else if(memcmp(chunk_hdr.id, "PRG", 3) == 0) {
					switch(chunk_hdr.id[3]) {
					case '0':
						cart->prg[0x00] = chunk_data.ptr8;
						cart->prg_pages[0x00] = (chunk_hdr.length >> 14);
						break;
					case '1':
						cart->prg[0x01] = chunk_data.ptr8;
						cart->prg_pages[0x01] = (chunk_hdr.length >> 14);
						break;
					case '2':
						cart->prg[0x02] = chunk_data.ptr8;
						cart->prg_pages[0x02] = (chunk_hdr.length >> 14);
						break;
					case '3':
						cart->prg[0x03] = chunk_data.ptr8;
						cart->prg_pages[0x03] = (chunk_hdr.length >> 14);
						break;
					case '4':
						cart->prg[0x04] = chunk_data.ptr8;
						cart->prg_pages[0x04] = (chunk_hdr.length >> 14);
						break;
					case '5':
						cart->prg[0x05] = chunk_data.ptr8;
						cart->prg_pages[0x05] = (chunk_hdr.length >> 14);
						break;
					case '6':
						cart->prg[0x06] = chunk_data.ptr8;
						cart->prg_pages[0x06] = (chunk_hdr.length >> 14);
						break;
					case '7':
						cart->prg[0x07] = chunk_data.ptr8;
						cart->prg_pages[0x07] = (chunk_hdr.length >> 14);
						break;
					case '8':
						cart->prg[0x08] = chunk_data.ptr8;
						cart->prg_pages[0x08] = (chunk_hdr.length >> 14);
						break;
					case '9':
						cart->prg[0x09] = chunk_data.ptr8;
						cart->prg_pages[0x09] = (chunk_hdr.length >> 14);
						break;
					case 'A':
						cart->prg[0x0a] = chunk_data.ptr8;
						cart->prg_pages[0x0a] = (chunk_hdr.length >> 14);
						break;
					case 'B':
						cart->prg[0x0b] = chunk_data.ptr8;
						cart->prg_pages[0x0b] = (chunk_hdr.length >> 14);
						break;
					case 'C':
						cart->prg[0x0c] = chunk_data.ptr8;
						cart->prg_pages[0x0c] = (chunk_hdr.length >> 14);
						break;
					case 'D':
						cart->prg[0x0d] = chunk_data.ptr8;
						cart->prg_pages[0x0d] = (chunk_hdr.length >> 14);
						break;
					case 'E':
						cart->prg[0x0e] = chunk_data.ptr8;
						cart->prg_pages[0x0e] = (chunk_hdr.length >> 14);
						break;
					case 'F':
						cart->prg[0x0f] = chunk_data.ptr8;
						cart->prg_pages[0x0f] = (chunk_hdr.length >> 14);
						break;
					default:
						SAFE_FREE(chunk_data.ptr);
					}
				} else if(memcmp(chunk_hdr.id, "CHR", 3) == 0) {
					switch(chunk_hdr.id[3]) {
					case '0':
						cart->chr[0x00] = chunk_data.ptr8;
						cart->chr_pages[0x00] = (chunk_hdr.length >> 13);
						break;
					case '1':
						cart->chr[0x01] = chunk_data.ptr8;
						cart->chr_pages[0x01] = (chunk_hdr.length >> 13);
						break;
					case '2':
						cart->chr[0x02] = chunk_data.ptr8;
						cart->chr_pages[0x02] = (chunk_hdr.length >> 13);
						break;
					case '3':
						cart->chr[0x03] = chunk_data.ptr8;
						cart->chr_pages[0x03] = (chunk_hdr.length >> 13);
						break;
					case '4':
						cart->chr[0x04] = chunk_data.ptr8;
						cart->chr_pages[0x04] = (chunk_hdr.length >> 13);
						break;
					case '5':
						cart->chr[0x05] = chunk_data.ptr8;
						cart->chr_pages[0x05] = (chunk_hdr.length >> 13);
						break;
					case '6':
						cart->chr[0x06] = chunk_data.ptr8;
						cart->chr_pages[0x06] = (chunk_hdr.length >> 13);
						break;
					case '7':
						cart->chr[0x07] = chunk_data.ptr8;
						cart->chr_pages[0x07] = (chunk_hdr.length >> 13);
						break;
					case '8':
						cart->chr[0x08] = chunk_data.ptr8;
						cart->chr_pages[0x08] = (chunk_hdr.length >> 13);
						break;
					case '9':
						cart->chr[0x09] = chunk_data.ptr8;
						cart->chr_pages[0x09] = (chunk_hdr.length >> 13);
						break;
					case 'A':
						cart->chr[0x0a] = chunk_data.ptr8;
						cart->chr_pages[0x0a] = (chunk_hdr.length >> 13);
						break;
					case 'B':
						cart->chr[0x0b] = chunk_data.ptr8;
						cart->chr_pages[0x0b] = (chunk_hdr.length >> 13);
						break;
					case 'C':
						cart->chr[0x0c] = chunk_data.ptr8;
						cart->chr_pages[0x0c] = (chunk_hdr.length >> 13);
						break;
					case 'D':
						cart->chr[0x0d] = chunk_data.ptr8;
						cart->chr_pages[0x0d] = (chunk_hdr.length >> 13);
						break;
					case 'E':
						cart->chr[0x0e] = chunk_data.ptr8;
						cart->chr_pages[0x0e] = (chunk_hdr.length >> 13);
						break;
					case 'F':
						cart->chr[0x0f] = chunk_data.ptr8;
						cart->chr_pages[0x0f] = (chunk_hdr.length >> 13);
						break;
					default:
						SAFE_FREE(chunk_data.ptr);
					}
				} else if(memcmp(chunk_hdr.id, "PCK", 3) == 0) {
					switch(chunk_hdr.id[3]) {
					case '0': cart->pck[0x00] = *chunk_data.ptr32; break;
					case '1': cart->pck[0x01] = *chunk_data.ptr32; break;
					case '2': cart->pck[0x02] = *chunk_data.ptr32; break;
					case '3': cart->pck[0x03] = *chunk_data.ptr32; break;
					case '4': cart->pck[0x04] = *chunk_data.ptr32; break;
					case '5': cart->pck[0x05] = *chunk_data.ptr32; break;
					case '6': cart->pck[0x06] = *chunk_data.ptr32; break;
					case '7': cart->pck[0x07] = *chunk_data.ptr32; break;
					case '8': cart->pck[0x08] = *chunk_data.ptr32; break;
					case '9': cart->pck[0x09] = *chunk_data.ptr32; break;
					case 'A': cart->pck[0x0a] = *chunk_data.ptr32; break;
					case 'B': cart->pck[0x0b] = *chunk_data.ptr32; break;
					case 'C': cart->pck[0x0c] = *chunk_data.ptr32; break;
					case 'D': cart->pck[0x0d] = *chunk_data.ptr32; break;
					case 'E': cart->pck[0x0e] = *chunk_data.ptr32; break;
					case 'F': cart->pck[0x0f] = *chunk_data.ptr32; break;
					default:
						break;
					}
					SAFE_FREE(chunk_data.ptr);
				} else if(memcmp(chunk_hdr.id, "CCK", 3) == 0) {
					switch(chunk_hdr.id[3]) {
					case '0': cart->cck[0x00] = *chunk_data.ptr32; break;
					case '1': cart->cck[0x01] = *chunk_data.ptr32; break;
					case '2': cart->cck[0x02] = *chunk_data.ptr32; break;
					case '3': cart->cck[0x03] = *chunk_data.ptr32; break;
					case '4': cart->cck[0x04] = *chunk_data.ptr32; break;
					case '5': cart->cck[0x05] = *chunk_data.ptr32; break;
					case '6': cart->cck[0x06] = *chunk_data.ptr32; break;
					case '7': cart->cck[0x07] = *chunk_data.ptr32; break;
					case '8': cart->cck[0x08] = *chunk_data.ptr32; break;
					case '9': cart->cck[0x09] = *chunk_data.ptr32; break;
					case 'A': cart->cck[0x0a] = *chunk_data.ptr32; break;
					case 'B': cart->cck[0x0b] = *chunk_data.ptr32; break;
					case 'C': cart->cck[0x0c] = *chunk_data.ptr32; break;
					case 'D': cart->cck[0x0d] = *chunk_data.ptr32; break;
					case 'E': cart->cck[0x0e] = *chunk_data.ptr32; break;
					case 'F': cart->cck[0x0f] = *chunk_data.ptr32; break;
					default:
						break;
					}
					SAFE_FREE(chunk_data.ptr);
				} else {
					/* since we aren't storing this pointer, free it */
					SAFE_FREE(chunk_data.ptr);
				}

			} else {
				fprintf(stderr, "%s\n", get_error_string_UNIF(ret_val));
				break;
			}
		}

		close_UNIF(unif_file);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: free_file_UNIF(unif_cart_t *cart)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE free_file_UNIF(unif_cart_t *cart) {
	size_t i;

	assert(cart != 0);

	SAFE_FREE(cart->mapr_name);
	SAFE_FREE(cart->dumper_info);
	SAFE_FREE(cart->read_text);
	SAFE_FREE(cart->name);

	SAFE_FREE(cart->batr_byte);
	SAFE_FREE(cart->ctrl_byte);
	SAFE_FREE(cart->mirr_data);
	SAFE_FREE(cart->tvci_byte);
	SAFE_FREE(cart->vror_byte);

	for(i = 0; i < 0x10; i++) {
		SAFE_FREE(cart->prg[i]);
		SAFE_FREE(cart->chr[i]);
	}

	return UNIF_OK;
}

