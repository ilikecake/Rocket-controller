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
*	\brief		Command interpreter header file
*	\author		Pat Satyshur
*	\version	1.2
*	\date		1/17/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	interface
*
*	@{
*/

#ifndef _COMMAND_H_
#define _COMMAND_H_

//Includes
#include "stdint.h"
#include "config.h"

#ifndef COMMAND_USER_CONFIG
	#error: Command interpreter settings not defined. See command.h for details.
#endif

/*These setting must be defined in your user code to use the command interpreter module
 * #define COMMAND_USER_CONFIG								//Define this in your user code to disable the above error.
 * #define COMMAND_STAT_SHOW_COMPILE_STRING			1		//Set to 1 to output the compile date/time string in the stat function					
 * #define COMMAND_STAT_SHOW_MEM_USAGE				1		//Set to 1 to show the memory usage in the stat function. NOTE: if this is enabled, the mem_usage.c must be included in the makefile
 * 
 * //Based on the setup above
 * #if COMMAND_STAT_SHOW_COMPILE_STRING == 1
 * extern const char fwCompileDate[] PROGMEM;				//The compile date/time string. This must be a string in flash called fwCompileDate.
 * #endif
 * 
 * #if COMMAND_STAT_SHOW_MEM_USAGE == 1
 * #include "mem_usage.h"									//The header that contains StackCount()
 * #endif
 */

#define COMMAND_MAX_DISPLAY_LENGTH	10
#define COMMAND_MAX_DISPLAY_LENGTH_S	"%-10s"

//The structure for the command list item, all commands must have all of these elements
//These functions are included in command.c for the universal functions, and commands.h for application specific commands
typedef struct CommandListItem 
{
	/**The name typed on the command line to invoke the command*/
	const char *name;
	
	/**The minimum number of arguments allowable*/
	int minArgs;
	
	/**The maximum number of arguments allowable*/
	int maxArgs;	

	/**The pointer to the command function*/
	int (*handler) (void);
	
	/**Text displayed when requesting the list of commands*/
	const char *DescText;
	
	/**Text displayed when requesting help on this command*/
	const char *HelpText;
} CommandListItem;

//Public functions

/**Returns the number of arguments entered for the command. If the command has no arguments, this function will return 0*/
uint8_t NumberOfArguments( void );

int32_t argAsInt(uint8_t argNum);

/**Access argument argNum as a string
*	\param[in]	argNum The argument number. Argument numbers start at 1. Setting argNum to 0 will give the command name.
*	\param[out]	ArgString A pointer to the string to put the resulting argument.
*/
void argAsChar(uint8_t argNum, char *ArgString);

/** Sends a single character to the command interpreter. This function should be called by when receiveing a character from a source such as USB or an UART port. 
*	Based on the configuration, the commands may also be run inside this function. This may be a problem if this function is run inside an interrupt.
*	This command will probably have to be run asynchronously for it to work with subcommands.
*		 \param[in]	c The received character.
*/
void CommandGetInputChar(uint8_t c);

/** Checks to see if a command is waiting, and if so, runs the command. This function can be continuously called (EX: in the main loop). */
void RunCommand( void );

//call this inside a running command to get a new user input. This will clear out the old command and arguments.
//TODO: Give this function a better name.
void GetNewCommand( void );

char WaitForAnyKey( void );

#endif
/** @} */
