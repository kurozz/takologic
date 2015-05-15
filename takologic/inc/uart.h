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

#ifndef _UART_H_
#define _UART_H_

#include <stm32f10x_conf.h>

//USART Initialization
void uartInit(uint32_t baud);

//Checks if there's data to be read
uint8_t uartAvailable();

//Writes a single byte
void uartPutc(char c);

//Writes a string
void uartPuts(char * s);

//Reads a single byte
char uartGetc();

#endif //_UART_H_
