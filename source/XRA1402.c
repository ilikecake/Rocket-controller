/*
 * XRA1402.c
 *
 *  Created on: Oct 26, 2014
 *      Author: Jon
 */

#include "main.h"



void XRA1402Init(void)
{
	//Set up the chip select line for the XRA1402
	Chip_IOCON_PinMux(LPC_IOCON, XRA1402_CS_PORT, XRA1402_CS_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, XRA1402_CS_PORT, XRA1402_CS_PIN, true);
	Chip_GPIO_WritePortBit(LPC_GPIO, XRA1402_CS_PORT, XRA1402_CS_PIN, true);

	//Setup all pins as outputs and put them all high
	XRA1402WriteReg(XRA1402_REG_GCR, 0x00);
	XRA1402WriteReg(XRA1402_REG_OCR, 0xFF);
	return;
}

void XRA1402Select(uint8_t sel)
{
	if(sel == 1)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, XRA1402_CS_PORT, XRA1402_CS_PIN, false);
	}
	else
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, XRA1402_CS_PORT, XRA1402_CS_PIN, true);
	}
	return;
}

void XRA1402WriteReg(uint8_t RegisterToWrite, uint8_t RegisterValue)
{
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;
	uint8_t DataToSend[2];

	//Set up the SSP mode 00
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	DataToSend[0] = (RegisterToWrite) & 0x7E;
	DataToSend[1] = RegisterValue;

	xf_setup.length = 2;
	xf_setup.tx_data = DataToSend;
	xf_setup.rx_data = NULL;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	//Select
	XRA1402Select(1);

	//Write data to XRA1402
	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	//Deselect
	XRA1402Select(0);
	return;
}

void XRA1402ReadReg(uint8_t RegisterToRead, uint8_t *RegVal)
{
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;
	uint8_t DataToSend[2];
	uint8_t DataRXBuffer[2];	//TODO: This is dumb, remove this later

	//Set up the SSP format mode 00
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	DataToSend[0] = ((RegisterToRead) & 0x7E) | 0x80;
	DataToSend[1] = 0x00;

	DataRXBuffer[0] = 0xFA;
	DataRXBuffer[1] = 0xAF;

	printf("TX: 0x%02X, 0x%02X\r\n", DataToSend[0], DataToSend[1]);

	xf_setup.length = 2;
	xf_setup.tx_data = DataToSend;
	xf_setup.rx_data = DataRXBuffer;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	//Select
	XRA1402Select(1);

	//Read/write data to XRA1402
	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	//Deselect
	XRA1402Select(0);

	printf("RX: 0x%02X, 0x%02X\r\n", DataRXBuffer[0], DataRXBuffer[1]);

	RegVal = &DataRXBuffer[1];
	return;
}

void XRA1402SetupOutput(uint8_t BitsToSet)
{
	uint8_t RegVal;

	//XRA1402ReadReg(XRA1402_REG_GCR, &RegVal);

	//BitsToSet

	return;
}
