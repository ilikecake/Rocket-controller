/*
 * TCA9554A.c
 *
 *  Created on: Oct 3, 2013
 *      Author: pat
 */

#include "main.h"

uint8_t TCA9554A_GetState(uint8_t sla)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer;
	uint8_t RXBuffer;

	TXBuffer = TCA9554A_CR_INPUT;

	xfer.slaveAddr = sla;
	xfer.txBuff = &TXBuffer;
	xfer.txSz = 1;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 1;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return RXBuffer;
}

void TCA9554A_SetState(uint8_t sla, uint8_t state)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer[2];
	uint8_t RXBuffer;

	TXBuffer[0] = TCA9554A_CR_OUTPUT;
	TXBuffer[1] = state;

	xfer.slaveAddr = sla;
	xfer.txBuff = TXBuffer;
	xfer.txSz = 2;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 0;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return;
}

void TCA9554A_SetStateBit(uint8_t sla, uint8_t BitNumber, uint8_t BitValue)
{
	uint8_t StateToSet;

	StateToSet = TCA9554A_GetState(sla);
	if(BitValue == 1)
	{
		StateToSet |= (1<<BitNumber);
	}
	else
	{
		StateToSet &= ~(1<<BitNumber);
	}
	TCA9554A_SetState(sla, StateToSet);
	return;
}

void TCA9554A_SetStateBits(uint8_t sla, uint8_t BitMask, uint8_t BitValues)
{
	uint8_t StateToSet;

	StateToSet = TCA9554A_GetState(sla);
	StateToSet &= ~BitMask;
	StateToSet |= BitValues;
	TCA9554A_SetState(sla, StateToSet);
	return;
}

uint8_t TCA9554A_GetConfig(uint8_t sla)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer;
	uint8_t RXBuffer;

	TXBuffer = TCA9554A_CR_CONFIG;

	xfer.slaveAddr = sla;
	xfer.txBuff = &TXBuffer;
	xfer.txSz = 1;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 1;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return RXBuffer;
}

void TCA9554A_SetConfig(uint8_t sla, uint8_t config)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer[2];
	uint8_t RXBuffer;

	TXBuffer[0] = TCA9554A_CR_CONFIG;
	TXBuffer[1] = config;

	xfer.slaveAddr = sla;
	xfer.txBuff = TXBuffer;
	xfer.txSz = 2;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 0;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return;
}

uint8_t TCA9554A_GetPol(uint8_t sla)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer;
	uint8_t RXBuffer;

	TXBuffer = TCA9554A_CR_POLARITY;

	xfer.slaveAddr = sla;
	xfer.txBuff = &TXBuffer;
	xfer.txSz = 1;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 1;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return RXBuffer;
}

void TCA9554A_SetPol(uint8_t sla, uint8_t polarity)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer[2];
	uint8_t RXBuffer;

	TXBuffer[0] = TCA9554A_CR_POLARITY;
	TXBuffer[1] = polarity;

	xfer.slaveAddr = sla;
	xfer.txBuff = TXBuffer;
	xfer.txSz = 2;
	xfer.rxBuff = &RXBuffer;
	xfer.rxSz = 0;

	Chip_I2C_MasterTransfer(TCA9554A_I2C_PORT, &xfer);

	return;
}
