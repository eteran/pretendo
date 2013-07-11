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

#include "load_ines.h"
#include "ines_crc32.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Flags in ines_header_t.ctrl1 */
#define INES_MIRROR  0x01
#define INES_SRAM    0x02
#define INES_TRAINER 0x04
#define INES_4SCREEN 0x08

#define PRG_BLOCK_SIZE 0x4000
#define CHR_BLOCK_SIZE 0x2000
#define TRAINER_SIZE   512


/*-----------------------------------------------------------------------------
// Name: ines_mapper
//---------------------------------------------------------------------------*/
static uint32_t ines_mapper(const ines_header_t *header, uint8_t version) {

	assert(header);

	switch(version) {
	case 2:
		return (((uint32_t)(header->extended.ines2.byte8 & 0x0f)) << 8) | (header->ctrl1 >> 4) | (header->ctrl2 & 0xf0);
	default:
		return (header->ctrl1 >> 4) | (header->ctrl2 & 0xf0);
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_submapper
//---------------------------------------------------------------------------*/
static uint32_t ines_submapper(const ines_header_t *header, uint8_t version) {

	assert(header);

	switch(version) {
	case 2:
		return (header->extended.ines2.byte8 & 0xf0) >> 4;
	default:
		return 0;
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_ppu
//---------------------------------------------------------------------------*/
static INES_PPU ines_ppu(const ines_header_t *header, uint8_t version) {
	
	assert(header);
	
	switch(version) {
	case 2:
		switch(header->extended.ines2.byte13 & 0x0f) {
		case 0x00: return PPU_RP2C03B;
		case 0x01: return PPU_RP2C03G;
		case 0x02: return PPU_RP2C04_0001;
		case 0x03: return PPU_RP2C04_0002;
		case 0x04: return PPU_RP2C04_0003;
		case 0x05: return PPU_RP2C04_0004;
		case 0x06: return PPU_RC2C03B;
		case 0x07: return PPU_RC2C03C;
		case 0x08: return PPU_RC2C05_01;
		case 0x09: return PPU_RC2C05_02;
		case 0x0a: return PPU_RC2C05_03;
		case 0x0b: return PPU_RC2C05_04;
		case 0x0c: return PPU_RC2C05_05;
		default:
			return PPU_UNKNOWN;
		}
	default:
		return PPU_UNKNOWN;
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_display
//---------------------------------------------------------------------------*/
static INES_DISPLAY ines_display(const ines_header_t *header, uint8_t version) {
	
	assert(header);
	
	switch(version) {
	case 2:
		switch(header->extended.ines2.byte12 & 0x03) {
		case 0x00: return DISP_NTSC;
		case 0x01: return DISP_PAL;
		case 0x02: return DISP_BOTH;
		case 0x03: return DISP_BOTH;
		}
	default:
		return DISP_BOTH;
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_system
//---------------------------------------------------------------------------*/
static INES_SYSTEM ines_system(const ines_header_t *header, uint8_t version) {
	assert(header);
	
	(void)version;

	switch(header->ctrl2 & 0x03) {
	case 0x01: return SYS_VS;
	case 0x02: return SYS_P10;
	default:
		return SYS_NES;
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_mirroring
//---------------------------------------------------------------------------*/
static INES_MIRRORING ines_mirroring(const ines_header_t *header, uint8_t version) {

	assert(header);
	
	(void)version;

	switch(header->ctrl1 & 0x09) {
	case 0x09:
	case 0x08:
		return MIRR_4SCREEN;
	case 0x01:
		return MIRR_VERTICAL;
	case 0x00:
	default:
		return MIRR_HORIZONTAL;
	}
}


/*-----------------------------------------------------------------------------
// Name: ines_trainer_present
//---------------------------------------------------------------------------*/
static int ines_trainer_present(const ines_header_t *header, uint8_t version) {

	assert(header);
	(void)version;

	return ((header->ctrl1 & INES_TRAINER) != 0);
}

/*-----------------------------------------------------------------------------
// Name: ines_prg_size
//---------------------------------------------------------------------------*/
static uint32_t ines_prg_size(const ines_header_t *header, uint8_t version) {

	assert(header);

	switch(version) {
	case 2:
		return header->prg_size | (((uint32_t)header->extended.ines2.byte9 & 0x0f) << 8);
	default:
		return header->prg_size;
	}
}

/*-----------------------------------------------------------------------------
// Name: ines_chr_size
//---------------------------------------------------------------------------*/
static uint32_t ines_chr_size(const ines_header_t *header, uint8_t version) {

	assert(header);

	switch(version) {
	case 2:
		return header->chr_size | (((uint32_t)header->extended.ines2.byte9 & 0xf0) << 4);
	default:
		return header->chr_size;
	}
}

/*------------------------------------------------------------------------------
// Name:
//----------------------------------------------------------------------------*/
static size_t next_power(size_t size) {

	/* returns 1 less than closest fitting power of 2
	 * is this number not a power of two or 0?
	 */
	if((size & (size - 1)) != 0) {
		--size;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		++size;
	} else if(size == 0) {
		++size;
	}

	return size;
}

/*-----------------------------------------------------------------------------
// Name: write_file_INES
//---------------------------------------------------------------------------*/
INES_RETURN_CODE write_file_INES(const char *filename, const ines_cart_t *cart) {

	INES_RETURN_CODE retcode = INES_OK;
	FILE *file = 0;

	assert(filename != 0);
	assert(cart != 0);

	file = fopen(filename, "wb");
	if(!file) {
		return INES_OPEN_FAILED;
	}

	if((retcode = write_header_INES(file, cart->header)) != INES_OK) {
		fclose(file);
		return retcode;
	}

	if(cart->trainer) {
		if(fwrite(cart->trainer, TRAINER_SIZE, 1, file) != 1) {
			fclose(file);
			return INES_WRITE_FAILED;
		}
	}

	if(cart->prg_size > 0) {
		assert(cart->prg_rom);
		if(fwrite(cart->prg_rom, cart->prg_size, 1, file) != 1) {
			fclose(file);
			return INES_WRITE_FAILED;
		}
	}

	if(cart->chr_size > 0) {
		assert(cart->chr_rom);
		if(fwrite(cart->chr_rom, cart->chr_size, 1, file) != 1) {
			fclose(file);
			return INES_WRITE_FAILED;
		}
	}

	fclose(file);
	return retcode;
}


/*-----------------------------------------------------------------------------
// Name: load_file_INES
//---------------------------------------------------------------------------*/
INES_RETURN_CODE load_file_INES(const char *filename, ines_cart_t *cart) {

	INES_RETURN_CODE retcode = INES_OK;

	FILE *file             = 0;
	size_t prg_alloc_size  = 0;
	size_t chr_alloc_size  = 0;
	int    has_trainer     = 0;
	
	ines_header_t header;

	void *header_ptr       = 0;
	void *prg_ptr          = 0;
	void *chr_ptr          = 0;
	void *trainer_ptr      = 0;

	assert(cart != 0);
	assert(filename != 0);

	cart->header  = 0;
	cart->trainer = 0;
	cart->prg_rom = 0;
	cart->chr_rom = 0;
	
	file = fopen(filename, "rb");
	if(!file) {
		return INES_OPEN_FAILED;
	}

	if((retcode = read_header_INES(file, &header)) != INES_OK) {
		goto error;
	}

	cart->version = ((header.ctrl2 & 0xc) == 0x8) ? 2 : 1;

	retcode = check_header_INES(&header, cart->version);
	if((retcode != INES_OK) && (retcode != INES_DIRTY_HEADER)) {
		goto error;
	}


	has_trainer = ines_trainer_present(&header, cart->version);
	
	cart->prg_size     = ines_prg_size    (&header, cart->version) * PRG_BLOCK_SIZE;
	cart->chr_size     = ines_chr_size    (&header, cart->version) * CHR_BLOCK_SIZE;
	cart->mirroring    = ines_mirroring   (&header, cart->version);
	cart->system       = ines_system      (&header, cart->version);
	cart->display      = ines_display     (&header, cart->version);
	cart->ppu          = ines_ppu         (&header, cart->version);
	cart->mapper       = ines_mapper      (&header, cart->version);
	cart->submapper    = ines_submapper   (&header, cart->version);
	
	prg_alloc_size = next_power(cart->prg_size);
	chr_alloc_size = next_power(cart->chr_size);

	/* allocate memory for the cart */
	header_ptr  = malloc(sizeof(ines_header_t));
	prg_ptr     = cart->prg_size ? malloc(prg_alloc_size) : 0;
	chr_ptr     = cart->chr_size ? malloc(chr_alloc_size) : 0;
	trainer_ptr = has_trainer    ? malloc(TRAINER_SIZE)   : 0;

	/* make sure it all went smoothly */
	if(!header_ptr || (cart->prg_size && !prg_ptr) || (cart->chr_size && !chr_ptr) || (has_trainer && !trainer_ptr)) {
		retcode = INES_OUT_OF_MEMORY;
		goto error;
	}

	/* assign some pointers */
	cart->header  = header_ptr;
	cart->prg_rom = prg_ptr;
	cart->chr_rom = chr_ptr;
	cart->trainer = trainer_ptr;

	memcpy(cart->header, &header, sizeof(ines_header_t));
	if(has_trainer) {
		if(fread(cart->trainer, TRAINER_SIZE, 1, file) != 1) {
			retcode = INES_READ_FAILED;
			goto error;
		}
	}

	if(cart->prg_size != 0) {
		if(fread(cart->prg_rom, cart->prg_size, 1, file) != 1) {
			retcode = INES_READ_FAILED;
			goto error;
		}
		
		if((prg_alloc_size - cart->prg_size) > 0x2000 && cart->prg_size >= 0x2000) {
			/* replicate the last bank if necessary */
			uint8_t *const last_8k = cart->prg_rom + cart->prg_size - 0x2000;
			uint8_t *p = cart->prg_rom + cart->prg_size;
			while(p < cart->prg_rom + prg_alloc_size) {
				memcpy(p, last_8k, 0x2000);
				p += 0x2000;
			}
		}
	}

	if(cart->chr_size != 0) {
		uint8_t *p;
		if(fread(cart->chr_rom, cart->chr_size, 1, file) != 1) {
			retcode = INES_READ_FAILED;
			goto error;
		}
		
		p = cart->chr_rom + cart->chr_size;
		while(p != cart->chr_rom + chr_alloc_size) {
			*p++ = 0xff;
		}
	}

	fclose(file);
	return retcode;

error:
	free(header_ptr);
	free(prg_ptr);
	free(chr_ptr);
	free(trainer_ptr);

	cart->header       = 0;
	cart->trainer      = 0;
	cart->prg_rom      = 0;
	cart->chr_rom      = 0;
	cart->prg_size     = 0;
	cart->chr_size     = 0;
	cart->mirroring    = 0;
	cart->system       = 0;
	cart->display      = 0;
	cart->ppu          = 0;
	cart->mapper       = 0;
	cart->submapper    = 0;

	fclose(file);
	return retcode;
}

/*-----------------------------------------------------------------------------
// Name: free_file_INES
//---------------------------------------------------------------------------*/
INES_RETURN_CODE free_file_INES(ines_cart_t *cart) {
	assert(cart);

	free(cart->header);
	free(cart->trainer);
	free(cart->prg_rom);
	free(cart->chr_rom);

	cart->header       = 0;
	cart->trainer      = 0;
	cart->prg_rom      = 0;
	cart->chr_rom      = 0;
	cart->prg_size     = 0;
	cart->chr_size     = 0;
	cart->mirroring    = 0;
	cart->system       = 0;
	cart->display      = 0;
	cart->ppu          = 0;
	cart->mapper       = 0;
	cart->submapper    = 0;

	return INES_OK;
}

/*-----------------------------------------------------------------------------
// Name: prg_hash_INES
//---------------------------------------------------------------------------*/
uint32_t prg_hash_INES(const ines_cart_t *cart) {
	return ines_crc32(cart->prg_rom, cart->prg_size, 0);
}

/*-----------------------------------------------------------------------------
// Name: chr_hash_INES
//---------------------------------------------------------------------------*/
uint32_t chr_hash_INES(const ines_cart_t *cart) {
	return ines_crc32(cart->chr_rom, cart->chr_size, 0);
}

/*-----------------------------------------------------------------------------
// Name: rom_hash_INES
//---------------------------------------------------------------------------*/
uint32_t rom_hash_INES(const ines_cart_t *cart) {
	
	const uint32_t hash1 = cart->trainer ? ines_crc32(cart->trainer, TRAINER_SIZE, 0)       : 0;
	const uint32_t hash2 = cart->prg_rom ? ines_crc32(cart->prg_rom, cart->prg_size, hash1) : hash1;
	const uint32_t hash3 = cart->chr_rom ? ines_crc32(cart->chr_rom, cart->chr_size, hash2) : hash2;
	
	return hash3;

}

