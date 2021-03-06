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

#ifndef __BOARD_NXP_XPRESSO_1769_H_
#define __BOARD_NXP_XPRESSO_1769_H_

#include "chip.h"
#include "board_api.h"
#include "lpc_phy.h"
#include "pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_NXP_XPRESSO_1769 NXP LPC1769 Xpresso board support functions
 * @ingroup BOARDS_17XX_40XX
 * @{
 */

/** @defgroup BOARD_NXP_XPRESSO_1769_OPTIONS BOARD: NXP LPC1769 Xpresso board build options
 * This board has options that configure its operation at build-time.<br>
 *
 * For more information on driver options see<br>
 * @ref LPCOPEN_DESIGN_ARPPROACH<br>
 * @{
 */

/**
 * @}
 */

#define BOARD_NXP_XPRESSO_1769	/*!< Build for Xpresso LPC1769 board */

#define LED0_GPIO_PORT_NUM				1
#define LED0_GPIO_BIT_NUM				9
#define LED1_GPIO_PORT_NUM				1
#define LED1_GPIO_BIT_NUM				10
#define LED2_GPIO_PORT_NUM				1
#define LED2_GPIO_BIT_NUM				14

#define XBEE_GPIO_PORT_NUM				1
#define XBEE_GPIO_BIT_NUM				31
#define USB_GPIO_PORT_NUM				3
#define USB_GPIO_BIT_NUM				26

//#define LPC_PCONP						0x400FC0C4	//address of power control for peripherals
//#define LPC_PCONP_PCPWM1				0x40 //mask bit for PWM power control bit
//#define LCP_PCLKSEL0					0x400FC1A8	//Peripheral Clock Selection registers 0
//#define LCP_PCLKSEL0_PCLK_PWM1			0x1800 //mask bits for Peripheral clock selection for PWM1
/*
#define LER0_EN			1 << 0
#define LER1_EN			1 << 1
#define LER2_EN			1 << 2
#define LER3_EN			1 << 3
#define LER4_EN			1 << 4
#define LER5_EN			1 << 5
#define LER6_EN			1 << 6
#define PWMENA1			1 << 9
#define PWMENA2			1 << 10
#define PWMENA3			1 << 11
#define PWMENA4			1 << 12
#define PWMENA5			1 << 13
#define PWMENA6			1 << 14
#define TCR_CNT_EN		0x00000001
#define TCR_RESET		0x00000002
#define TCR_PWM_EN		0x00000008
*/




#define TOTAL_DO_CHANNELS				16
#define TOTAL_SERVO_CHANNELS			2
#define AI_CHIPS						2
#define AI_CHANNELS_PER_CHIP			8
#define TC_CHIPS						8
#define TC_CHANNELS_PER_CHIP			1

/**
 * LED defines
 */
//#define LEDS_LED1           0x01
//#define LEDS_LED2           0x02
//#define LEDS_LED3           0x04
//#define LEDS_LED4           0x08
//#define LEDS_NO_LEDS        0x00


void sendSerialUint8(uint8_t msg, LPC_USART_T *pUART);
void sendSerialUInt16(uint16_t msg, LPC_USART_T *pUART);
void sendSerialUInt32(uint32_t msg, LPC_USART_T *pUART);
void sendSerialNewline(char num, LPC_USART_T *pUART);

void UART_RTSConfig(LPC_USART_T *pUART, uint8_t RTSState);

void Board_DO_Set(uint16_t DOstates);

void Board_Xbee_Set(bool state);

void Board_LED_Init(void);

void InitPWM(void);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 * @note    Returns the MAC address used by Ethernet
 */
//void Board_ENET_GetMacADDR(uint8_t *mcaddr);


/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Initialize pin muxing for SPI interface
 * @param	isMaster	: true for master mode, false for slave mode
 * @return	Nothing
 */
void Board_SPI_Init(bool isMaster);

/**
 * @brief	Assert SSEL pin
 * @return	Nothing
 */
void Board_SPI_AssertSSEL(void);

/**
 * @brief	De-assert SSEL pin
 * @return	Nothing
 */
void Board_SPI_DeassertSSEL(void);

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: ID of I2C peripheral
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_FAST_MODE_PLUS);
}

/**
 * @brief	Disables I2C Fast plus mode and enable normal mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
}


#ifdef __cplusplus
}
#endif

#endif /* __BOARD_NXP_XPRESSO_1769_H_ */
