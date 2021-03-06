/*
 * @brief	32-bit Timer/PWM control functions
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

#include "timer_001.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Resets the timer terminal and prescale counts to 0 */
void IP_TIMER_Reset(IP_TIMER_001_T *pTimer)
{
	uint32_t reg;

	/* Disable timer, set terminal count to non-0 */
	reg = pTimer->TCR;
	pTimer->TCR = 0;
	pTimer->TC = 1;

	/* Reset timer counter */
	pTimer->TCR = TIMER_RESET;

	/* Wait for terminal count to clear */
	while (pTimer->TC != 0) {}

	/* Restore timer state */
	pTimer->TCR = reg;
}

/* Sets external match control (MATn.matchnum) pin control */
void IP_TIMER_ExtMatchControlSet(IP_TIMER_001_T *pTimer, int8_t initial_state,
								 IP_TIMER_PIN_MATCH_STATE_T matchState, int8_t matchnum)
{
	pTimer->EMR = (((uint32_t) initial_state) << matchnum) |
				  (((uint32_t) matchState) << (4 + (matchnum * 2)));
}

/* Sets timer count source and edge with the selected passed from CapSrc */
/* NOTE: this function appeared to be named incorrectly. The original declaration is
 * 		void IP_TIMER_SetTimerClockSrc(IP_TIMER_001_T *pTimer, IP_TIMER_CAP_SRC_STATE_T capSrc, int8_t capnum)
 */
void IP_TIMER_SetCountClockSrc(IP_TIMER_001_T *pTimer,
							   IP_TIMER_CAP_SRC_STATE_T capSrc, int8_t capnum)
{
	pTimer->CTCR = (uint32_t) capSrc | ((uint32_t) capnum) << 2;
}
