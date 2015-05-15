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

#include "timer.h"

void timerConfig(uint32_t freq) {
	GPIO_InitTypeDef GPIO_InitStructure;                        // GPIOA structure
	NVIC_InitTypeDef NVIC_TIM3;                                 // NVIC structure
	TIM_TimeBaseInitTypeDef TIM3_TimeBase;                      // Time base structure
	TIM_OCInitTypeDef TIM3_OC;                            		// Output Compare structure

	uint16_t period;
	uint16_t prescaler;

	if(freq > 100000) {
		period = ( 36000000/(freq) );
		prescaler = 1;
	} else if(freq > 1000) {
		period = ( 9000000/(freq) );
		prescaler = 7;
	} else {
		period = ( 50000/(freq) );
		prescaler = 1439;
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // Clocking GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        // Clocking TIM3

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;                 // Ch.1 (PA6)
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;            // Alternative function Push Pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           // 50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      // Initializing GPIOA structure

	NVIC_TIM3.NVIC_IRQChannel                   = TIM3_IRQn;    // Specifying the channel
	NVIC_TIM3.NVIC_IRQChannelPreemptionPriority = 0;            // Only matters for multiple interrupts
	NVIC_TIM3.NVIC_IRQChannelSubPriority        = 0;            // Only matters for multiple interrupts
	NVIC_TIM3.NVIC_IRQChannelCmd                = ENABLE;       // Enabling global interrupt
	NVIC_Init(&NVIC_TIM3);                                      // Initializing NVIC structure

	TIM3_TimeBase.TIM_ClockDivision = 0;                        // Not dividing
	TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;       // Upcounting configuration
	TIM3_TimeBase.TIM_Period        = period;                	// Autoreload value (ARR)
	TIM3_TimeBase.TIM_Prescaler     = prescaler;           				// Dividing APB1 by 2
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);                     // Initializing Time Base structure

	TIM3_OC.TIM_OCMode      = TIM_OCMode_Toggle;                // Output compare toggling mode
	TIM3_OC.TIM_OutputState = TIM_OutputState_Enable;           // Enabling the Output Compare state
	TIM3_OC.TIM_OCPolarity  = TIM_OCPolarity_Low;               // Reverse polarity
	TIM3_OC.TIM_Pulse       = period;                        	// Output Compare 1 reg value
	TIM_OC1Init(TIM3, &TIM3_OC);                                // Initializing Output Compare 1 structure
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);          // Disabling Ch.1 Output Compare preload

	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);                     // Enabling TIM3 Ch.1 interrupts
}

void timerDisable() {
	TIM_Cmd(TIM3, DISABLE);
}

void timerEnable() {
	TIM_Cmd(TIM3, ENABLE);
}
