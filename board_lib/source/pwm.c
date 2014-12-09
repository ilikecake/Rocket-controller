/*
 * pwm.c
 *
 *  Created on: Dec 1, 2014
 *      Author: Jon
 */

#include "board.h"


#define PWM1_GPIO_PORT_NUM				2
#define PWM1_GPIO_BIT_NUM				5//PWM channel 1.6
#define PWM2_GPIO_PORT_NUM				2
#define PWM2_GPIO_BIT_NUM				4//PWM channel 1.5
#define PWM3_GPIO_PORT_NUM				2
#define PWM3_GPIO_BIT_NUM				3//PWM channel 1.4

void InitPWM(void)
{
	//uint32_t ClkRate;
	//uint32_t FreqInHz;

	//Enable the clock to the PWM module
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);

	//Set the clock divider of the PWM clock
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_PWM1, SYSCTL_CLKDIV_8);

	//Set up pins for PWM output
	Chip_IOCON_PinMux(LPC_IOCON, PWM1_GPIO_PORT_NUM, PWM1_GPIO_BIT_NUM, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, PWM2_GPIO_PORT_NUM, PWM2_GPIO_BIT_NUM, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, PWM3_GPIO_PORT_NUM, PWM3_GPIO_BIT_NUM, IOCON_MODE_INACT, IOCON_FUNC1);

	//setup PWM outputs
	Chip_GPIO_WriteDirBit(LPC_GPIO, PWM1_GPIO_PORT_NUM, PWM1_GPIO_BIT_NUM, true); //PWM channel 4
	Chip_GPIO_WriteDirBit(LPC_GPIO, PWM2_GPIO_PORT_NUM, PWM2_GPIO_BIT_NUM, true); //PWM channel 5
	Chip_GPIO_WriteDirBit(LPC_GPIO, PWM3_GPIO_PORT_NUM, PWM3_GPIO_BIT_NUM, true); //PWM channel 6

	LPC_PWM1->TCR = 0x02;	//Reset PWM timer until this bit is set to zero

	PWM_SetFrequency(50);
	//FreqInHz=50;
	//LPC_PWM1->PR = ClkRate/100/FreqInHz;//set frequency of PWM (uses the max count set below)
	LPC_PWM1->CTCR = 0;		//PWM in timer mode

	LPC_PWM1->MR[0] = 100;	//set max count
	LPC_PWM1->MR4 = 50;		//set duty cycle of PWM4
	LPC_PWM1->MR5 = 50;		//set duty cycle of PWM5
	LPC_PWM1->MR6 = 50;		//set duty cycle of PWM6

	LPC_PWM1->MCR = 2;

	//Update match registers
	LPC_PWM1->LER = 1|(1<<4)|(1<<5)|(1<<6);

	//Enable outputs for PWM 4-6
	LPC_PWM1->PCR = (1<<12)|(1<<13)|(1<<14);

	//LPC_PWM1->TCR = 9;	//Enable PWM counter, enable PWM

	return;
}

void PWM_SetFrequency(uint32_t FreqInHz)
{
	uint32_t ClkRate;
	uint8_t IsRunning = 0;

	//TODO: The divider is set to 8 in the initialization function and not changed here. This gives us an upper limit of ~150kHz at a 120MHz core clock speed. If a higher PWM speed is desired, the prescaler should be changed here.
	//TODO: The frequency to be set is also dependent on the MR[0] value. It is currently hard coded as 100.
	if((FreqInHz > 150000) || (FreqInHz < 1)) return;

	ClkRate = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_PWM1);

	if(LPC_PWM1->TCR == 9)
	{
		IsRunning = 1;
	}

	LPC_PWM1->TCR = 0x02;
	LPC_PWM1->PR = ClkRate/100/FreqInHz;

	if(IsRunning == 1)
	{
		LPC_PWM1->TCR = 9;	//Enable PWM counter, enable PWM
	}

	return;
}

void PWM_Enable(uint8_t state)
{
	if (state==1)
	{
		LPC_PWM1->TCR = 9;	//Enable PWM counter, enable PWM
		Board_LED_Set(1, true);
	}
	else
	{
		LPC_PWM1->TCR = 0x02;	//Reset PWM timer until this bit is set to zero
		Board_LED_Set(1, false);
	}
	return;
}


void PWM_SetDutyCycle(uint8_t PWMNumber, uint8_t DutyCycle)
{
	switch(PWMNumber)
	{
		case 1:
			LPC_PWM1->MR[1] = DutyCycle;
			break;
		case 2:
			LPC_PWM1->MR[2] = DutyCycle;
			break;
		case 3:
			LPC_PWM1->MR[3] = DutyCycle;
			break;
		case 4:
			LPC_PWM1->MR4 = DutyCycle;
			break;
		case 5:
			LPC_PWM1->MR5 = DutyCycle;
			break;
		case 6:
			LPC_PWM1->MR6 = DutyCycle;
			break;
	}

	//Update match register
	LPC_PWM1->LER = (1<<PWMNumber);

	return;
}

