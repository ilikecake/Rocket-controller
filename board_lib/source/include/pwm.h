/*
 * pwm.h
 *
 *  Created on: Dec 1, 2014
 *      Author: Jon
 */

#ifndef PWM_H_
#define PWM_H_

#include "sys_config.h"
#include "cmsis.h"

#define LPC_PWM1				((IP_PWM_001_T *) LPC_PWM1_BASE)

/**
 * @brief 32-bit PWM register block structure. See manual p515 for register definitions
 */
typedef struct {					/*!< PWM Structure       */
	__IO uint32_t IR;				/*!< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending. */
	__IO uint32_t TCR;				/*!< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR. */
	__IO uint32_t TC;				/*!< Timer Counter. The 32 bit TC is incremented every PR+1 cycles of PCLK. The TC is controlled through the TCR. */
	__IO uint32_t PR;				/*!< Prescale Register. The Prescale Counter (below) is equal to this value, the next clock increments the TC and clears the PC. */
	__IO uint32_t PC;				/*!< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
	__IO uint32_t MCR;				/*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
	__IO uint32_t MR[4];			/*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t CCR;				/*!< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
	__IO uint32_t CR[4];			/*!< Capture Register. CR is loaded with the value of TC when there is an event on the CAPn.0 input. */
	__I  uint32_t RESERVED0;
	__IO uint32_t MR4;
	__IO uint32_t MR5;
	__IO uint32_t MR6;
	__IO uint32_t PCR;
	__IO uint32_t LER;
	__I  uint32_t RESERVED1[7];
	__IO uint32_t CTCR;
} IP_PWM_001_T;

typedef IP_PWM_001_T LPC_PWM_T;

void InitPWM(void);

void PWM_SetFrequency(uint32_t FreqInHz);

//Duty cycle is 0-100
void PWM_SetDutyCycle(uint8_t PWMNumber, uint8_t DutyCycle);

void PWM_Enable(uint8_t state);

uint8_t PWM_GetState();

#endif /* PWM_H_ */
