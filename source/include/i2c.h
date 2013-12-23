/*
 * i2c.h
 *
 *  Created on: Oct 3, 2013
 *      Author: pat
 */

#ifndef I2C_H_
#define I2C_H_


#define DEFAULT_I2C          I2C0

#define I2C_EEPROM_BUS       DEFAULT_I2C
#define I2C_IOX_BUS          DEFAULT_I2C

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000









void i2c_set_mode(I2C_ID_T id, int polling);
void i2c_app_init(I2C_ID_T id, int speed);


#endif /* I2C_H_ */
