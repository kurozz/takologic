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

#include <stm32f10x.h>
#include <stm32f10x_conf.h>
#include <stdbool.h>

#include "sump.h"
#include "timer.h"
#include "uart.h"

// Debug mode (UART at 9600 baud)
//#define DEBUG

// Firmware version
#define VERSION "Takologic v0.1"

// Sample buffer size
#define SAMPLESIZE 10240
#define MAX_SAMPLERATE 10000000

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
uint32_t serialTriggerBuffer;
struct trigger {
	uint32_t armed;
	uint32_t mask;
	uint32_t value;
	uint32_t delay;
	uint8_t level;
	uint8_t channel;
	bool serial;
} trigger;

// Milliseconds counter
volatile uint64_t millis = 0;

// LED last state
uint8_t led = 0;

// System Ticker
void SysTick_Handler() {
	millis++;
}

void TIM3_IRQHandler() {
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		if(trigger.armed != 0) {	// Wait for trigger
			if(trigger.serial == false) {	// Parallel trigger
				// Wait for trigger conditions being met
				if((GPIOB->IDR & trigger.mask) == trigger.armed) {
					sampling.data[sampling.index] = ( (GPIOB->IDR >> 8) & 0xFF );
					sampling.index--;

					trigger.armed = 0;
				}
			} else {	// Serial trigger
				serialTriggerBuffer <<= 1;
				serialTriggerBuffer |= ( (GPIOB->IDR >> (trigger.channel+8)) & 0x01 );

				if((serialTriggerBuffer & trigger.mask) == trigger.armed) {
					trigger.armed = 0;
				}
			}
		} else if(trigger.delay == 0) {	// Already triggered, capture data
			// Acquire data
			sampling.data[sampling.index] = ( (GPIOB->IDR >> 8) & 0xFF );
			sampling.index--;

			// Check if end of sampling
			if(sampling.index == 0) {
				timerDisable();
				sampling.ongoing = false;
				sampling.done = true;
			}
		} else {	// Waiting for sampling delay after trigger
			//Subtract a sampling time from the sampling delay
			trigger.delay--;
		}
	}
}

// Configures the input and output pins
void pinConfig();

// Toggles the LED
void toggleLed();

// Initializes sampling, trigger variables and the sampling timer
void startSampling();

// Sends metadata to PC
void sendMeta();

int main() {
	uint64_t lastMillis = 0;

	// Configure SysTick to 1ms
	SysTick_Config(SystemCoreClock / 1000);


	pinConfig();

#ifdef DEBUG
	uartInit(9600);
#else
	uartInit(115200);
#endif

	sampling.index = 0;
	sampling.ongoing = false;
	sampling.done = false;

	trigger.mask = 0x0000;
	trigger.value = 0x0000;
	trigger.delay = 0;
	trigger.serial = false;
	trigger.channel = 0;

	config.divider = 1;
	config.readCount = SAMPLESIZE;
	config.delayCount = SAMPLESIZE;

	// Main loop
	while(true) {
		// Flash led if sampling
		if(sampling.ongoing == true) {
			if( (millis - lastMillis) > 50 ) {
				lastMillis = millis;
				toggleLed();
			}
		}

		// If sampling is done, send data to PC
		if(sampling.done == true) {
			// Reset done flag
			sampling.done = false;

			// Make sure LED is turned off
			GPIO_SetBits(GPIOC, GPIO_Pin_13);

			// Send data
			for(sampling.index = 0; sampling.index < config.readCount; sampling.index++) {
				uartPutc(sampling.data[sampling.index]);
			}
		}

		// Check for incoming data
		if(uartAvailable()) {
			unsigned char cmd;
			unsigned char long_data[5];

			// Get UART data
			cmd = uartGetc();
			long_data[0] = 0;

			switch (cmd) {
				case SUMP_RESET:			// Reset command (do nothing for now)
					break;

				case SUMP_ARM:
					startSampling();		// Starts sampling for data
					break;

				case SUMP_QUERY:
					uartPuts("1ALS");		// Returns the query
					break;

				case SUMP_GET_METADATA:		// Returns metadata
					sendMeta();
					break;

				case SUMP_TRIG_MASK_S1:		// Get extra data from long commands
				case SUMP_TRIG_MASK_S2:
				case SUMP_TRIG_MASK_S3:
				case SUMP_TRIG_MASK_S4:
				case SUMP_TRIG_VAL_S1:
				case SUMP_TRIG_VAL_S2:
				case SUMP_TRIG_VAL_S3:
				case SUMP_TRIG_VAL_S4:
				case SUMP_TRIG_CFG_S1:
				case SUMP_TRIG_CFG_S2:
				case SUMP_TRIG_CFG_S3:
				case SUMP_TRIG_CFG_S4:
				case SUMP_DIVIDER:
				case SUMP_READ_DELAY:
				case SUMP_SET_FLAGS:
					// Store cmd in long_data
					long_data[0] = cmd;

					// Use cmd as index to get next data
					for(cmd = 1; cmd < 5; cmd++) {
						while(!uartAvailable());
						long_data[cmd] = uartGetc();
					}
					break;
			}

			switch(long_data[0]) {
				case SUMP_DIVIDER:			// Configure the divider
					config.divider = 0;
					config.divider = long_data[3];
					config.divider <<= 8;
					config.divider |= long_data[2];
					config.divider <<= 8;
					config.divider |= long_data[1];
					config.divider++;
					break;

				case SUMP_TRIG_MASK_S1:		// Set trigger mask for stage 1
					trigger.mask = (long_data[1] << 8);
					break;

				case SUMP_TRIG_VAL_S1:		// Set trigger value for stage 1
					trigger.value = (long_data[1] << 8);
					break;

				case SUMP_TRIG_CFG_S1:		// Configure the trigger for stage 1
					// Configure the trigger delay
					trigger.delay = 0;
					trigger.delay = (long_data[3] & 0xC0);
					trigger.delay <<= 8;
					trigger.delay |= long_data[2];
					trigger.delay <<= 8;
					trigger.delay |= long_data[1];

					// Serial mode?
					if((long_data[4] & 0x04) == 0x04) {
						trigger.serial = true;
					} else {
						trigger.serial = false;
					}

					// Channel (channel 0 only, for now)
					trigger.channel = (long_data[4] & 0x01);
					trigger.channel <<= 1;
					trigger.channel |= ( (long_data[3] >> 4) & 0x0F );
					break;

				case SUMP_READ_DELAY:
					// Configure the read counter
					config.readCount = 0;
					config.readCount = long_data[2];
					config.readCount <<= 8;
					config.readCount |= long_data[1];
					config.readCount = (config.readCount*4) + 4;

					// Configure the delay counter
					config.delayCount = 0;
					config.delayCount = long_data[4];
					config.delayCount <<= 8;
					config.delayCount |= long_data[3];
					config.delayCount = config.delayCount*4;
					break;
			}
		}
	}

	return 0;
}

// Configure Inputs and LED output
void pinConfig() {
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable GPIOB and GPIOC clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	//GPIOB pin 8 to 15 as floating inputs, 50MHz
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_8 |
									GPIO_Pin_9 |
									GPIO_Pin_10 |
									GPIO_Pin_11 |
									GPIO_Pin_12 |
									GPIO_Pin_13 |
									GPIO_Pin_14 |
									GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOC pin 13 push-pull output, 2MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Turn off PC13 led
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

// Toggle the LED
void toggleLed() {
	led = !led;

	if(led == 0) {
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
	} else {
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}

// Start the sampling procedure
void startSampling() {
    // Configure the timer
    timerConfig(100000000/config.divider);

    //Arm the trigger
    trigger.armed = (trigger.value & trigger.mask);
    if(trigger.serial == true) {
    	trigger.mask >>= 8;
    	trigger.armed >>= 8;
    	serialTriggerBuffer = 0;
    }

    // Set sampling flag
    sampling.ongoing = true;
    sampling.index = (config.delayCount - 1);

    // Enable Timer
    timerEnable();
}

// Send metadata to client
void sendMeta() {
	// Device name
	uartPutc(0x01);
	uartPuts(VERSION);
	uartPutc(0x00);
	// Amount of sample memory available (bytes)
	sump_sendmeta_uint32(0x21, SAMPLESIZE);
	// Maximum sample rate (Hz)
	sump_sendmeta_uint32(0x23, MAX_SAMPLERATE);
	// Number of usable probes (short)
	sump_sendmeta_uint8(0x40, 0x08);
	// Protocol version (short)
	sump_sendmeta_uint8(0x41, 0x02);
	// End of meta data
	uartPutc(0x00);
}
