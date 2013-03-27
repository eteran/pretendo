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

#ifndef INES_CONVERT_20080314_H_
#define INES_CONVERT_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "unif_types.h"

#include <stdio.h>

typedef enum {
	CART_UNIF,
	CART_INES,
	CART_INVALID
} CART_TYPE;

typedef struct {
	int     has_chr_rom;
	uint8_t ines_number;
	int     four_screen;
} ines_info_t;

typedef struct {
	const char *board_name;
	uint8_t     ines_number;
	int         chr_rom;
	int         four_screen;
} mapr_num_table_t;

void make_unif_file_from_nes(const char *unif_file, const char *ines_file);
int get_ines_mapper(const char *board_name, ines_info_t *info);
CART_TYPE get_cart_type(const char *filename);

UNIF_RETURN_CODE write_header(FILE *file);
UNIF_RETURN_CODE write_mapr(FILE *file);
UNIF_RETURN_CODE write_name(FILE *file);
UNIF_RETURN_CODE write_prg(FILE *file, uint8_t *prg_code, size_t size);
UNIF_RETURN_CODE write_chr(FILE *file, uint8_t *chr_code, size_t size);
UNIF_RETURN_CODE write_read(FILE *file);
UNIF_RETURN_CODE write_tvsc(FILE *file);
UNIF_RETURN_CODE write_dinf(FILE *file);
UNIF_RETURN_CODE write_batr(FILE *file);
UNIF_RETURN_CODE write_pck(FILE *file, uint8_t *prg_code, size_t size);
UNIF_RETURN_CODE write_cck(FILE *file, uint8_t *chr_code, size_t size);

#ifdef __cplusplus
}
#endif

#endif

