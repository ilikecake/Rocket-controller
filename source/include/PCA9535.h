/*
 * PCA9535.h
 *
 *  Created on: Nov 4, 2014
 *      Author: Jon
 */

#ifndef PCA9535_H_
#define PCA9535_H_

#define PCA9535_ADDRESS		0x20
#define PCA9535_I2C_PORT	I2C1

#define PCA9535_CMD_INPUT_PORT_0	0
#define PCA9535_CMD_INPUT_PORT_1	1
#define PCA9535_CMD_OUTPUT_PORT_0	2
#define PCA9535_CMD_OUTPUT_PORT_1	3
#define PCA9535_CMD_POL_INV_0		4
#define PCA9535_CMD_POL_INV_1		5
#define PCA9535_CMD_CONFIG_0		6
#define PCA9535_CMD_CONFIG_1		7

void PCA9535_Init(void);

void PCA9535_WriteReg(uint8_t RegToWrite, uint8_t BytesToWrite, uint8_t *DataBuffer);

void PCA9535_ReadReg(uint8_t RegToRead, uint8_t BytesToRead, uint8_t *ReadBuffer);

void PCA9535_SetOutputs(uint8_t *OutputStates);

void PCA9535_SetOutput(uint8_t OutputNumber, uint8_t OutputState);

#endif /* PCA9535_H_ */
