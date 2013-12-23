/*
 * @brief 11xx 16/32-bit Timer/PWM control functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Returns the clock from the speicifc timer base address */
static CHIP_SYSCTL_CLOCK_T Chip_Timer_GetClock(LPC_TIMER_T *pTMR)
{
	CHIP_SYSCTL_CLOCK_T clk;

	if (pTMR == LPC_TIMER32_0) {
		clk = SYSCTL_CLOCK_CT32B0;
	}
	else if (pTMR == LPC_TIMER32_0) {
		clk = SYSCTL_CLOCK_CT32B0;
	}
	else if (pTMR == LPC_TIMER16_0) {
		clk = SYSCTL_CLOCK_CT16B0;
	}
	else {
		clk = SYSCTL_CLOCK_CT16B1;
	}

	return clk;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize a timer */
void Chip_TIMER_Init(LPC_TIMER_T *pTMR)
{
	/* Enable timer clock */
	Chip_Clock_EnablePeriphClock(Chip_Timer_GetClock(pTMR));
}

/*	Shutdown a timer */
void Chip_TIMER_DeInit(LPC_TIMER_T *pTMR)
{
	/* Disable timer clock */
	Chip_Clock_DisablePeriphClock(Chip_Timer_GetClock(pTMR));
}
