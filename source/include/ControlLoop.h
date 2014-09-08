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
*	\brief		Header file for the AD7193 A/D converter.
*	\author		Jonathan McCabe, Pat Satyshur
*	\version	1.0
*	\date		12/01/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

#ifndef _CONTROLLOOP_H_
#define _CONTROLLOOP_H_


#define MAX_COMMANDS			16
#define MAX_REDLINES			5



//Global Variables
//char command[MAX_COMMAND_DESCRIPTION_LENGTH+1];

//char trigger;
unsigned int commandNum;
unsigned int commandMax;
unsigned int commandTime[MAX_COMMANDS];					//time that action will occur
Bool DO_Command[MAX_COMMANDS][TOTAL_DO_CHANNELS];		//state of digital output
int Servo_Command[MAX_COMMANDS][TOTAL_SERVO_CHANNELS];	//state of propellant valve at commandTime

int redlineTimeStart[MAX_REDLINES];		//time at which redline is activated
int redlineTimeEnd[MAX_REDLINES];		//time at which redline is deactivated
int redlineChannel[MAX_REDLINES];		//channel sampled by redline (TC channels are given as TC# + TOTAL_AI_CHANNELS)
int redlineMin[MAX_REDLINES];			//minimum allowable value below which the test will be aborted
int redlineMax[MAX_REDLINES];			//maximum allowable value above which the test will be aborted

//
uint8_t runningControl;
uint8_t runningData;
uint8_t	activeSaveData;
uint8_t activeEstop;
//uint32_t tData, tControl;


//buffers to store all data
xSemaphoreHandle dataSemaphore;	//control data handling
uint32_t dataTime[2];	//store start and end time of data sample
uint16_t analogBuffer[AI_CHIPS*AI_CHANNELS_PER_CHIP];	//store all analog input channels
uint16_t TCbuffer[2*TC_CHIPS*TC_CHANNELS_PER_CHIP];			//store temperature and cold junction  for each TC channel
uint16_t ServoPosition[TOTAL_SERVO_CHANNELS];		//store current position of servo motors

			
void InitControl(void);
//void MainControlLoop(void);
void UpdateCommand(uint32_t tNow);
void ReadData(void);

void SaveDataToFlash(void);//uint32_t *dataTime[], uint16_t *analogBuffer[], uint16_t *TCbuffer[], uint16_t *ServoPosition[], uint8_t runningControl, uint8_t activeEstop);
//void FinishSaveData();

void Redlines(uint32_t tNow);


#endif
