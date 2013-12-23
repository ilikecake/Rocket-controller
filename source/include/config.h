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
*	\brief		Project specific config options for the standalone timer module
*	\author		Pat Satyshur
*	\version	1.1
*	\date		1/20/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	beer_heater_v1_main
*
*	@{
*/

//Global config options
//Put options here to configure the common modules.

#ifndef _CONFIG_H_
#define _CONFIG_H_

//Setup for the command interpreter
#define COMMAND_USER_CONFIG
#define COMMAND_STAT_SHOW_COMPILE_STRING			0		//Set to 1 to output the compile date/time string in the stat function
#define COMMAND_STAT_SHOW_MEM_USAGE					0		//Set to 1 to show the memory usage in the stat function. NOTE: if this is enabled, the mem_usage.c must be included in the makefile

//Based on the setup above
#if COMMAND_STAT_SHOW_COMPILE_STRING == 1
	extern const char fwCompileDate[] PROGMEM;				//The compile date/time string. This must be a string in flash called fwCompileDate.
#endif

#if COMMAND_STAT_SHOW_MEM_USAGE == 1
	#include "mem_usage.h"									//The header that contains StackCount()
#endif

//Config for the TWI module
#define	TWI_USER_CONFIG							//Define this in your user code to allow use of the TWI module
#undef	TWI_USE_ISR								//Define this to enable interrupt driven TWI interface (this code does not work).
//#define	_TWI_DEBUG								//Define this to enable the output of debug messages.
#undef	TWI_USE_INTERNAL_PULLUPS				//Define this to use the internal pull-up resistors of the device.
#define	TWI_SCL_FREQ_HZ				100000		//Set the SCL frequency

#endif

/** @} */
