/*
 * i2c.c
 *
 *  Created on: Oct 3, 2013
 *      Author: pat
 */

/*
 * @brief I2C example
 * This example show how to use the I2C interface
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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static int mode_poll;   				/* Poll/Interrupt mode flag */
static I2C_ID_T i2cDev = DEFAULT_I2C; 	/* Currently active I2C device */

/* EEPROM SLAVE data */
//#define I2C_SLAVE_EEPROM_SIZE       64
//#define I2C_SLAVE_EEPROM_ADDR       0x5A
//#define I2C_SLAVE_IOX_ADDR          0x5B

/* Xfer structure for slave operations */
//static I2C_XFER_T seep_xfer;
//static I2C_XFER_T iox_xfer;

/* Data area for slave operations */
//static uint8_t seep_data[I2C_SLAVE_EEPROM_SIZE + 1];
//static uint8_t buffer[2][256];
//static uint8_t iox_data[2]; /* PORT0 input port, PORT1 Output port */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	} else {
		Chip_I2C_SlaveStateHandler(id);
	}
}

/*static void i2c_rw_input(I2C_XFER_T *xfer, int ops)
{
	int tmp, i;

	tmp = con_get_input("Enter 7-Bit Slave address : ");
	tmp &= 0xFF;
	xfer->slaveAddr = tmp;
	xfer->rxBuff = 0;
	xfer->txBuff = 0;
	xfer->txSz = 0;
	xfer->rxSz = 0;

	if (ops & 1) {
		tmp = con_get_input("Enter number of bytes to read : ");
		tmp &= 0xFF;
		xfer->rxSz = tmp;
		xfer->rxBuff = buffer[1];
	}

	if (ops & 2) {
		tmp = con_get_input("Enter number of bytes to write : ");
		tmp &= 0xFF;
		for (i = 0; i < tmp; i++) {
			DEBUGOUT("%d:", i + 1);
			buffer[0][i] = con_get_input("Enter Data: ");
		}
		xfer->txSz = tmp;
		xfer->txBuff = buffer[0];
	}
}*/

/* Set I2C mode to polling/interrupt */
void i2c_set_mode(I2C_ID_T id, int polling)
{
	if(!polling) {
		mode_poll &= ~(1 << id);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
		NVIC_EnableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
	} else {
		mode_poll |= 1 << id;
		NVIC_DisableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
	}
}

/* Initialize the I2C bus */
void i2c_app_init(I2C_ID_T id, int speed)
{
	Board_I2C_Init(id);

	/* Initialize I2C */
	Chip_I2C_Init(id);
	Chip_I2C_SetClockRate(id, speed);

	/* Set default mode to interrupt */
	i2c_set_mode(id, 0);
}

/* Function that probes all available slaves connected to an I2C bus */
/*static void i2c_probe_slaves(I2C_ID_T i2c)
{
	int i;
	uint8_t ch[2];

	DEBUGOUT("Probing available I2C devices...\r\n");
	DEBUGOUT("\r\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	DEBUGOUT("\r\n====================================================");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F)) DEBUGOUT("\r\n%02X  ", i >> 4);
		if (i <= 7 || i > 0x78) {
			DEBUGOUT("   ");
			continue;
		}
		// Address 0x48 points to LM75AIM device which needs 2 bytes be read
		if(Chip_I2C_MasterRead(i2c, i, ch, 1 + (i == 0x48)) > 0)
			DEBUGOUT(" %02X", i);
		else
			DEBUGOUT(" --");
	}
	DEBUGOUT("\r\n");
}*/

/*static int i2c_menu(void)
{
	DEBUGOUT(menu, i2cDev, (mode_poll & (1 << i2cDev)) ? "POLLING" : "INTERRUPT");
	return con_get_input("\r\nSelect an option [0 - 6] :");
}*/


/*****************************************************************************
 * Public functions
 ****************************************************************************/


/**
 * @brief	I2C Interrupt Handler
 * @return	None
 */
void I2C1_IRQHandler(void)
{
	i2c_state_handling(I2C1);
}

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}



/**
 * @}
 */
