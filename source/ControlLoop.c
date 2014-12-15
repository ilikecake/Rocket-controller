
#include "main.h"


void InitControl(void)
{
	int i;
	int channel;

	//setup the fire sequence
	//commandMax=MAX_COMMANDS-1;
	
	//initialize command sequence to all off all the time
	for(i=0;i<=MAX_COMMANDS;i++)
	{
		//Timing
		commandTime[i] = 0;//milliseconds

		DO_Command[i] = 0;//sets all DO channels to 0
		Spark_Command[i] =0;//keeps spark always off

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

	redlineNumber=0xFE;//reset null redline number value
	redlinesEnabled = 1;		//redlines active
	emergencyStop = 0;		//reset Emergency stop flag to not triggered
	runningControl = 0;		//not running a test sequence
	runningData = 0;		//not reading data from TC chip and A/D
	activeSaveData=0;	//not saving data to flash memory

	lastServoCommand[0]=0;
	lastServoCommand[1]=0;

	//give values to servo position
	for (i=0;i<TOTAL_SERVO_CHANNELS;i++)
	{
		dataServo[i] = 0;
	}

	return;
	
}

void ReadData(void)
{	
	uint8_t channel;
	uint8_t chipsel;
	uint8_t i;
	uint16_t DataSet[8];

	uint32_t dataTime[2];								//store start and end time of data sample
	uint16_t dataAnalog[AI_CHIPS*AI_CHANNELS_PER_CHIP];	//store all analog input channels
	uint16_t dataTC[2*TC_CHIPS*TC_CHANNELS_PER_CHIP];	//store temperature and cold junction  for each TC channel
	uint32_t averageTjunction;
	uint8_t dataDOstate[2];

	//record data acquisition start time
	dataTime[0] = xTaskGetTickCount();

	//put analog channel data into buffer
	//get analog data from chip 1
	chipsel=1;
	AD7606GetDataSet(chipsel, DataSet);
	for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
	{
		dataAnalog[channel+0*AI_CHANNELS_PER_CHIP] = DataSet[channel];
	}
	//get analog data from chip 3
	chipsel=3;
	AD7606GetDataSet(chipsel, DataSet);
	for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
	{
		dataAnalog[channel+1*AI_CHANNELS_PER_CHIP] = DataSet[channel];
	}


	//get temperature and cold junction data for "channel"
	for(channel=0;channel<TC_CHIPS;channel++)
	{
		dataTC[channel]=MAX31855read(channel, &dataTC[channel+TC_CHIPS]);
	}

	//servo data is read on a separate thread because it is slow

	//record data acquisition end time
	dataTime[1] = xTaskGetTickCount();


	if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
	{
		dataSendTime[0] = dataTime[0] - fireStartTime;	//store start and end time of data sample
		dataSendTime[1] = dataTime[1] - fireStartTime;	//record data acquisition end time

		i=0;

		//put analog channel data into send buffer
		chipsel=0;
		for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
		{
			dataSendBuffer[i] = dataAnalog[channel+0*AI_CHANNELS_PER_CHIP];
			i++;
		}
		chipsel=2;
		for(channel=0;channel<AI_CHANNELS_PER_CHIP;channel++)
		{
			dataSendBuffer[i] = dataAnalog[channel+1*AI_CHANNELS_PER_CHIP];
			i++;
		}

		//update temperature data to buffer
		for(channel=0;channel<TC_CHIPS;channel++)
		{
			dataSendBuffer[i]=dataTC[channel];
			i++;
		}

		//calculate and send average cold junction temperature
		averageTjunction=0;
		for(channel=TC_CHIPS;channel<2*TC_CHIPS;channel++)
		{
			averageTjunction+=(uint32_t)dataTC[channel];
		}
		dataSendBuffer[i]=(uint16_t)(averageTjunction/(uint32_t)TC_CHIPS);
		i++;

		//servo buffer will not be time consistent
		// if we want all servo data to be from the same time we need to update buffer from servo read thread
		//for (channel=0;channel<TOTAL_SERVO_CHANNELS*2;channel++)
		//{
		//	dataSendBuffer[AI_CHIPS*AI_CHANNELS_PER_CHIP+2*TC_CHIPS*TC_CHANNELS_PER_CHIP+i] = dataServo[i];
		//	i++;
		//}
		i=i+4;

		//read states of digital output IO expander
		PCA9535_GetOutputs(dataDOstate);
		dataSendBuffer[i]=(((uint16_t)dataDOstate[1])<<8) | ((uint16_t)dataDOstate[0]);//DO_Command[commandNum];	//current DO state
		i++;

		//byte 44: contains RunState, E-State
		dataSendBuffer[i]=runningControl;//bit 0: runningControl
		dataSendBuffer[i]=dataSendBuffer[i] | (runningData<<1);//bit 1: runningData
		dataSendBuffer[i]=dataSendBuffer[i] | (activeSaveData<<2);//bit 2: activeSaveData, saving data to flash memory
		dataSendBuffer[i]=dataSendBuffer[i] | (redlinesEnabled<<3);//bit 3: redlinesEnabled, redlines active
		dataSendBuffer[i]=dataSendBuffer[i] | (emergencyStop<<4);//bit 4: emergencyStop, reset Emergency stop flag
		dataSendBuffer[i]=dataSendBuffer[i] | (PWM_GetState()<<5);//bit 5: state of spark PWM driver //Spark_Command[commandNum]
		i++;

		dataSendBuffer[i]=(uint16_t) redlineNumber;//45;//spare
		i++;
		dataSendBuffer[i]=i;//46;//spare


		CheckRedlines();//check redline conditions and set heater state


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


void CheckRedlines()
{
	//check each redline condition to see if it is violated
	int n;
	if (runningControl==1 && redlinesEnabled==1)
	{
		for(n=1;n<MAX_REDLINES;n++)
		{
			//check to see if redline is within active time range
			if ((redlineTimeStart[n] <= dataSendTime[1]) && (redlineTimeEnd[n] > dataSendTime[1]))
			{
				//check to see if value is outside of safe range
				if ( (redlineMin[n] > dataSendBuffer[redlineChannel[n]]) || (redlineMax[n] < dataSendBuffer[redlineChannel[n]]) )
				{//abort the test
					redlineNumber = n;//store index of redline that ended the test
					vTaskResume(vEStopTaskHandle);//resume Redline task that is poised to shut everything down
				}
			}

		}
	}
	else // when test is not running
	{	//check pressure in N2O tank and turn on heater if it is too high or low
		if (redlineMin[0] > dataSendBuffer[redlineChannel[0]])
		{
			PCA9535_SetOutput(DO_HEATER, 1);//turn on heater
		}
		else if	(redlineMax[0] < dataSendBuffer[redlineChannel[0]])
		{
			PCA9535_SetOutput(DO_HEATER, 0);//turn off heater
		}

	}

	return;
}



void SetServoPosition(uint8_t servoNum, uint16_t position)
{
	//send position commands to servos.  this will take a while
	//vTaskResume(vServoWriteTaskHandle);
	//servoNum=0 is N2O valve (valveID = 1)
	//servoNum=1 is Fuel valve (valveID = 2)
	uint8_t servoID;
	servoID=servoNum+1;

	if (servoExists[servoNum]==1)
	{
		//if( xSemaphoreTake( servoSemaphore, ( portTickType ) 200 ) == pdTRUE )	//take servo semaphore
		//{
			//only send a new servo command if it is different from previous command
			if (position != lastServoCommand[servoNum])
			{
				MX106T_Set16bit(servoID,SERVO_GOAL_POSITION_16,position);
			}

			//xSemaphoreGive(servoSemaphore);//give back servo semaphore
			lastServoCommand[servoNum]=position;
		//}
		//else
		//{	//if a servo command failed, trigger an emergency stop
		//	vTaskResume(vEStopTaskHandle);
		//}
	}

	return;
}

void ReadServo(void)
{
	//Read position and torque on both servos.
	//Allow control loop to take semaphore between servo data requests
	uint8_t servoNum;
	uint8_t servoID;
	uint8_t i;
	uint8_t msg;
	uint16_t val;

	if (1)//(servoCommandFlag==0)//don't read from servo when a servo command is pending
	{
		for (servoNum=0;servoNum<TOTAL_SERVO_CHANNELS;servoNum++)
		{
			servoID=servoNum+1;
			if (servoExists[servoNum]==1)
			{
				dataServo[servoNum] = MX106T_Read16bit(servoID, SERVO_PRESENT_POSITION_16, &msg);
			}
			else
			{
				dataServo[servoNum] = 0;
			}

		}
		for (servoNum=0;servoNum<TOTAL_SERVO_CHANNELS;servoNum++)
		{
			servoID=servoNum+1;
			if (servoExists[servoNum]==1)
			{
				val= MX106T_Read16bit(servoID,SERVO_PRESENT_LOAD_16, &msg);
				//convert to from direction bit to load value centered on 1024
				if(val < 1024)
				{
					val = val + 1024;
				}
				else
				{
					val = 2047 - val;
				}
				dataServo[servoNum+TOTAL_SERVO_CHANNELS] = val;

				//dataServo[servoNum+TOTAL_SERVO_CHANNELS] = MX106T_Read8bit(servoID,SERVO_PRESENT_TEMPERATURE_8, &msg);
			}
			else
			{
				dataServo[servoNum+TOTAL_SERVO_CHANNELS] = 0;
			}

		}


		//update databuffer with new servo data
		if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
		{
			for (i=0;i<TOTAL_SERVO_CHANNELS*2;i++)
			{
				dataSendBuffer[2*AI_CHANNELS_PER_CHIP+TC_CHIPS*TC_CHANNELS_PER_CHIP+1+i] = dataServo[i];
			}
			xSemaphoreGive(dataSemaphore);//give back Data semaphore
		}

	}
	return;
}

void SendData(void)
{
	uint8_t channel;
	//uint8_t chipsel;

	//select UART channel which will send data back to the computer

	if( xSemaphoreTake( dataSemaphore, ( portTickType ) 100 ) == pdTRUE )	//take data buffer semaphore
	{

		sendSerialUint8(0xFF, DEBUG_UART);//send data start flag
		sendSerialUint8(0xFE, DEBUG_UART);//send data start flag

		sendSerialUInt32(dataSendTime[0],DEBUG_UART);//send data acquisition start time
		sendSerialUInt32(dataSendTime[1],DEBUG_UART);//send data acquisition end time

		//for(channel=0;channel<AI_CHIPS*AI_CHANNELS_PER_CHIP+2*TC_CHIPS*TC_CHANNELS_PER_CHIP+TOTAL_SERVO_CHANNELS*2+4;channel++)
		for(channel=0;channel<2*AI_CHANNELS_PER_CHIP+TC_CHIPS*TC_CHANNELS_PER_CHIP+1+TOTAL_SERVO_CHANNELS*2+4;channel++)
		{
			sendSerialUInt16(dataSendBuffer[channel],DEBUG_UART);
		}

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


