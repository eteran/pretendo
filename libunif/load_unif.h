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

#ifndef LOAD_UNIF_20080314_H_
#define LOAD_UNIF_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "lib_unif.h"

typedef struct {
	uint32_t       pck[0x10];
	uint32_t       cck[0x10];
	uint8_t       *prg[0x10];
	uint8_t       *chr[0x10];
	uint32_t       prg_pages[0x10];
	uint32_t       chr_pages[0x10];
	unif_header_t  header;
	dumper_info_t *dumper_info;
	char          *mapr_name;
	char          *read_text;
	char          *name;
	uint8_t       *tvci_byte;
	uint8_t       *ctrl_byte;
	uint8_t       *batr_byte;
	uint8_t       *vror_byte;
	uint8_t       *mirr_data;
} unif_cart_t;

/* some example implementations for loading and freeing a UNIF file */
UNIF_RETURN_CODE load_file_UNIF(const char *filename, unif_cart_t *cart);
UNIF_RETURN_CODE free_file_UNIF(unif_cart_t *cart);

#ifdef __cplusplus
}
#endif

#endif


