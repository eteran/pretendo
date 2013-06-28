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

#ifndef LIB_INES_20080314_H_
#define LIB_INES_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "ines_types.h"
#include <stdio.h>

/* layout of first sixteen bytes of nes cartridge in ines format */
typedef struct {
	char    ines_signature[4]; /* 0x1A53454E (NES file signature) */
	uint8_t prg_size;          /* in 16k banks */
	uint8_t chr_size;          /* in 8k banks */
	uint8_t ctrl1;             /* %NNNN.FTBM */
	uint8_t ctrl2;             /* %NNNN.SSPV (version 2.0 when SS = 10b*/

	/* in iNES 2.0, these mean something, otherwise, should be 0 */
	union {
		struct ines2_t {
			uint8_t byte8;  /* %SSSS.MMMM */
			uint8_t byte9;  /* %CCCC.PPPP */
			uint8_t byte10; /* %pppp.PPPP */
			uint8_t byte11; /* %cccc.CCCC */
			uint8_t byte12; /* %xxxx.xxBP */
			uint8_t byte13; /* %MMMM.PPPP */
			uint8_t byte14; /* reserved */
			uint8_t byte15; /* reserved */
		} ines2;

		struct ines1_t {
			uint32_t reserved_1;
			uint32_t reserved_2;
		} ines1;
	} extended;
} ines_header_t;

/* function prototypes */
INES_RETURN_CODE check_header_INES(const ines_header_t *header, int version);
INES_RETURN_CODE close_INES(FILE *file);
INES_RETURN_CODE open_INES(const char *filename, FILE **file, INES_OPEN_MODE mode);
INES_RETURN_CODE read_data_INES(FILE *file, uint8_t *dst, size_t len);
INES_RETURN_CODE read_header_INES(FILE *file, ines_header_t *header);
INES_RETURN_CODE write_header_INES(FILE *file, const ines_header_t *header);

#ifdef __cplusplus
}
#endif

#endif

