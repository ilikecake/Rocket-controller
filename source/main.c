/*
 * main.c
 *
 *  Created on: Sep 24, 2013
 *      Author: pat
 */


/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "main.h"

//#include "board.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"


//#include "command.h"

/** @defgroup EXAMPLES_FREERTOS_17XX40XX_BLINKY LPC17xx/40xx FrreRTOS blinky example
 * @ingroup EXAMPLES_FREERTOS_17XX40XX
 * <b>Example description</b><br>
 * Welcome to the FreeRTOS basic blinky example. This example starts up
 * FreeRTOS and creates 3 tasks. Tasks 1 and 2 blink different LEDs at
 * different rates. Task 3 outputs a tick count to the debug channel (UART)
 * every second.<br>
 *
 * By default, tickless idle is used with this example for FreeRTOS. Normal
 * tick operation can be used by setting the configUSE_TICKLESS_IDLE definition
 * to 0 in FreeRTOSConfig.h<br>
 *
 * To use the example, connect a serial cable to the board's RS232/UART port
 * and start a terminal program (115.2K8N1) to monitor the port. The LEDs will
 * also toggle based on the task execution.<br>
 *
 * <b>Special connection requirements</b><br>
 * - Embedded Artists' LPC1788 Developer's Kit:<br>
 * - Embedded Artists' LPC4088 Developer's Kit:<br>
 * There are no special connection requirements for this example.<br>
 * - LPCXpresso LPC1769:<br>
 * Need to connect with base board for using RS232/UART port.<br>
 *
 * <b>Build procedures:</b><br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_KEIL<br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_IAR<br>
 * @ref LPCOPEN_17XX40XX_BUILDPROCS_XPRESSO<br>
 *
 * <b>Supported boards and board setup:</b><br>
 * @ref LPCOPEN_17XX40XX_BOARD_EA1788<br>
 * @ref LPCOPEN_17XX40XX_BOARD_EA4088<br>
 * @ref LPCOPEN_17XX40XX_BOARD_XPRESSO_1769<br>
 *
 * <b>Submitting LPCOpen issues:</b><br>
 * @ref LPCOPEN_COMMUNITY
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	Board_Init();

	Board_LED_Set(0, false);
	Board_LED_Set(1, true);
	Board_LED_Set(2, false);

	MX106T_Init();

	/* SSP initialization */
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_Enable(LPC_SSP1);

	i2c_app_init(I2C1, SPEED_400KHZ);//SPEED_100KHZ);//SPEED_400KHZ);//

	XRA1402Init();

	//AD5666Init();//set up Analog Output chip
	AD7606Init();//set up Analog Input chip

	PCA9535_Init();

	//check for plugged in USB line
	if (Chip_GPIO_ReadPortBit(LPC_GPIO, USB_GPIO_PORT_NUM, USB_GPIO_BIT_NUM ) == true)
	{
		Board_Xbee_Set(false);//disable Xbee if USB is plugged in
		printf("USB detected, Xbee disabled\r\n");
	}
	else
	{
		Board_Xbee_Set(true);
		printf("USB not detected, Xbee enabled\r\n");
	}

	dataRate = 10;//Hz set default data aquisition rate

}

/* Emergency Stop thread */
void vEStopTask(void * pvParameters ) {
	vTaskSuspend( NULL ); //suspend current task until triggered by emergency stop event
	emergencyStop = 1;

	//kill control task
	vTaskDelete(vFireControlTaskHandle);

	//Set all outputs to emergency state
	uint8_t i;
	for (i=0;i<TOTAL_DO_CHANNELS;i++)
	{
		Board_DO_Set((uint16_t) 0);//set all outputs to 0
	}

	//set servos to closed
	if( xSemaphoreTake( servoSemaphore, ( portTickType ) 200 ) == pdTRUE )	//take data buffer semaphore
	{
		MX106T_Set16bit(1,SERVO_PRESENT_POSITION_16,0);//N2O Valve
		MX106T_Set16bit(2,SERVO_PRESENT_POSITION_16,0);//Fuel Valve
		xSemaphoreGive(servoSemaphore);//give back servo semaphore
		servoCommandFlag = 0;
	}

	//wait for a few seconds.  Continue recording data for 5 seconds during shutdown.
	vTaskDelay(configTICK_RATE_HZ * 5);

	activeSaveData = 0;//stop recording data

	//kill E-Stop task
	vTaskDelete(vEStopTaskHandle);
}


/* Run test sequence */
void vFireControlTask(void * pvParameters ) {
	portTickType tickTime;
	portTickType interval;
	//uint8_t channel;

	//Ensure that a valid control sequence has been uploaded

	runningControl = 1;//show that control sequence is running
	redlinesEnabled = 1;
	emergencyStop = 0;//the emergency stop has not been triggered

	commandNum = 0;//reset sequence to start at command 0
	servoCommandFlag = 0;  //allow servo to be queried for position

	fireStartTime = xTaskGetTickCount(); //store the time at which the control sequence was started

	runningData = 1;
	vTaskResume(vDataAquisitionTaskHandle);//make sure data is being acquired

	while ((commandNum < MAX_COMMANDS) && (commandTime[commandNum]>0)) {

		tickTime = xTaskGetTickCount();

		if (fireStartTime+commandTime[commandNum] > tickTime)//wait for commandTime if we haven't passed it yet
		{
			if (fireStartTime+commandTime[commandNum] - SERVO_DEADBAND > tickTime) //if we have time, wake up in time to stop the ReadServo
			{
				//wait until SERVO_DEADBAND milliseconds before next command time
				interval=(fireStartTime+commandTime[commandNum]) - SERVO_DEADBAND - tickTime;
				vTaskDelayUntil(&tickTime,interval);

				servoCommandFlag = 1;//prevent ReadServo from starting a read that will conflict with the servo command timing
				tickTime = xTaskGetTickCount();
			}

			//wait for next command time
			interval=(fireStartTime+commandTime[commandNum]) - tickTime;
			vTaskDelayUntil(&tickTime,interval);
		}

		//set digital outputs
		/*for (channel=0;channel<TOTAL_DO_CHANNELS;channel++)
		{
			//DO0 is LSB, DO16 is MSB
			PCA9535_SetOutput(channel, (DO_Command[commandNum]&(1<<channel))>>channel );
			Board_DO_Set
		}*/
		Board_DO_Set(DO_Command[commandNum]);//set all DO channel states at once


		//send position commands to servos.  this will take a while
		//vTaskResume(vServoWriteTaskHandle);
		if( xSemaphoreTake( servoSemaphore, ( portTickType ) 50 ) == pdTRUE )	//take servo semaphore
		{
			if (commandNum>0)
			{
				//only send a new servo command if it is different from previous command
				if (Servo_Command[commandNum][0] != Servo_Command[commandNum-1][0])
				{
					MX106T_Set16bit(1,SERVO_PRESENT_POSITION_16,Servo_Command[commandNum][0]);//N2O Valve
				}
				if (Servo_Command[commandNum][1] != Servo_Command[commandNum-1][1])
				{
					MX106T_Set16bit(1,SERVO_PRESENT_POSITION_16,Servo_Command[commandNum][1]);//Fuel Valve
				}
			}
			else
			{
				MX106T_Set16bit(1,SERVO_PRESENT_POSITION_16,Servo_Command[commandNum][0]);//N2O Valve
				MX106T_Set16bit(2,SERVO_PRESENT_POSITION_16,Servo_Command[commandNum][1]);//Fuel Valve
			}
			xSemaphoreGive(servoSemaphore);//give back servo semaphore
			servoCommandFlag = 0;
		}
		else
		{	//if a servo command failed, trigger an emergency stop
			vTaskResume(vEStopTaskHandle);
		}

		commandNum++;

	}

	runningControl = 0;
	redlinesEnabled = 0;

	//Board_LED_Set(0,1);
	//Board_LED_Set(1,0);
	//Board_LED_Set(2,1);

	//kill E-Stop task
	vTaskDelete(vEStopTaskHandle);

	//kill control task
	vTaskDelete(vFireControlTaskHandle);

}

/* This task reads data from analog inputs and TCs */
void vDataAquisitionTask(void * pvParameters ) {
	portTickType tickTime;
	portTickType interval;
	vTaskSuspend( NULL );//suspend current task

	while (1)
	{
		tickTime = xTaskGetTickCount();
		vTaskResume(vDataReadTaskHandle);
		vTaskResume(vServoReadTaskHandle);
		vTaskResume(vDataSendTaskHandle);

		//if (activeSaveData == 1)
		//{
		//	//save data to flash or SD card
		//}

		interval=configTICK_RATE_HZ/dataRate;//Set frequency to 100 loops per second (must be less than 125Hz)
		vTaskDelayUntil(&tickTime,interval);
	}
}

/* This task sends data to remote computer */
void vDataReadTask(void * pvParameters ) {
	vTaskSuspend( NULL );//suspend current task
	while (1)
	{
		//send data to remote computer via wireless RS232
		ReadData();//takes care of semaphore internally
		vTaskSuspend( NULL );//suspend current task
	}
}

/* This task sends data to remote computer */
void vDataSendTask(void * pvParameters ) {
	vTaskSuspend( NULL );//suspend current task
	while (1)
	{
		//send data to remote computer via wireless RS232
		SendData();//takes care of semaphore internally
		vTaskSuspend( NULL );//suspend current task
	}
}

/* This task reads data from the servo motors */
void vServoReadTask(void * pvParameters ) {
	vTaskSuspend( NULL );//suspend current task
	while (1)
	{
		ReadServo();
		vTaskSuspend( NULL );//suspend current task
	}
}

/* This task writes commands to servo motors */
/*void vServoWriteTask(void * pvParameters ) {
	vTaskSuspend( NULL );//suspend current task
	while (1)
	{
		ReadServo();
		vTaskSuspend( NULL );//suspend current task
	}
}
*/


/* This task looks for waiting commands from UART and runs them */
void vRunCommandTask(void * pvParameters ) {

	while (1)
	{
		RunCommand();

		Board_LED_Set(0, false);

		vTaskDelay(configTICK_RATE_HZ/5);
	}
}

/* UART input thread */
void vUARTTask(void * pvParameters ) {
	int tickCnt = 0;
	while (1)
	{
		tickCnt = Board_UARTGetChar();
		if(tickCnt != EOF)
		{
			CommandGetInputChar((char)(tickCnt));
		}
	}

}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();

	activeSaveData = 0;

	vSemaphoreCreateBinary(dataSemaphore);
	vSemaphoreCreateBinary(servoSemaphore);

	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, true);

	/* UART input thread */
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				256, NULL, (tskIDLE_PRIORITY + 1UL),
				&vUARTTaskHandle);

	/* Run user commands thread */
	xTaskCreate(vRunCommandTask, (signed char *) "vCommand",
				256, NULL, (tskIDLE_PRIORITY + 2UL),
				&vRunCommandTaskHandle);

	// Send analog data back to computer via wireless RS232:  is immediately suspended
	xTaskCreate(vDataSendTask, (signed char *) "vDataSendTask",
				256, NULL, (tskIDLE_PRIORITY + 3UL),
				&vDataSendTaskHandle);

	// Read Stats from servo motors
	xTaskCreate(vServoReadTask, (signed char *) "vDataSendTask",
				configMINIMAL_STACK_SIZE, NULL,
				(tskIDLE_PRIORITY + 4UL),
				&vServoReadTaskHandle);

	// Read all analog & TC data:  is immediately suspended
	xTaskCreate(vDataReadTask, (signed char *) "vDataSendTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 5UL),
				&vDataReadTaskHandle);

	// Read analog data (save it to SD card):  is immediately suspended
	//vDataAquisitionTaskHandle=NULL;
	xTaskCreate(vDataAquisitionTask, (signed char *) "vDAQTask",
				configMINIMAL_STACK_SIZE+128, NULL, (tskIDLE_PRIORITY + 6UL),
				&vDataAquisitionTaskHandle);

	//task 7 is fire
	//task 8 is e-stop





	/* Start the scheduler */
	vTaskStartScheduler();

	while (1) {}


	/* Should never arrive here */
	return 1;
}





/**
 * @}
 */
