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


	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 17, true);
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 17, false);

	/* SSP initialization */
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_Enable(LPC_SSP1);

	//Initalize I2C
	i2c_app_init(I2C0, SPEED_100KHZ);
	/* Set default mode to interrupt */
	i2c_set_mode(I2C0, 0);


	AD5666Init();
	AD7606Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/* LED1 toggle thread */
static portTASK_FUNCTION(vLEDTask1, pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

/* LED2 toggle thread */
//static portTASK_FUNCTION(vLEDTask2, pvParameters) {
//	bool LedState = false;
//
//	while (1) {
//		Board_LED_Set(1, LedState);
//		LedState = (bool) !LedState;
//
//		/* About a 7Hz on/off toggle rate */
//		vTaskDelay(configTICK_RATE_HZ / 14);
//	}
//}

/* This task looks for waiting commands and runs them */
static portTASK_FUNCTION(vRunCommandTask, pvParameters) {

	while (1)
	{
		RunCommand();
		vTaskDelay(configTICK_RATE_HZ/5);

	}
}



/* UART (or output) thread */
static portTASK_FUNCTION(vUARTTask, pvParameters) {
	int tickCnt = 0;

	while (1)
	{
		tickCnt = Board_UARTGetChar();
		if(tickCnt != EOF)
		{
			CommandGetInputChar((char)(tickCnt));
			//Board_UARTPutChar((char)(tickCnt));
		}
	}

	//while (1) {
	//	DEBUGOUT("Tick: %d\r\n", tickCnt);
	//	tickCnt++;

		/* About a 1s delay here */
	//	vTaskDelay(configTICK_RATE_HZ);
	//}
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

	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, (signed char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 3UL),
				(xTaskHandle *) NULL);

	/* LED2 toggle thread */
	//xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
				//configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 3UL),
				//(xTaskHandle *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vRunCommandTask, (signed char *) "vTaskRunCommand",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/*vPortEnterCritical();

	Chip_IOCON_PinMux(LPC_IOCON, 4, 28, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 4, 28, true);

	while(1)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 17, true);
		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 17, false);
	}*/

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
