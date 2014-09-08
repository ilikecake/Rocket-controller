/* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
* \brief AT45DB321D dataflash driver.
* \author Pat Satyshur
* \version 1.0
* \date 2/17/2013
* \copyright Copyright 2013, Pat Satyshur
* \ingroup hardware
*
* @{
*/

#include "main.h"
#include "board.h"


void AT45DB321D_Init(void)
{
	AT45DB321D_Deselect();
	currentPage = AT45DB321D_FirstBlankPage();
	currentByte = 0;
		
	return;
}



void AT45DB321D_Select(void)
{
	/*
	//PORTD &= ~(1<<5);
	Chip_GPIO_WritePortBit(LPC_GPIO, AT45DB321D_CS_PORT, AT45DB321D_CS1_PIN, false);	//select the chip
	*/
	return;
}

void AT45DB321D_Deselect(void)
{
	/*
	//PORTD |= (1<<5);
	Chip_GPIO_WritePortBit(LPC_GPIO, AT45DB321D_CS_PORT, AT45DB321D_CS1_PIN, true);	//select the chip
*/
	return;
}

uint8_t AT45DB321DSendCommand(uint8_t *SendBuffer, uint8_t *ReceiveBuffer, uint8_t BytesToSend, uint8_t BytesToReceive)
{
	/*
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	//Chip_SSP_Enable(LPC_SSP1);
	AT45DB321D_Select();

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

	AT45DB321D_Deselect();
*/
	return 0;
}



uint8_t AT45DB321D_ReadStatus(void)
{

	uint8_t StatusByte=0;
	/*
	uint8_t SendBuffer[1];
	uint8_t ReceiveBuffer[1];
	SendBuffer[0] = AT45DB321D_CMD_READ_STATUS;
	ReceiveBuffer[0]=0;
	AT45DB321DSendCommand(&SendBuffer,&ReceiveBuffer,1,1 );
*/
	/*
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL0;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	AT45DB321D_Select();

	xf_setup.length = 1;
	xf_setup.tx_data = SendBuffer;
	xf_setup.rx_data = ReceiveBuffer;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	AT45DB321D_Deselect ();
	*/


	//StatusByte = ReceiveBuffer[0];
	return StatusByte;
}


//
void AT45DB321D_BufferRead(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataReadBuffer[], uint16_t BytesToRead)
{
/*
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}

	uint8_t SendBufferA[1];
	uint8_t ReceiveBuffer[1];

	if(Buffer == 1)
	{
		SendBufferA[0] = AT45DB321D_CMD_BUFFER1_READ_HS;
	}
	else
	{
		SendBufferA[0] = AT45DB321D_CMD_BUFFER2_READ_HS;
	}
	ReceiveBufferA[0]=0;
	AT45DB321DSendCommand(&SendBufferA,&ReceiveBuffer,1,1);


	//Send address to read
	//The address is 3 bytes, but only the 10 LSBs matter
	uint8_t SendBuffer[4];
	SendBuffer[0] = (BufferStartAddress & 0x0300)>>8;
	SendBuffer[1] = BufferStartAddress & 0xFF;
	SendBuffer[2] = 0x00;
	SendBuffer[2] = 0x00;//An extra byte needs to be clocked in to initalize the read

	AT45DB321DSendCommand(&SendBuffer,&DataReadBuffer,3,BytesToRead);
	// I may have to use separate send commands for each byte as it was originally done

 	 */

	/*
	uint16_t i;
	//TODO: add check for length and start address
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}

	AT45DB321D_Select();
	if(Buffer == 1)
	{
		SpiSend(AT45DB321D_CMD_BUFFER1_READ_HS);
	}
	else
	{
		SpiSend(AT45DB321D_CMD_BUFFER2_READ_HS);
	}

	//Send address to read
	//The address is 3 bytes, but only the 10 LSBs matter
	SpiSend(0x00);
	SpiSend((BufferStartAddress & 0x0300)>>8);
	SpiSend(BufferStartAddress & 0xFF);

	//An extra byte needs to be clocked in to initalize the read
	SpiSend(0x00);

	for(i = 0; i<BytesToRead; i++)
	{
		*DataReadBuffer = SpiSend(0x00);
		//printf_P(PSTR("[%u]: 0x%02X\n"), i, *DataReadBuffer);
		DataReadBuffer++;
	}
	AT45DB321D_Deselect();
*/

	return;
}

void AT45DB321D_BufferWrite(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataWriteBuffer[], uint16_t BytesToWrite)
{
	/*
	uint16_t i;
	
	//No funny stuff...
	//TODO: add check for length and start address
	if( (Buffer != 1) && (Buffer != 2) )
	{
		return;
	}
	
	AT45DB321D_Select();
	if(Buffer == 1)
	{
		SpiSend(AT45DB321D_CMD_BUFFER1_WRITE);
	}
	else
	{
		SpiSend(AT45DB321D_CMD_BUFFER2_WRITE);
	}
	
	//Send address to read
	//The address is 3 bytes, but only the 10 LSBs matter (9 LSBs for 512 mode)
	SpiSend(0x00);
	SpiSend((BufferStartAddress & 0x0300)>>8);
	SpiSend(BufferStartAddress & 0xFF);

	for(i = 0; i<BytesToWrite; i++)
	{
		//printf_P(PSTR("write: 0x%02X\n"), *DataWriteBuffer);
		SpiSend(*DataWriteBuffer);
		DataWriteBuffer++;
	}
	AT45DB321D_Deselect();
	
	currentByte = BufferStartAddress + BytesToWrite;
	*/
	return;
}

void AT45DB321D_CopyPageToBuffer(uint8_t Buffer, uint16_t PageAddress)
{
	/*
	//No funny stuff...
	//TODO: add check for PageAddress
	if( (Buffer != 1) && (Buffer != 2) )
	{
	return;
	}

	AT45DB321D_Select();
	if(Buffer == 1)
	{
	SpiSend(AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER1);
	}
	else
	{
	SpiSend(AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER2);
	}

	//Send page address, this is different for 512 and 528 mode
	AT45DB321D_SendPageAddress(PageAddress);

	AT45DB321D_Deselect();

	//Add code to check for complete?

	return;
	}

	void AT45DB321D_CopyBufferToPage(uint8_t Buffer, uint16_t PageAddress)
	{
	//No funny stuff...
	//TODO: add check for PageAddress
	if( (Buffer != 1) && (Buffer != 2) )
	{
	return;
	}

	AT45DB321D_Select();
	if(Buffer == 1)
	{
	SpiSend(AT45DB321D_CMD_BUFFER1_TO_PAGE_ERASE);
	}
	else
	{
	SpiSend(AT45DB321D_CMD_BUFFER2_TO_PAGE_ERASE);
	}

	//Send page address, this is different for 512 and 528 mode
	AT45DB321D_SendPageAddress(PageAddress);

	AT45DB321D_Deselect();

	//Add code to check for complete?
	*/
	return;
}

void AT45DB321D_ErasePage(uint16_t PageAddress)
{
	/*
	AT45DB321D_Select();
	SpiSend(AT45DB321D_CMD_PAGE_ERASE);
	AT45DB321D_SendPageAddress(PageAddress);
	AT45DB321D_Deselect();
	*/
	return;
}

uint8_t AT45DB321D_WaitForReady(void)
 {
	uint8_t StatusByte = 0x00;
	while((StatusByte & AT45DB321D_STATUS_READY_MASK) != AT45DB321D_STATUS_READY_MASK)
	{
	StatusByte = AT45DB321D_ReadStatus();
	}
	return StatusByte;
 }

void AT45DB321D_SendPageAddress(uint16_t PageAddress)
{
	/*
	//Send page address, this is different for 512 and 528 mode
	#if AT45DB321D_PAGE_SIZE_BYTES == 512
	SpiSend( (uint8_t)(PageAddress>>7) );
	SpiSend( (uint8_t)(PageAddress<<1) );
	SpiSend(0x00);
	#else
	SpiSend( (uint8_t)(PageAddress>>6) );
	SpiSend( (uint8_t)(PageAddress<<2) );
	SpiSend(0x00);
	#endif
	*/
	return;
}

uint16_t AT45DB321D_FirstBlankPage(void)
{
	uint16_t n;
	uint8_t m;
	uint8_t bytestoread[50];
	uint8_t dataFound;

	for (m=0;m<50;m++)
	{
		bytestoread[m] = 0;
	}
	
	for(n=0;n<8191;n++)
	{
		AT45DB321D_CopyPageToBuffer(1,n);//copy the page out onto buffer 2 for reading
		AT45DB321D_WaitForReady();
		AT45DB321D_BufferRead(1,0,bytestoread,50);//read the first 50 bytes of the buffer
		
		dataFound=0;
		for (m=0;m<50;m++)
		{
			if (bytestoread[m] != 255)
			{
				dataFound = 1;
				m = 50;
			}
		}
		
		if (dataFound==0)
		{
			return n;	
		}
	
	}
	
	printf("FirstBlankPage scan error\n");
	return 0;
	
}


//Untested
void AT45DB321D_Powerdown(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(AT45DB321D_CMD_POWERDOWN);
	AT45DB321D_Deselect();
	*/
	return;
}

void AT45DB321D_Powerup(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(AT45DB321D_CMD_POWERUP);
	AT45DB321D_Deselect();
	*/
	return;
}

void AT45DB321D_ChipErase(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(AT45DB321D_CMD_CHIP_ERASE1);
	SpiSend(AT45DB321D_CMD_CHIP_ERASE2);
	SpiSend(AT45DB321D_CMD_CHIP_ERASE3);
	SpiSend(AT45DB321D_CMD_CHIP_ERASE4);
	AT45DB321D_Deselect();
	*/
	return;
}

void AT45DB321D_Protect(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(0x3D);
	SpiSend(0x2A);
	SpiSend(0x7F);
	SpiSend(0xA9);
	AT45DB321D_Deselect();
	*/
	return;
}

void AT45DB321D_Unprotect(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(0x3D);
	SpiSend(0x2A);
	SpiSend(0x7F);
	SpiSend(0x9A);
	AT45DB321D_Deselect();
	*/
	return;
}

//This command should only need to be sent once. It can not be undone.
//The device needs to be power cycled after this command is sent.
//The change can be verified by reading the status register.
//Make sure to set the 'AT45DB321D_PAGE_SIZE_BYTES' variable in at45db321d.h correctly.
#if AT45DB321D_PAGE_SIZE_BYTES != 512
void AT45DB321D_SwitchTo512(void)
{
	/*
	AT45DB321D_Select();
	SpiSend(0x3D);
	SpiSend(0x2A);
	SpiSend(0x80);
	SpiSend(0xA6);
	AT45DB321D_Deselect();
	AT45DB321D_WaitForReady();
	printf("Device page size set to 512. Please power cycle the device\n");
	*/
	return;
}
#endif

/** @} */
