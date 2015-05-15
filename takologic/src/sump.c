/********************************************************************
 * Takologic v0.1 - A SUMP compatible logic analyzer
 *
 * Copyright (C) 2015  Adriano Zenzen
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:	Kuro
 * Email:	kuro@lzzbr.com
 *
 * Page:	http://hackaday.io/project/5742-takologic
 *
 * Last update: 15/05/2015
 *
 ********************************************************************/

#include "sump.h"

// send 32bit unsigned integer as SUMP metadata
void sump_sendmeta_uint32(uint8_t type, uint32_t i) {
	uartPutc(type);
	uartPutc((i >> 24) & 0xff);
	uartPutc((i >> 16) & 0xff);
	uartPutc((i >> 8) & 0xff);
	uartPutc(i & 0xff);
}

// send 8bit unsigned integer as SUMP metadata
void sump_sendmeta_uint8(uint8_t type, uint8_t i) {
	uartPutc(type);
    uartPutc(i);
}
