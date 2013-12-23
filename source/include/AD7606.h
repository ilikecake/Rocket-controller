/*
 * AD7606.h
 *
 *  Created on: Oct 11, 2013
 *      Author: pat
 */


#ifndef AD7606_H_
#define AD7606_H_

//GPIO defines
// P1.14:	ADSTART
// P0.5:	ADBUSY
// P0.6:	ADFDATA
#define AD7606_ADSTART_PORT		1
#define AD7606_ADSTART_PIN		14
#define AD7606_ADBUSY_PORT		0
#define AD7606_ADBUSY_PIN		5
#define AD7606_ADFDATA_PORT		0
#define AD7606_ADFDATA_PIN		6


//Bit definitions for I/O expander bits
#define AD7606_OS0_BIT		0x00
#define AD7606_OS1_BIT		0x01
#define AD7606_OS2_BIT		0x02
#define AD7606_STANDBY_BIT	0x05
#define AD7606_RANGE_BIT	0x06
#define AD7606_RESET_BIT	0x07

#define AD7606_IO_EXP_ADDR	0x38

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
#define AD7606_DATA_CLOCK_HZ	1000

//Status defines
#define AD7606_STATUS_SHUTDOWN		0x01		//Indicates the ADC is in shutdown mode
#define AD7606_STATUS_STANDBY		0x02		//Indicates the ADC is in standby mode
#define AD7606_STATUS_READY			0x03		//Indicates the ADC is in powerup mode
//TODO: Add more stuff here?
//TODO: Make this bitwise?
#define AD7606_STATUS_DATAREADY 	0x08		//Indicated that a new dataset is ready. This bit is set when the data is read from the ADC and cleared when the ADC data is read.

STATIC INLINE void AD7606_Start(bool setting)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_ADSTART_PORT, AD7606_ADSTART_PIN, setting);
}








void AD7606Init(void);

void AD7606SetOSMode(uint8_t OSModeToSet);
void AD7606Reset(void);
void AD7606SetPowerMode(uint8_t PowerModeToSet);
void AD7606SetRange(uint8_t RangeToSet);

void AD7606WaitReady(void);




void AD7606StartDataClock(uint32_t ClockRateToSet);

uint8_t AD7606GetStatus(void);
void AD7606GetDataSet(void);//uint16_t *DataSet[]);




#endif
