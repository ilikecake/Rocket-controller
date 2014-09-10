/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Header file for the MAX31855 Thermocouple converter.
*	\author		Jonathan McCabe
*	\version	1.0
*	\date		12/01/2012
*	\copyright	Copyright 2012, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _MAX31855_H_
#define _MAX31855_H_
#include "main.h"

#define MAX31855_CS_PORT		0x00
#define MAX31855_CS1_PIN		17
#define MAX31855_CS2_PIN		18
#define MAX31855_CS3_PIN		19
#define MAX31855_CS4_PIN		20


void MAX31855Init(void);
void MAX31855Select( uint8_t sel );

uint16_t MAX31855read( uint8_t sel, uint16_t *coldJunction );

#endif
