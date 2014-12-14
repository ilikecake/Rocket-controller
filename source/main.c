/*
 * main.c
 * @brief HPN (NE-1) Rocket project controller board *
 *  Created on: Sep 24, 2014
 *      Author: pat
 */
/** @defgroup EXAMPLES_FREERTOS_17XX40XX_BLINKY LPC17xx/40xx FrreRTOS blinky example
 */

#include "main.h"


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	Board_Init();

	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, false);

	InitPWM();


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
	{//USB power is plugged in
		Board_Xbee_Set(false);//disable Xbee if USB is plugged in
		usb_detect = 1;
		Board_LED_Set(1, true);
		Board_LED_Set(2, false);
		printf("USB detected, Xbee disabled\r\n");
	}
	else
	{//USB power is not plugged in
		Board_Xbee_Set(true);
		usb_detect = 0;
		//Board_LED_Set(0, true);
		Board_LED_Set(1, false);
		Board_LED_Set(2, true);
		printf("USB not detected, Xbee enabled\r\n");
	}

	Board_LED_Init();


	dataRate = 10;//Hz set default data aquisition rate

}

/* Emergency Stop thread */
void vEStopTask(void * pvParameters ) {
	uint8_t i;
	uint16_t finalState;

	while (1)
	{
		vTaskSuspend( NULL ); //suspend current task until triggered by emergency stop event

		if (runningControl==1 && redlinesEnabled==1)
		{//only proceed with emergency stop if the control sequence is running
			emergencyStop = 1;
			redlinesEnabled=0;//disable redlines to prevent redline from triggering again

			//Board_LED_Set(0, true);
			//Board_LED_Set(1, false);
			//Board_LED_Set(2, true);

			//Set all outputs to final state
			//Board_DO_Set((uint16_t) 0);//set all outputs to 0

			//make sure servos stay on
			finalState=DO_Command[commandFinal]  | (1<<DO_SERVO1) | (1<<DO_SERVO2);

			Board_DO_Set(finalState);//set all DO channel states at once
			vTaskDelay(5);
			PWM_Enable(0);//turn off spark

			//set servos to closed (try twice to make sure it works
			servoCommandFlag = 1;
			for(i=0;i<2;i++)
			{
				vTaskDelay(5);
				SetServoPosition(0, Servo_Command[commandFinal][0]);//N2O Valve
				vTaskDelay(50);
				SetServoPosition(1, Servo_Command[commandFinal][1]);//Fuel Valve
				vTaskDelay(100);
			}

			servoCommandFlag = 0;


			Board_LED_Set(0, true);
			//Board_LED_Set(1, true);
			//Board_LED_Set(2, true);

		}

	}
}


/* Run test sequence */
void vFireControlTask(void * pvParameters ) {
	portTickType tickTime;
	portTickType interval;
	uint8_t exitCondition; //0 if sequence loop should be exited

	while (1)
	{
		vTaskSuspend( NULL ); //suspend current task until triggered by emergency stop event

		Board_LED_Set(0, false);
		Board_LED_Set(1, false);
		Board_LED_Set(2, false);

		//set initial DO states
		PWM_Enable(0);//set spark to initial state
		PWM_SetFrequency(50);

		//Ensure that a valid control sequence has been uploaded
		redlinesEnabled = 1;//set flag to show that redlines are active and should be checked when reading data
		emergencyStop = 0;//the emergency stop has not been triggered
		redlineNumber=0xFE;//reset null redline number value

		runningControl = 1;//show that control sequence is running

		fireStartTime = xTaskGetTickCount(); //store the time at which the control sequence was started
		runningData = 1;//show that data is being read from sensors
		vTaskResume(vDataAquisitionTaskHandle);//make sure data is being acquired

		activeSaveData=1; //begin saving data
		commandNum = 0;//reset sequence to start at command 0
		servoCommandFlag = 0;  //allow servo to be queried for position

		exitCondition=1;
		while ((commandNum <= commandFinal) && (exitCondition==1) && emergencyStop == 0) {

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

			if (emergencyStop==0)
			{
				//set digital outputs
				Board_DO_Set(DO_Command[commandNum]);//set all DO channel states at once
				//printf("Set DO(%u): %u\r\n",commandNum, DO_Command[commandNum]);
				vTaskDelay(5);
				if (Spark_Command[commandNum]==1)
				{
					PWM_Enable(1);//set spark state
				}
				else
				{
					PWM_Enable(0);//set spark state
				}

				//printf("Set spark(%u): %u\r\n",commandNum, Spark_Command[commandNum]);

				servoCommandFlag = 1;
				vTaskDelay(5);
				//send position commands to servos.  this will take a while
				SetServoPosition(0, Servo_Command[commandNum][0]);//N2O Valve
				vTaskDelay(5);
				SetServoPosition(1, Servo_Command[commandNum][1]);//Fuel Valve
				servoCommandFlag = 0;

				commandNum++;
			}

			//exit loop if the next command has an invalid time
			if ((commandNum>0) && (commandNum<=commandFinal))
			{
				if (commandTime[commandNum] < commandTime[commandNum-1])
				{
					exitCondition = 0;
				}
			}
		}

		Board_LED_Set(1, true);

		runningControl = 0;
		redlinesEnabled = 0;

		//wait for a few seconds.  Continue recording data for 5 seconds during shutdown.
		vTaskDelay(configTICK_RATE_HZ * 5);
		activeSaveData = 0;//stop recording data

	}

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


/* This task looks for waiting commands from UART and runs them */
void vRunCommandTask(void * pvParameters ) {

	while (1)
	{
		RunCommand();


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

//	Board_LED_Set(0, false);
//	Board_LED_Set(1, false);
//	Board_LED_Set(2, true);

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

	// Read Stats from servo motors: is immediately suspended
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

	// Control Digital Outputs and Servos during run time
	xTaskCreate(vFireControlTask, (signed char *) "vFireControlTask",
				256, NULL, (tskIDLE_PRIORITY + 7UL),
				&vFireControlTaskHandle);

	// setup Emergency Stop task
	xTaskCreate(vEStopTask, (signed char *) "vEStopTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 8UL),
				&vEStopTaskHandle);

	/* Start the scheduler */
	vTaskStartScheduler();


	while (1) {}

	/* Should never arrive here */
	return 1;
}





/**
 * @}
 */
