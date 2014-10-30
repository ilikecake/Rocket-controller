//Communication with the MX-106T servo motor

#include "main.h"

//allow servo to rotate freely
void MX106T_SetWheelMode(uint8_t servoIDl)
{
	//to enable wheel mode the CW angle limit and CCW angle limit must both be set to 0
	MX106T_Set16bit(1,SERVO_CW_ANGLE_LIMIT_16,0);
	MX106T_Set16bit(1,SERVO_CCW_ANGLE_LIMIT_16,0);
	return;
}

//set a servo property that has only one byte
uint8_t MX106T_Set8bit(uint8_t servoID, uint8_t address, uint8_t value)
{
	uint8_t parameters[5];
	uint8_t statusPacket[4];
	uint8_t outlength;
	outlength = 4;				//set number of input parameters

	parameters[0] = servoID;	//use 0xFE to command all connected servos
	parameters[1] = outlength;	//parameter length
	parameters[2] = SERVO_INST_WRITE_DATA;
	parameters[3] = address;	//address at which to write data
	parameters[4] = value;		//data to write

	MX106T_Send(parameters, outlength, statusPacket, 3);
	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=checksum
	return statusPacket[2];
}

//set a 2 byte servo property
uint8_t MX106T_Set16bit(uint8_t servoID, uint8_t address, uint16_t value)
{
	uint8_t parameters[6];
	uint8_t statusPacket[4];
	uint8_t outlength;
	outlength = 5;			//set number of input parameters

	parameters[0] = servoID;					//use 0xFE to command all connected servos
	parameters[1] = outlength;					//parameter length
	parameters[2] = SERVO_INST_WRITE_DATA;
	parameters[3] = address;					//address at which to write data
	parameters[4] = (uint8_t)(value & 0xFF);	//write Low byte of data
	parameters[5] = (uint8_t)(value>>8);		//write High byte of data

	MX106T_Send(parameters, outlength, statusPacket, 3);
	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=checksum
	return statusPacket[2];
}

//read a servo property that is 1 byte
uint8_t MX106T_Read8bit(uint8_t servoID, uint8_t address)
{
	uint8_t parameters[5];
	uint8_t statusPacket[5];
	uint8_t outlength;

	outlength = 4;//set number of input parameters
	parameters[0] = servoID;
	parameters[1] = outlength;//parameter length
	parameters[2] = SERVO_INST_READ_DATA;
	parameters[3] = address;	//address of data on servo register
	parameters[4] = 0x01;	//bytes of data to read

	MX106T_Send(parameters, outlength, statusPacket, 4);
	//_statusPacket[0]=ID
	//_statusPacket[1]=length
	//_statusPacket[2]=error
	//_statusPacket[3]=data
	//_statusPacket[4]=checksum
	return statusPacket[3];
}

//read a servo property that is 2 bytes
uint16_t MX106T_Read16bit(uint8_t servoID, uint8_t address)
{
	uint8_t parameters[5];
	uint8_t statusPacket[6];
	uint8_t outlength;

	outlength = 2+2;//set number of input parameters
	parameters[0] = servoID;
	parameters[1] = outlength;//parameter length
	parameters[2] = SERVO_INST_READ_DATA;
	parameters[3] = 0x00;	//address of data on servo register
	parameters[4] = 0x02;	//bytes of data to read

	MX106T_Send(parameters, outlength, statusPacket, 5);
	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=model# L
	//statusPacket[4]=model# H
	//statusPacket[5]=checksum
	return ((uint16_t)statusPacket[3]) + (((uint16_t)statusPacket[4]) << 8);
}


void MX106T_Send(uint8_t* parameters, uint8_t outLength, uint8_t *inputBuffer, uint8_t inLength){
	//IP_UART_ID_T servoUARTchannel;
	uint8_t data;
	uint8_t checksum;
	uint8_t i;
	uint32_t timeout; //watchdog time //portTickType
	uint32_t currentTime;
	//servoUARTchannel=LPC_UART0;

	//fill in checksum for this set of output parameters
	checksum = 0;
	for(i=0;i<=outLength;i++)
	{
		checksum = checksum + parameters[i];
	}
	checksum = ~checksum;
	
	if (SERVO_UART==DEBUG_UART)//check for interference from DEBUG_UART port
	{
		vTaskSuspend( vUARTTask );//suspend UART task until servo is done with UART
	}
	
	UART_RTSConfig(SERVO_UART,1);//set Ready To Send

	sendSerialUint8(0xFF,SERVO_UART);
	sendSerialUint8(0xFF,SERVO_UART);
	for(i=0;i<=outLength;i++)
	{
		sendSerialUint8(parameters[i],SERVO_UART);
	}
	sendSerialUint8(checksum,SERVO_UART);

	UART_RTSConfig(SERVO_UART,0);//clear Ready To Send

	if (parameters[0] != 0xFE)//0xFE means the command was sent to all servos.  no response will be returne
	{
		//vTaskDelay(configTICK_RATE_HZ*.004 );//wait ~4ms for servo to start responding


		//listen for response.  This should happen on separate interrupt
		i=0;
		timeout = (uint32_t)xTaskGetTickCount() + configTICK_RATE_HZ/25;//set watchdog time for 40ms
		while (i<2)
		{
			if (Chip_UART_ReceiveByte(DEBUG_UART, &data) == SUCCESS) {
				if(i<2){//wait for 2 0xFF bytes to be recieved
					if(data==0xFF) i+=1;
				}
			}
			currentTime=xTaskGetTickCount() ;
			if (currentTime> timeout) i=3;//exit loop if timeout has occurred
		}


		i=0;
		timeout = xTaskGetTickCount() + configTICK_RATE_HZ/25; //set watchdog time for 40ms
		while (i<inLength)
		{

			if (Chip_UART_ReceiveByte(DEBUG_UART, &data) == SUCCESS){
				inputBuffer[i] = data;
				i+=1;
			}
			currentTime=xTaskGetTickCount() ;
			if (currentTime > timeout) i=inLength+1;//exit loop if timeout has occurred

		}


		if (SERVO_UART==DEBUG_UART)//check for interference from DEBUG_UART port
		{
			vTaskResume( vUARTTask );//resume UART task when servo is done with UART port
		}
	}

}
