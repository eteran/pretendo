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
#include "unif_crc32.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*------------------------------------------------------------------------------
// Name:
//----------------------------------------------------------------------------*/
size_t next_power(size_t size) {

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
UNIF_RETURN_CODE write_file_INES(const char *filename, const ines_cart_t *cart) {

	UNIF_RETURN_CODE retcode = UNIF_OK;
	FILE *file = 0;
	const int prg_size = prg_size_INES(cart) * 16384;
	const int chr_size = chr_size_INES(cart) * 8192;

	assert(filename != 0);
	assert(cart != 0);

	if((retcode = open_INES(filename, &file, UNIF_OPEN_WRITE)) != UNIF_OK) {
		return retcode;
	}

	if((retcode = write_header_INES(file, cart->header)) != UNIF_OK) {
		close_INES(file);
		return retcode;
	}

	if((cart->header->ctrl1 & INES_TRAINER) != 0) {
		if(fwrite(cart->trainer, 512, 1, file) != 1) {
			close_INES(file);
			return UNIF_WRITE_FAILED;
		}
	}

	if(prg_size > 0) {
		if(fwrite(cart->prg_rom, prg_size, 1, file) != 1) {
			close_INES(file);
			return UNIF_WRITE_FAILED;
		}
	}

	if(chr_size > 0) {
		if(fwrite(cart->chr_rom, chr_size, 1, file) != 1) {
			close_INES(file);
			return UNIF_WRITE_FAILED;
		}
	}

	close_INES(file);
	return retcode;
}

/*-----------------------------------------------------------------------------
// Name: load_ptr_INES
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE load_ptr_INES(const uint8_t *rom, size_t size, ines_cart_t *cart) {

    UNIF_RETURN_CODE retcode = UNIF_OK;
	const uint8_t *rom_ptr = 0;
    size_t prg_size        = 0;
    size_t chr_size        = 0;
    size_t trainer_size    = 0;
	size_t prg_alloc_size  = 0;
	size_t chr_alloc_size  = 0;
	ines_header_t header;

	void *header_ptr       = 0;
	void *prg_ptr          = 0;
	void *chr_ptr          = 0;
	void *trainer_ptr      = 0;

	assert(cart != 0);
	assert(rom != 0);

	if(size < sizeof(ines_header_t)) {
		return UNIF_LENGTH_ERROR;
	}

	memcpy(&header, rom, sizeof(ines_header_t));

	cart->header  = 0;
	cart->trainer = 0;
	cart->prg_rom = 0;
	cart->chr_rom = 0;
    cart->version = ((header.ctrl2 & 0xc) == 0x8) ? 2 : 1;

	retcode = check_header_INES(&header, cart->version);
	if((retcode != UNIF_OK) && (retcode != UNIF_DIRTY_HEADER)) {
		return retcode;
	}

	/* temporarily make the cart point to our read header */
	cart->header = &header;

	if((header.ctrl1 & INES_TRAINER) != 0) {
		trainer_size = 512;
	}

	prg_size = prg_size_INES(cart) * 16384;
	chr_size = chr_size_INES(cart) * 8192;
	
	prg_alloc_size = next_power(prg_size);
	chr_alloc_size = next_power(chr_size);

	/* NULL the cart header pointer, it was temporary */
	cart->header = 0;

	if(size < sizeof(ines_header_t) + trainer_size + prg_size + chr_size) {
		return UNIF_LENGTH_ERROR;
	}

	/* allocate memory for the cart */
	header_ptr  = malloc(sizeof(ines_header_t));
	prg_ptr     = prg_size     ? malloc(prg_alloc_size) : 0;
	chr_ptr     = chr_size     ? malloc(chr_alloc_size) : 0;
	trainer_ptr = trainer_size ? malloc(trainer_size)   : 0;

	/* make sure it all went smoothly */
	if((!header_ptr) || (prg_size & !prg_ptr) || (chr_size & !chr_ptr) || (trainer_size & !trainer_ptr)) {
		free(header_ptr);
		free(prg_ptr);
		free(chr_ptr);
		free(trainer_ptr);

		cart->header  = 0;
		cart->trainer = 0;
		cart->prg_rom = 0;
		cart->chr_rom = 0;

		return UNIF_OUT_OF_MEMORY;
	}

	/* assign some pointers */
	cart->header  = header_ptr;
	cart->prg_rom = prg_ptr;
	cart->chr_rom = chr_ptr;
	cart->trainer = trainer_ptr;

	rom_ptr = rom;

	memcpy(cart->header, rom_ptr, sizeof(ines_header_t));
	rom_ptr += sizeof(ines_header_t);

	if(trainer_size) {
		memcpy(cart->trainer, rom_ptr, trainer_size);
		rom_ptr += trainer_size;
	}

	memcpy(cart->prg_rom, rom_ptr, prg_size);
	if((prg_alloc_size - prg_size) > 0x2000 && prg_size >= 0x2000) {
		/* replicate the last bank if necessary */
		uint8_t *const last_8k = cart->prg_rom + prg_size - 0x2000;
		uint8_t *p = cart->prg_rom + prg_size;
		while(p < cart->prg_rom + prg_alloc_size) {
			memcpy(p, last_8k, 0x2000);
			p += 0x2000;
		}
	}

	rom_ptr += prg_size;

	if(chr_size != 0) {
		memcpy(cart->chr_rom, rom_ptr, chr_size);
		if(chr_size != chr_alloc_size) {
			uint8_t *p = cart->chr_rom + chr_size;
			while(p != cart->chr_rom + chr_alloc_size) {
				*p++ = 0xff;
			}
		}
	}

    return retcode;
}

/*-----------------------------------------------------------------------------
// Name: load_file_INES
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE load_file_INES(const char *filename, ines_cart_t *cart) {

	UNIF_RETURN_CODE retcode = UNIF_OK;

	FILE *file            = 0;
	size_t trainer_size   = 0;
	size_t prg_size       = 0;
	size_t chr_size       = 0;
	size_t prg_alloc_size = 0;
	size_t chr_alloc_size = 0;
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

	if((retcode = open_INES(filename, &file, UNIF_OPEN_READ)) != UNIF_OK) {
		return retcode;
	}

	if((retcode = read_header_INES(file, &header)) != UNIF_OK) {
		close_INES(file);
		return retcode;
	}

	cart->version = ((header.ctrl2 & 0xc) == 0x8) ? 2 : 1;

	retcode = check_header_INES(&header, cart->version);
	if((retcode != UNIF_OK) && (retcode != UNIF_DIRTY_HEADER)) {
		close_INES(file);
		return retcode;
	}

	/* temporarily make the cart point to our read header */
	cart->header = &header;

	if((header.ctrl1 & INES_TRAINER) != 0) {
		trainer_size = 512;
	}

	prg_size = prg_size_INES(cart) * 16384;
	chr_size = chr_size_INES(cart) * 8192;
	
	prg_alloc_size = next_power(prg_size);
	chr_alloc_size = next_power(chr_size);

	/* NULL the cart header pointer, it was temporary */
	cart->header = 0;

	/* allocate memory for the cart */
	header_ptr  = malloc(sizeof(ines_header_t));
	prg_ptr     = prg_size     ? malloc(prg_alloc_size) : 0;
	chr_ptr     = chr_size     ? malloc(chr_alloc_size) : 0;
	trainer_ptr = trainer_size ? malloc(trainer_size)   : 0;

	/* make sure it all went smoothly */
	if((!header_ptr) || (prg_size & !prg_ptr) || (chr_size & !chr_ptr) || (trainer_size & !trainer_ptr)) {
		retcode = UNIF_OUT_OF_MEMORY;
		goto error;
	}

	/* assign some pointers */
	cart->header  = header_ptr;
	cart->prg_rom = prg_ptr;
	cart->chr_rom = chr_ptr;
	cart->trainer = trainer_ptr;

	memcpy(cart->header, &header, sizeof(ines_header_t));
	if(trainer_size != 0) {
		retcode = read_data_INES(file, cart->trainer, trainer_size);
		if(retcode != UNIF_OK) {
			goto error;
		}
	}

	if(prg_size != 0) {
		retcode = read_data_INES(file, cart->prg_rom, prg_size);
		if(retcode != UNIF_OK) {
			goto error;
		}
		
		if((prg_alloc_size - prg_size) > 0x2000 && prg_size >= 0x2000) {
			/* replicate the last bank if necessary */
			uint8_t *const last_8k = cart->prg_rom + prg_size - 0x2000;
			uint8_t *p = cart->prg_rom + prg_size;
			while(p < cart->prg_rom + prg_alloc_size) {
				memcpy(p, last_8k, 0x2000);
				p += 0x2000;
			}
		}
	}

	if(chr_size != 0) {
		retcode = read_data_INES(file, cart->chr_rom, chr_size);
		if(retcode != UNIF_OK) {
			goto error;
		}
		
		uint8_t *p = cart->chr_rom + chr_size;
		while(p != cart->chr_rom + chr_alloc_size) {
			*p++ = 0xff;
		}
	}

	close_INES(file);
	return retcode;

error:
	free(header_ptr);
	free(prg_ptr);
	free(chr_ptr);
	free(trainer_ptr);

	cart->header  = 0;
	cart->trainer = 0;
	cart->prg_rom = 0;
	cart->chr_rom = 0;

	close_INES(file);
	return retcode;
}

/*-----------------------------------------------------------------------------
// Name: free_file_INES
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE free_file_INES(ines_cart_t *cart) {
	assert(cart);

	free(cart->header);
	free(cart->trainer);
	free(cart->prg_rom);
	free(cart->chr_rom);

	cart->header  = 0;
	cart->trainer = 0;
	cart->prg_rom = 0;
	cart->chr_rom = 0;

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// Name: mirroring_INES
//---------------------------------------------------------------------------*/
INES_MIRRORING mirroring_INES(const ines_cart_t *cart) {

	assert(cart);

	switch(cart->header->ctrl1 & 0x09) {
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
// Name: system_INES
//---------------------------------------------------------------------------*/
INES_SYSTEM system_INES(const ines_cart_t *cart) {
	assert(cart);

	switch(cart->header->ctrl2 & 0x03) {
	case 0x01: return SYS_VS;
	case 0x02: return SYS_P10;
	default:
		return SYS_NES;
	}
}

/*-----------------------------------------------------------------------------
// Name: mapper_INES
//---------------------------------------------------------------------------*/
uint32_t mapper_INES(const ines_cart_t *cart) {

	assert(cart);

	switch(cart->version) {
	case 2:
		return (((uint32_t)(cart->header->extended.ines2.byte8 & 0x0f)) << 8) | (cart->header->ctrl1 >> 4) | (cart->header->ctrl2 & 0xf0);
	default:
		return (cart->header->ctrl1 >> 4) | (cart->header->ctrl2 & 0xf0);
	}
}

/*-----------------------------------------------------------------------------
// Name: mapper_INES
//---------------------------------------------------------------------------*/
uint32_t submapper_INES(const ines_cart_t *cart) {

	assert(cart);

	switch(cart->version) {
	case 2:
		return (cart->header->extended.ines2.byte8 & 0xf0) >> 4;
	default:
		return 0;
	}
}

/*-----------------------------------------------------------------------------
// Name: prg_size_INES
//---------------------------------------------------------------------------*/
uint32_t prg_size_INES(const ines_cart_t *cart) {

	assert(cart);

	switch(cart->version) {
	case 2:
		return cart->header->prg_size | (((uint32_t)cart->header->extended.ines2.byte9 & 0x0f) << 8);
	default:
		return cart->header->prg_size;
	}
}

/*-----------------------------------------------------------------------------
// Name: chr_size_INES
//---------------------------------------------------------------------------*/
uint32_t chr_size_INES(const ines_cart_t *cart) {

	assert(cart);

	switch(cart->version) {
	case 2:
		return cart->header->chr_size | (((uint32_t)cart->header->extended.ines2.byte9 & 0xf0) << 4);
	default:
		return cart->header->chr_size;
	}
}

/*-----------------------------------------------------------------------------
// Name: display_INES
//---------------------------------------------------------------------------*/
INES_DISPLAY display_INES(const ines_cart_t *cart) {
	switch(cart->version) {
	case 2:
		switch(cart->header->extended.ines2.byte12 & 0x03) {
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
// Name: ppu_INES
//---------------------------------------------------------------------------*/
INES_PPU ppu_INES(const ines_cart_t *cart) {
	switch(cart->version) {
	case 2:
		switch(cart->header->extended.ines2.byte13 & 0x0f) {
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
// Name: prg_hash_INES
//---------------------------------------------------------------------------*/
uint32_t prg_hash_INES(const ines_cart_t *cart) {
	return unif_crc32(cart->prg_rom, prg_size_INES(cart) * 0x4000, 0);
}

/*-----------------------------------------------------------------------------
// Name: chr_hash_INES
//---------------------------------------------------------------------------*/
uint32_t chr_hash_INES(const ines_cart_t *cart) {
	return unif_crc32(cart->chr_rom, chr_size_INES(cart) * 0x2000, 0);
}


/*-----------------------------------------------------------------------------
// Name: rom_hash_INES
//---------------------------------------------------------------------------*/
uint32_t rom_hash_INES(const ines_cart_t *cart) {
	
	const uint32_t hash1 = cart->trainer ? unif_crc32(cart->trainer, 512, 0)                              : 0;
	const uint32_t hash2 = cart->prg_rom ? unif_crc32(cart->prg_rom, prg_size_INES(cart) * 0x4000, hash1) : hash1;
	const uint32_t hash3 = cart->prg_rom ? unif_crc32(cart->chr_rom, chr_size_INES(cart) * 0x2000, hash2) : hash2;
	
	return hash3;

}

