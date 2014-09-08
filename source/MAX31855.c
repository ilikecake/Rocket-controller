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
*	\brief		Hardware driver for the MAX31855 Thermocouple converter
*	\author		Jonathan McCabe, Pat Satyshur
*	\version	1.0
*	\date		12/01/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

/*#include "MAX31855.h"
#include "avr/io.h"
#include "spi.h"*/

#include "main.h"

void MAX31855Init(void)
{
	
	//Set up pins
	Chip_IOCON_PinMux(LPC_IOCON, MAX31855_CS_PORT, MAX31855_CS1_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, MAX31855_CS_PORT, MAX31855_CS2_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, MAX31855_CS_PORT, MAX31855_CS3_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, MAX31855_CS_PORT, MAX31855_CS4_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	//Setup pins as output
	Chip_GPIO_WriteDirBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS1_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS2_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS3_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS4_PIN, true);

	//Set these pins high
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS1_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS2_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS3_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS4_PIN, true);				//chip is deselected

	return;
}


//sel = 1 to select the chip
void MAX31855Select( uint8_t sel )
{

	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS1_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS2_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS3_PIN, true);				//chip is deselected
	Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS4_PIN, true);				//chip is deselected

	switch (sel)
		{
	case 0:
		Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS1_PIN, false);	//chip is selected
		break;
	case 1:
		Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS2_PIN, false);	//chip is selected
		break;
	case 2:
		Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS3_PIN, false);	//chip is selected
		break;
	case 3:
		Chip_GPIO_WritePortBit(LPC_GPIO, MAX31855_CS_PORT, MAX31855_CS4_PIN, false);	//chip is selected
		break;
		
		}


	
	return;
}

uint32_t MAX31855read( uint8_t sel, uint16_t *coldJunction )
{
	//return temperature. 0 is at 0deg C.  Each bit is .25 deg C
	uint8_t ReceiveBuffer[4];
	uint32_t temperature;
	uint8_t err;
	
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	ReceiveBuffer[0]=0;
	ReceiveBuffer[1]=0;
	ReceiveBuffer[2]=0;
	ReceiveBuffer[3]=0;
	
	MAX31855Select (sel);

	xf_setup.length = 4;
	xf_setup.tx_data = NULL;
	xf_setup.rx_data = ReceiveBuffer;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	MAX31855Select (0xFF);//deselect all TC chips
	

	temperature = ((uint32_t)ReceiveBuffer[0] << 6) | ((uint32_t)ReceiveBuffer[1] >> 2);//.25deg C per bit starting at 0C
	*coldJunction = ((uint32_t)ReceiveBuffer[2]<<4) | ((uint32_t)ReceiveBuffer[3]>>4);//.0625deg C per bit starting at 0C
	
	printf("%u %u %u %u\r\n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],ReceiveBuffer[3]);
	err = (ReceiveBuffer[3] & 0x1);

	if (err == 1)
	{
		temperature = 0;//0xFFFFFFFF;
	}
	
	return temperature;
}
