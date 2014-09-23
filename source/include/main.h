/*
 * main.h
 *
 *  Created on: Sep 24, 2013
 *      Author: pat
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "command.h"
#include "AD5666.h"
#include "i2c.h"
#include "TCA9554A.h"
#include "AD7606.h"
#include "MAX31855.h"
#include "MX106T.h"

#include "ControlLoop.h"
#include "at45db321d.h" //flash memory driver


void vEStopTask(void * pvParameters );
void vFireControlTask(void * pvParameters );
void vDataAquisitionTask(void * pvParameters );
void vServoReadTask(void * pvParameters );
void vDataSendTask(void * pvParameters );
void vRunCommandTask(void * pvParameters );
void vUARTTask(void * pvParameters );


xTaskHandle vEStopTaskHandle;
xTaskHandle vFireControlTaskHandle;
xTaskHandle vDataAquisitionTaskHandle;
xTaskHandle vServoReadTaskHandle;
xTaskHandle vDataSendTaskHandle;
xTaskHandle vUARTTaskHandle;
xTaskHandle vRunCommandTaskHandle;


#endif /* MAIN_H_ */
