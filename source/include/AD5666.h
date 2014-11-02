/*
 * AD5666.h
 *
 *  Created on: Sep 8, 2013
 *      Author: pat
 */

#ifndef AD5666_H_
#define AD5666_H_

#include <stdint.h>

#define AD5666_CS_PORT			2
#define AD5666_CS_PIN			8
#define AD5666_UPDATE_PORT		2
#define AD5666_UPDATE_PIN		7
#define AD5666_CLEAR_PORT		2
#define AD5666_CLEAR_PIN		6

#define AD5666_CMD_WRITE_REG				0x00
#define AD5666_CMD_UPDATE_REG				0x01
#define AD5666_CMD_WRITE_REG_UPDATE_ALL		0x02
#define AD5666_CMD_WRITE_AND_UPDATE_REG		0x03
#define AD5666_CMD_POWER_MODE				0x04
#define AD5666_CMD_LOAD_CLEAR_CODE			0x05
#define AD5666_CMD_LOAD_LDAC				0x06
#define AD5666_CMD_RESET					0x07
#define AD5666_CMD_SETUP_DCEN_REF			0x08

#define AD5666_ADDR_DAC_A		0x00
#define AD5666_ADDR_DAC_B		0x10
#define AD5666_ADDR_DAC_C		0x20
#define AD5666_ADDR_DAC_D		0x30
#define AD5666_ADDR_ALL_DAC		0xF0

#define AD5666_REF_ON			0x01
#define AD5666_REF_OFF			0x00
#define AD5666_MODE_STANDALONE	0x00
#define AD5666_MODE_REPEATER	0x02

#define AD5666_POWER_UP			0x00
#define AD5666_POWER_DOWN_1K	0x01
#define AD5666_POWER_DOWN_100K	0x02
#define AD5666_POWER_DOWN_HIZ	0x03

#define AD5666_CLEAR_CODE_LOW	0x00
#define AD5666_CLEAR_CODE_MID	0x01
#define AD5666_CLEAR_CODE_HIGH	0x02

uint8_t AD5666Init(void);

void AD5666Select(bool toSelect);
void AD5666Update(bool toUpdate);
void AD5666Clear(bool toClear);

uint8_t AD5666SendCommand(uint8_t *SendBuffer, uint8_t *ReceiveBuffer, uint8_t BytesToSend, uint8_t BytesToReceive);

void AD5666SetVoltage(uint8_t ChannelNumber, uint16_t ValueToSet);








#endif /* AD5666_H_ */
