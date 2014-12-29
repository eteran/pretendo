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

#ifndef LOAD_INES_20080314_H_
#define LOAD_INES_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "lib_ines.h"

typedef enum {
	MIRR_HORIZONTAL,
	MIRR_VERTICAL,
	MIRR_4SCREEN
} INES_MIRRORING;

typedef enum {
	SYS_NES,
	SYS_P10,
	SYS_VS
} INES_SYSTEM;

typedef enum {
	DISP_NTSC,
	DISP_PAL,
	DISP_BOTH
} INES_DISPLAY;

typedef enum {
	PPU_UNKNOWN,
	PPU_RP2C03B,     /* (bog standard RGB palette) */
	PPU_RP2C03G,     /* (similar pallete to above, might have 1 changed colour) */
	PPU_RP2C04_0001, /* (scrambled palette + new colours) */
	PPU_RP2C04_0002, /* (same as above, different scrambling, diff new colours) */
	PPU_RP2C04_0003, /* (similar to above) */
	PPU_RP2C04_0004, /* (similar to above) */
	PPU_RC2C03B,     /* (bog standard palette, seems identical to RP2C03B) */
	PPU_RC2C03C,     /* (similar to above, but with 1 changed colour or so) */
	PPU_RC2C05_01,   /* (all five of these have the normal palette... */
	PPU_RC2C05_02,   /* (...but with different bits returned on 2002) */
	PPU_RC2C05_03,
	PPU_RC2C05_04,
	PPU_RC2C05_05,
	PPU_RESERVED_1,
	PPU_RESERVED_2,
	PPU_RESERVED_3
} INES_PPU;

/* abstract description of a iNES file */
typedef struct {
	ines_header_t *header;       /* raw iNES header */
	uint8_t       *trainer;      /* pointer to 512 byte trainer data or NULL */
	uint8_t       *prg_rom;      /* pointer to PRG data */
	uint8_t       *chr_rom;      /* pointer to CHR data or NULL */
	uint8_t        version;      /* iNES header version */
	uint32_t       prg_size;     /* size of PRG data */
	uint32_t       chr_size;     /* size of CHR data or 0 */
	INES_MIRRORING mirroring;
	INES_SYSTEM    system;
	INES_DISPLAY   display;
	INES_PPU       ppu;
	uint16_t       mapper;       /* iNES mapper number */
	uint16_t       submapper;    /* iNES sub-mapper number */
} ines_cart_t;

/* functions for loading and freeing a iNES file */
INES_RETURN_CODE free_file_INES(ines_cart_t *cart);
INES_RETURN_CODE load_file_INES(const char *filename, ines_cart_t *cart);
INES_RETURN_CODE load_gzip_INES(const char *filename, ines_cart_t *cart);
INES_RETURN_CODE write_file_INES(const char *filename, const ines_cart_t *cart);

/* API access to iNES data, works with version 2.0 ROMs as well */
uint32_t prg_hash_INES(const ines_cart_t *cart);
uint32_t chr_hash_INES(const ines_cart_t *cart);
uint32_t rom_hash_INES(const ines_cart_t *cart);

#ifdef __cplusplus
}
#endif

#endif

