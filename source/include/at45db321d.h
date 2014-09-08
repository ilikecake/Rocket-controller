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
* \brief Header file for the AT45DB321D dataflash driver.
* \author Pat Satyshur
* \version 1.0
* \date 2/17/2013
* \copyright Copyright 2013, Pat Satyshur
* \ingroup hardware
*
* @{
*/

#ifndef _AT45DB321D_H_
#define _AT45DB321D_H_

#include "stdint.h"

#define AT45DB321D_PAGE_SIZE_BYTES 528
#if (AT45DB321D_PAGE_SIZE_BYTES != 512) && (AT45DB321D_PAGE_SIZE_BYTES != 528)
#error: Page size is incorrect. Must be 512 or 528.
#endif

#define AT45DB321D_CMD_ARRAY_READ_LEGACY 0xE8
#define AT45DB321D_CMD_ARRAY_READ_HF 0x0B
#define AT45DB321D_CMD_ARRAY_READ_LF 0x03
#define AT45DB321D_CMD_PAGE_READ 0xD2
#define AT45DB321D_CMD_BUFFER1_READ_HS 0xD4
#define AT45DB321D_CMD_BUFFER1_READ_LS 0xD1
#define AT45DB321D_CMD_BUFFER2_READ_HS 0xD6
#define AT45DB321D_CMD_BUFFER2_READ_LS 0xD3

#define AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER1 0x53
#define AT45DB321D_CMD_TRANSFER_PAGE_TO_BUFFER2 0x55
#define AT45DB321D_CMD_MEMORY_TO_BUFFER1_COMPARE 0x60
#define AT45DB321D_CMD_MEMORY_TO_BUFFER2_COMPARE 0x61
#define AT45DB321D_CMD_PAGE_REWRITE_BUFFER1 0x58
#define AT45DB321D_CMD_PAGE_REWRITE_BUFFER2 0x59
#define AT45DB321D_CMD_READ_STATUS 0xD7
#define AT45DB321D_CMD_READ_DEVICE_ID 0x9F

#define AT45DB321D_CMD_POWERDOWN 0xB9
#define AT45DB321D_CMD_POWERUP 0xAB

#define AT45DB321D_CMD_BUFFER1_WRITE 0x84
#define AT45DB321D_CMD_BUFFER2_WRITE 0x87
#define AT45DB321D_CMD_BUFFER1_TO_PAGE_ERASE 0x83
#define AT45DB321D_CMD_BUFFER2_TO_PAGE_ERASE 0x86
#define AT45DB321D_CMD_BUFFER1_TO_PAGE_NOERASE 0x88
#define AT45DB321D_CMD_BUFFER2_TO_PAGE_NOERASE 0x89
#define AT45DB321D_CMD_PAGE_ERASE 0x81
#define AT45DB321D_CMD_BLOCK_ERASE 0x50
#define AT45DB321D_CMD_SECTOR_ERASE 0x7C
#define AT45DB321D_CMD_PAGE_PROGRAM_BUFFER1 0x82
#define AT45DB321D_CMD_PAGE_PROGRAM_BUFFER2 0x85

#define AT45DB321D_CMD_READ_PROTECTED_SECTORS 0x32
#define AT45DB321D_CMD_READ_LOCKED_SECTORS 0x35
#define AT45DB321D_CMD_READ_SECURITY_REG 0x77

//All four of these need to be sent to erase the chip
#define AT45DB321D_CMD_CHIP_ERASE1 0xC7
#define AT45DB321D_CMD_CHIP_ERASE2 0x94
#define AT45DB321D_CMD_CHIP_ERASE3 0x80
#define AT45DB321D_CMD_CHIP_ERASE4 0x9A

//Status register masks
#define AT45DB321D_STATUS_READY_MASK 0x80

uint16_t currentPage;
uint16_t currentByte;

void AT45DB321D_Init(void);
void AT45DB321D_Select(void);
void AT45DB321D_Deselect(void);

uint8_t AT45DB321DSendCommand(uint8_t *SendBuffer, uint8_t *ReceiveBuffer, uint8_t BytesToSend, uint8_t BytesToReceive);

/** Returns the status register of the device */
uint8_t AT45DB321D_ReadStatus(void);

/** Reads 'BytesToRead' bytes buffer number 'Buffer' starting at address 'BufferStartAddress' to 'DataReadBuffer' */
void AT45DB321D_BufferRead(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataReadBuffer[], uint16_t BytesToRead);

/** Writes 'BytesToWrite' bytes from 'DataWriteBuffer' to buffer number 'Buffer' starting at address 'BufferStartAddress' */
void AT45DB321D_BufferWrite(uint8_t Buffer, uint16_t BufferStartAddress, uint8_t DataWriteBuffer[], uint16_t BytesToWrite);

/** Copies the contents of main memory page 'PageAddress' into buffer number 'Buffer' */
void AT45DB321D_CopyPageToBuffer(uint8_t Buffer, uint16_t PageAddress);

/** Copies the contents of 'Buffer' into main memory page 'PageAddress.' Uses the write-with-erase command. */
void AT45DB321D_CopyBufferToPage(uint8_t Buffer, uint16_t PageAddress);

/** Erase buffer to 255 */
void AT45DB321D_EraseBuffer(uint8_t Buffer);


/** Waits for the RDY/BUSY bit in the status register to go high. This indicates that the part is ready for another command.
*
* Returns the status register
*/
uint8_t AT45DB321D_WaitForReady(void);

/** Send the page address to the device */
void AT45DB321D_SendPageAddress(uint16_t PageAddress);

/** Powerdown the device. Once powered down, the device will ignore all commands except the power up command */
void AT45DB321D_Powerdown(void);

/** Power up the device from power down mode */
void AT45DB321D_Powerup(void);

/** Erase the entire chip */
void AT45DB321D_ChipErase(void);

/** Enables write protection of the sectors defined in the sector protection register */
void AT45DB321D_Protect(void);

/** Disabled write protection of the sectors defined in the sector protection register */
void AT45DB321D_Unprotect(void);

/** Set the page size to 512 */
void AT45DB321D_SwitchTo512(void);

/** returns the first page that does not already have data witten on it */
uint16_t AT45DB321D_FirstBlankPage(void);

//not implemented yet
void AT45DB321D_BlockErase(uint16_t BlockAddress);
void AT45DB321D_SectorErase(uint8_t SectorAddress);

void AT45DB321D_ReadProtectedSectors(uint8_t *ProtectData);
void AT45DB321D_ProtectSectors(uint8_t *ProtectData);

void AT45DB321D_ReadLockedSectors(uint8_t *ProtectData);
void AT45DB321D_LockSectors(uint8_t *ProtectData);	//I probably will not implement this

void AT45DB321D_ReadSecurityRegister(uint8_t *SecurityReg);
void AT45DB321D_WriteSecurityRegister(uint8_t *SecurityReg);

#endif
/** @} */
