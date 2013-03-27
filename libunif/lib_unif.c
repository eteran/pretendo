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

#include "lib_unif.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*-----------------------------------------------------------------------------
// name: open_UNIF(const char *filename, FILE **file, UNIF_OPEN_MODE mode)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE open_UNIF(const char *filename, FILE **file, UNIF_OPEN_MODE mode) {

	/* check pointers */
	assert(file != 0);
	assert(filename != 0);

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
// name: check_header_UNIF(unif_header_t *header)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE check_header_UNIF(unif_header_t *header) {

	/* check pointers */
	assert(header != 0);

	/* check if the tag is right */
	if(memcmp(header->id, "UNIF", 4) != 0) {
		return UNIF_BAD_HEADER;
	}

	/* is this revision too high for this lib? */
	if(header->revision > UNIF_REVISION) {
		return UNIF_BAD_REVISION_NUMBER;
	}

	/* make sure reserved space is all zeros */
	if(memcmp(
			header->expansion,
			"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			sizeof(header->expansion)
			)) {
		return UNIF_DIRTY_HEADER;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: close_UNIF(FILE *file)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE close_UNIF(FILE *file) {

	/* check pointers */
	assert(file != 0);

	/* close file */
	if(fclose(file) == EOF) {
		return UNIF_CLOSE_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: read_header_UNIF(FILE *file, unif_header_t *header)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE read_header_UNIF(FILE *file, unif_header_t *header) {

	/* check pointers */
	assert(file != 0);
	assert(header != 0);

	/* header is ALWAYS at begining of file */
	rewind(file);

	/* read the header data */
	if(fread(header, 1, sizeof(unif_header_t), file) != sizeof(unif_header_t)) {
		return UNIF_READ_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: write_header_UNIF(FILE *file, unif_header_t *header)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_header_UNIF(FILE *file, unif_header_t *header) {

	/* check pointers */
	assert(file != 0);
	assert(header != 0);

	/* header is ALWAYS at begining of file */
	rewind(file);

	/* write the header data */
	if(fwrite(header, 1, sizeof(unif_header_t), file) != sizeof(unif_header_t)) {
		return UNIF_WRITE_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: read_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void **chunk_data)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE read_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void **chunk_data) {

	/* check pointers */
	assert(file != 0);
	assert(chunk_header != 0);
	assert(chunk_data != 0);

	/* read the header of the chunk */
	if(fread(chunk_header, 1, sizeof(unif_chunk_t), file) != sizeof(unif_chunk_t)) {
		/* on a failed read, check if it was because of EOF */
		if(feof(file)) {
			return UNIF_END_OF_FILE;
		}

		return UNIF_READ_FAILED;
	}

	/* allocate memory for the chunk data */
	*chunk_data = malloc(chunk_header->length);
	if(*chunk_data == 0) {
		return UNIF_OUT_OF_MEMORY;
	}

	/* read the chunk data */
	if(fread(*chunk_data, 1, chunk_header->length, file) != chunk_header->length) {
		free(*chunk_data);
		return UNIF_READ_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: write_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void *chunk_data)
//---------------------------------------------------------------------------*/
UNIF_RETURN_CODE write_chunk_UNIF(FILE *file, unif_chunk_t *chunk_header, void *chunk_data) {

	/* check pointers */
	assert(file != 0);
	assert(chunk_header != 0);
	assert(chunk_data != 0);

	/* write the header of the chunk */
	if(fwrite(chunk_header, 1, sizeof(unif_chunk_t), file) != sizeof(unif_chunk_t)) {
		return UNIF_WRITE_FAILED;
	}

	/* write the chunk data */
	if(fwrite(chunk_data, 1, chunk_header->length, file) != chunk_header->length) {
		return UNIF_WRITE_FAILED;
	}

	return UNIF_OK;
}

/*-----------------------------------------------------------------------------
// name: get_error_string_UNIF(UNIF_RETURN_CODE code)
//---------------------------------------------------------------------------*/
const char *get_error_string_UNIF(UNIF_RETURN_CODE code) {

	switch (code) {
	case UNIF_OK:
		return "no error";
	case UNIF_END_OF_FILE:
		return "buffer has hit end of file";
	case UNIF_BAD_HEADER:
		return "invalid header";
	case UNIF_BAD_REVISION_NUMBER:
		return "revision number of this ROM is not supported";
	case UNIF_DIRTY_HEADER:
		return "the reserved area of the header has junk in it";
	case UNIF_OUT_OF_MEMORY:
		return "failed to allocate memory";
	case UNIF_OPEN_FAILED:
		return "could not open file for reading/writing";
	case UNIF_READ_FAILED:
		return "could not read file";
	case UNIF_CLOSE_FAILED:
		return "could not properly close file";
	case UNIF_WRITE_FAILED:
		return "could not write to file";
	case UNIF_INPUT_FAIL:
		return "input error";
	case UNIF_LENGTH_ERROR:
		return "invalid length provided";
    default:
		return "unknown error";
    }
}


