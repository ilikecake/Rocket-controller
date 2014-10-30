/*
 * TCA9554A.h
 *
 *  Created on: Oct 3, 2013
 *      Author: pat
 */

#ifndef TCA9554A_H_
#define TCA9554A_H_

#define TCA9554A_I2C_PORT		I2C1

#define TCA9554A_CR_INPUT		0x00
#define TCA9554A_CR_OUTPUT		0x01
#define TCA9554A_CR_POLARITY	0x02
#define TCA9554A_CR_CONFIG		0x03

uint8_t TCA9554A_GetState(uint8_t sla);
void TCA9554A_SetState(uint8_t sla, uint8_t state);
void TCA9554A_SetStateBit(uint8_t sla, uint8_t BitNumber, uint8_t BitValue);
void TCA9554A_SetStateBits(uint8_t sla, uint8_t BitMask, uint8_t BitValues);

uint8_t TCA9554A_GetConfig(uint8_t sla);
void TCA9554A_SetConfig(uint8_t sla, uint8_t config);

uint8_t TCA9554A_GetPol(uint8_t sla);
void TCA9554A_SetPol(uint8_t sla, uint8_t polarity);

#endif /* TCA9554A_H_ */
