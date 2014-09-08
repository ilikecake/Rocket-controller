/*
 * ad7606.c
 *
 *  Created on: Oct 11, 2013
 *      Author: pat
 */


#include "main.h"

uint8_t ADC_Dataclock_Count;
uint8_t ADC_SampleCount;
int16_t ADC_DataArray[8];
uint8_t ADC_Status;

//uint8_t ADC_DataArray2[16];		//TODO: remove this later




void AD7606Init(void)
{
	ADC_Dataclock_Count = 0;
	ADC_SampleCount = 0;

	//Setup pins
	// P1.14:	ADSTART (output, low)
	// P0.5:	ADBUSY
	// P0.6:	ADFDATA
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_ADSTART_PORT, AD7606_ADSTART_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_ADBUSY_PORT, AD7606_ADBUSY_PIN, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, AD7606_ADFDATA_PORT, AD7606_ADFDATA_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_ADSTART_PORT, AD7606_ADSTART_PIN, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_ADBUSY_PORT, AD7606_ADBUSY_PIN, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, AD7606_ADFDATA_PORT, AD7606_ADFDATA_PIN, false);

	//Conversion control pin idles low
	AD7606_Start(false);

	TCA9554A_SetConfig(AD7606_IO_EXP_ADDR, 0x18);//~((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT) | (1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT) | (1<<AD7606_RESET_BIT)) );

	AD7606SetOSMode(AD7606_OS_RATIO_4);
	AD7606SetPowerMode(AD7606_POWER_MODE_ON);
	AD7606SetRange(AD7606_RANGE_5V);

	//Setup the timer for the ADC clock
	Chip_IOCON_PinMux(LPC_IOCON, 4, 28, IOCON_MODE_INACT, IOCON_FUNC2);		//Setup P4.28 as MAT2.0

	//Initialize timer 2 for the ADC data clock
	Chip_TIMER_Init(LPC_TIMER2);

	//Set timer2 clock source to PCLK
	Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER2 ,TIMER_CAPSRC_RISING_PCLK, 0);

	//Set prescalar to zero. If we need a very slow clock (compared to 30MHz), this may need to change
	Chip_TIMER_PrescaleSet(LPC_TIMER2, 0);

	//Set timer2 match value
	//TODO: check to see if the desired clock rate is a significant fraction of pclk, and output a warning if so
	//TODO: Maybe make a variable for pclk so I don't have to call the function 3 times?
	/*if(AD7606_DATA_CLOCK_HZ > Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2))
	{
		AD7606_DATA_CLOCK_HZ = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2);
	}*/

	Chip_TIMER_SetMatch(LPC_TIMER2, 0, ((Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2)/(2*AD7606_DATA_CLOCK_HZ))-1) );

	//Timer2 should reset on match
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 0);

	//Timer2 should toggle MR2.0 on match0
	Chip_TIMER_ExtMatchControlSet(LPC_TIMER2, 1, TIMER_EXTMATCH_TOGGLE, 0);

	//Setup an interrupt to run opposite of the adc master clock
	Chip_TIMER_SetMatch(LPC_TIMER2, 1, ((Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2)/(2*AD7606_DATA_CLOCK_HZ))-1)/2 );		//Match register 1 should be set to half of match register 0
	Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);	//Interrupts will be generated when MR1 matches the timer value

	NVIC_DisableIRQ(TIMER2_IRQn);
	NVIC_SetPriority(TIMER2_IRQn, 0x00);	//Set interrupt to high priority (0 is highest, 31 is 0x1F is lowest.)
	NVIC_EnableIRQ(TIMER2_IRQn);




	AD7606Reset();

	ADC_Status = AD7606_STATUS_READY;

	return;
}

void AD7606SetOSMode(uint8_t OSModeToSet)
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

	TCA9554A_SetStateBits(AD7606_IO_EXP_ADDR, ((1<<AD7606_OS0_BIT) | (1<<AD7606_OS1_BIT) | (1<<AD7606_OS2_BIT)), BitsToSet);
	return;
}

void AD7606Reset(void)
{
	TCA9554A_SetStateBit(AD7606_IO_EXP_ADDR, AD7606_RESET_BIT, 1);
	TCA9554A_SetStateBit(AD7606_IO_EXP_ADDR, AD7606_RESET_BIT, 0);
	return;
}

//Note: The part will need to be reinitialized on power up.
void AD7606SetPowerMode(uint8_t PowerModeToSet)
{
	if(PowerModeToSet == AD7606_POWER_MODE_STANDBY)
	{
		//Range bit = 1
		//Standby bit = 0
		TCA9554A_SetStateBits(AD7606_IO_EXP_ADDR, ((1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT)), (1<<AD7606_RANGE_BIT));
	}
	else if (PowerModeToSet == AD7606_POWER_MODE_SHUTDOWN)
	{
		//Range bit = 0
		//Standby bit = 0
		TCA9554A_SetStateBits(AD7606_IO_EXP_ADDR, ((1<<AD7606_STANDBY_BIT) | (1<<AD7606_RANGE_BIT)), 0x00);
	}
	else
	{
		//The range bit may need to be changed after power up
		//The part must also be reset if powering up from shutdown
		TCA9554A_SetStateBit(AD7606_IO_EXP_ADDR, AD7606_STANDBY_BIT, 1);
	}
	return;
}

//The part will probably need a reset after this is changed
void AD7606SetRange(uint8_t RangeToSet)
{
	if(RangeToSet == AD7606_RANGE_5V)
	{
		TCA9554A_SetStateBit(AD7606_IO_EXP_ADDR, AD7606_RANGE_BIT, 0);
	}
	else if(RangeToSet == AD7606_RANGE_5V)
	{
		TCA9554A_SetStateBit(AD7606_IO_EXP_ADDR, AD7606_RANGE_BIT, 1);
	}

	return;
}

//Poll the ready pin. When the pin goes low, the data is ready
void AD7606WaitReady(void)
{
	uint8_t ReadyBit;
	uint8_t OldReadyBit;
	uint32_t Timeout;

	Timeout = 0;
	ReadyBit = 0;
	OldReadyBit = 0;
	while(1)
	{
		ReadyBit = Chip_GPIO_ReadPortBit(LPC_GPIO, AD7606_ADBUSY_PORT, AD7606_ADBUSY_PIN);
		if(((OldReadyBit == 1) && (ReadyBit == 0)) || (Timeout > 100000))
		{
			if(Timeout > 100000)
			{
				printf("Timeout\r\n");
			}
			return;
		}
		OldReadyBit = ReadyBit;
		Timeout++;
	}
	return;
}

void AD7606StartDataClock(uint32_t ClockRateToSet)
{
	//Start timer 2
	Chip_TIMER_StopOnMatchDisable(LPC_TIMER2, 0);
	Chip_TIMER_Enable(LPC_TIMER2);
	return;
}

void TIMER2_IRQHandler(void)
{
	//ADC_DataArray

	Chip_TIMER_ClearMatch(LPC_TIMER2, 1);

	/*if(ADC_Dataclock_Count == 0)
	{
		ADC_Dataclock_Count = 1;
	}
	else
	{
		ADC_Dataclock_Count = 0;
	}

	Board_LED_Set(2, (bool)ADC_Dataclock_Count);*/

	if(ADC_Dataclock_Count == 1)
	{
		//ADC_DataArray2[ADC_SampleCount*2] = (uint8_t)(Chip_GPIO_ReadPort(LPC_GPIO, 2) & 0xFF);
		ADC_DataArray[ADC_SampleCount] = (int16_t)((Chip_GPIO_ReadPort(LPC_GPIO, 2) & 0xFF) << 8);
		//ADC_DataArray[ADC_SampleCount] = (int16_t)(Chip_GPIO_ReadPort(LPC_GPIO, 2) & 0xFF);
		ADC_Dataclock_Count++;
	}
	else if(ADC_Dataclock_Count == 3)
	{
		//ADC_DataArray2[ADC_SampleCount*2+1] = (uint8_t)(Chip_GPIO_ReadPort(LPC_GPIO, 2) & 0xFF);
		ADC_DataArray[ADC_SampleCount] |= (int16_t)((Chip_GPIO_ReadPort(LPC_GPIO, 2)) & 0xFF);
		//ADC_DataArray[ADC_SampleCount] |= (int16_t)((Chip_GPIO_ReadPort(LPC_GPIO, 2) & 0xFF) << 8);

		ADC_Dataclock_Count = 0;

		if(ADC_SampleCount == 7)
		{
			//Turn of the timer here
			Chip_TIMER_StopOnMatchEnable(LPC_TIMER2, 0);	//Timer will stop on next interrupt
			//Chip_TIMER_Disable(LPC_TIMER2);
			//Chip_TIMER_ExtMatchControlSet(LPC_TIMER2, 1, TIMER_EXTMATCH_TOGGLE, 0);
			ADC_SampleCount = 0;
			ADC_Status = AD7606_STATUS_DATAREADY;
		}
		else
		{
			ADC_SampleCount++;
		}
	}
	else
	{
		ADC_Dataclock_Count++;
	}

	return;
}

uint8_t AD7606GetStatus(void)
{
	return ADC_Status;
}

void AD7606GetDataSet(uint8_t sel, uint16_t *DataSet)
{
	uint8_t i;

	//printf("start: %u\r\n", Chip_GPIO_ReadDirBit(LPC_GPIO, AD7606_ADSTART_PORT, AD7606_ADSTART_PIN));
	//printf("busy: %u\r\n", Chip_GPIO_ReadDirBit(LPC_GPIO, AD7606_ADBUSY_PORT, AD7606_ADBUSY_PIN));
	//printf("fdata: %u\r\n", Chip_GPIO_ReadDirBit(LPC_GPIO, AD7606_ADFDATA_PORT, AD7606_ADFDATA_PIN));

	//Start conversion
	AD7606_Start(true);

	//Wait for data to become available
	AD7606WaitReady();
	AD7606_Start(false);

	AD7606StartDataClock(AD7606_DATA_CLOCK_HZ);

	//Get data from the parallel interface, this happens in an interrupt
	while(1)
	{
		if(ADC_Status == AD7606_STATUS_DATAREADY)
		{
			break;
		}
	}

	for(i=0; i<8; i++)
	{
		DataSet[i]=ADC_DataArray[i];
	}

/*
	for(i=0; i<8; i++)
	{
		//printf("ADC[%u]: 0x%02X, 0x%02X,", i, ADC_DataArray2[i*2], ADC_DataArray2[i*2+1]);
		//ADC_DataArray[i] = (int16_t)((ADC_DataArray2[i*2] & 0xFF) << 8);
		//ADC_DataArray[i] |= (int16_t)((ADC_DataArray2[i*2+1]) & 0xFF);
		//printf(" %d counts\r\n", ADC_DataArray[i]);
		printf("ADC[%u]: %d counts\r\n", i, ADC_DataArray[i]);
	}
*/
	ADC_Status = AD7606_STATUS_READY;
	return;
}

