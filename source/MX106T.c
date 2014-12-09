//Communication with the MX-106T servo motor

#include "main.h"

//Local defines for the servo
#define SERVO_RX_TX_PORT	0
#define SERVO_RX_TX_PIN		17

#define SERVO_RX_PORT		0
#define SERVO_RX_PIN		11
#define SERVO_TX_PORT		0
#define SERVO_TX_PIN		10


void MX106T_Init(void)
{
	//initialize the UART pins for the servo
	Chip_IOCON_PinMux(LPC_IOCON, SERVO_RX_PORT,	SERVO_RX_PIN,	IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, SERVO_TX_PORT,	SERVO_TX_PIN,	IOCON_MODE_INACT, IOCON_FUNC1);

	//Initialize the RX/TX pin and set it low
	Chip_IOCON_PinMux(LPC_IOCON, SERVO_RX_TX_PORT,	SERVO_RX_TX_PIN,	IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_GPIO_WriteDirBit(LPC_GPIO, SERVO_RX_TX_PORT,	SERVO_RX_TX_PIN,	true);
	Chip_GPIO_WritePortBit(LPC_GPIO, SERVO_RX_TX_PORT, SERVO_RX_TX_PIN, false);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(SERVO_UART);
	Chip_UART_SetBaud(SERVO_UART, SERVO_BAUD);// 115200);//
	Chip_UART_ConfigData(SERVO_UART, UART_DATABIT_8, UART_PARITY_NONE, UART_STOPBIT_1);

	/* Enable UART Transmit */
	Chip_UART_TxCmd(SERVO_UART, ENABLE);

	return;
}



uint8_t MX106T_Ping(uint8_t servoID)
{//returns 1 if servo with servoID exists
	uint8_t result;
	uint8_t msg = 0;//error message
	/*uint8_t parameters[5];
	uint8_t statusPacket[3];
	uint8_t outlength;

	outlength = 2;				//set number of input parameters

	parameters[0] = servoID;	//use 0xFE to command all connected servos
	parameters[1] = outlength;	//parameter length
	parameters[2] = SERVO_INST_PING;

	result=MX106T_Send(parameters, outlength, statusPacket,3);

	if (result==1)
	{return 1;}
	else
	{return 0;}
	*/

	result=MX106T_Read8bit(servoID,SERVO_ID_8, &msg);

	if ((result==servoID) && (msg==1))
	{return 1;}//servoID detected
	else
	{return 0;}//servoID not detected
}


//Set the RX/TX line low to enable transmit mode
//State = 0 for transmit
void MX106T_SetState(uint8_t State)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, SERVO_RX_TX_PORT, SERVO_RX_TX_PIN, State);
}



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
{//returns 0 if no errors occurred
	uint8_t parameters[5];
	uint8_t statusPacket[4];
	uint8_t outlength;
	uint8_t stat;

	outlength = 4;				//set number of input parameters

	parameters[0] = servoID;	//use 0xFE to command all connected servos
	parameters[1] = outlength;	//parameter length
	parameters[2] = SERVO_INST_WRITE_DATA;
	parameters[3] = address;	//address at which to write data
	parameters[4] = value;		//data to write

	statusPacket[0]=0xFF;
	statusPacket[1]=0xFF;
	statusPacket[2]=0xFF;
	statusPacket[3]=0xFF;

	stat=MX106T_Send(parameters, outlength, statusPacket, 3); //3
	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=checksum

	if (stat==1)//successful send
	{
		return statusPacket[2];//return error message
	}
	else if (stat>0)
	{
		return stat;//error occurred in message delivery
	}
	else
	{
		return 2;
	}
}

//set a 2 byte servo property
uint8_t MX106T_Set16bit(uint8_t servoID, uint8_t address, uint16_t value)
{//returns 0 if no errors occurred
	uint8_t parameters[6];
	uint8_t statusPacket[4];
	uint8_t outlength;
	uint8_t stat;

	outlength = 5;			//set number of input parameters

	parameters[0] = servoID;					//use 0xFE to command all connected servos
	parameters[1] = outlength;					//parameter length
	parameters[2] = SERVO_INST_WRITE_DATA;
	parameters[3] = address;					//address at which to write data
	parameters[4] = (uint8_t)(value & 0xFF);	//write Low byte of data
	parameters[5] = (uint8_t)(value>>8);		//write High byte of data

	stat=MX106T_Send(parameters, outlength, statusPacket, 3);
	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=checksum

	if (stat==1)//successful send
	{
		return statusPacket[2];//return error message
	}
	else if (stat>0)
	{
		return stat;//error occurred in message delivery
	}
	else
	{
		return 2;
	}
}

//read a servo property that is 1 byte
uint8_t MX106T_Read8bit(uint8_t servoID, uint8_t address, uint8_t *msg)
{//returns data, msg = 1 if no error has occurred
	uint8_t parameters[5];
	uint8_t statusPacket[5];
	uint8_t outlength;

	outlength = 4;//set number of input parameters
	parameters[0] = servoID;
	parameters[1] = outlength;//parameter length
	parameters[2] = SERVO_INST_READ_DATA;
	parameters[3] = address;	//address of data on servo register
	parameters[4] = 0x01;	//bytes of data to read

	statusPacket[0]=0xFF;
	statusPacket[1]=0xFF;
	statusPacket[2]=0xFF;
	statusPacket[3]=0xFF;
	statusPacket[4]=0xFF;

	*msg = MX106T_Send(parameters, outlength, statusPacket, 4);

	//printf("Stat: 0x%02X\r\n", *msg);
	//_statusPacket[0]=ID
	//_statusPacket[1]=length
	//_statusPacket[2]=error
	//_statusPacket[3]=data
	//_statusPacket[4]=checksum
	if(*msg == 1)
	{//data was read successfully
		return statusPacket[3];
	}
	else
	{//error occurred
		return 0;
	}
}

//read a servo property that is 2 bytes
uint16_t MX106T_Read16bit(uint8_t servoID, uint8_t address, uint8_t *msg)
{//returns 16bit data, msg = 1 if no error has occurred
	uint8_t parameters[5];
	uint8_t statusPacket[6];
	uint8_t outlength;

	outlength = 2+2;//set number of input parameters
	parameters[0] = servoID;
	parameters[1] = outlength;//parameter length
	parameters[2] = SERVO_INST_READ_DATA;
	parameters[3] = address;	//address of data on servo register
	parameters[4] = 0x02;	//bytes of data to read

	statusPacket[0]=0xFF;
	statusPacket[1]=0xFF;
	statusPacket[2]=0xFF;
	statusPacket[3]=0xFF;
	statusPacket[4]=0xFF;
	statusPacket[5]=0xFF;

	*msg=MX106T_Send(parameters, outlength, statusPacket, 5);

	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=model# L
	//statusPacket[4]=model# H
	//statusPacket[5]=checksum
	if (*msg==1)
	{//data was read successfully
		return ((uint16_t)statusPacket[3]) + (((uint16_t)statusPacket[4]) << 8);
	}
	else
	{//error occurred
		return 0;
	}

}


uint8_t MX106T_Send(uint8_t* parameters, uint8_t outLength, uint8_t *inputBuffer, uint8_t inLength){
	//return 1 if successful, return 0xFD if unable to get semaphore

	uint8_t data;
	uint16_t checksum;
	uint8_t notcheck;
	uint8_t i;
	uint32_t timeout; //watchdog time //portTickType
	uint32_t currentTime;

	if( xSemaphoreTake( servoSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take servo transmit semaphore
	{
		//fill in checksum for this set of output parameters
		checksum = 0;
		//printf("To Servo: ");
		for(i=0;i<=outLength;i++)
		{
			checksum = checksum + parameters[i];
			//printf("0x%02X: ",  parameters[i]);

		}
		notcheck = ~(checksum & 0xFF);
		//printf("0x%02X\r\n", notcheck);

		MX106T_SetState(0);

		sendSerialUint8(0xFF,SERVO_UART);
		sendSerialUint8(0xFF,SERVO_UART);
		for(i=0;i<=outLength;i++)
		{
			sendSerialUint8(parameters[i],SERVO_UART);
		}
		sendSerialUint8(notcheck,SERVO_UART);

		while(Chip_UART_CheckBusy(SERVO_UART) == SET)
		{
			//vTaskDelay(1);
		}

		//wait for a short period
		for(i=0;i<5;i++){}

		//the RX/TX lines must switch as fast as possible after SERVO_UART send is complete
		MX106T_SetState(1);


		if (parameters[0] != 0xFE)//0xFE means the command was sent to all servos.  no response will be returned
		{
			//listen for response.  This should happen on separate interrupt
			i=0;
			timeout = (uint32_t)xTaskGetTickCount() + configTICK_RATE_HZ /20;	//set watchdog time for 50ms
			while (i<2)
			{
				vTaskDelay(3);//pause before checking again
				if (Chip_UART_ReceiveByte(SERVO_UART, &data) == SUCCESS) {
					if(i<2){//wait for 2 0xFF bytes to be recieved
						if(data==0xFF) i+=1;
					}
				}
				currentTime= (uint32_t)xTaskGetTickCount() ;
				if (currentTime> timeout)
				{
					xSemaphoreGive(servoSemaphore);//give back servo semaphore
					return 0xFE; //i=3;//exit loop if timeout has occurred
				}
			}

			i=0;
			timeout = xTaskGetTickCount() + configTICK_RATE_HZ/20; //set watchdog time for 50ms
			while (i<=inLength)
			{
				vTaskDelay(3);//pause before checking again
				if (Chip_UART_ReceiveByte(SERVO_UART, &data) == SUCCESS){
					inputBuffer[i] = data;
					i+=1;
				}
				currentTime= (uint32_t)xTaskGetTickCount() ;
				if (currentTime > timeout)
				{
					xSemaphoreGive(servoSemaphore);//give back servo semaphore
					return 0xFF;
				}
			}

			/*printf("From Servo: ");
			for (i=0;i<=inLength;i++)
			{
				printf("0x%02X: ", inputBuffer[i]);
			}
			printf("\r\n");*/


		}

		xSemaphoreGive(servoSemaphore);//give back servo semaphore
		return 1;
	}
	else
	{//Unable to get servo semaphore
		return 0xFD;
	}
}
