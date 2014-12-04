/*
 * @brief NXP LPC1769 Xpresso board file
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

#include "board.h"
#include "string.h"

#include "lpc_phy_smsc87x0.c"
#include "retarget.c"

/** @ingroup BOARD_NXP_XPRESSO_1769
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define BUTTONS_BUTTON1_GPIO_PORT_NUM           2
#define BUTTONS_BUTTON1_GPIO_BIT_NUM            10
#define JOYSTICK_UP_GPIO_PORT_NUM               2
#define JOYSTICK_UP_GPIO_BIT_NUM                3
#define JOYSTICK_DOWN_GPIO_PORT_NUM             0
#define JOYSTICK_DOWN_GPIO_BIT_NUM              15
#define JOYSTICK_LEFT_GPIO_PORT_NUM             2
#define JOYSTICK_LEFT_GPIO_BIT_NUM              4
#define JOYSTICK_RIGHT_GPIO_PORT_NUM            0
#define JOYSTICK_RIGHT_GPIO_BIT_NUM             16
#define JOYSTICK_PRESS_GPIO_PORT_NUM            0
#define JOYSTICK_PRESS_GPIO_BIT_NUM             17


//#define LED3_GPIO_PORT_NUM                      1
//#define LED3_GPIO_BIT_NUM                       29

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*!< System Clock Frequency (Core Clock) */
uint32_t SystemCoreClock;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initializes board LED(s) */
void Board_LED_Init(void)
{
	/* Pin PIO0_22 is configured as GPIO pin during SystemInit */
	/* Set the PIO_22 as output */
	Chip_GPIO_WriteDirBit(LPC_GPIO, LED0_GPIO_PORT_NUM, LED0_GPIO_BIT_NUM, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, LED1_GPIO_PORT_NUM, LED1_GPIO_BIT_NUM, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, LED2_GPIO_PORT_NUM, LED2_GPIO_BIT_NUM, true);

	//LEDs start off (high)
	Chip_GPIO_WritePortBit(LPC_GPIO, LED0_GPIO_PORT_NUM, LED0_GPIO_BIT_NUM, false);
	Chip_GPIO_WritePortBit(LPC_GPIO, LED1_GPIO_PORT_NUM, LED1_GPIO_BIT_NUM, false);
	Chip_GPIO_WritePortBit(LPC_GPIO, LED2_GPIO_PORT_NUM, LED2_GPIO_BIT_NUM, false);

}

/* Sets the state of a board LED to on or off
 * Note: a high level on the pin turns off the LED
 * Note: LED numbers start at 1						*/
void Board_DO_Set(uint16_t DOstates)
{
	uint8_t states[1];

	states[0] = (uint8_t) (DOstates>>8);
	states[1] = (uint8_t) (DOstates & 0xFF);
	PCA9535_SetOutputs(states);

	return;
}

void Board_Xbee_Set(bool state)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, XBEE_GPIO_PORT_NUM, XBEE_GPIO_BIT_NUM, state);
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

//send a 32 bit number over UART
void sendSerialUInt32(uint32_t msg, LPC_USART_T *pUART){
	sendSerialUint8(  (uint8_t)(msg>>24), pUART);//send MSB
	sendSerialUint8(  (uint8_t)(msg>>16), pUART);
	sendSerialUint8(  (uint8_t)(msg>>8) , pUART);
	sendSerialUint8(  (uint8_t)(msg) , pUART);//send LSB
}

//send a 16 bit number over UART
void sendSerialUInt16(uint16_t msg, LPC_USART_T *pUART){
	sendSerialUint8(  (uint8_t)(msg>>8) , pUART);//send MSB
	sendSerialUint8(  (uint8_t)(msg) , pUART);//send LSB

	//IP_UART_SendByte( channel, msg>>8);//send MSB
	//IP_UART_SendByte( channel, msg && 0xFF);//send LSB

}

//send character over UART
void sendSerialUint8(uint8_t msg, LPC_USART_T *pUART) {
	//send a single character over serial
	//do not send a new line using this function

	//while(!(LPC_UART0->LSR & 0x20));	// Block until tx empty
	//LPC_UART0->THR = msg;
	while (Chip_UART_SendByte(pUART, msg) == ERROR) {}//IP_UART_SendByte should also work

}

//send NewLine over UART
void sendSerialNewline(char num, LPC_USART_T *pUART){
    signed char i;

    for (i = 0; i < num; i++){
    	sendSerialUint8(0x0A, pUART);
    	sendSerialUint8(0x0D, pUART);
    }

}

/* Update system core clock rate, should be called if the system has
   a clock rate change */
void SystemCoreClockUpdate(void)
{
	/* CPU core speed */
	SystemCoreClock = Chip_Clock_GetSystemClockRate();
}

/* Initialize UART pins */
void Board_UART_Init(LPC_USART_T *pUART)
{
	/* Pin Muxing has already been done during SystemInit */
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_ENABLE)
	Board_UART_Init(DEBUG_UART);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 230400);// 115200);//
	Chip_UART_ConfigData(DEBUG_UART, UART_DATABIT_8, UART_PARITY_NONE, UART_STOPBIT_1);

	/* Enable UART Transmit */
	Chip_UART_TxCmd(DEBUG_UART, ENABLE);
#endif
}


/*******************************************************************************
* Function Name : UART_RTSConfig
* Description : Sets or Resets the RTS signal
* Input : - UARTx: where x can be 0,1 or 2 to select the UART peripheral.
* - RTSState: new state of the RTS signal.
* This parameter can be: RTSSET or RTSRESET
* Output : None
* Return : None
*******************************************************************************/
#define U1MCR (uint32_t)0x40010010; //UART1 Modem Control Register
void UART_RTSConfig( LPC_USART_T *pUART, uint8_t RTSState)//LPC_USART_T
{
	/*
	if(RTSState == 1)
	{
	UARTx->CR |= UART_MCR_RTS_CTRL; //UART_INTSTS_RTS;
	}
	else if(RTSState == RTSSET)
	{
	UARTx->CR &= ~UART_MCR_RTS_CTRL; //UART_INTSTS_RTS;
	*/
}


/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_ENABLE)
	while (Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch) == ERROR) {}
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_ENABLE)
	uint8_t data;

	if (Chip_UART_ReceiveByte(DEBUG_UART, &data) == SUCCESS) {
		return (int) data;
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_ENABLE)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Sets the state of a board LED to on or off
 * Note: a high level on the pin turns off the LED
 * Note: LED numbers start at 1						*/
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	//On = !On;
	if (LEDNumber == 0)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, LED0_GPIO_PORT_NUM, LED0_GPIO_BIT_NUM, On);
	}
	else if(LEDNumber == 1)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, LED1_GPIO_PORT_NUM, LED1_GPIO_BIT_NUM, On);
	}
	else if(LEDNumber == 2)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, LED2_GPIO_PORT_NUM, LED2_GPIO_BIT_NUM, On);
	}

}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO, LED0_GPIO_PORT_NUM, LED0_GPIO_BIT_NUM);
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	//uint32_t temp;

	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Updates SystemCoreClock global var with current clock speed */
	SystemCoreClockUpdate();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);

	//Initialize Xbee radio enable pin
	Chip_GPIO_WriteDirBit(LPC_GPIO, XBEE_GPIO_PORT_NUM, XBEE_GPIO_BIT_NUM, true);

	//initialize USB detect pin
	Chip_GPIO_WriteDirBit(LPC_GPIO, USB_GPIO_PORT_NUM, USB_GPIO_BIT_NUM, false); //pin26
	Chip_IOCON_PinMux(LPC_IOCON, USB_GPIO_PORT_NUM, USB_GPIO_BIT_NUM, IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	//InitPWM();


	/* Initialize LEDs */
	Board_LED_Init();
}


/* Initialize pin muxing for SSP interface */
void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		/* Set up clock and muxing for SSP1 interface */
		/*
		 * Initialize SSP0 pins connect
		 * P0.7: SCK
		 * P0.6: SSEL
		 * P0.8: MISO
		 * P0.9: MOSI
		 */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_INACT, IOCON_FUNC2);
		//Chip_IOCON_PinMux(LPC_IOCON, 0, 6, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 8, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC2);
	}
	else {
		/* Set up clock and muxing for SSP0 interface */
		/*
		 * Initialize SSP0 pins connect
		 * P1.20: SCK
		 * P1.21: SSEL
		 * P1.23: MISO
		 * P1.24: MOSI
		 */
		Chip_IOCON_PinMux(LPC_IOCON, 1, 20, IOCON_MODE_INACT, IOCON_FUNC2);
		//Chip_IOCON_PinMux(LPC_IOCON, 1, 21, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 1, 23, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 1, 24, IOCON_MODE_INACT, IOCON_FUNC2);
	}
}

/* Initialize pin muxing for SPI interface */
void Board_SPI_Init(bool isMaster)
{
	/* Set up clock and muxing for SSP0 interface */
	/*
	 * Initialize SSP0 pins connect
	 * P0.15: SCK
	 * P0.16: SSEL
	 * P0.17: MISO
	 * P0.18: MOSI
	 */
	Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_PULLDOWN, IOCON_FUNC3);
	if (isMaster) {
		Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_PULLUP, IOCON_FUNC0);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 16, true);
		Board_SPI_DeassertSSEL();

	}
	else {
		Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_PULLUP, IOCON_FUNC3);
	}
	Chip_IOCON_PinMux(LPC_IOCON, 0, 17, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 18, IOCON_MODE_INACT, IOCON_FUNC3);
}

/* Assert SSEL pin */
void Board_SPI_AssertSSEL(void)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, false);
}

/* De-Assert SSEL pin */
void Board_SPI_DeassertSSEL(void)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, true);
}


/* Sets up board specific I2C interface */
void Board_I2C_Init(I2C_ID_T id)
{
	switch (id) {
	case I2C0:
		Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);
		Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
		break;

	case I2C1:
		//TODO: These are not always the right pins
		Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC3);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC3);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);

		//Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC3);
		//Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC3);
		//Chip_IOCON_EnableOD(LPC_IOCON, 0, 0);
		//Chip_IOCON_EnableOD(LPC_IOCON, 0, 1);

		break;

	case I2C2:
		Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 10);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 11);
		break;
	}

	return;
}




/**
 * @}
 */
