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



#include "main.h"


//sel = 0-7 to select a chip, anything else deselects all chips
void MAX31855Select( uint8_t sel )
{
	if(sel < 8)
	{
		XRA1402WriteReg(XRA1402_REG_OCR, ~(1<<sel));
	}
	else
	{
		XRA1402WriteReg(XRA1402_REG_OCR, 0xFF);
	}

	return;
}

uint16_t MAX31855read( uint8_t sel, uint16_t *coldJunction )
{
	//return temperature. 0 is at 0deg C.  Each bit is .25 deg C
	uint8_t ReceiveBuffer[4];
	uint16_t temperature;
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
	
	temperature = ((uint16_t)ReceiveBuffer[0] << 6) | ((uint16_t)ReceiveBuffer[1] >> 2);//.25deg C per bit starting at 0C
	*coldJunction = ((uint16_t)ReceiveBuffer[2]<<4) | ((uint16_t)ReceiveBuffer[3]>>4);//.0625deg C per bit starting at 0C


	//MAX318555 has temp start at zero then go up, but below zero it starts at 0x3FFF and count back down
	//shift positive temperatures to read  0x3FFF at 0C.
	if (temperature < 0x3FFF) //((temperature >> 13) != 1)
	{//temperature is greater than 0C
		temperature = temperature + 0x4000;//
	}

	//MAX318555 has coldJunction temp start at zero then go up, but below zero it starts at 0xFFF and count back down
	//shift positive temperatures to read  0xFFF at 0C.
	if (*coldJunction < 0xFFF) //((coldJunction >>11) != 1)
	{//coldJunction is greater than 0C
		*coldJunction = *coldJunction + 0x1000;//
	}


	//printf("%u %u %u %u\r\n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2],ReceiveBuffer[3]);
	err = (ReceiveBuffer[3] & 0x1);

	if (err == 1)
	{
		temperature = 0;//0xFFFFFFFF;
	}
	
	return temperature;
}
