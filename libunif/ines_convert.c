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

#include "ines_convert.h"
#include "load_ines.h"
#include "load_unif.h"
#include "std_func.h"
#include "unif_crc32.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
Tags not supprted yet in conversion
	READ_TAG
*/

/*------------------------------------------------------------------------------
// Name: recommend_board
//----------------------------------------------------------------------------*/
static void recommend_board(const ines_cart_t *cart) {
	assert(cart != 0);

	do {
		const uint32_t ines_mapper    = mapper_INES(cart);
		const uint32_t ines_submapper = submapper_INES(cart);
		const uint32_t prg_size       = prg_size_INES(cart) * 16384;

		(void)ines_submapper;

		/* TODO: expand this to many more mappers :-) as we can best detect them */
		if(ines_mapper == 0) {
			switch(prg_size * 8) {
			case 0x40000:
				printf("Recommended Board: NES-NROM-256\n");
				break;
			case 0x20000:
				printf("Recommended Board: NES-NROM-128\n");
				break;
			default:
				printf("Recommended Board: NES-NROM\n");
				break;
			}
		} else if(ines_mapper == 2) {
			switch(prg_size) {
			case 0x40000:
				printf("Recommended Board: NES-UOROM\n");
				break;
			case 0x20000:
				printf("Recommended Board: NES-UNROM\n");
				break;
			default:
				break;
			}
		}
	} while(0);
}

/*------------------------------------------------------------------------------
// Name: unif_strncasecmp
//----------------------------------------------------------------------------*/
int unif_strncasecmp(const char *s1, const char *s2, size_t n) {
	int ret = 0;

	assert(s1 != 0);
	assert(s2 != 0);

	while(!ret && (*s1 || *s2) && n--) {
		const char cs1 = (const char)tolower(*s1);
		const char cs2 = (const char)tolower(*s2);
		ret = (cs2 - cs1);
		++s1;
		++s2;
	}
	return ret;
}

/*-----------------------------------------------------------------------------
// Name: write_header
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_header(FILE *file) {

	unif_header_t hdr;
	assert(file != 0);

	/* ensure that all reserved bits are 0 */
	memset(&hdr, 0, sizeof(unif_header_t));

	/* setup tag and revision number */
	memcpy(hdr.id, "UNIF", 4);
	hdr.revision = UNIF_REVISION;

	/* write to a file */
	return write_header_UNIF(file, &hdr);
}

/*-----------------------------------------------------------------------------
// Name: write_read
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_read(FILE *file){

#ifdef ENABLE_READ
	unif_chunk_t chunk_header_UNIF;
	/* TODO: come up with a better solution than this */
	char read_data[0x4000]; /* should be enough */
#endif

	assert(file != 0);

	(void)file;

	if(ask_question_yn("do you want a READ block (y/n)? [n]")) {
#ifndef ENABLE_READ
		printf("%s\n", "Temporarily disabled");
#endif
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_pck
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_pck(FILE *file, uint8_t *prg_code, size_t size) {

	assert(file != 0);
	assert(prg_code != 0);
	assert(size != 0);

	if(ask_question_yn("do you want a PCK0 block (PRG0 CRC) (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		uint32_t     crc;

		memcpy(chunk_header_UNIF.id, "PCK0", 4);
		chunk_header_UNIF.length = sizeof(uint32_t);

		crc = unif_crc32(prg_code, (uint32_t)size, 0);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &crc);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_cck
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_cck(FILE * file, uint8_t *chr_code, size_t size) {

	assert(file != 0);
	assert(chr_code != 0);
	assert(size != 0);

	if(ask_question_yn("do you want a CCK0 block (CHR0 CRC) (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		uint32_t     crc;

		memcpy(chunk_header_UNIF.id, "CCK0", 4);
		chunk_header_UNIF.length = sizeof(uint32_t);

		crc = unif_crc32(chr_code, (uint32_t)size, 0);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &crc);
	}

	return UNIF_OK;
}


/*-----------------------------------------------------------------------------
// Name: write_batr
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_batr(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("does this rom have a battery backup (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		uint8_t      chunk_data = 0;

		memcpy(chunk_header_UNIF.id, "BATR", 4);
		chunk_header_UNIF.length = sizeof(uint8_t);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &chunk_data);
	}

	return UNIF_OK;
}


/*-----------------------------------------------------------------------------
// Name: write_vror
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_vror(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("enable VRAM override (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		uint8_t      chunk_data = 0;

		memcpy(chunk_header_UNIF.id, "VROR", 4);
		chunk_header_UNIF.length = sizeof(uint8_t);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &chunk_data);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_mirr
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_mirr(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("do you want a MIRR block (y/n)? [n] ")) {
		unif_chunk_t  chunk_header_UNIF;

		size_t user_input = display_menu(
			6,												/* numer of items */
			"Please select mirroring type: (0,1,2,3,4,5)",	/* prompt */
			"Horizontal Mirroring (Hard Wired)",			/* options */
			"Vertical Mirroring (Hard Wired)",
			"Mirror All Pages From $2000 (Hard Wired)",
			"Mirror All Pages From $2400 (Hard Wired)",
			"Four Screens of VRAM (Hard Wired)",
			"Mirroring Controlled By Mapper Hardware"
			);

		/* write block */
		memcpy(chunk_header_UNIF.id, "MIRR", 4);
		chunk_header_UNIF.length = sizeof(user_input);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &user_input);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_mapr
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_mapr(FILE *file) {

	unif_chunk_t chunk_header_UNIF;
	char         board_name[64];

	assert(file != 0);

	memcpy(chunk_header_UNIF.id, "MAPR", 4);
	chunk_header_UNIF.length = sizeof(board_name);

	printf("%s%lu%s", "what is the board type/name? ( MAX ", sizeof(board_name), " chars ) ");
	memset(board_name, 0, sizeof(board_name));
	if(fgets(board_name, sizeof(board_name), stdin) == 0) {
		return UNIF_INPUT_FAIL;
	}

	/* eliminate new line */
	if(board_name[strlen(board_name) - 1] == '\n') {
		board_name[strlen(board_name) - 1] = '\0';
	}

	return write_chunk_UNIF(file, &chunk_header_UNIF, board_name);
}

/*-----------------------------------------------------------------------------
// Name: write_name
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_name(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("do you want an internal ROM name (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		char         internal_name[0x400]; /* more than enough ? */

		printf("what is the internal name? ");
		memset(internal_name, 0, sizeof(internal_name));
		if(fgets(internal_name, sizeof(internal_name), stdin) == 0) {
			return UNIF_INPUT_FAIL;
		}

		/* eliminate new line */
		if(internal_name[strlen(internal_name) - 1] == '\n') {
			internal_name[strlen(internal_name) - 1] = '\0';
		}

		memcpy(chunk_header_UNIF.id, "NAME", 4);
		chunk_header_UNIF.length = (uint32_t)strlen(internal_name) + 1;

		return write_chunk_UNIF(file, &chunk_header_UNIF, internal_name);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_prg
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_prg(FILE * file, uint8_t *prg_code, size_t size) {

	unif_chunk_t chunk_header_UNIF;

	assert(file != 0);
	assert(prg_code != 0);
	assert(size != 0);

	/* write PRG code */
	memcpy(chunk_header_UNIF.id, "PRG0", 4);
	chunk_header_UNIF.length = (uint32_t)size;

	return write_chunk_UNIF(file, &chunk_header_UNIF, prg_code);
}

/*-----------------------------------------------------------------------------
// Name: write_chr
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_chr(FILE *file, uint8_t *chr_code, size_t size) {

	unif_chunk_t chunk_header_UNIF;

	assert(file != 0);

	if(size != 0) {
		/* write CHR code */
		memcpy(chunk_header_UNIF.id, "CHR0", 4);
		chunk_header_UNIF.length = (uint32_t)size;

		return write_chunk_UNIF(file, &chunk_header_UNIF, chr_code);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: write_tvci
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_tvci(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("do you want a TVCI block (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;

		size_t user_input = display_menu(
			3,											/* numer of items */
			"Please select TV compatibility: (0,1,2)",	/* prompt */
			"Originally NTSC cartridge",				/* options */
			"Originally PAL cartridge",
			"Does not matter" );

		/* write block */
		memcpy(chunk_header_UNIF.id, "TVCI", 4);
		chunk_header_UNIF.length = sizeof(user_input);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &user_input);
	}

	return UNIF_OK;
}


/*-----------------------------------------------------------------------------
// Name: write_ctrl
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_ctrl(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("do you want a CTRL block (y/n)? [n] ")) {
		unif_chunk_t chunk_header_UNIF;
		uint8_t      ctrl_data = 0;

		if(ask_question_yn( "does this cart use a standard joypad (y/n)? [n] ")) {
			ctrl_data |= CTRL_STDJOY;
		}

		if(ask_question_yn( "does this cart use a zapper (y/n)? [n] ")) {
			ctrl_data |= CTRL_ZAPPER;
		}

		if(ask_question_yn( "does this cart use a R.O.B. (y/n)? [n] ")) {
			ctrl_data |= CTRL_ROB;
		}

		if(ask_question_yn( "does this cart use the arkanoid controller (y/n)? [n] ")) {
			ctrl_data |= CTRL_ARKANOID;
		}

		if(ask_question_yn( "does this cart use the powerpad (y/n)? [n] ")) {
			ctrl_data |= CTRL_POWPAD;
		}

		if(ask_question_yn( "does this cart use the Four-Score adapter (y/n)? [n] ")) {
			ctrl_data |= CTRL_4SCORE;
		}

		/* write block */
		memcpy(chunk_header_UNIF.id, "CTRL", 4);
		chunk_header_UNIF.length = sizeof(ctrl_data);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &ctrl_data);
	}

	return UNIF_OK;
}


/*-----------------------------------------------------------------------------
// Name: write_dinf
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_dinf(FILE *file) {

	assert(file != 0);

	if(ask_question_yn("do you want a DINF block (y/n)? [n] ")) {
		unif_chunk_t  chunk_header_UNIF;
		dumper_info_t dinf_block;
		unsigned      tmp_num;
		char          numeric_line[256];

		printf("%s\n", "Name? ( 100 chars max ):");
		memset(dinf_block.dumper_name, 0, sizeof(dinf_block.dumper_name));
		if(fgets(dinf_block.dumper_name, sizeof(dinf_block.dumper_name), stdin) == 0) {
			return UNIF_INPUT_FAIL;
		}

		/* eliminate new line */
		if(dinf_block.dumper_name[strlen(dinf_block.dumper_name) - 1] == '\n') {
			dinf_block.dumper_name[strlen(dinf_block.dumper_name) - 1] = '\0';
		}


		do {
			printf("Day? ");

			if(fgets(numeric_line, sizeof(numeric_line), stdin) == 0) {
				return UNIF_INPUT_FAIL;
			}

			if(sscanf(numeric_line, "%u", &tmp_num) == 1) {
				break;
			}

		} while(1);

		dinf_block.day = (unsigned char)tmp_num;

		do {
			printf("Month? ");
			if(fgets(numeric_line, sizeof(numeric_line), stdin) == 0) {
				return UNIF_INPUT_FAIL;
			}

			if(sscanf(numeric_line, "%u", &tmp_num) == 1) {
				break;
			}
		} while(1);

		dinf_block.month = (unsigned char)tmp_num;

		do {
			printf("Year? ");
			if(fgets(numeric_line, sizeof(numeric_line), stdin) == 0) {
				return UNIF_INPUT_FAIL;
			}

			if(sscanf(numeric_line, "%u", &tmp_num) == 1) {
				break;
			}
		} while(1);

		dinf_block.year = (unsigned short)tmp_num;

		printf("%s\n", "Dumper Agent? ( 100 chars max ) ");
		memset(dinf_block.dumper_agent, 0, sizeof(dinf_block.dumper_agent));
		if(fgets(dinf_block.dumper_agent, sizeof(dinf_block.dumper_agent), stdin) == 0) {
			return UNIF_INPUT_FAIL;
		}

		/* eliminate new line */
		if(dinf_block.dumper_agent[strlen(dinf_block.dumper_agent) - 1] == '\n') {
			dinf_block.dumper_agent[strlen(dinf_block.dumper_agent) - 1] = '\0';
		}

		memcpy(chunk_header_UNIF.id, "DINF", 4);
		chunk_header_UNIF.length = sizeof(dumper_info_t);

		return write_chunk_UNIF(file, &chunk_header_UNIF, &dinf_block);
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: make_unif_file_from_nes
//---------------------------------------------------------------------------*/
void make_unif_file_from_nes(const char *unif_file, const char *ines_file) {

	FILE *file_dest = 0;
	ines_cart_t cart;
	INES_MIRRORING mirroring;

	assert(unif_file != 0);
	assert(ines_file != 0);

	/* read the source .nes file */
	if(load_file_INES(ines_file, &cart) != UNIF_OK) {
		printf("error loading iNES file\n");
		return;
	}

	mirroring = mirroring_INES(&cart);

	printf("source filename     - %s\n", ines_file);
	printf("iNES mapper #       - %d.%d\n", mapper_INES(&cart), submapper_INES(&cart));


	switch(mirroring) {
	case MIRR_HORIZONTAL:
		printf("mirroring           - %s\n", "horizontal");
		break;
	case MIRR_VERTICAL:
		printf("mirroring           - %s\n", "vertical");
		break;
	case MIRR_4SCREEN:
		printf("mirroring           - %s\n", "4 screen");
		break;
	}

	printf("sram enabled        - %d\n", (cart.header->ctrl1 & INES_SRAM) != 0);
	printf("trainer present     - %d\n", (cart.header->ctrl1 & INES_TRAINER) != 0);
	printf("# of 16k PRG Pages  - %d\n", prg_size_INES(&cart));
	printf("# of 8k CHR Pages   - %d\n", chr_size_INES(&cart));
	recommend_board(&cart);

	open_UNIF(unif_file, &file_dest, UNIF_OPEN_WRITE);

	write_header(file_dest);
	write_mapr(file_dest);
	write_name(file_dest);
	write_mirr(file_dest);
	write_ctrl(file_dest);
	write_tvci(file_dest);
	write_read(file_dest);
	write_batr(file_dest);
	write_vror(file_dest);
	write_dinf(file_dest);
	write_prg(file_dest, cart.prg_rom, prg_size_INES(&cart) * 16384);
	write_pck(file_dest, cart.prg_rom, prg_size_INES(&cart) * 16384);
	write_chr(file_dest, cart.chr_rom, chr_size_INES(&cart) * 8192);
	write_cck(file_dest, cart.chr_rom, chr_size_INES(&cart) * 8192);

	close_UNIF(file_dest);
	free_file_INES(&cart);
}


/*-----------------------------------------------------------------------------
// Name: get_ines_mapperstrSourceNES
//---------------------------------------------------------------------------*/
int get_ines_mapper(const char *board_name, ines_info_t *info) {

	const mapr_num_table_t *tbl_ptr = 0;
	mapr_num_table_t table[] = {
		/* name, ines #, chr-rom, four_screen */
		{ "NES-NROM",     0, 1, 0 }, /* NROM: No mapper, PRG-ROM, CHR-ROM */
		{ "UNL-SA-NROM",  0, 1, 0 },
		{ "NES-NROM-128", 0, 1, 0 }, /* NROM-128: No mapper, PRG-ROM, CHR-ROM */
		{ "NES-NROM-256", 0, 1, 0 }, /* NROM-256: No mapper, PRG-ROM, CHR-ROM */
		{ "NES-RROM",     0, 1, 0 }, /* NES-RROM: Same as NROM (Only used in Clu Clu land) */

		{ "NES-SAROM",    1, 1, 0 }, /* 128KB PRG (28-pin), 64KB CHR, 8KB S-RAM or W-RAM */
		{ "NES-SBROM",    1, 1, 0 }, /* 128KB PRG (28-pin), 64KB CHR (28-pin) */
		{ "NES-SCROM",    1, 1, 0 }, /* 128KB PRG (28-pin), 128KB CHR (32-pin) */
		{ "NES-SEROM",    1, 1, 0 }, /* 32KB not banked, 64KB CHR (28-pin) */
		{ "NES-SFROM",    1, 1, 0 }, /* 256KB PRG (32-pin), 64KB CHR (28-pin)  */
		{ "NES-SGROM",    1, 0, 0 }, /* 256KB PRG, 8KB C-RAM banked */
		{ "NES-SHROM",    1, 1, 0 }, /* 32KB not banked, 128KB CHR (32-pin) */
		{ "NES-SJROM",    1, 0, 0 }, /* 256KB PRG, 8KB C-RAM, 8KB S-RAM or W-RAM */
		{ "NES-SKROM",    1, 1, 0 }, /* 256KB PRG, 128KB CHR, 8KB S-RAM or W-RAM */
		{ "NES-SLROM",    1, 1, 0 }, /* 256KB PRG, 128KB CHR */
		{ "NES-SL1ROM",   1, 1, 0 }, /* */
		{ "NES-SL2ROM",   1, 1, 0 }, /* */
		{ "NES-SL3ROM",   1, 1, 0 }, /* */
		{ "NES-SLRROM",   1, 1, 0 }, /* 256KB PRG, 128KB CHR (non JEDEC-pinout) */
		{ "NES-SNROM",    1, 0, 0 }, /* 256KB PRG, 8KB C-RAM banked, 8KB S-RAM or W-RAM  */
		{ "NES-SOROM",    1, 0, 0 }, /* 256KB PRG, 128KB CHR, 16KB S-RAM or W-RAM thru CHR bit */
		{ "NES-SUROM",    1, 0, 0 }, /* 512K PRG thru CHR bit, 8KB C-RAM banked, 8KB S-RAM */
		{ "NES-SVROM",    1, 1, 0 }, /* 512KB PRG thru CHR bit, 8KB CHR banked, 16KB S-RAM or W-RAM thru CHR bit */

		{ "NES-UNROM",    2, 0, 0 }, /* UNROM: 74LS32+74LS161 mapper, 128k PRG, 8k CHR-RAM */
		{ "NES-UOROM",    2, 0, 0 }, /* UOROM: 74LS32+74LS161 mapper, 128k PRG, 8k CHR-RAM */

		{ "NES-CNROM",    3, 0, 0 }, /* CNROM: LS161 mapper, PRG-ROM, CHR-ROM?/CHR-RAM */
		{ "NES-TBROM",    4, 1, 0 },
		{ "NES-TEROM",    4, 1, 0 }, /* TEROM: MMC3A, PRG ROM, CHR ROM, (32k ROMs) */
		{ "NES-TFROM",    4, 1, 0 }, /* TFROM: MMC3B, PRG ROM, CHR ROM (64K of CHR only) */
		{ "NES-TGROM",    4, 0, 0 }, /* TGROM: MMC3C, PRG ROM, VRAM (512K of PRG) */
		{ "NES-TVROM",    4, 1, 0 }, /* TVROM: MMC3B, PRG ROM, CHR ROM, 4K of Nametable RAM (4-screen) */
		{ "NES-TSROM",    4, 1, 0 }, /* TSROM: MMC3A, PRG ROM, CHR ROM, 8k optional RAM */
		{ "NES-TQROM",    4, 0, 0 }, /* TQROM: MMC3B+74HC32, PRG ROM, CHR ROM + 8k of CHR-RAM */
		{ "NES-TKROM",    4, 1, 0 }, /* TKROM: MMC3A, PRG ROM, CHR ROM, 8k optional RAM (battery) */
		{ "NES-TKROM",    4, 1, 0 }, /* TKROM: MMC3A, PRG ROM, CHR ROM, 8k optional RAM (battery) */
		{ "NES-TLSROM",   4, 0, 0 }, /* TLSROM: Same as TLROM */
		{ "NES-DRROM",    4, 1, 1 }, /* DRROM: MMC3, 4K of nametable RAM (for 4-screen), PRG-ROM, CHR-ROM (only in Gauntlet) */
		{ "NES-TLROM",    4, 1, 0 }, /* TLROM: MMC3B, PRG ROM, CHR ROM */
		{ "ELROM",        5, 1, 0 }, /* ELROM: MMC5, PRG-ROM, CHR-ROM */
		{ "ETROM",        5, 1, 0 }, /* ETROM: MMC5, PRG-ROM, CHR-ROM, 2x 8k optionnal RAM (battery) */
		{ "EWROM",        5, 1, 0 }, /* EWROM: MMC5, PRG-ROM, CHR-ROM, 32k optionnal RAM (battery) */
		{ "NES-AOROM",    7, 1, 0 }, /* AOROM: LS161 mapper, PRG-ROM, CHR-ROM */
		{ "NES-ANROM",    7, 0, 0 }, /* ANROM: LS161+LS02 mapper, PRG-ROM, CHR-RAM */
		{ "PNROM",        9, 1, 0 }, /* PNROM: MMC2, PRG-ROM, CHR-ROM */
		{ 0, 0, 0, 0  }

/*
351258: UNROM
351298: UNROM
351908
352026: TLROM (w/ LS32 for VROM enable control)
51555: Acclaim, MMC3B mapper, PRG ROM, CHR ROM
AMROM: LS161, VRAM, PRG-ROM
BNROM: LS161, VRAM, PRG-ROM (Different LS161 bits?  Only used on Deadly Towers)
COB:   "Glop Top" style board
CPROM: LS04, LS08, LS161, 32K ROM, 16K VRAM (bankswitched, Videomation only)
GNROM: LS161 mapper, PRG ROM, CHR ROM
HKROM: MMC6B, PRG-ROM, CHR-ROM, Battery
MHROM: LS161 mapper, black blob chips. Mario Bros / Duck Hunt multi
NES-B4: Same as TLROM
NES-BTR: Sunsoft FME7 mapper, PRG ROM, CHR ROM, 8k optionnal RAM
SC1ROM: MMC1B, PRG ROM, CHR ROM
SCROM: LS161, LS02, VRAM, PRG-ROM (Similar to UNROM)
SEROM: MMC1B, PRG ROM, CHR ROM
SKROM: MMC1B, PRG ROM, CHR ROM, 8k optional RAM (battery)
SN1-ROM AW (Overlord only)
TL1ROM: Same as TLROM
*/
	};


	assert(board_name != 0);
	assert(info != 0);

	info->has_chr_rom = 0;
	info->ines_number = 0;
	info->four_screen = 0;

	for(tbl_ptr = table; tbl_ptr->board_name != 0; ++tbl_ptr) {

		if(unif_strncasecmp(board_name, tbl_ptr->board_name, 32) == 0) {
			info->has_chr_rom = tbl_ptr->chr_rom;
			info->ines_number = tbl_ptr->ines_number;
			info->four_screen = tbl_ptr->four_screen;
			return 0;
		}
	}

	return -1;
}

/*-----------------------------------------------------------------------------
// Name: get_cart_type
//---------------------------------------------------------------------------*/
CART_TYPE get_cart_type(const char *filename) {
    union {
    	ines_header_t ines_header;
    	unif_header_t unif_header;
    } headers;

	FILE *file = fopen(filename, "rb");
	UNIF_RETURN_CODE ret = UNIF_BAD_HEADER;

	/* header is ALWAYS at begining of file */
	fseek(file, 0, SEEK_SET);

	/* read the header data */
	if(fread(&headers, 1, sizeof(headers), file) == 0) {
		fclose(file);
		return CART_INVALID;
	}

	fclose(file);

	ret = check_header_UNIF(&(headers.unif_header));
	if(ret == UNIF_OK || ret == UNIF_DIRTY_HEADER) {
		return CART_UNIF;
    }

	ret = check_header_INES(&(headers.ines_header), 1);
	if(ret == UNIF_OK || ret == UNIF_DIRTY_HEADER) {
		return CART_INES;
    }

	return CART_INVALID;
}
