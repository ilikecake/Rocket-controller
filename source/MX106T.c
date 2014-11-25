//Communication with the MX-106T servo motor

#include "main.h"

//Local defines for the servo
#define SERVO_RX_TX_PORT	0
#define SERVO_RX_TX_PIN		17
#define SERVO_UART			LPC_UART2

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
	Chip_UART_SetBaud(SERVO_UART, 57600);// 115200);//
	Chip_UART_ConfigData(SERVO_UART, UART_DATABIT_8, UART_PARITY_NONE, UART_STOPBIT_1);

	/* Enable UART Transmit */
	Chip_UART_TxCmd(SERVO_UART, ENABLE);

	return;
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

	MX106T_Send(parameters, outlength, statusPacket, 3); //3
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
	uint8_t stat;

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

	stat = MX106T_Send(parameters, outlength, statusPacket, 4);

	printf("Stat: 0x%02X\r\n", stat);
	//_statusPacket[0]=ID
	//_statusPacket[1]=length
	//_statusPacket[2]=error
	//_statusPacket[3]=data
	//_statusPacket[4]=checksum
	//if(stat == 0)
	//{
		return statusPacket[3];
	//}
	//else
	//{
	//	return stat;
	//}
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
	parameters[3] = address;	//address of data on servo register
	parameters[4] = 0x02;	//bytes of data to read

	statusPacket[0]=0xFF;
	statusPacket[1]=0xFF;
	statusPacket[2]=0xFF;
	statusPacket[3]=0xFF;
	statusPacket[4]=0xFF;
	statusPacket[5]=0xFF;

	MX106T_Send(parameters, outlength, statusPacket, 5);

	//statusPacket[0]=ID
	//statusPacket[1]=length
	//statusPacket[2]=error
	//statusPacket[3]=model# L
	//statusPacket[4]=model# H
	//statusPacket[5]=checksum
	return ((uint16_t)statusPacket[3]) + (((uint16_t)statusPacket[4]) << 8);
}


uint8_t MX106T_Send(uint8_t* parameters, uint8_t outLength, uint8_t *inputBuffer, uint8_t inLength){
	//IP_UART_ID_T servoUARTchannel;
	uint8_t data;
	uint16_t checksum;
	uint8_t notcheck;
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
	notcheck = ~(checksum & 0xFF);
	

	MX106T_SetState(0);
	//UART_RTSConfig(SERVO_UART,1);//set Ready To Send

	sendSerialUint8(0xFF,SERVO_UART);
	sendSerialUint8(0xFF,SERVO_UART);
	for(i=0;i<=outLength;i++)
	{
		sendSerialUint8(parameters[i],SERVO_UART);
	}
	sendSerialUint8(notcheck,SERVO_UART);

	while(Chip_UART_CheckBusy(SERVO_UART) == SET)
	{
		vTaskDelay(2);
	}

	//timeout = (uint32_t)xTaskGetTickCount() + configTICK_RATE_HZ/400;//set watchdog time for 20ms
	//while (currentTime > timeout) {
	//	currentTime=xTaskGetTickCount();
	//}
	vTaskDelay(1);//configTICK_RATE_HZ*.001 );//wait ~4ms for servo to start responding

	MX106T_SetState(1);

	/*for (i=0;i<=outLength;i++)
	{
		printf("OutputByte %u: 0x%02X\r\n",i, parameters[i]);
	}
	printf("Output Checksum: 0x%02X\r\n", notcheck);
*/


	if (parameters[0] != 0xFE)//0xFE means the command was sent to all servos.  no response will be returned
	{
		//printf("waiting for response\r\n");
		//vTaskDelay(configTICK_RATE_HZ*.003 );//wait ~4ms for servo to start responding

		//listen for response.  This should happen on separate interrupt
		i=0;
		timeout = (uint32_t)xTaskGetTickCount() + configTICK_RATE_HZ/2;//set watchdog time for 40ms
		while (i<2)
		{
			vTaskDelay(3);//pause before checking again
			if (Chip_UART_ReceiveByte(SERVO_UART, &data) == SUCCESS) {
				if(i<2){//wait for 2 0xFF bytes to be recieved
					if(data==0xFF) i+=1;
				}
			}
			currentTime= (uint32_t)xTaskGetTickCount() ;
			if (currentTime> timeout) return 0xFE; //i=3;//exit loop if timeout has occurred
		}

		i=0;
		timeout = xTaskGetTickCount() + configTICK_RATE_HZ/2; //set watchdog time for 40ms
		while (i<inLength)
		{
			vTaskDelay(3);//pause before checking again
			if (Chip_UART_ReceiveByte(SERVO_UART, &data) == SUCCESS){
				inputBuffer[i] = data;
				i+=1;
			}
			currentTime= (uint32_t)xTaskGetTickCount() ;
			if (currentTime > timeout) return 0xFF;
		}

		/*for (i=0;i<=inLength;i++)
		{
			printf("InputByte %u: 0x%02X\r\n",i, inputBuffer[i]);
		}*/


	}

	return 0;
}
