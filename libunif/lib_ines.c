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

#include "lib_ines.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
// open_INES(const char *filename, FILE **file, UNIF_OPEN_MODE mode)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE open_INES(const char *filename, FILE **file, UNIF_OPEN_MODE mode) {

	/* check pointers */
	assert(filename != 0);
	assert(file != 0);

	/* open the file */
	if(mode == UNIF_OPEN_READ) *file = fopen(filename, "rb");
	else                       *file = fopen(filename, "wb");

	/* check if successful */
	if(*file == 0) {
		return UNIF_OPEN_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// close_INES(FILE *file)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE close_INES(FILE *file) {

	/* check pointers */
	assert(file != 0);

	/* close file */
	if(fclose(file) == EOF) {
		return UNIF_CLOSE_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// read_header_INES(FILE *file, ines_header_t *header)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE read_header_INES(FILE *file, ines_header_t *header) {

	/* check pointers */
	assert(file != 0);
	assert(header != 0);

	/* header is ALWAYS at begining of file */
	rewind(file);

	/* read the header data */
	if(fread(header, 1, sizeof(ines_header_t), file) != sizeof(ines_header_t)) {
		return UNIF_READ_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// write_header_INES(FILE *file, const ines_header_t *header)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_header_INES(FILE *file, const ines_header_t *header) {
	/* check pointers */
	assert(file != 0);
	assert(header != 0);

	/* header is ALWAYS at begining of file */
	rewind(file);

	/* write the header data */
	if(fwrite(header, 1, sizeof(ines_header_t), file) != sizeof(ines_header_t)) {
		return UNIF_WRITE_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// check_header_INES(const ines_header_t *header, int version)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE check_header_INES(const ines_header_t *header, int version) {

	assert(header != 0);

	if(memcmp(header->ines_signature, "NES\x1a", 4) != 0) {
		return UNIF_BAD_HEADER;
	}

	if(version != 2) {
		if(header->extended.ines1.reserved_2 != 0 || header->extended.ines1.reserved_1 != 0) {
			return UNIF_DIRTY_HEADER;
		}
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// read_data_INES( FILE * file, uint8_t *dst, size_t len )
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE read_data_INES(FILE *file, uint8_t *dst, size_t len) {
	assert(file != 0);
	assert(dst != 0);

	if(fread(dst, 1, len, file) != len) {
		return UNIF_READ_FAILED;
	}

	return UNIF_OK;
}
