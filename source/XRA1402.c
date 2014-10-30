/*
 * XRA1402.c
 *
 *  Created on: Oct 26, 2014
 *      Author: Jon
 */

#include "main.h"




void XRA1402WriteReg(uint8_t RegisterToWrite, uint8_t RegisterValue)
{
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;


	/*
	//Set up the SSP format for the XRA1402, SPI mode 10
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA0_CPOL1;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	//Chip_SSP_Enable(LPC_SSP1);
	xf_setup.length = 16;

	xf_setup.tx_data = NULL;
	xf_setup.rx_data = ADInputData;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;


	AD7606_Select(1,1);

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	AD7606_Select(1,0);*/

	return;
}


void XRA1402ReadReg(uint8_t RegisterToRead, uint8_t *RegVal);



void XRA1402Init(void)
{

	//Set up the chip select line for the XRA1402
	Chip_IOCON_PinMux(LPC_IOCON, XRA1402_CS_PORT, CRA1402_CS_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, XRA1402_CS_PORT, CRA1402_CS_PIN, true);
	Chip_GPIO_WritePortBit(LPC_GPIO, XRA1402_CS_PORT, CRA1402_CS_PIN, true);







	return;
}

