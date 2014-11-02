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
const uint8_t NumCommands = 13;

//Handler function declarations

//Digital Output control function
static int _F1_Handler (void);
const char _F1_NAME[] 			= "setdo";
const char _F1_DESCRIPTION[] 	= "Turn valve on or off";
const char _F1_HELPTEXT[] 		= "setdo <DO channel> <state>";

//Set Servo Postion
static int _F2_Handler (void);
const char _F2_NAME[] 			= "setservo";
const char _F2_DESCRIPTION[] 	= "Set servo position";
const char _F2_HELPTEXT[] 		= "setservo <servo channel> <pos>";

//read Analog data
static int _F3_Handler (void);
const char _F3_NAME[]  			= "getai";
const char _F3_DESCRIPTION[]  	= "read analog inputs";
const char _F3_HELPTEXT[]  		= "'getai' has no parameters";

//read TC data
static int _F4_Handler (void);
const char _F4_NAME[]  			= "gettc";
const char _F4_DESCRIPTION[]  	= "read temperatures";
const char _F4_HELPTEXT[]  		= "'gettc' has no parameters";

//Set Command Sequence
static int _F5_Handler (void);
const char _F5_NAME[]  			= "sequence";
const char _F5_DESCRIPTION[]  	= "set/get command sequence";
const char _F5_HELPTEXT[]  		= "'sequence' <1> <2> <3>";//<DO channel> <time> <state>

//Set Redlines
static int _F6_Handler (void);
const char _F6_NAME[]  			= "setred";
const char _F6_DESCRIPTION[]  	= "set redline triggers";
const char _F6_HELPTEXT[]  		= "'setred' <1> <2> <3>";//<DO channel> <time> <state>

//Read Time
static int _F7_Handler (void);
const char _F7_NAME[] 			= "gettime";
const char _F7_DESCRIPTION[] 	= "Get realtime clock time";
const char _F7_HELPTEXT[]		= "'gettime' has no parameters";

//start test sequence
static int _F8_Handler (void);
const char _F8_NAME[] 			= "fire";
const char _F8_DESCRIPTION[] 	= "start test sequence";
const char _F8_HELPTEXT[]		= "'fire' has no parameters";



//Send commands to DAC chip
static int _F9_Handler (void);
const char _F9_NAME[] 			= "setupdac";
const char _F9_DESCRIPTION[] 	= "Send DAC commands";
const char _F9_HELPTEXT[] 		= "dac <1> <2>";

//Send commands to ADC chip
static int _F10_Handler (void);
const char _F10_NAME[]  			= "setupadc";
const char _F10_DESCRIPTION[]  	= "Send ADC commands";
const char _F10_HELPTEXT[]  		= "setupadc <1> <2>";

//Send commands to TC chip
static int _F11_Handler (void);
const char _F11_NAME[]  			= "setuptc";
const char _F11_DESCRIPTION[]  	= "Send TC commands";
const char _F11_HELPTEXT[]  		= "setuptc <1> <2>";

//look for i2C devices
static int _F12_Handler (void);
const char _F12_NAME[]  			= "i2cscan";
const char _F12_DESCRIPTION[]  	= "Scan the I2C Bus";
const char _F12_HELPTEXT[]  		= "'i2cscan' has no parameters";

//Enables or disables data read and data send
static int _F13_Handler (void);
const char _F13_NAME[]  			= "datastart";
const char _F13_DESCRIPTION[]  	= "Enable/Disable data";
const char _F13_HELPTEXT[]  		= "'datastart' <1>";



/*

//Dataflash functions
static int _F13_Handler (void);
const char _F13_NAME[] PROGMEM 			= "mem";
const char _F13_DESCRIPTION[] PROGMEM 	= "dataflash functions";
const char _F13_HELPTEXT[] PROGMEM 		= "mem <1> <2> <3>";
*/

//Command list
const CommandListItem AppCommandList[] =
{
	{ _F1_NAME,		1,  2,	_F1_Handler,	_F1_DESCRIPTION,	_F1_HELPTEXT	},		//setdo: Set Digital Output Channel
	{ _F2_NAME,		1,  2,	_F2_Handler,	_F2_DESCRIPTION,	_F2_HELPTEXT	},		//Set servo position
	{ _F3_NAME, 	0,  0,	_F3_Handler,	_F3_DESCRIPTION,	_F3_HELPTEXT	},		//getai: read Analog data
	{ _F4_NAME, 	0,  0,	_F4_Handler,	_F4_DESCRIPTION,	_F4_HELPTEXT	},		//getTC: read TC data
	{ _F5_NAME, 	0,  3+TOTAL_SERVO_CHANNELS,	_F5_Handler,	_F5_DESCRIPTION,	_F5_HELPTEXT	},		//sequence: Set Command Sequence
	{ _F6_NAME, 	0,  6,	_F6_Handler,	_F6_DESCRIPTION,	_F6_HELPTEXT	},		//setred: Set Redlines
	{ _F7_NAME, 	0,  0,	_F7_Handler,	_F7_DESCRIPTION,	_F7_HELPTEXT	},		//Read Time
	{ _F8_NAME, 	1,  1,	_F8_Handler,	_F8_DESCRIPTION,	_F8_HELPTEXT	},		//fire: start test sequence
	{ _F9_NAME,		1,  1,	_F9_Handler,	_F9_DESCRIPTION,	_F9_HELPTEXT	},		//setupdac: Send commands to DAC chip
	{ _F10_NAME,	1,  1,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//setupadc: Send commands to ADC chip
	{ _F11_NAME,	0,  2,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//setuptc: Send commands to TC chip
	{ _F12_NAME,	0,  0,	_F12_Handler,	_F12_DESCRIPTION,	_F12_HELPTEXT	},		//i2cscan
	{ _F13_NAME,	1,  1,	_F13_Handler,	_F13_DESCRIPTION,	_F13_HELPTEXT	},		//datastart: enables or disables data read and data send
	/*	{ _F13_NAME,	1,  3,	_F13_Handler,	_F13_DESCRIPTION,	_F13_HELPTEXT	},		//twiscan
	*/
};

//Command functions

//setdo:	Digital Output control function
static int _F1_Handler (void)
{
	uint8_t n;

	//argAsInt(1) is Digital Output channel
	//argAsInt(2) is state of valve
	if(NumberOfArguments() == 2)
	{
		Board_DO_Set(argAsInt(1), argAsInt(2));
	}
	else
	{
		//Turn off all channels
		for (n=1;n<=TOTAL_DO_CHANNELS;n++)
		{
			Board_DO_Set(n, 0);
		}

	}
	return 0;
}

//Servo Valve Control Function
static int _F2_Handler (void)
{

	//argAsInt(1) is Digital Output channel
	//argAsInt(2) is state of valve
	if(NumberOfArguments() == 2)
	{
		//Board_DO_Set(argAsInt(1), argAsInt(2));
	}
	else
	{
		//Turn off all valves


	}
	return 0;
}

//getai: read Analog Inputs
static int _F3_Handler (void)
{

	uint8_t channel;
	uint8_t chipNumber;
	uint16_t DataSet[8];


	//read analog channel data
	//for(chipNumber=1;chipNumber<=AI_CHIPS;chipNumber++)
	//{
		//get analog data from chip "chipNumber"
		AD7606GetDataSet(1, DataSet);
		for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
		{
		 printf("ADC1[%u]: %d counts\r\n", channel, DataSet[channel]);
		}

		//get analog data from chip "chipNumber"
		AD7606GetDataSet(3, DataSet);
		for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
		{
		 printf("ADC3[%u]: %d counts\r\n", channel, DataSet[channel]);
		}

	//}


	/*
	portTickType tickTime;
	portTickType interval;
	//interval=configTICK_RATE_HZ/100;//Set frequency to 100 loops per second
	interval=configTICK_RATE_HZ/5;//Set frequency to 5 loops per second

	while (1)
	{
		tickTime = xTaskGetTickCount();
		ReadData();
		SendData();
		vTaskDelayUntil(&tickTime,interval);
	}*/


	return 0;
}

//getTC: read Temperatures
static int _F4_Handler (void)
{
	uint8_t sel;
	uint16_t coldJunction;
	uint16_t temperature;
	//float temp;
	//float tempCold;

	for(sel=0;sel<8;sel++)
	{
		temperature=MAX31855read(sel, &coldJunction);
		printf("TC%u: %u, %u \r\n",sel,temperature,coldJunction);

		//temp = ((float)temperature)*.25;
		//tempCold=((float)coldJunction)*.0625;
		//printf("TC%u: %s, %s \r\n",sel,temp,tempCold);

	}

	return 0;
}

// Set/Get Command Sequence
static int _F5_Handler (void)
{
	//setup the fire sequence
	//commandMax=MAX_COMMANDS-1;
	uint8_t channel;

	if(NumberOfArguments() == 3+TOTAL_SERVO_CHANNELS)
	{
		commandNum = argAsInt(1);//when setup is complete CommandNum will be the last command
		if (commandNum >= MAX_COMMANDS)
		{
			printf("Exceeded maximum sequence lines:\r\n");
		}
		else
		{
			commandTime[commandNum] =  argAsInt(2);//milliseconds

			//the lowest 16 bits of argAsInt(3) represent the state of each DO channel
			//DO0 is LSB, DO16 is MSB
			DO_Command[commandNum]=argAsInt(3);

		/*
			//the lowest 16 bits of argAsInt(3) represent the state of each DO channel
			//DO0 is LSB, DO16 is MSB
			for(channel=0;channel<TOTAL_DO_CHANNELS;channel++)
			{
				//DO_Command[commandNum] = DO_Command[commandNum] & (1<<channel);//mask out this channel
				DO_Command[commandNum] = (argAsInt(3) & (1<<channel)) >>channel;
				//printf("DO%u:%u\r\n",channel,DO_Command[commandNum][channel]);
			}
		 */

			for(channel=0;channel<TOTAL_SERVO_CHANNELS;channel++)
			{
				Servo_Command[commandNum][channel] = argAsInt(4+channel);
			}


		}

	}
	else
	{
		//printf("Wrong number of arguments.  Sequence must be of form:\r\n");
		//printf("CommandNum, milliseconds, %u DO bits, %u servo\r\n", TOTAL_DO_CHANNELS, TOTAL_SERVO_CHANNELS);

		//return the entire command sequence

		printf("Time:");
		for(commandNum=0;commandNum<MAX_COMMANDS;commandNum++)
		{
			printf(" %u", commandTime[commandNum] );
		}
		printf("\r\n");

		for(channel=0;channel<TOTAL_DO_CHANNELS;channel++)
		{
			printf("DO%u:",channel);
			for(commandNum=0;commandNum<MAX_COMMANDS;commandNum++)
			{
				printf(" %u", (DO_Command[commandNum] >> channel) & 1);
			}
			printf("\r\n");
		}

		for(channel=0;channel<TOTAL_SERVO_CHANNELS;channel++)
		{
			printf("DO%u:",channel);
			for(commandNum=0;commandNum<MAX_COMMANDS;commandNum++)
			{
				printf(" %u",Servo_Command[commandNum][channel]);
			}
			printf("\r\n");
		}
	}

	return 0;
}


//setup the Redline Triggers
static int _F6_Handler (void)
{
	uint8_t RLnum;
	//RLnum, RLstart, RLend, channel, min, max

	if(NumberOfArguments() == 6)
	{
		RLnum = argAsInt(1);
		if (RLnum<MAX_REDLINES){
			redlineTimeStart[RLnum] = argAsInt(2);
			redlineTimeEnd[RLnum] = argAsInt(3);
			redlineChannel[RLnum] = argAsInt(4);
			redlineMin[RLnum] = argAsInt(5);
			redlineMax[RLnum] = argAsInt(6);
		}
	}
	else
	{
		printf("Wrong number of arguments.  Sequence must be of form:\r\n");
		printf("RLnum, RLstart, RLend, channel, min, max\r\n");
	}


	return 0;
}

static int _F7_Handler (void)
{
	printf("Chip Status:\r\n");
	printf("Input Clock Rate: %u MHz\r\n", Chip_Clock_GetMainOscRate()/1000000);
	printf("System Clock Rate: %u MHz\r\n", Chip_Clock_GetSystemClockRate()/1000000);
	printf("Peripheral Clock Rate: %u MHz\r\n", Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2)/1000000);
	printf("CompileDate: %s%s\r\n",__DATE__,__TIME__);

	return 0;
}


//fire.  Start test sequence
static int _F8_Handler (void)
{

	//Board_LED_Set(0,1);
	//Board_LED_Set(1,0);
	//Board_LED_Set(2,1);

	if(argAsInt(1) == 1)
	{
		// setup Emergency Stop task
		xTaskCreate(vEStopTask, (signed char *) "vEStopTask",
					configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 7UL),
					&vEStopTaskHandle);


		// Control Digital Outputs and Servos during run time
		xTaskCreate(vFireControlTask, (signed char *) "vFireControlTask",
					configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 8UL),
					&vFireControlTaskHandle);

	}
	else
	{
		//trigger emergency stop
		emergencyStop = 1;
		vTaskResume(vEStopTaskHandle);//make sure data is being acquired

	}

	return 0;
}


//setupdac: Send commands to DAC chip
static int _F9_Handler (void)
{
	//send commands to the Analog Output chip

	//uint8_t command2 = argAsInt(2);
	//uint8_t SendBuffer[4];

	uint8_t CommandNumber = argAsInt(1);
	uint16_t DACValue = argAsInt(2);

	if((CommandNumber > 0) && (CommandNumber < 6))
	{
		printf("Set DAC %u to 0x%04X\r\n", CommandNumber, DACValue);
		AD5666SetVoltage(CommandNumber, DACValue);
	}

	else
	{
		switch(CommandNumber)
		{
		case 6:
			//Modify the CS pin
			printf("Select %u\r\n", DACValue);
			AD5666Select((uint8_t)DACValue);
			break;

		case 7:
			//Modify the update pin
			printf("Update %u\r\n", DACValue);
			AD5666Update((uint8_t)DACValue);
			break;

		case 8:
			//Modify the clear pin
			printf("Clear %u\r\n", DACValue);
			AD5666Clear((uint8_t)DACValue);
			break;

		}
	}

	return 0;
}

//setupadc: Send commands to ADC chip
static int _F10_Handler (void)
{
	uint8_t channel;
	uint8_t chipsel;
	uint16_t DataSet[8];

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
			if(argAsInt(2) == 1)
			{
				printf("Select ADC1\r\n");
				AD7606_Select(1, 1);
			}
			else
			{
				printf("Deselect ADC1\r\n");
				AD7606_Select(1, 0);
			}
			//printf("start timer at %u kHz\r\n", argAsInt(2));
			//AD7606StartDataClock(argAsInt(2)*1000);
			//while(AD7606GetStatus() != AD7606_STATUS_DATAREADY) {}
			//AD7606GetDataSet();
			break;

		case 8:



			chipsel=0;

			//get analog data from chip "chipsel"
			AD7606GetDataSet(chipsel, DataSet);

			for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
			{
				printf("ADC[%u]: %d counts\r\n", channel, DataSet[channel]);
				//analogBuffer[channel] = DataSet[channel-chipsel*AI_CHANNELS_PER_CHIP];
			}


			/*
			uint8_t i;
			for(i=0; i<8; i++)
			{
				//printf("ADC[%u]: 0x%02X, 0x%02X,", i, ADC_DataArray2[i*2], ADC_DataArray2[i*2+1]);
				//ADC_DataArray[i] = (int16_t)((ADC_DataArray2[i*2] & 0xFF) << 8);
				//ADC_DataArray[i] |= (int16_t)((ADC_DataArray2[i*2+1]) & 0xFF);
				//printf(" %d counts\r\n", ADC_DataArray[i]);
				printf("ADC[%u]: %d counts\r\n", i, ADC_DataArray[i]);
			}
*/
			break;

		//case 9:
		//	AD7606_Start(chipsel, argAsInt(2));
		//	break;
		}
	}
	return 0;
}

//setuptc: Send commands to TC chip
static int _F11_Handler (void)
{
	uint8_t Val1;
	uint8_t Val2;

	Val1 = argAsInt(1);

	if(NumberOfArguments() == 1)
	{
		if(Val1 == 0xF1)
		{
			printf("Select\r\n");
			XRA1402Select(1);
		}
		else if (Val1 == 0xF0)
		{
			printf("Deselect\r\n");
			XRA1402Select(0);
		}
		else
		{
			XRA1402ReadReg((Val1<<1), &Val2);
			printf("Register 0x%02X: 0x%02X\r\n", Val1, Val2);
		}
	}
	if(NumberOfArguments() == 2)
	{
		Val2 = argAsInt(2);
		printf("Writing 0x%02X to register 0x%02X...", Val2, Val1);
		XRA1402WriteReg((Val1<<1), Val2);
		printf("Done\r\n");
	}

	return 0;
}

//i2cscan
static int _F12_Handler (void)
{
	uint8_t i;
	uint8_t DummyByte = 0x0A;

	printf("Scanning I2C Bus\r\n");

	for (i = 0; i <= 0x7F; i++)
	{
		if(Chip_I2C_MasterSend(I2C1, i, &DummyByte, 1) > 0)
		{
			printf("Device responded at 0x%02X\r\n", i);
		}
	}
	printf("Scan complete\r\n");
	return 0;
}


//datastart: Enable/Disable dataread and datasend tasks
static int _F13_Handler (void)
{
	portTickType tickTime;
	portTickType interval;
	uint8_t g = 0;

	if (argAsInt(1)==1)
	{
		runningData = 1;
		vTaskResume(vDataAquisitionTaskHandle);//make sure data is being acquired
		//vTaskResume(vServoReadTask);//make sure data is being acquired
		//vTaskResume(vDataSendTask);//make sure data is being acquired
	}
	else if (argAsInt(1)==0)
	{
		vTaskSuspend(vDataAquisitionTaskHandle);//make sure data is being acquired
		runningData = 0;
		//vTaskSuspend(vServoReadTask);//make sure data is being acquired
		//vTaskSuspend(vDataSendTask);//make sure data is being acquired
	}
	else if (argAsInt(1)==3)
	{
		//vTaskResume(vDataAquisitionTaskHandle);//make sure data is being acquired
		//vTaskResume(vServoReadTask);//make sure data is being acquired
		//vTaskResume(vDataSendTask);//make sure data is being acquired

		printf("Data Speed Test Start\r\n");
		tickTime = xTaskGetTickCount();
		for(g=0;g<50;g++)
		{
			ReadData();
			SendData();//takes 6.88ms

			vTaskDelay(10);
		}
		interval= xTaskGetTickCount();
		printf("\r\nElapsed Time =%dms\r\n",interval-tickTime-500);

	/*
	3.22	ms/ AI read
	1.94	ms/TC read
	6.86	ms/send
	0.2		ms/DO
	2?		ms/servo out
	2?		ms/servo read
	62	Hz max
	 */
	}

	return 0;
}




/** @} */
