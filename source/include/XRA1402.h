/*
 * XRA1402.h
 *
 *  Created on: Oct 26, 2014
 *      Author: Jon
 */

#ifndef XRA1402_H_
#define XRA1402_H_

#define XRA1402_CS_PORT		2
#define XRA1402_CS_PIN		12

//These are left shifted by one from the datasheet values.
#define XRA1402_REG_GSR		0x00		//GPIO state
#define XRA1402_REG_OCR		0x02		//Output control
#define XRA1402_REG_PIR		0x04		//Input polarity inversion
#define XRA1402_REG_GCR		0x06		//GPIO configuration
#define XRA1402_REG_PUR		0x08		//Input internal pull-up resistor enable/disable
#define XRA1402_REG_IER		0x0A		//Input interrupt enable
#define XRA1402_REG_TSCR	0x0C		//Output three-state control
#define XRA1402_REG_ISR		0x0E		//Input interrupt status
#define XRA1402_REG_REIR	0x10		//Input rising edge interrupt enable
#define XRA1402_REG_FEIR	0x12		//Input falling edge interrupt enable
#define XRA1402_REG_IFR		0x14		//Input filter enable/disable

void XRA1402Init(void);

//Sel = 1 to select, sel=0 to deselect
void XRA1402Select(uint8_t sel);

void XRA1402WriteReg(uint8_t RegisterToWrite, uint8_t RegisterValue);
void XRA1402ReadReg(uint8_t RegisterToRead, uint8_t *RegVal);

//BitsToSet is a bitmask indicating which bits should be set to output
void XRA1402SetupOutput(uint8_t BitsToSet);




#endif /* XRA1402_H_ */
