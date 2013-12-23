/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		AD5666 DAC Software
*	\author		Pat Satyshur
*	\version	1.0
*	\date		9/8/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	Hardware Drivers
*
*	@{
*/

#include "board.h"
#include "AD5666.h"

/** Initalize the AD5666 DAC
 * Setup the CS, update, and clear pins as GPIO
 *
 *
 * NOTE: The pins for the SPI interface are not set up here. They need to be setup before this function is called.
 */
uint8_t AD5666Init(void)
{
	uint8_t SendBuffer[4];

	//Set up pins
	Chip_IOCON_PinMux(LPC_IOCON, AD5666_CS_PORT, AD5666_CS_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD5666_UPDATE_PORT, AD5666_UPDATE_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD5666_CLEAR_PORT, AD5666_CLEAR_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	//Setup pins as output
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD5666_CS_PORT, AD5666_CS_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD5666_UPDATE_PORT, AD5666_UPDATE_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD5666_CLEAR_PORT, AD5666_CLEAR_PIN, true);

	//Set these pins high
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_CS_PORT, AD5666_CS_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_UPDATE_PORT, AD5666_UPDATE_PIN, true);		//update occurs on a falling edge
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_CLEAR_PORT, AD5666_CLEAR_PIN, true);		//clear occurs on a falling edge

	AD5666Clear(0);
	AD5666Update(0);

	//Turn on internal reference
	SendBuffer[0] = AD5666_CMD_SETUP_DCEN_REF;
	SendBuffer[1] = 0x00;
	SendBuffer[2] = 0x00;
	SendBuffer[3] = AD5666_REF_ON | AD5666_MODE_STANDALONE;
	AD5666Select(1);
	AD5666SendCommand(SendBuffer, NULL, 4, 0);
	AD5666Select(0);

	//Send a dummy byte to delay the device
	//TODO: add a real delay function here...
	AD5666SendCommand(SendBuffer, NULL, 1, 0);

	//vTaskDelay(1);

	//Power up all channels
	SendBuffer[0] = AD5666_CMD_POWER_MODE;
	SendBuffer[1] = 0x00;
	SendBuffer[2] = AD5666_POWER_UP;
	SendBuffer[3] = 0x0F;
	AD5666Select(1);
	AD5666SendCommand(SendBuffer, NULL, 4, 0);
	AD5666Select(0);

	//Clear the DAC outputs
	AD5666Clear(1);
	AD5666SendCommand(SendBuffer, NULL, 1, 0);	//TODO: Replace this with a real delay function
	AD5666Clear(0);

	Board_LED_Set(3, 1);
	return 0;
}

void AD5666Select(bool toSelect)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_CS_PORT, AD5666_CS_PIN, !toSelect);
	return;
}

void AD5666Update(bool toUpdate)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_UPDATE_PORT, AD5666_UPDATE_PIN, !toUpdate);
	return;
}

void AD5666Clear(bool toClear)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, AD5666_CLEAR_PORT, AD5666_CLEAR_PIN, !toClear);
	return;
}

uint8_t AD5666SendCommand(uint8_t *SendBuffer, uint8_t *ReceiveBuffer, uint8_t BytesToSend, uint8_t BytesToReceive)
{
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	//Chip_SSP_Enable(LPC_SSP1);

	if(BytesToSend > BytesToReceive)
	{
		xf_setup.length = BytesToSend;
	}
	else
	{
		xf_setup.length = BytesToReceive;
	}

	xf_setup.tx_data = SendBuffer;
	xf_setup.rx_data = ReceiveBuffer;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	return 0;
}

void AD5666SetVoltage(uint8_t ChannelNumber, uint16_t ValueToSet)
{
	uint8_t SendBuffer[4];

	/*Map the channel labels to the channels on the device
	 *Label	->	Hardware
	 *  1	->	  C
	 *  2	->	  A
	 *  3	->	  B
	 *  4	->	  D
	 *  5	->	 ALL
	 */

	if(ChannelNumber == 1)
	{
		SendBuffer[1] = AD5666_ADDR_DAC_C;
	}
	else if(ChannelNumber == 2)
	{
		SendBuffer[1] = AD5666_ADDR_DAC_A;
	}
	else if(ChannelNumber == 3)
	{
		SendBuffer[1] = AD5666_ADDR_DAC_B;
	}
	else if(ChannelNumber == 4)
	{
		SendBuffer[1] = AD5666_ADDR_DAC_D;
	}
	else if(ChannelNumber == 5)
	{
		SendBuffer[1] = AD5666_ADDR_ALL_DAC;
	}
	else
	{
		return;
	}

	//Send command to update DAC
	SendBuffer[0] = AD5666_CMD_WRITE_AND_UPDATE_REG;
	SendBuffer[1] |= ((ValueToSet >> 12)&0x0F);
	SendBuffer[2] = ((ValueToSet >> 4)&0xFF);
	SendBuffer[3] = ((ValueToSet << 4)&0xF0);
	AD5666Select(1);
	AD5666SendCommand(SendBuffer, NULL, 4, 0);
	AD5666Select(0);

	return;
}



/** @} */
