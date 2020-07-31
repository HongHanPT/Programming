/** @file    cool.c
  * @version 0.1
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

#ifndef __COOLING_H__
#define __COOLING_H__
  
  /*********************************************************************************
 * INCLUDE
 */
#include "stdint.h"
/*********************************************************************************
 * DEFINE
 */

/**
 * @defgroup  GPIO PORT 
 * @brief
 */

/**
 * @defgroup  GPIO PIN 
 * @brief
 */

/**
 * @defgroup  Function code 
 * @brief     Function code of request master
 */
#define THREE_MIN 		180000
#define EIGHT_MIN 		480000
#define TWELVE_MIN 		720000
#define FIFTEEN_MIN             900000
#define FIFTEEN_SEC             15000

/*********************************************************************************
 * MACRO
 */
 
#define TURN_ON_FAN
#define TURN_OFF_FAN
#define TURN_ON_COOLER
#define TURN_OFF_COOLER
#define getTemp 1

/*********************************************************************************
 * TYPEDEFS
 */

/**
  * @brief  States of Cooler Sytem
  */  
typedef enum{
	IDLE =0,
	START,
	COOLER_SYSTEM_TURN_ON,
	COOLER_SYSTEM_TURN_OFF,
}state_cooling_t;

/**
  * @brief  State of time cout
  */  
typedef enum{
	NO_COUT =0,
	COUNTING,
}state_time_t;

/**
  * @brief  State of cooling machine system 
  */  
typedef enum{
	TURNING_OFF =0,
	TURNING_ON,
}state_cooling_machine_sytems_t;

typedef enum{
	NO_RUN_OVER_TIME =0,
	RUN_OVER_TIME,
}state_run_over_time_t;

/**
  * @brief  Define for a cooling
  */  
typedef struct{
	state_cooling_t state;
	state_run_over_time_t runOverTimeState;
	uint32_t timeStart;
	uint32_t timeOn;
	uint32_t timeOff;
	uint32_t timeRunOver;
        uint32_t timeVetify;
	state_time_t timeStartState;
	state_time_t timeOnState;
	state_time_t timeOffState;
	state_time_t timeRunOverState;
        state_time_t timeVetifyState;
	state_cooling_machine_sytems_t fanState;
	state_cooling_machine_sytems_t coolerState;
	
}cooling_t;

 /*********************************************************************************
 * VARIABLE
 */
cooling_t *p_cooling;
uint32_t g_timeSysTick;
uint8_t temp, threshold;

#endif /* __COOLING_H__ */