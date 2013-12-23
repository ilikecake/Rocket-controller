/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Command interpreter application specific functions
*	\author		Pat Satyshur
*	\version	1.1
*	\date		1/13/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	beer_heater_main
*
*	@{
*/

#include "main.h"
//#include "commands.h"
//#include <stdio.h>
//#include "board.h"

//The number of commands
const uint8_t NumCommands = 5;

//Handler function declerations

//LED control function
static int _F1_Handler (void);
const char _F1_NAME[] 			= "led";
const char _F1_DESCRIPTION[] 	= "Turn LED on or off";
const char _F1_HELPTEXT[] 		= "led <number>";

//DAC commands
static int _F2_Handler (void);
const char _F2_NAME[] 			= "dac";
const char _F2_DESCRIPTION[] 	= "dac commands";
const char _F2_HELPTEXT[] 		= "dac <1> <2>";


//Get time from RTC
static int _F3_Handler (void);
const char _F3_NAME[]  			= "i2cscan";
const char _F3_DESCRIPTION[]  	= "Scan the I2C Bus";
const char _F3_HELPTEXT[]  		= "'i2cscan' has no parameters";


//Set time on RTC
static int _F4_Handler (void);
const char _F4_NAME[]  			= "adset";
const char _F4_DESCRIPTION[]  	= "setup adc";
const char _F4_HELPTEXT[]  		= "none";


//Read a register from the ADC
static int _F5_Handler (void);
const char _F5_NAME[] 			= "stat";
const char _F5_DESCRIPTION[] 	= "Get status of the device";
const char _F5_HELPTEXT[]		= "'stat' has no parameters";

/*
//Write a register to the ADC
static int _F6_Handler (void);
const char _F6_NAME[] PROGMEM 			= "adwrite";
const char _F6_DESCRIPTION[] PROGMEM 	= "write to a register on the ADC";
const char _F6_HELPTEXT[] PROGMEM 		= "adwrite <register> <data>";

//Set up the calibration for the internal temperature sensor
static int _F7_Handler (void);
const char _F7_NAME[] PROGMEM 			= "tempcal";
const char _F7_DESCRIPTION[] PROGMEM 	= "Calibrate the internal temperature sensor";
const char _F7_HELPTEXT[] PROGMEM 		= "'tempcal' has no parameters";

//Test the buzzer
static int _F8_Handler (void);
const char _F8_NAME[] PROGMEM 			= "beep";
const char _F8_DESCRIPTION[] PROGMEM 	= "Test the buzzer";
const char _F8_HELPTEXT[] PROGMEM 		= "beep <time>";

//Turn the relay on or off
static int _F9_Handler (void);
const char _F9_NAME[] PROGMEM 			= "relay";
const char _F9_DESCRIPTION[] PROGMEM 	= "Control the relay";
const char _F9_HELPTEXT[] PROGMEM 		= "relay <state>";

//Manual calibration of the ADC
static int _F10_Handler (void);
const char _F10_NAME[] PROGMEM 			= "cal";
const char _F10_DESCRIPTION[] PROGMEM 	= "Calibrate the ADC";
const char _F10_HELPTEXT[] PROGMEM 		= "'cal' has no parameters";

//Get temperatures from the ADC
static int _F11_Handler (void);
const char _F11_NAME[] PROGMEM 			= "temp";
const char _F11_DESCRIPTION[] PROGMEM 	= "Get temperatures from the ADC";
const char _F11_HELPTEXT[] PROGMEM 		= "'temp' has no parameters";

//Scan the TWI bus for devices
static int _F12_Handler (void);
const char _F12_NAME[] PROGMEM 			= "twiscan";
const char _F12_DESCRIPTION[] PROGMEM 	= "Scan for TWI devices";
const char _F12_HELPTEXT[] PROGMEM 		= "'twiscan' has no parameters";

//Dataflash functions
static int _F13_Handler (void);
const char _F13_NAME[] PROGMEM 			= "mem";
const char _F13_DESCRIPTION[] PROGMEM 	= "dataflash functions";
const char _F13_HELPTEXT[] PROGMEM 		= "mem <1> <2> <3>";
*/

//Command list
const CommandListItem AppCommandList[] =
{
	{ _F1_NAME,		1,  2,	_F1_Handler,	_F1_DESCRIPTION,	_F1_HELPTEXT	},		//led
	{ _F2_NAME, 	1,  2,	_F2_Handler,	_F2_DESCRIPTION,	_F2_HELPTEXT	},		//dac
	{ _F3_NAME, 	0,  0,	_F3_Handler,	_F3_DESCRIPTION,	_F3_HELPTEXT	},		//gettime
	{ _F4_NAME, 	0,  3,	_F4_Handler,	_F4_DESCRIPTION,	_F4_HELPTEXT	},		//ad
	{ _F5_NAME, 	0,  0,	_F5_Handler,	_F5_DESCRIPTION,	_F5_HELPTEXT	},		//STAT
	/*
	{ _F6_NAME, 	2,  2,	_F6_Handler,	_F6_DESCRIPTION,	_F6_HELPTEXT	},		//adwrite	
	{ _F7_NAME, 	0,  0,	_F7_Handler,	_F7_DESCRIPTION,	_F7_HELPTEXT	},		//tempcal
	{ _F8_NAME,		1,  1,	_F8_Handler,	_F8_DESCRIPTION,	_F8_HELPTEXT	},		//beep
	{ _F9_NAME,		1,  1,	_F9_Handler,	_F9_DESCRIPTION,	_F9_HELPTEXT	},		//relay
	{ _F10_NAME,	0,  0,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//cal
	{ _F11_NAME,	0,  0,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//temp
	{ _F12_NAME,	0,  0,	_F12_Handler,	_F12_DESCRIPTION,	_F12_HELPTEXT	},		//twiscan
	{ _F13_NAME,	1,  3,	_F13_Handler,	_F13_DESCRIPTION,	_F13_HELPTEXT	},		//twiscan
	*/
};

//Command functions

//LED control function
static int _F1_Handler (void)
{
	if(NumberOfArguments() == 2)
	{
		Board_LED_Set(argAsInt(1), argAsInt(2));
	}
	else
	{
		if(argAsInt(1) == 1)
		{
			//Turn on all leds
			Board_LED_Set(1, 1);
			Board_LED_Set(2, 1);
			Board_LED_Set(3, 1);
		}
		else
		{
			//Turn off all leds
			Board_LED_Set(1, 0);
			Board_LED_Set(2, 0);
			Board_LED_Set(3, 0);
		}

	}
	return 0;
}

static int _F2_Handler (void)
{
	//uint8_t command2 = argAsInt(2);
	//uint8_t SendBuffer[4];

	uint8_t CommandNumber = argAsInt(1);
	uint16_t DACVaule = argAsInt(2);

	if((CommandNumber > 0) && (CommandNumber < 6))
	{
		printf("Set DAC %u to 0x%04X\r\n", CommandNumber, DACVaule);
		AD5666SetVoltage(CommandNumber, DACVaule);
	}

	else
	{
		switch(CommandNumber)
		{
		case 6:
			//Modify the CS pin
			printf("Select %u\r\n", DACVaule);
			AD5666Select((uint8_t)DACVaule);
			break;

		case 7:
			//Modify the update pin
			printf("Update %u\r\n", DACVaule);
			AD5666Update((uint8_t)DACVaule);
			break;

		case 8:
			//Modify the clear pin
			printf("Clear %u\r\n", DACVaule);
			AD5666Clear((uint8_t)DACVaule);
			break;

		}
	}

	return 0;
}


//i2cscan
static int _F3_Handler (void)
{
	uint8_t i;
	uint8_t DummyByte = 0x0A;

	printf("Scanning I2C Bus\r\n");

	for (i = 0; i <= 0x7F; i++)
	{
		if(Chip_I2C_MasterSend(I2C0, i, &DummyByte, 1) > 0)
		{
			printf("Device responded at 0x%02X\r\n", i);
		}
	}
	printf("Scan complete\r\n");
	return 0;
}

//adset
static int _F4_Handler (void)
{
	if(NumberOfArguments() >= 2)
	{
		switch (argAsInt(1))
		{
		case 1:
			printf("Setup 0x%02X\r\n", TCA9554A_GetConfig(0x38));
			break;

		case 2:
			TCA9554A_SetConfig(0x38, argAsInt(2));
			printf("OK\r\n");
			break;

		case 3:
			printf("State 0x%02X\r\n", TCA9554A_GetState(0x38));
			break;

		case 4:
			TCA9554A_SetState(0x38, argAsInt(2));
			printf("OK\r\n");
			break;

		case 5:
			TCA9554A_SetStateBit(0x38, argAsInt(2), argAsInt(3));
			printf("OK\r\n");
			break;

		case 6:
			TCA9554A_SetStateBits(0x38, argAsInt(2), argAsInt(3));
			printf("OK\r\n");
			break;

		case 7:
			printf("start timer at %u kHz\r\n", argAsInt(2));
			AD7606StartDataClock(argAsInt(2)*1000);
			//while(AD7606GetStatus() != AD7606_STATUS_DATAREADY) {}
			//AD7606GetDataSet();
			break;

		case 8:
			AD7606GetDataSet();
			printf("Get Data\r\n");
			break;

		case 9:
			AD7606_Start(argAsInt(2));
			break;

		}
	}
	return 0;
}

static int _F5_Handler (void)
{
	printf("Chip Status:\r\n");
	printf("Input Clock Rate: %u MHz\r\n", Chip_Clock_GetMainOscRate()/1000000);
	printf("System Clock Rate: %u MHz\r\n", Chip_Clock_GetSystemClockRate()/1000000);
	printf("Peripheral Clock Rate: %u MHz\r\n", Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2)/1000000);

	return 0;
}

/** @} */
