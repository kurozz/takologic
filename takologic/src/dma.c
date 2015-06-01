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

#include "dma.h"

void dmaConfig(uint32_t address, uint16_t size, bool mode_8mhz, bool mode_16bit) {
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&GPIOB->IDR;				// GPIOB read address
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)address;						// Write address
	DMA_InitStructure.DMA_BufferSize = size;										// Memory size to write
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;								// GPIOB as source of data
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// Peripheral index increment disable
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							// Memory index increment enable
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;			// Peripheral data size
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;									// Non-circular buffer
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;							// Set priority as very high

	if(mode_8mhz == true) {
		DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;									// Enable M2M
	} else {
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// Disable M2M
	}

	if(mode_16bit == true) {
		// Set memory size to 16 bit if reading entire the GPIOB port
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	} else {
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	}

	// Initialize DMA1 Channel 6
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);

	// Enable DMA1 Channel 6 transfer complete interruption
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

	// Enable DMA1 Channel 6 IRQ with highest priority
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable DMA1 Channel 6
	DMA_Cmd(DMA1_Channel6, ENABLE);
}
