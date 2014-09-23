
#include "main.h"


void InitControl(void)
{
	int i;
	int channel;

	//setup the fire sequence
	commandMax=MAX_COMMANDS-1;
	
	//initialize command sequence to all off all the time
	for(i=0;i<=MAX_COMMANDS;i++)
	{
		//Timing
		commandTime[i] = i*1000;//milliseconds

		for(channel=0;channel<=TOTAL_DO_CHANNELS;channel++)
		{
			DO_Command[i][channel] = 0;
		}

		for(channel=0;channel<=TOTAL_SERVO_CHANNELS;channel++)
		{
			Servo_Command[i][channel] = 0;
		}
	}

	
	//initialize redlines to all off all the time
	for(i=0;i<=MAX_REDLINES;i++)
	{
		redlineTimeStart[i] = 0;//time at which redline is activated
		redlineTimeEnd[i] = -1;	//time at which redline is deactivated
		redlineChannel[i] = 0;	//channel sampled by redline  (TC channels are given as TC# + TOTAL_AI_CHANNELS)
		redlineMax[i] = 0;		//channel must be below this or the test will be aborted
		redlineMin[i] =0;		//channel must be above this or the test will be aborted
	}


	activeEstop=1;		//redlines active
	runningControl = 0;		//not running a test sequence
	runningData = 0;		//not reading data from TC chip and A/D
	activeSaveData=0;	//not saving data to flash memory


	//give values to servo position
	for (i=0;i<TOTAL_SERVO_CHANNELS;i++)
	{
		ServoPosition[i] = 0;
	}

	return;
	
}

void UpdateCommand(uint32_t tNow)
{
	uint8_t i;
	uint8_t g;
	g=0;
	for (i=0;i<TOTAL_DO_CHANNELS;i++)
	{
		if (g==0)
		{g=1;}
		else if (g==1)
		{g=0;}

		Board_DO_Set(i, g);//set all outputs to 0
	}

	/*
	if (runningControl==1)
	{
		if (commandNum<=commandMax)
		{
			//wait for next command time
			if (commandTime[commandNum] <= tNow-tControl)
			{
				Servo(1, Servo_Command[commandNum][0]);     //N2O Valve
				Servo(2, Servo_Command[commandNum][1]);     //Fuel Valve
				
				commandNum++;
				//tNow = TimeMS();			
			}
		}
		else
		{	
			//control sequence has ended
			LED(2,0);
			//turn off relays after test
			Relay(1,0);					//ign relay
			Relay(2,0);					//fuel isolation relay
			Relay(3,0);					//N2O isolation relay

			runningControl=0;
		}
	}
	else
	{
		//control sequence has ended
		LED(2,0);
		runningControl=0;
	}	
	*/
	return;	
}

void ReadData(void)
{	
	uint8_t channel;
	uint8_t chipsel;
	uint16_t DataSet[8];

	if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
	{

		//record data acquisition start time
		dataTime[0] = xTaskGetTickCount();

		//put analog channel data into buffer
		for(chipsel=0;chipsel<AI_CHIPS;chipsel++)
		{
			//get analog data from chip "chipsel"
			AD7606GetDataSet(chipsel, DataSet);

			for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
			{
				analogBuffer[channel] = DataSet[channel-chipsel*AI_CHANNELS_PER_CHIP];
			}
		}


		for(channel=0;channel<TC_CHIPS;channel++)
		{
			//get temperature and cold junction data for "channel"
			TCbuffer[channel]=MAX31855read(channel, &TCbuffer[channel+TC_CHIPS]);
		}

		//record data acquisition end time
		dataTime[1] = xTaskGetTickCount();


		//give data buffer semaphore
		xSemaphoreGive(dataSemaphore);
	}
	else
	{
		// We could not obtain the semaphore and can therefore not access
		// the shared resource safely.
	}

	return;
}

void SendData(void)
{
	uint8_t channel;
	uint8_t chipsel;
	//uint16_t DataSet[8];


	//select UART channel which will send data back to the computer

	if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
	{

		sendSerialUint8(0xFF, DEBUG_UART);//send data start flag

		sendSerialUInt32(dataTime[0],DEBUG_UART);//send data acquisition start time
		sendSerialUInt32(dataTime[1],DEBUG_UART);//send data acquisition end time

		//send analog channel data from buffer
		for(chipsel=0;chipsel<AI_CHIPS;chipsel++)
		{
			for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
			{
				sendSerialUInt16(analogBuffer[channel],DEBUG_UART);
				//sendSerialUInt16(chipsel*AI_CHANNELS_PER_CHIP+channel,DEBUG_UART);
			}
		}


		for(channel=0;channel<2*TC_CHIPS;channel++)
		{
			//send temperature then cold junction data for each TC "channel"
			sendSerialUInt16(TCbuffer[channel],DEBUG_UART);
		}


		for(channel=0;channel<TOTAL_SERVO_CHANNELS;channel++)
		{
			//sendSerialUInt16(ServoPosition[channel],DEBUG_UART);
			//sendSerialUInt16(ServoPosition[channel],DEBUG_UART);//servo torque
			sendSerialUInt16(42,DEBUG_UART);
			sendSerialUInt16(43,DEBUG_UART);//servo torque
		}


		//spare outputs
		sendSerialUInt16(44,DEBUG_UART);//spare
		sendSerialUInt16(45,DEBUG_UART);//spare
		sendSerialUInt16(46,DEBUG_UART);//spare
		sendSerialUInt16(47,DEBUG_UART);//spare


		//give data buffer semaphore
		xSemaphoreGive(dataSemaphore);
	}
	else
	{
		// We could not obtain the semaphore and can therefore not access
		// the shared resource safely.
	}


	return;
}



void SaveDataToFlash(void) //uint32_t *dataTime[], uint16_t *analogBuffer[], uint16_t *TCbuffer[], uint16_t *ServoPosition[], uint8_t runningControl, uint8_t activeEstop)
{

     if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
     {
    	 //save data to SD card

		//give data buffer semaphore
		 xSemaphoreGive(dataSemaphore);
     }
     else
     {
         // We could not obtain the semaphore and can therefore not access
         // the shared resource safely.
     }





	/*
	uint8_t bytestowrite[DATASIZE];
	uint8_t n;
	
	if (activeSaveData==1)
	{
		//tNow 32bit
		bytestowrite[0] = (uint8_t)((tStart & 0xFF000000)>>24);
		bytestowrite[1] = (uint8_t)((tStart & 0x00FF0000)>>16);
		bytestowrite[2] = (uint8_t)((tStart & 0x0000FF00)>>8);
		bytestowrite[3] = (uint8_t)((tStart & 0x000000FF)>>0);
			
		//TC1 24bit
		bytestowrite[4] = (uint8_t)((TC1a & 0xFF0000)>>16);
		bytestowrite[5] = (uint8_t)((TC1a & 0x00FF00)>>8);
		bytestowrite[6] = (uint8_t)((TC1a & 0x0000FF)>>0);

		//TC2 24bit2
		bytestowrite[7] = (uint8_t)((TC2a & 0xFF0000)>>16);
		bytestowrite[8] = (uint8_t)((TC2a & 0x00FF00)>>8);
		bytestowrite[9] = (uint8_t)((TC2a & 0x0000FF)>>0);
			
		//TC3 24bit
		bytestowrite[10] = (uint8_t)((TC3a & 0xFF0000)>>16);
		bytestowrite[11] = (uint8_t)((TC3a & 0x00FF00)>>8);
		bytestowrite[12] = (uint8_t)((TC3a & 0x0000FF)>>0);
			
		//TC4 24bit
		bytestowrite[13] = (uint8_t)((TC4a & 0xFF0000)>>16);
		bytestowrite[14] = (uint8_t)((TC4a & 0x00FF00)>>8);
		bytestowrite[15] = (uint8_t)((TC4a & 0x0000FF)>>0);
	
		//TCcold1 16bit
		bytestowrite[16] = (uint8_t)((TCcold1 & 0xFF00)>>8);
		bytestowrite[17] = (uint8_t)((TCcold1 & 0x00FF)>>0);
		
		//TCcold2 16bit
		bytestowrite[18] = (uint8_t)((TCcold2 & 0xFF00)>>8);
		bytestowrite[19] = (uint8_t)((TCcold2 & 0x00FF)>>0);

		//TCcold3 16bit
		bytestowrite[20] = (uint8_t)((TCcold3 & 0xFF00)>>8);
		bytestowrite[21] = (uint8_t)((TCcold3 & 0x00FF)>>0);
			
		//TCcold4 16bit
		bytestowrite[22] = (uint8_t)((TCcold4 & 0xFF00)>>8);
		bytestowrite[23] = (uint8_t)((TCcold4 & 0x00FF)>>0);
			
		for(n=0;n<10;n++)
		{		
			//data[n]
			bytestowrite[24+n*3] = (uint8_t)((data[n] & 0xFF0000)>>16);
			bytestowrite[25+n*3] = (uint8_t)((data[n] & 0x00FF00)>>8);
			bytestowrite[26+n*3] = (uint8_t)((data[n] & 0x0000FF)>>0);
		}
		
		//runningControl
		bytestowrite[54] = runningControla;
			
		//PWMservo1 16bit
		bytestowrite[55] = (uint8_t)((PWMservo1a & 0xFF00)>>8);
		bytestowrite[56] = (uint8_t)((PWMservo1a & 0x00FF)>>0);
			
		//PWMservo2 16bit
		bytestowrite[57] = (uint8_t)((PWMservo2a & 0xFF00)>>8);
		bytestowrite[58] = (uint8_t)((PWMservo2a & 0x00FF)>>0);
	
		//tNow 32bit
		bytestowrite[69] = (uint8_t)((tEnd & 0xFF000000)>>24);
		bytestowrite[60] = (uint8_t)((tEnd & 0x00FF0000)>>16);
		bytestowrite[61] = (uint8_t)((tEnd & 0x0000FF00)>>8);
		bytestowrite[62] = (uint8_t)((tEnd & 0x000000FF)>>0);
		
		

		if (currentByte+DATASIZE>AT45DB321D_PAGE_SIZE_BYTES)
		{
			AT45DB321D_WaitForReady();
			AT45DB321D_CopyBufferToPage(1,currentPage);
			
			//advance to the next page and resent the byte location to zero
			currentPage++;
			currentByte=0;
			if (currentPage>8191)
			{
				currentPage=0;					
			}
			
			//clear target page and buffer
			AT45DB321D_WaitForReady();
			AT45DB321D_ErasePage(currentPage);
			AT45DB321D_WaitForReady();
			AT45DB321D_CopyPageToBuffer(1,currentPage);
		}
		
		AT45DB321D_WaitForReady();
		AT45DB321D_BufferWrite(1,currentByte,bytestowrite,DATASIZE);
		
	}
	*/
	return;
		
}

void FinishSaveData()
{
	/*
	//write unsaved data (anything on the buffer) to flash
	AT45DB321D_WaitForReady();
	AT45DB321D_CopyBufferToPage(1,currentPage);
	AT45DB321D_WaitForReady();
	*/
	return;
}


void OpenDataFromFlash(void)
{
	/*
	uint32_t tStart;
	uint32_t tEnd;
	uint32_t TC1a, TC2a, TC3a, TC4a;
	uint16_t TCcold1, TCcold2, TCcold3, TCcold4;
	uint32_t data[10];
	uint8_t runningControla;
	uint16_t PWMservo1a, PWMservo2a;
	
	uint16_t page,lastpage;
	uint16_t bytelocation;
	uint8_t bytestoread[DATASIZE];
	uint8_t n;
	
	lastpage = AT45DB321D_FirstBlankPage();
	
	for (page=0;page<lastpage;page++)
	{
		
		AT45DB321D_WaitForReady();
		AT45DB321D_CopyPageToBuffer(1,page);
		AT45DB321D_WaitForReady();
		
		for(bytelocation=0;bytelocation<AT45DB321D_PAGE_SIZE_BYTES-DATASIZE;bytelocation+=DATASIZE)
		{
			
			AT45DB321D_BufferRead(1,bytelocation,bytestoread,DATASIZE);
			AT45DB321D_WaitForReady();			
			
			//tNow 32bit
			tStart = (uint32_t)(((uint32_t)(bytestoread[0])<<24)|((uint32_t)(bytestoread[1])<<16)|((uint32_t)(bytestoread[2])<<8)|((uint32_t)(bytestoread[3])<<0));
			
			//TC1 24bit
			TC1a = (uint32_t)((((uint32_t)bytestoread[4])<<16)|(((uint32_t)bytestoread[5])<<8)|(((uint32_t)bytestoread[6])<<0));
			
			//TC2 24bit2
			TC2a = (uint32_t)((((uint32_t)bytestoread[7])<<16)|(((uint32_t)bytestoread[8])<<8)|(((uint32_t)bytestoread[9])<<0));
			
			//TC3 24bit
			TC3a = (uint32_t)((((uint32_t)bytestoread[10])<<16)|(((uint32_t)bytestoread[11])<<8)|(((uint32_t)bytestoread[12])<<0));
			
			//TC4 24bit
			TC4a = (uint32_t)((((uint32_t)bytestoread[13])<<16)|(((uint32_t)bytestoread[14])<<8)|(((uint32_t)bytestoread[15])<<0));
			
			//TCcold1 16bit
			TCcold1 = (uint16_t)((bytestoread[16]<<8)|(bytestoread[17]<<0));
			
			//TCcold2 16bit
			TCcold2 = (uint16_t)((bytestoread[18]<<8)|(bytestoread[19]<<0));
			
			//TCcold3 16bit
			TCcold3 = (uint16_t)((bytestoread[20]<<8)|(bytestoread[21]<<0));
			
			//TCcold4 16bit
			TCcold4 = (uint16_t)((bytestoread[22]<<8)|(bytestoread[23]<<0));
			
			for(n=0;n<10;n++)
			{		
				//data[n]
				data[n] = (uint32_t)((((uint32_t)bytestoread[24+n*3])<<16)|(((uint32_t)bytestoread[25+n*3])<<8)|(((uint32_t)bytestoread[26+n*3])<<0));
			}
			
			//runningControl
			runningControla=bytestoread[54];
			
			//PWMservo1 16bit
			PWMservo1a =  (uint16_t)((bytestoread[55]<<8)|(bytestoread[56]<<0));
			
			//PWMservo2 16bit
			PWMservo2a = (uint16_t)((bytestoread[57]<<8)|(bytestoread[58]<<0));
			
			//tNow 32bit
			tEnd = (uint32_t)((((uint32_t)bytestoread[59])<<24)|(((uint32_t)bytestoread[60])<<16)|(((uint32_t)bytestoread[61])<<8)|(((uint32_t)bytestoread[62])<<0));
			
			
			printf("a%lu b%lu c%lu d%lu e%lu",tStart,TC1a,TC2a,TC3a,TC4a);
			printf(" f%u g%u h%u i%u",TCcold1,TCcold2,TCcold3,TCcold4);
			
			printf(" j%lu k%lu l%lu",data[0],data[1],data[2]);
			printf(" m%lu n%lu o%lu p%lu q%lu r%lu s%lu t%u u%u v%u w%lu \n",data[3],data[4],data[5],data[6],data[7],data[8],data[9],runningControla,PWMservo1a,PWMservo2a,tEnd);
			
		}
	}
	*/
	return;
		
}


void Redlines(uint32_t tNow)
{
	/*
	int n;
	if (runningControl==1 && activeEstop==1)
	{

		for(n=0;n<5;n++)
		{
			//check to see if redline is active
			if (redlineTimeStart[n] <= tNow-tControl && redlineTimeEnd[n] >= tNow-tControl)
			{
				if (redlineMin[n] > data[redlineChannel[n]])
				{//abort the test
					//Servo(1, Valve1close);		//N2O Valve Closed
					//Servo(2, Valve2close);		//Fuel Valve Closed
					
					//turn off relays during abort
					Relay(1,0);					//ign relay
					Relay(2,0);					//fuel isolation relay
					Relay(3,0);					//N2O isolation relay
					runningControl=0;			//abort the test
					
				}
						
			}
		
		}
	}
	*/
	return;
}
