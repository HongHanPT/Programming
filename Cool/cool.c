/** @file    modbus.c
  * @version 0.3
  * @author  minhneo
  * @brief   Modbus RTU protocol (Slave) library for STM32 using RS485. 
  *          This file provides firmware functions to manage the following functionalities of the Modbus RTU.
  *          + Initialization and de-initialization functions
  *             ++ 
  *          + Operation functions
  *             ++
  *          + Control functions
  *             ++
  *          + State functions
  *             ++
  */

#include "cool.h"

/*********************************************************************************
 * EXPORTED FUNCTION
 */


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
void procesColling(void)
{
	temp = getTemp;
	switch (p_cooling -> state)
	{
		case START:
			if (p_cooling -> fanState == TURNING_OFF)
			{
				p_cooling -> fanState = TURNING_ON;
				TURN_ON_FAN;
			}
			if(p_cooling -> timeStartState == NO_COUT)
			{
				p_cooling -> timeStartState = COUNTING;
				p_cooling -> timeStart = g_timeSysTick + THREE_MIN;
			}
			if (temp > threshold + 2)
			{
				if (p_cooling -> timeVetifyState == NO_COUT)
				{
					p_cooling -> timeVetifyState = COUNTING;
					p_cooling -> timeVetify = g_timeSysTick + FIFTEEN_SEC;
				}
				
				if(g_timeSysTick > p_cooling -> timeVetify)
				{	
					if ( g_timeSysTick > p_cooling -> timeStart)
					{
						p_cooling -> timeVetifyState = NO_COUT;
						p_cooling -> timeStartState = NO_COUT;
						p_cooling -> state = COOLER_SYSTEM_TURN_ON;
						break;
					}
				}
			}
			else 
			{
				p_cooling -> timeVetifyState = NO_COUT;
			}
			break;
		case COOLER_SYSTEM_TURN_ON:
			if (p_cooling -> coolerState == TURNING_OFF)
			{
				p_cooling -> coolerState = TURNING_ON;
				TURN_ON_COOLER;
			}
			if(p_cooling -> timeOnState == NO_COUT)
			{
				p_cooling -> timeOnState = COUNTING;
				p_cooling -> timeOn = g_timeSysTick + TWELVE_MIN;
			}
			if ((temp <=threshold+1)&&(temp>threshold -2))  			  //Run Time Over
			{
				if (p_cooling -> timeRunOverState == NO_COUT)
				{
					p_cooling -> timeRunOverState = COUNTING;
					p_cooling -> timeRunOver  = g_timeSysTick + FIFTEEN_MIN;
				}
			}
			else 
			{
				//Can vetify
				p_cooling -> timeRunOverState = NO_COUT;
			}
			//Truong hop bi run over time
			if (g_timeSysTick > p_cooling -> timeRunOver)
			{
				p_cooling -> timeRunOverState = NO_COUT;
				p_cooling -> timeOnState = NO_COUT;
				p_cooling -> state = COOLER_SYSTEM_TURN_OFF;
				p_cooling -> runOverTimeState = RUN_OVER_TIME;
				break;
			}				
			
			// Truong hop do duoi nguong
			if (temp < threshold -2)
			{
				if (p_cooling -> timeVetifyState == NO_COUT)
				{
					p_cooling -> timeVetifyState = COUNTING;
					p_cooling -> timeVetify = g_timeSysTick + FIFTEEN_SEC;
				}
				if (g_timeSysTick > p_cooling -> timeVetify)
				{
					if (g_timeSysTick > p_cooling -> timeOn)
					{
						p_cooling -> timeRunOverState = NO_COUT;
						p_cooling -> timeOnState = NO_COUT;
						p_cooling -> state = COOLER_SYSTEM_TURN_OFF;
						p_cooling -> runOverTimeState = NO_RUN_OVER_TIME;
						break;
					}
				}
			}
			else 
			{
				p_cooling -> timeVetifyState = NO_COUT;
			}
			break;
		case COOLER_SYSTEM_TURN_OFF:
			if (p_cooling -> coolerState == TURNING_ON)
			{
				p_cooling -> coolerState = TURNING_OFF;
				TURN_OFF_COOLER;
			}
			if (p_cooling -> runOverTimeState == NO_RUN_OVER_TIME)
			{
				if(p_cooling -> timeOffState == NO_COUT)
				{
					p_cooling -> timeOffState = COUNTING;
					p_cooling -> timeOff = g_timeSysTick + EIGHT_MIN;
				}

			}
			else 
			{
				if(p_cooling -> timeOffState == NO_COUT)
				{
					p_cooling -> timeOffState = COUNTING;
					p_cooling -> timeOff = g_timeSysTick + TWELVE_MIN;
				}
			}
			
			if (temp > threshold + 2)
			{
				if (p_cooling -> timeVetifyState == NO_COUT)
				{
					p_cooling -> timeVetifyState = COUNTING;
					p_cooling -> timeVetify = g_timeSysTick + FIFTEEN_SEC;
				}
				if (g_timeSysTick > p_cooling -> timeVetify)
				{
					if (g_timeSysTick > p_cooling -> timeOff)
					{
						p_cooling -> timeOffState = NO_COUT;
						p_cooling -> state = COOLER_SYSTEM_TURN_ON;
						break;
					}
				}
			}
			else 
			{
				p_cooling -> timeVetifyState = NO_COUT;
			}
			break;
	}
}

