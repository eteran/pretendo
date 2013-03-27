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

#ifndef UNIF_TYPES_20080314_H_
#define UNIF_TYPES_20080314_H_

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
#elif defined (__BEOS__)
	#include <support/SupportDefs.h>
	typedef int8  int8_t;
	typedef int16 int16_t;
	typedef int32 int32_t;
	typedef int64 int64_t;

	typedef uint8  uint8_t;
	typedef uint16 uint16_t;
	typedef uint32 uint32_t;
	typedef uint64 uint64_t;
#elif defined (Linux)
	#include <stdint.h>
#elif defined(SunOS)
	#include <sys/types.h>
#endif

/* enum for return info */
typedef enum {
	UNIF_OK,
	UNIF_END_OF_FILE,
	UNIF_BAD_HEADER,
	UNIF_BAD_REVISION_NUMBER,
	UNIF_DIRTY_HEADER,
	UNIF_OPEN_FAILED,
	UNIF_CLOSE_FAILED,
	UNIF_READ_FAILED,
	UNIF_WRITE_FAILED,
	UNIF_OUT_OF_MEMORY,
	UNIF_INPUT_FAIL,
	UNIF_LENGTH_ERROR
} UNIF_RETURN_CODE;

/* enum for open type */
typedef enum {
	UNIF_OPEN_READ,
	UNIF_OPEN_WRITE
} UNIF_OPEN_MODE;

#ifdef __cplusplus
}
#endif

#endif

