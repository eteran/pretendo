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

#ifndef LIB_UNIF_20080314_H_
#define LIB_UNIF_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "unif_types.h"

#include <stdio.h>

/* the file we read MAY NOT have a revision greater than this */
#define UNIF_REVISION 0x00000007

/* definitions for the CTRL block */
#define CTRL_STDJOY   0x01 /* Bit 0: Regular Joypad           */
#define CTRL_ZAPPER   0x02 /* Bit 1: Zapper                   */
#define CTRL_ROB      0x04 /* Bit 2: R.O.B                    */
#define CTRL_ARKANOID 0x08 /* Bit 3: Arkanoid Controller      */
#define CTRL_POWPAD   0x10 /* Bit 4: Power Pad                */
#define CTRL_4SCORE   0x20 /* Bit 5: Four-Score adapter       */
#define CTRL_RESERVE1 0x40 /* Bit 6: Expansion (Do not touch) */
#define CTRL_RESERVE2 0x80 /* Bit 7: Expansion (Do not touch) */

/* header structure */
typedef struct {
	char     id[4];         /* MUST be "UNIF", NOT null terminated */
	uint32_t revision;      /* Revision number                     */
	uint8_t  expansion[24]; /* reserved                            */
} unif_header_t;

/* chunk header */
typedef struct {
	char id[4];      /* Chunk identification string.         */
	uint32_t length; /* Data length, in little-endian format */
} unif_chunk_t;

/* handy for reading dumper info */
typedef struct {
	char dumper_name[100];  /* NULL-terminated string containing the name of
	                         * the person who dumped the cart.               */
	unsigned char day;      /* Day of the month when cartridge was dumped    */
	unsigned char month;    /* Month of the year when cartridge was dumped   */
	unsigned short year;    /* Year during which the cartridge was dumped    */
	char dumper_agent[100]; /* NULL-terminated string containing the name of
                             * the ROM-dumping means used                    */
} dumper_info_t;


/* function prototypes */
UNIF_RETURN_CODE open_UNIF(const char *filename, FILE **file, UNIF_OPEN_MODE mode);
UNIF_RETURN_CODE close_UNIF(FILE *file);

UNIF_RETURN_CODE read_header_UNIF(FILE *file, unif_header_t *header);
UNIF_RETURN_CODE write_header_UNIF(FILE *file, unif_header_t *header);
UNIF_RETURN_CODE check_header_UNIF(unif_header_t *header);

UNIF_RETURN_CODE read_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void **chunk_data );
UNIF_RETURN_CODE write_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void *chunk_data );

const char *get_error_string_UNIF(UNIF_RETURN_CODE code);

#ifdef __cplusplus
}
#endif

#endif

