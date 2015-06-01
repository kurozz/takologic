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

#include "uart.h"

//USART Initialization
void uartInit(uint32_t baud) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// USART2 GPIO PIN configuration
	GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);

	// Configure USART Tx as alternate function push pull output
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
}

//Checks if there's data to be read
uint8_t uartAvailable() {
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) {
		return 1;
	} else {
		return 0;
	}
}

//Writes a single byte
void uartPutc(char c) {
	USART_SendData(USART2, c);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

//Writes a string
void uartPuts(char * s) {
    while(*s != 0) {
    	uartPutc(*s);
        s++;
    }
}

//Reads a single byte
char uartGetc() {
	return (char)USART_ReceiveData(USART2);
}
