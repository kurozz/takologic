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

#include "clock.h"

void clockConfig(uint32_t freq, enum clock_mode mode) {
	GPIO_InitTypeDef GPIO_InitStructure;                        // GPIOA structure
	NVIC_InitTypeDef NVIC_InitStructure;                        // NVIC structure
	EXTI_InitTypeDef EXTI_InitStructure;
	TIM_TimeBaseInitTypeDef TIM3_TimeBase;                      // Time base structure
	TIM_OCInitTypeDef TIM3_OC;                           		// Output Compare structure

	uint16_t period;
	uint16_t prescaler;
	bool _16bit = false;

	if(flags.groups == 0x0C || flags.groups == 0x0D) {
		_16bit = true;
	}

	if(freq > 5000000) {
		period = 8;
		prescaler = 0;
	} else if(freq > 500000) {
		period = ( 72000000/(freq) ) - 1;
		prescaler = 0;
	} else if(freq > 100000) {
		period = ( 36000000/(freq) ) - 1;
		prescaler = 1;
	} else if(freq > 1000) {
		period = ( 9000000/(freq) ) - 1;
		prescaler = 7;
	} else {
		period = ( 50000/(freq) ) - 1;
		prescaler = 1439;
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // Clocking GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        // Clocking TIM3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		// Clocking AFIO

	// Timer Pin configuration
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;                 // Ch.1 (PA6)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           // 50MHz
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;      		// Alternative function Push Pull
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      // Initializing GPIOA structure

	// EXTI Pin configuration
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;					// Pin PA0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         	// 50MHz
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;		// Mode as input floating
	GPIO_Init(GPIOA, &GPIO_InitStructure);						// Initializing GPIOA structure

	if(mode == INTERNAL) {
	// Timer configuration
		TIM3_TimeBase.TIM_ClockDivision = 0;                        // Not dividing
		TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;       // Upcounting configuration
		TIM3_TimeBase.TIM_Period        = period;                	// Autoreload value (ARR)
		TIM3_TimeBase.TIM_Prescaler     = prescaler;           		// Dividing APB1 by 2
		TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);                     // Initializing Time Base structure

		TIM3_OC.TIM_OCMode      = TIM_OCMode_Toggle;                // Output compare toggling mode
		TIM3_OC.TIM_OutputState = TIM_OutputState_Enable;           // Enabling the Output Compare state
		TIM3_OC.TIM_OCPolarity  = TIM_OCPolarity_Low;               // Reverse polarity
		TIM3_OC.TIM_Pulse       = period;                        	// Output Compare 1 reg value
		TIM_OC1Init(TIM3, &TIM3_OC);                                // Initializing Output Compare 1 structure
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);          // Disabling Ch.1 Output Compare preload

		if(freq < 1000000) {
			// Trigered sampling through interrupts
			NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;    // Specifying the channel
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;            // Only matters for multiple interrupts
			NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;            // Only matters for multiple interrupts
			NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;       // Enabling global interrupt
			NVIC_Init(&NVIC_InitStructure);		                                // Initializing NVIC structure
			TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);                     // Enabling TIM3 Ch.1 interrupts

			TIM_Cmd(TIM3, ENABLE);	// Enable timer
		} else if(freq <= 5000000) {
			// DMA sampling through TIM3 requests
			dmaConfig(sampling.data, config.readCount, false, _16bit);
			// Enable TIM3 DMA
			TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);

			TIM_Cmd(TIM3, ENABLE);	// Enable timer
		} else {
			// Memory to Memory DMA sampling
			dmaConfig(sampling.data, config.readCount, true, _16bit);
			// Enable TIM3 DMA
			TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
		}
	} else {
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;					// Select EXTI0
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			// Interrupt mode
		if(mode == EXT_RISING) {
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	// Interrupt on rising edge
		} else if(mode == EXT_FALLING) {
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	// Interrupt on falling edge
		}
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;					// Enable EXTI0
		EXTI_Init(&EXTI_InitStructure);								// Init EXTI0

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	// Connect EXTI0 Line to PA0 pin

		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			// EXTI0 interrupt
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// Preemption group as 0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			// Priority as 2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// Enable interrupt
		NVIC_Init(&NVIC_InitStructure);								// Init interrupt
	}
}

void clockDisable() {
	NVIC_InitTypeDef NVIC_InitStructure;

	EXTI_DeInit();

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;    // Specifying the channel
	NVIC_InitStructure.NVIC_IRQChannelCmd                = DISABLE;       // Enabling global interrupt
	NVIC_Init(&NVIC_InitStructure);		                                // Initializing NVIC structure

	TIM_Cmd(TIM3, DISABLE);
}
