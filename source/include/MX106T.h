

#include "main.h"

#define SERVO_INST_none 		0x0
#define SERVO_INST_PING 		0x1
#define SERVO_INST_READ_DATA 	0x2
#define SERVO_INST_WRITE_DATA	0x3
#define SERVO_INST_REG_WRITE 	0x4  //sends data to buffer and awaits Action Instruction
#define SERVO_INST_ACTION 		0x5  //commands registerd action to occur (can start action on all servos at once)
#define SERVO_INST_RESET 		0x6		//simultaneously write to several servos
#define SERVO_INST_SYNC_WRITE 	0x83

#define SERVO_BPS_100000		1
#define SERVO_BPS_500000		3
#define SERVO_BPS_400000		4
#define SERVO_BPS_250000		7
#define SERVO_BPS_200000		9
#define SERVO_BPS_117647		16
#define SERVO_BPS_57142			34
#define SERVO_BPS_19230			103
#define SERVO_BPS_9615			207

#define SERVO_MODEL_NUMBER_16		0X00
#define SERVO_FIRMWARE_VERSION_8	0X02
#define SERVO_ID_8					0X03
#define SERVO_BAUD_RATE_8			0X04
#define SERVO_RETURN_DELAY_8		0X05
#define SERVO_CW_ANGLE_LIMIT_16		0X06
#define SERVO_CCW_ANGLE_LIMIT_16	0X08
#define SERVO_DRIVE_MODE_8			0X0A
#define SERVO_TEMPERATURE_LIMIT_8	0X0B
#define SERVO_VOLTAGE_LIMIT_LOW_8	0X0C
#define SERVO_VOLTAGE_LIMIT_HIGH_8	0X0D
#define SERVO_TORQUE_MAX_16			0X0E
#define SERVO_STATUS_RETURN_LEVEL_8	0X10
#define SERVO_ALARM_LED_8			0X11
#define SERVO_ALARM_SHUTDOWN_8		0X12
#define SERVO_MULTI_TURN_OFFSET_16	0X14
#define SERVO_RESOLUTION_DIVIDER_8	0X16//?
#define SERVO_TORQUE_ENABLE_8		0X18
#define SERVO_LED_8					0X19
#define SERVO_D_GAIN_8				0X1A
#define SERVO_I_GAIN_8				0X1B
#define SERVO_P_GAIN_8				0X1C
#define SERVO_GOAL_POSITION_16		0X1E
#define SERVO_GOAL_SPEED_16			0X20
#define SERVO_TORQUE_LIMIT_16		0X22
#define SERVO_PRESENT_POSITION_16	0X24//0 to 0xFFF, unit of .088deg (in multi-turn mode range is -28672 to 28672 with resolution of .088/ResolutionDivider
#define SERVO_PRESENT_SPEED_16		0X26//first 9 bits are rotation rate (units of ~11.4rpm), 10th bit is direction (0=CCW, 1=CW),1024=0
#define SERVO_PRESENT_LOAD_16		0X28//first 9 bits are 0 to max torque, 10th bit is direction (0=CCW, 1=CW),1024=0
#define SERVO_PRESENT_VOLTAGE_8		0X2A
#define SERVO_PRESENT_TEMPERATURE_8	0X2B//Internal temperature (unit is 1 degree C)
#define SERVO_REGISTERED_8			0X2C//Means if instruction is registered
#define SERVO_MOVING_8				0X2D//Means if there is any movement
#define SERVO_LOCK_8				0X2F//Locking EEPROM
#define SERVO_PUNCH_16				0X30
#define SERVO_CURRENT_16			0X44//Current consumption.  I = 0.45mA*(CURRENT-2048) amps
#define SERVO_TORQUE_CONTROL_8		0X46
#define SERVO_GOAL_TORQUE_16		0X47
#define SERVO_GOAL_ACCELERATION_8	0X49

#define SERVO_DEADBAND	10	//milliseconds it takes to finish processing servo requests
//#define SERVO_UART LPC_UART0

void MX106T_Init(void);

uint8_t MX106T_Ping(uint8_t servoID);//returns true if servo with servoID exists

void MX106T_SetState(uint8_t State);	//1 to transmit, 0 to recieve

uint8_t MX106T_Send(uint8_t *parameters, uint8_t outLength, uint8_t *inputBuffer, uint8_t inLength);

//allow servo to rotate freely
void MX106T_SetWheelMode(uint8_t servoIDl);

//set a servo property that has only one byte
uint8_t MX106T_Set8bit(uint8_t servoID, uint8_t address, uint8_t value);

//set a 2 byte servo property
uint8_t MX106T_Set16bit(uint8_t servoID, uint8_t address, uint16_t value);

//read a servo property that is 1 byte
uint8_t MX106T_Read8bit(uint8_t servoID, uint8_t address);

//read a servo property that is 2 bytes
uint16_t MX106T_Read16bit(uint8_t servoID, uint8_t address);

