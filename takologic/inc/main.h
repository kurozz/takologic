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
 * Last update: 01/06/2015
 *
 ********************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stm32f10x.h>
#include <stm32f10x_conf.h>
#include <stdbool.h>

#include "sump.h"
#include "clock.h"
#include "uart.h"
#include "dma.h"

// Firmware version
#define VERSION "Takologic v0.2"

// Sample buffer size
#define SAMPLESIZE 1024*10
#define MAX_SAMPLERATE 8000000

// Sampling variables
struct sampling {
	uint8_t data[SAMPLESIZE];	// Sampling data buffer
	uint16_t index;				// Sampling data buffer index
	bool ongoing;				// True if there's a sampling ongoing
	bool done;					// True if there's data to be sent
} sampling;

// Sampling configuration
struct config {
	uint32_t divider;			// Used to set the sampling time
	uint32_t readCount;
	uint32_t delayCount;
} config;

// Trigger configuration
struct trigger {
	uint32_t armed;
	uint32_t mask;
	uint32_t value;
	uint32_t delay;
	uint8_t level;
	uint8_t channel;
	bool serial;
	bool start;
} trigger;

struct flags {
	bool demux;
	bool filter;
	uint8_t groups;
	bool external;
	bool inverted;
} flags;

uint8_t resetCount;

#endif //_MAIN_H_
