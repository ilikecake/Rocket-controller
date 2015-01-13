/*
 * ad7606.c

 *
 *  Created on: Oct 11, 2013
 *      Author: pat
 */
//Analog to digital converter

#include "main.h"

uint8_t ADC_Dataclock_Count;
uint8_t ADC_SampleCount;
int16_t ADC_DataArray[8];
uint8_t ADC_Status;


void AD7606Init(void)
{
	ADC_Dataclock_Count = 0;
	ADC_SampleCount = 0;

	//Setup pins for chip 1
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_1_ADSTART_PORT,	AD7606_1_ADSTART_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_1_CS_PORT,		AD7606_1_CS_PIN,		IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_1_ADBUSY_PORT,	AD7606_1_ADBUSY_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_1_ADFDATA_PORT,	AD7606_1_ADFDATA_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_1_ADSTART_PORT,	AD7606_1_ADSTART_PIN,	true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_1_CS_PORT,		AD7606_1_CS_PIN,		true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_1_ADBUSY_PORT,	AD7606_1_ADBUSY_PIN,	false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_1_ADFDATA_PORT,	AD7606_1_ADFDATA_PIN,	false);

	//Setup pins for chip 2
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_2_ADSTART_PORT,	AD7606_2_ADSTART_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_2_CS_PORT,		AD7606_2_CS_PIN,		IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_2_ADBUSY_PORT,	AD7606_2_ADBUSY_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_2_ADFDATA_PORT,	AD7606_2_ADFDATA_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_2_ADSTART_PORT,	AD7606_2_ADSTART_PIN,	true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_2_CS_PORT,		AD7606_2_CS_PIN,		true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_2_ADBUSY_PORT,	AD7606_2_ADBUSY_PIN,	false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_2_ADFDATA_PORT,	AD7606_2_ADFDATA_PIN,	false);


	//Setup pins for chip 3
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_3_ADSTART_PORT,	AD7606_3_ADSTART_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_3_CS_PORT,		AD7606_3_CS_PIN,		IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_3_ADBUSY_PORT,	AD7606_3_ADBUSY_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_3_ADFDATA_PORT,	AD7606_3_ADFDATA_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_3_ADSTART_PORT,	AD7606_3_ADSTART_PIN,	true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_3_CS_PORT,		AD7606_3_CS_PIN,		true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_3_ADBUSY_PORT,	AD7606_3_ADBUSY_PIN,	false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_3_ADFDATA_PORT,	AD7606_3_ADFDATA_PIN,	false);


	//Conversion control pin idles low
	AD7606_Start(1,false);
	AD7606_Start(2,false);
	AD7606_Start(3,false);

	//Set up options for A/D1 in the I/O expander
	TCA9554A_SetConfig(AD7606_1_IO_EXP_ADDR, 0x18);//~((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT) | (1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT) | (1<<AD7606_RESET_BIT)) );
	TCA9554A_SetConfig(AD7606_2_IO_EXP_ADDR, 0x18);
	TCA9554A_SetConfig(AD7606_3_IO_EXP_ADDR, 0x18);

	AD7606SetOSMode(1,AD7606_OS_RATIO_64);//set how many samples will be filtered to get one output
	AD7606SetPowerMode(1,AD7606_POWER_MODE_ON);
	AD7606SetRange(1,AD7606_RANGE_10V);
	AD7606Reset(1);

	AD7606SetOSMode(2,AD7606_OS_RATIO_64);//set how many samples will be filtered to get one output
	AD7606SetPowerMode(2,AD7606_POWER_MODE_ON);
	AD7606SetRange(2,AD7606_RANGE_10V);
	AD7606Reset(2);

	AD7606SetOSMode(3,AD7606_OS_RATIO_64);//set how many samples will be filtered to get one output
	AD7606SetPowerMode(3,AD7606_POWER_MODE_ON);
	AD7606SetRange(3,AD7606_RANGE_5V);
	AD7606Reset(3);

	AD7606_Select(1,0);
	AD7606_Select(2,0);
	AD7606_Select(3,0);

	ADC_Status = AD7606_STATUS_READY;

	return;
}

void AD7606SetOSMode(uint8_t chipNumber, uint8_t OSModeToSet)
{
	uint8_t BitsToSet = 0;

	if((OSModeToSet & 0x01) == 0x01)
	{
		BitsToSet |= (1 << AD7606_OS0_BIT);
	}
	if((OSModeToSet & 0x02) == 0x02)
	{
		BitsToSet |= (1 << AD7606_OS1_BIT);
	}
	if((OSModeToSet & 0x04) == 0x04)
	{
		BitsToSet |= (1 << AD7606_OS2_BIT);
	}

	switch (chipNumber){
	case 1:
		TCA9554A_SetStateBits(AD7606_1_IO_EXP_ADDR, ((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT)), BitsToSet);
		break;
	case 2:
		TCA9554A_SetStateBits(AD7606_2_IO_EXP_ADDR, ((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT)), BitsToSet);
		break;
	case 3:
		TCA9554A_SetStateBits(AD7606_3_IO_EXP_ADDR, ((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT)), BitsToSet);
		break;
	}

	return;
}

void AD7606Reset(uint8_t chipNumber)
{
	switch (chipNumber){
	case 1:
		TCA9554A_SetStateBit(AD7606_1_IO_EXP_ADDR, AD7606_RESET_BIT, 1);
		TCA9554A_SetStateBit(AD7606_1_IO_EXP_ADDR, AD7606_RESET_BIT, 0);
		break;
	case 2:
		TCA9554A_SetStateBit(AD7606_2_IO_EXP_ADDR, AD7606_RESET_BIT, 1);
		TCA9554A_SetStateBit(AD7606_2_IO_EXP_ADDR, AD7606_RESET_BIT, 0);
		break;
	case 3:
		TCA9554A_SetStateBit(AD7606_3_IO_EXP_ADDR, AD7606_RESET_BIT, 1);
		TCA9554A_SetStateBit(AD7606_3_IO_EXP_ADDR, AD7606_RESET_BIT, 0);
		break;
	}

	return;
}

//Note: The part will need to be reinitialized on power up.
void AD7606SetPowerMode(uint8_t chipNumber, uint8_t PowerModeToSet)
{
	uint8_t exp_addr;
	switch (chipNumber){
	case 1:
		exp_addr=AD7606_1_IO_EXP_ADDR;
		break;
	case 2:
		exp_addr=AD7606_2_IO_EXP_ADDR;
		break;
	case 3:
		exp_addr=AD7606_3_IO_EXP_ADDR;
		break;
	}

	if(PowerModeToSet == AD7606_POWER_MODE_STANDBY)
	{
		//Range bit = 1
		//Standby bit = 0
		TCA9554A_SetStateBits(exp_addr, ((1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT)), (1<<AD7606_RANGE_BIT));
	}
	else if (PowerModeToSet == AD7606_POWER_MODE_SHUTDOWN)
	{
		//Range bit = 0
		//Standby bit = 0
		TCA9554A_SetStateBits(exp_addr, ((1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT)), 0x00);
	}
	else
	{
		//The range bit may need to be changed after power up
		//The part must also be reset if powering up from shutdown
		TCA9554A_SetStateBit(exp_addr, AD7606_STANDBY_BIT, 1);
	}
	return;
}

//The part will probably need a reset after this is changed
void AD7606SetRange(uint8_t chipNumber, uint8_t RangeToSet)
{
	uint8_t bit;

	if(RangeToSet == AD7606_RANGE_5V)
	{
		bit=0;
	}
	else if(RangeToSet == AD7606_RANGE_10V)
	{
		bit=1;
	}

	switch (chipNumber){
	case 1:
		TCA9554A_SetStateBit(AD7606_1_IO_EXP_ADDR, AD7606_RANGE_BIT, bit);
		break;
	case 2:
		TCA9554A_SetStateBit(AD7606_2_IO_EXP_ADDR, AD7606_RANGE_BIT, bit);
		break;
	case 3:
		TCA9554A_SetStateBit(AD7606_3_IO_EXP_ADDR, AD7606_RANGE_BIT, bit);
		break;
	}

	return;
}

//Poll the ready pin. When the pin goes low, the data is ready
void AD7606WaitReady(uint8_t chipNumber)
{
	uint8_t ReadyBit;
	uint8_t OldReadyBit;
	uint32_t Timeout;

	Timeout = 0;
	ReadyBit = 0;
	OldReadyBit = 0;
	while(1)
	{
		switch (chipNumber){
		case 1:
			ReadyBit = Chip_GPIO_ReadPortBit(LPC_GPIO, AD7606_1_ADBUSY_PORT, AD7606_1_ADBUSY_PIN);
			break;
		case 2:
			ReadyBit = Chip_GPIO_ReadPortBit(LPC_GPIO, AD7606_2_ADBUSY_PORT, AD7606_2_ADBUSY_PIN);
			break;
		case 3:
			ReadyBit = Chip_GPIO_ReadPortBit(LPC_GPIO, AD7606_3_ADBUSY_PORT, AD7606_3_ADBUSY_PIN);
			break;
		}


		if(((OldReadyBit == 1) && (ReadyBit == 0)) || (Timeout > 100000))
		{
			if(Timeout > 100000)
			{
				//TODO: There should not be a printf statment here. Return an error instead.
				if(runningData==0) printf("Timeout\r\n");
			}
			return;
		}
		OldReadyBit = ReadyBit;
		Timeout++;
	}
	return;
}

void AD7606_Select(uint8_t chipNumber, uint8_t Select)
{
	switch (chipNumber){
	case 1:
		if(Select == 1)
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_1_CS_PORT, AD7606_1_CS_PIN, false);		//Low to select
		}
		else
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_1_CS_PORT, AD7606_1_CS_PIN, true);		//High to deselect
		}
		break;
	case 2:
		if(Select == 1)
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_2_CS_PORT, AD7606_2_CS_PIN, false);		//Low to select
		}
		else
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_2_CS_PORT, AD7606_2_CS_PIN, true);		//High to deselect
		}
		break;
	case 3:
		if(Select == 1)
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_3_CS_PORT, AD7606_3_CS_PIN, false);		//Low to select
		}
		else
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_3_CS_PORT, AD7606_3_CS_PIN, true);		//High to deselect
		}
		break;
	}

	return;
}

uint8_t AD7606GetStatus(void)
{
	return ADC_Status;
}

void AD7606GetDataSet(uint8_t chipNumber, uint16_t* DataSet)
{
	uint8_t i;
	Chip_SSP_DATA_SETUP_T xf_setup;
	SSP_ConfigFormat ssp_format;
	uint8_t ADInputData[16];

	//Set up the SSP format for the AD7606, SPI mode 10
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_CPHA1_CPOL1;// SSP_CLOCK_CPHA0_CPOL1;
	Chip_SSP_SetFormat(LPC_SSP1, &ssp_format);

	//Start conversion
	AD7606_Start(chipNumber,true);

	//Wait for data to become available
	AD7606WaitReady(chipNumber);
	AD7606_Start(chipNumber,false);

	//Chip_SSP_Enable(LPC_SSP1);
	xf_setup.length = 16;
	xf_setup.tx_data = NULL;
	xf_setup.rx_data = ADInputData;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

	//Select A/D 1
	AD7606_Select(chipNumber,1);

	//Read data from A/D 1
	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &xf_setup);

	//Deselect A/D 1
	AD7606_Select(chipNumber,0);


	for(i=0; i<8; i++)
	{
		DataSet[i] = ( (((uint16_t)ADInputData[i*2])<<8) + ((uint16_t)ADInputData[i*2+1]) );

		if (runningData==0)
		{
			printf("Ch:%u = %u %u\r\n",i,ADInputData[i*2],ADInputData[i*2+1]);
		}

		//prevent slightly negative voltages from reading as very high numbers.
		//This will inadvertently cause legitimately high values to read zero
		//if(DataSet[i]>65500)
		//{
		//	DataSet[i]=DataSet[i]-65536;
		//}

/*
		//bit 16 is sign.  shift data to be centered on 2^15
		if ((DataSet[i]>>16)==1)
		{
			DataSet[i]=DataSet[i]-0x8000;//negative numbers count back from 32768
		}
		else
		{
			DataSet[i]=DataSet[i]+0x8000;//positive numbers count up from 32768
		}
*/

		//DataSet[i] = chipNumber*1000+i*10;
	}

	//for(i=0; i<16; i++)
	//{
	//	printf("ADC[%u]: %d counts\r\n", i, ADInputData[i]);
	//}

	ADC_Status = AD7606_STATUS_READY;
	return;
}

void AD7606_Start(uint8_t chipNumber, bool Started)
{
	//TODO: move this to the .c file, update to handle multiple AD7606 chips
	switch (chipNumber)
	{
		case 1:
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_1_ADSTART_PORT, AD7606_1_ADSTART_PIN, Started);
			break;

		case 2:
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_2_ADSTART_PORT, AD7606_2_ADSTART_PIN, Started);
			break;

		case 3:
			Chip_GPIO_WritePortBit(LPC_GPIO, AD7606_3_ADSTART_PORT, AD7606_3_ADSTART_PIN, Started);
			break;
	}

	return;
}

