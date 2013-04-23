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

#ifndef STD_FUNC_20080314_H_
#define STD_FUNC_20080314_H_

#ifdef __cplusplus
extern "C"{
#endif

/* some general purpose functions to trim code */
unsigned int display_menu(unsigned int n, const char *prompt, ...);
int ask_question_yn(const char *query);

#ifdef __cplusplus
}
#endif

#endif

