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

#ifndef INES_TYPES_20080314_H_
#define INES_TYPES_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

#if defined(_WIN32) || defined(_WIN64)

	#if defined(_MSC_VER) && _MSC_VER >= 1600
		#include <stdint.h>
	#else
		typedef unsigned __int8  uint8_t;
		typedef unsigned __int16 uint16_t;
		typedef unsigned __int32 uint32_t;
		typedef unsigned __int64 uint64_t;

		typedef __int8  int8_t;
		typedef __int16 int16_t;
		typedef __int32 int32_t;
		typedef __int64 int64_t;
	#endif
#else
	#include <stdint.h>
#endif

/* enum for return info */
typedef enum {
	INES_OK,
	INES_BAD_HEADER,
	INES_CLOSE_FAILED,
	INES_DIRTY_HEADER,
	INES_LENGTH_ERROR,
	INES_OPEN_FAILED,
	INES_OUT_OF_MEMORY,
	INES_READ_FAILED,
	INES_WRITE_FAILED
} INES_RETURN_CODE;

/* enum for open type */
typedef enum {
	INES_OPEN_READ,
	INES_OPEN_WRITE
} INES_OPEN_MODE;

#ifdef __cplusplus
}
#endif

#endif

