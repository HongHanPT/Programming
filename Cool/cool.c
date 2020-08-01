/** @file    cool.c
  * @version 1.0
  * @author  HongHan
  * @brief   Library for ARM process cooler. 
  *          This file provides firmware functions to process cooler.
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
 * VARIABLE
 */
//extern cooling_t *p_cooling;
uint32_t g_timeSysTick=0, time_debug=0;
uint8_t temp=16, threshold =10;

/*********************************************************************************
 * EXPORTED FUNCTION
 */


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
void procesColling(cooling_t *p_cooling)
{
	//temp = getTemp;
	switch (p_cooling -> state)
	{
		case START:
			if (p_cooling -> fanState == TURNING_OFF)
			{
				p_cooling->fanState = TURNING_ON;
				TURN_ON_FAN;
			}
			if(p_cooling -> timeStartState == NO_COUT)
			{
				p_cooling -> timeStartState = COUNTING;
				p_cooling -> timeStart = g_timeSysTick;
			}
			if (temp >= threshold + 2)
			{
				if (p_cooling -> timeVetifyState == NO_COUT)
				{
					p_cooling -> timeVetifyState = COUNTING;
					p_cooling -> timeVetify = g_timeSysTick;
				}
				
				if((g_timeSysTick - p_cooling -> timeVetify) >FIFTEEN_SEC)
				{	
					if ( (g_timeSysTick - p_cooling -> timeStart) > THREE_MIN)
					{
                            time_debug = g_timeSysTick;
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
				p_cooling -> timeOn = g_timeSysTick;
			}
			if ((temp <=threshold+1)&&(temp>threshold -2))  			  //Run Time Over
			{
				if (p_cooling -> timeRunOverState == NO_COUT)
				{
					p_cooling -> timeRunOverState = COUNTING;
					p_cooling -> timeRunOver  = g_timeSysTick;
				}
                                			//Truong hop bi run over time
                                if ((g_timeSysTick - p_cooling -> timeRunOver)>FIFTEEN_MIN)
                                {
                                  time_debug = g_timeSysTick;
                                        p_cooling -> timeRunOverState = NO_COUT;
                                        p_cooling -> timeOnState = NO_COUT;
                                        p_cooling -> state = COOLER_SYSTEM_TURN_OFF;
                                        p_cooling -> runOverTimeState = RUN_OVER_TIME;
                                        break;
                                }
			}
			else 
			{
				p_cooling -> timeRunOverState = NO_COUT;
			}
				
			
			// Truong hop do duoi nguong
			if (temp <= threshold -2)
			{
				if (p_cooling -> timeVetifyState == NO_COUT)
				{
					p_cooling -> timeVetifyState = COUNTING;
					p_cooling -> timeVetify = g_timeSysTick;
				}
				if ((g_timeSysTick - p_cooling -> timeVetify) > FIFTEEN_SEC)
				{
					if ((g_timeSysTick - p_cooling -> timeOn) >TWELVE_MIN)
					{
                                          time_debug = g_timeSysTick;
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
                        if(p_cooling -> timeOffState == NO_COUT)
                        {
                                p_cooling -> timeOffState = COUNTING;
                                p_cooling -> timeOff = g_timeSysTick;
                        }
                        if (temp >= threshold + 2)
                        {
                                if (p_cooling -> timeVetifyState == NO_COUT)
                                {
                                        p_cooling -> timeVetifyState = COUNTING;
                                        p_cooling -> timeVetify = g_timeSysTick;
                                }
                                if ((g_timeSysTick - p_cooling -> timeVetify) > FIFTEEN_SEC)
                                {
                                    if (p_cooling -> runOverTimeState == NO_RUN_OVER_TIME)
                                    {
                                        if ((g_timeSysTick - p_cooling -> timeOff) > EIGHT_MIN)
                                        {
                                          time_debug = g_timeSysTick;
                                                p_cooling -> timeOffState = NO_COUT;
                                                p_cooling -> state = COOLER_SYSTEM_TURN_ON;
                                                break;
                                        }
                                    }
                                    else 
                                    {   
                                        if ((g_timeSysTick - p_cooling -> timeOff) > TWELVE_MIN)
                                        {
                                          time_debug = g_timeSysTick;
                                                p_cooling -> timeOffState = NO_COUT;
                                                p_cooling -> state = COOLER_SYSTEM_TURN_ON;
                                                break;
                                        }
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
//void getTem(uint8_t *_temp, uint32_t *_g_Tick)
//{
//  &temp = _temp;
//  &g_timeSysTick = _g_Tick;
//}