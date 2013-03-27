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

#ifndef UNIF_CRC32_20080314_H_
#define UNIF_CRC32_20080314_H_

#include "unif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t unif_crc32(const void *data, uint32_t length, uint32_t initial_value);

#ifdef __cplusplus
}
#endif


#endif

