/*
 * PCA9535.c
 * I/O expander for digital outputs
 *  Created on: Nov 4, 2014
 *      Author: Jon
 */


#include "main.h"

void PCA9535_Init(void)
{
	//Different I2C pins are used for this chip
	//We probably need to set those up here...
	uint8_t RegData;

	//Set all output states to low
	//A zero in each bit sets the output state of that I/O pin to low
	RegData = 0x00;
	PCA9535_WriteReg(PCA9535_CMD_OUTPUT_PORT_0, 1, &RegData);
	PCA9535_WriteReg(PCA9535_CMD_OUTPUT_PORT_1, 1, &RegData);

	//Set all I/O as output
	//A zero in each bit sets the I/O pin to output mode
	RegData = 0x00;
	PCA9535_WriteReg(PCA9535_CMD_CONFIG_0, 1, &RegData);
	PCA9535_WriteReg(PCA9535_CMD_CONFIG_1, 1, &RegData);

	return;
}

//This function will only write one or two bytes
void PCA9535_WriteReg(uint8_t RegToWrite, uint8_t BytesToWrite, uint8_t *DataBuffer)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer[3];


	Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 0);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 1);

	//TXBuffer[0] = PCA9535_ADDRESS;
	TXBuffer[0] = RegToWrite;
	TXBuffer[1] = *DataBuffer;
	if(BytesToWrite == 2)
	{
		xfer.txSz = 3;
		DataBuffer++;
		TXBuffer[2] = *DataBuffer;
		//printf("TX Buff: 0x%02X, 0x%02X\r\n",TXBuffer[1], TXBuffer[2]);
	}
	else
	{
		xfer.txSz = 2;
	}

	xfer.slaveAddr = PCA9535_ADDRESS;
	xfer.txBuff = TXBuffer;
	xfer.rxBuff = NULL;
	xfer.rxSz = 0;

	Chip_I2C_MasterTransfer(PCA9535_I2C_PORT, &xfer);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);

	return;
}

//This function will only read one or two bytes
void PCA9535_ReadReg(uint8_t RegToRead, uint8_t BytesToRead, uint8_t *ReadBuffer)
{
	I2C_XFER_T xfer;
	uint8_t TXBuffer;

	Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 0);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 1);

	//TXBuffer[0] = PCA9535_ADDRESS;
	TXBuffer = RegToRead;
	xfer.txSz = 1;
	if(BytesToRead == 2)
	{
		xfer.rxSz = 2;
	}
	else
	{
		xfer.rxSz = 1;
	}

	xfer.slaveAddr = PCA9535_ADDRESS;
	xfer.txBuff = &TXBuffer;
	xfer.rxBuff = ReadBuffer;

	Chip_I2C_MasterTransfer(PCA9535_I2C_PORT, &xfer);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
	Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);

	return;
}

void PCA9535_GetOutputs(uint8_t *OutputStates)
{//get all digital outputs at once
	PCA9535_ReadReg(PCA9535_CMD_OUTPUT_PORT_0, 2, OutputStates);
	return ;
}

void PCA9535_SetOutputs(uint8_t *OutputStates)
{//set all digital outputs at once
	PCA9535_WriteReg(PCA9535_CMD_OUTPUT_PORT_0, 2, OutputStates);

	return;
}

//Change one output pin without affecting the state of the others
void PCA9535_SetOutput(uint8_t OutputNumber, uint8_t OutputState)
{
	uint8_t Reg;
	uint8_t RegData;

	//Determine which output register to modify
	if(OutputNumber < 8)
	{
		Reg = PCA9535_CMD_OUTPUT_PORT_0;
	}
	else
	{
		Reg = PCA9535_CMD_OUTPUT_PORT_1;
		OutputNumber = OutputNumber - 8;
	}

	//Get the current state of the outputs
	PCA9535_ReadReg(Reg, 1, &RegData);

	//Modify the state of the outputs
	if(OutputState == 1)
	{
		//Set the desired bit to one
		RegData |= (1<<OutputNumber);
	}
	else
	{
		//Set the desired bit to zero
		RegData &= ~(1<<OutputNumber);
	}

	//write the modified state back to the output register
	PCA9535_WriteReg(Reg, 1, &RegData);

	return;
}
