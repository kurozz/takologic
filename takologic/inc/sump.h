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

#ifndef _SUMP_H_
#define _SUMP_H_

#include "uart.h"

// send 32bit unsigned integer as SUMP metadata
void sump_sendmeta_uint32(uint8_t type, uint32_t i);

// send 8bit unsigned integer as SUMP metadata
void sump_sendmeta_uint8(uint8_t type, uint8_t i);

// SUMP short commands defintions
#define SUMP_RESET 0x00
#define SUMP_ARM   0x01
#define SUMP_QUERY 0x02
#define SUMP_SELFTEST 0x03
#define SUMP_GET_METADATA 0x04
#define SUMP_XON 0x11
#define SUMP_XOFF 0x13

// SUMP long commands defintions
#define SUMP_TRIG_MASK_S1 0xC0
#define SUMP_TRIG_MASK_S2 0xC4
#define SUMP_TRIG_MASK_S3 0xC8
#define SUMP_TRIG_MASK_S4 0xCC
#define SUMP_TRIG_VAL_S1 0xC1
#define SUMP_TRIG_VAL_S2 0xC5
#define SUMP_TRIG_VAL_S3 0xC9
#define SUMP_TRIG_VAL_S4 0xCD
#define SUMP_TRIG_CFG_S1 0xC2
#define SUMP_TRIG_CFG_S2 0xC6
#define SUMP_TRIG_CFG_S3 0xCA
#define SUMP_TRIG_CFG_S4 0xCE
#define SUMP_DIVIDER 0x80
#define SUMP_READ_DELAY 0x81
#define SUMP_SET_FLAGS 0x82

#endif //_SUMP_H_
