/*
 * AD7606.h
 *
 *  Created on: Oct 11, 2013
 *      Author: pat
 */


#ifndef AD7606_H_
#define AD7606_H_

//GPIO defines
//A/D 1:
// P1.0:	ADSTART-1
// P1.1:	CS-ADC1
// P1.4:	ADBUSY-1
// P1.8:	ADFDATA-1
#define AD7606_1_ADSTART_PORT		1
#define AD7606_1_ADSTART_PIN		0
#define AD7606_1_CS_PORT			1
#define AD7606_1_CS_PIN				1
#define AD7606_1_ADBUSY_PORT		1
#define AD7606_1_ADBUSY_PIN			4
#define AD7606_1_ADFDATA_PORT		1
#define AD7606_1_ADFDATA_PIN		8
#define AD7606_1_IO_EXP_ADDR	0x38


//A/D 2:
// P0.6:	ADSTART-1
// P2.0:	CS-ADC1
// P2.1:	ADBUSY-1
// P2.2:	ADFDATA-1
#define AD7606_2_ADSTART_PORT		0
#define AD7606_2_ADSTART_PIN		6
#define AD7606_2_CS_PORT			2
#define AD7606_2_CS_PIN				0
#define AD7606_2_ADBUSY_PORT		2
#define AD7606_2_ADBUSY_PIN			1
#define AD7606_2_ADFDATA_PORT		2
#define AD7606_2_ADFDATA_PIN		2
#define AD7606_2_IO_EXP_ADDR		0x39


//A/D 3:
// P0.18:	ADSTART-1
// P0.21:	CS-ADC1
// P0.22:	ADBUSY-1
// P2.11:	ADFDATA-1
#define AD7606_3_ADSTART_PORT		0
#define AD7606_3_ADSTART_PIN		18
#define AD7606_3_CS_PORT			0
#define AD7606_3_CS_PIN				21
#define AD7606_3_ADBUSY_PORT		0
#define AD7606_3_ADBUSY_PIN			22
#define AD7606_3_ADFDATA_PORT		2
#define AD7606_3_ADFDATA_PIN		2
#define AD7606_3_IO_EXP_ADDR		0x3B

//Bit definitions for I/O expander bits
#define AD7606_OS0_BIT		0x00
#define AD7606_OS1_BIT		0x01
#define AD7606_OS2_BIT		0x02
#define AD7606_STANDBY_BIT	0x05
#define AD7606_RANGE_BIT	0x06
#define AD7606_RESET_BIT	0x07


#define AD7606_OS_RATIO_0	0x00	//200kHz
#define AD7606_OS_RATIO_2	0x01	//100kHz
#define AD7606_OS_RATIO_4	0x02	//50kHz
#define AD7606_OS_RATIO_8	0x03	//25kHz
#define AD7606_OS_RATIO_16	0x04	//12.5kHz
#define AD7606_OS_RATIO_32	0x05	//6.25kHz
#define AD7606_OS_RATIO_64	0x06	//3.125kHz

#define AD7606_POWER_MODE_ON		0x00
#define AD7606_POWER_MODE_STANDBY	0x01
#define AD7606_POWER_MODE_SHUTDOWN	0x02

#define AD7606_RANGE_5V		0x00
#define AD7606_RANGE_10V	0x01

//ADC clock rate in HZ (NOTE: this clock rate will get inaccurate above ~1MHZ with the current clocking (30MHZ PCLK))
#define AD7606_DATA_CLOCK_HZ	10000//was 1000

//Status defines
#define AD7606_STATUS_SHUTDOWN		0x01		//Indicates the ADC is in shutdown mode
#define AD7606_STATUS_STANDBY		0x02		//Indicates the ADC is in standby mode
#define AD7606_STATUS_READY			0x03		//Indicates the ADC is in powerup mode
//TODO: Add more stuff here?
//TODO: Make this bitwise?
#define AD7606_STATUS_DATAREADY 	0x08		//Indicated that a new dataset is ready. This bit is set when the data is read from the ADC and cleared when the ADC data is read.










void AD7606Init(void);

void AD7606SetOSMode(uint8_t chipNumber, uint8_t OSModeToSet);
void AD7606Reset(uint8_t chipNumber);
void AD7606SetPowerMode(uint8_t chipNumber, uint8_t PowerModeToSet);
void AD7606SetRange(uint8_t chipNumber, uint8_t RangeToSet);

void AD7606WaitReady(uint8_t chipNumber);




//void AD7606StartDataClock(uint32_t ClockRateToSet);

/** Select an AD7606 chip
 *   ChipNumber: The number of the chip to select (1,2,3)
 *   Slect:	1=select, 0=deselect
 *   TODO: implement chips 2 and 3, check for invalid chips
 */
void AD7606_Select(uint8_t chipNumber, uint8_t Select);

void AD7606_Start(uint8_t chipNumber, bool Started);

uint8_t AD7606GetStatus(void);
void AD7606GetDataSet(uint8_t sel, uint16_t* DataSet);




#endif
