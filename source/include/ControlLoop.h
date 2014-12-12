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

#define DO_HEATER				15	//DO channel for 120VAC heater power
#define DO_SPARK1				6	//DO channel for 12VDC power to servo 1
#define DO_SPARK2				5	//DO channel for 12VDC power to servo 2


//Global Variables
//char command[MAX_COMMAND_DESCRIPTION_LENGTH+1];

//char trigger;
unsigned int commandNum;
unsigned int commandFinal;
unsigned int dataRate; //rate of data aquisition in Hz
unsigned int commandTime[MAX_COMMANDS];					//time that action will occur
uint16_t DO_Command[MAX_COMMANDS];		//state of digital.  DO0 is LSB, DO16 is MSB
uint8_t Spark_Command[MAX_COMMANDS];	//state of PWM that drives spark

uint16_t Servo_Command[MAX_COMMANDS][TOTAL_SERVO_CHANNELS];	//state of propellant valve at commandTime
uint8_t lastServoCommand[1];//last position command that was sent to servo (allows us to avoid repeat sends)


int redlineTimeStart[MAX_REDLINES];		//time at which redline is activated
int redlineTimeEnd[MAX_REDLINES];		//time at which redline is deactivated
int redlineChannel[MAX_REDLINES];		//channel sampled by redline (TC channels are given as TC# + TOTAL_AI_CHANNELS)
int redlineMin[MAX_REDLINES];			//minimum allowable value below which the test will be aborted
int redlineMax[MAX_REDLINES];			//maximum allowable value above which the test will be aborted

//
portTickType fireStartTime;
uint8_t runningControl;
uint8_t runningData;
uint8_t	activeSaveData;
uint8_t redlinesEnabled;
uint8_t emergencyStop;
uint8_t redlineNumber; //index of the redline that ended a test

//buffers to store all data
xSemaphoreHandle dataSemaphore;	//control data handling

uint8_t servoCommandFlag;	//this is set to 1 a few milliseconds before the control updates.  It prevents read requests from being sent to the servo when they would conflict
xSemaphoreHandle servoSemaphore;	//don't allow new commands to be sent to servo before old commands are complete
uint16_t dataServo[TOTAL_SERVO_CHANNELS*2];		//store current position of servo motors
uint8_t servoExists[TOTAL_SERVO_CHANNELS];

uint32_t dataSendTime[2];	//store start and end time of data sample
uint16_t dataSendBuffer[AI_CHIPS*AI_CHANNELS_PER_CHIP+2*TC_CHIPS*TC_CHANNELS_PER_CHIP+TOTAL_SERVO_CHANNELS*2+4];	//store all channels



void InitControl(void);
//void MainControlLoop(void);
void UpdateCommand(uint32_t tNow);
void ReadData(void);
void SetServoPosition(uint8_t servoNum, uint16_t position);
void ReadServo(void);
void SendData(void);

void SaveDataToFlash(void);//uint32_t *dataTime[], uint16_t *dataAnalog[], uint16_t *dataTC[], uint16_t *ServoPosition[], uint8_t runningControl, uint8_t activeEstop);
//void FinishSaveData();

void CheckRedlines();


#endif
