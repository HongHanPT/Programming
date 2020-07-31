/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_lpuart.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "string.h"

#include "module_sim_800c.h"
#include "string_uart.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPUART LPUART0
#define DEMO_LPUART_CLKSRC kCLOCK_ScgSysOscClk
#define DEMO_LPUART_CLK_FREQ CLOCK_GetFreq(kCLOCK_ScgSysOscClk)
#define ECHO_BUFFER_LENGTH 8

#define GSM_LPUART LPUART2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
lpuart_handle_t g_lpuartHandle;

uint8_t g_tipString[] =
    "Lpuart interrupt example with seven data bits\r\nBoard receives 8 characters then sends them out\r\nNow please "
    "input:\r\n";

uint8_t _ringBuffer[256]="";

volatile uint32_t g_systickCounter=0;

    
/*******************************************************************************
 * Code
 ******************************************************************************/


static void delay(unsigned int time)
{
    volatile uint32_t i = 0;
    for (unsigned int cnt = 0; cnt<=time;cnt++)
    {
      for (i = 0; i < 100; ++i)
      {
          __asm("NOP"); /* delay */
      }
    }
}


uint32_t dem;
/*!
 * @brief Main function
 */
//typedef enum
//{
//    IDLE =0,
//    RESET,
//    INIT,
//    SET_MIC_GAIN,
//    GET_CSQ,
//}state_sim_t;
//
//typedef enum
//{
//    IDLE_INIT_PRROCESS = 0,
//    CHECK_SIM,
//    READ_MODULE,
//    READ_SIM,
//}state_init_t;
//
//typedef struct{
//    state_sim_t state;    
//    state_init_t stateInit;
//    bool isCheckedSim;
//    uint8_t respone[256];
//    uint8_t str;
//    void (*fnSimCallBack)(state_sim_t _state, state_init_t _stateInit);
//}sim_t;
//
//
//void processInit(sim_t *g_pSim)
//{
//    switch(g_pSim -> stateInit)
//    {   
//    
//    case IDLE_INIT_PRROCESS:   break;
//    case CHECK_SIM: 
//          
//          SIM_SEND_COMMAND("AT+CPIN=?\r\n");
//         // delay(2000);
//          getString(g_pSim -> respone);
//          if(findString("ERROR",g_pSim -> respone)== ISFINDED)
//          {
//              g_pSim -> isCheckedSim = true; 
//          }
//          else
//          {
//              g_pSim -> isCheckedSim = 1;
//          if(g_pSim -> fnSimCallBack!= NULL)
//          {
//              g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//          }
//          }
//          break;
//    case READ_MODULE: 
//          if(g_pSim -> isCheckedSim == 1)
//          {
//              if(g_pSim -> fnSimCallBack!= NULL)
//              {
//                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//              }
//              g_pSim ->stateInit = IDLE_INIT_PRROCESS;
//          }
//          else
//              g_pSim ->stateInit = CHECK_SIM;
//          break;
//    case READ_SIM:
//          if(g_pSim -> isCheckedSim == 1)
//          {
//              if(g_pSim -> fnSimCallBack!= NULL)
//              {
//                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//              }
//              g_pSim ->stateInit = IDLE_INIT_PRROCESS;
//          }
//          else
//              g_pSim -> stateInit = CHECK_SIM;
//          break;
//    }
//}
//
//void processSIM(sim_t *g_pSim)
//{
//    switch (g_pSim ->state)
//    {
//    case IDLE: break;
//    case RESET: 
//          //g_systickCounter2 =0;
//          if(g_pSim -> fnSimCallBack!= NULL)
//              {
//                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//              } 
//              g_pSim -> state = IDLE;       
//          break;
//    case INIT:  
//           processInit(g_pSim);
//          break;
//    case SET_MIC_GAIN: 
//          if(g_pSim -> isCheckedSim == 1)
//          {
//              if(g_pSim -> fnSimCallBack!= NULL)
//              {
//                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//              } 
//              g_pSim -> state = IDLE;
//          }
//          else
//              g_pSim -> state = INIT;
//          break;
//    case GET_CSQ: 
//          if(g_pSim -> isCheckedSim == 1)
//          {
//              if(g_pSim -> fnSimCallBack!= NULL)
//              {
//                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
//              }
//          }
//          else
//              g_pSim -> state = INIT;
//          break;
//    }
//}

void fnCallBackUser(state_sim_t _state, state_init_t _stateInit)
{
      if(_state == IDLE);
      if(_state == RESET);
      if(_state == INIT);
      if(_stateInit == SET_MIC_GAIN)
      {
        uint8_t ch2[]= "AT+CIPGSMLOC=?\r\n";
              
        SIM_SEND_COMMAND(ch2);
        getString(_ringBuffer);
        
        LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
      }
      if(_state == GET_CSQ)
      {
        uint8_t ch2[]= "AT+CSQ\r\n";
              
        SIM_SEND_COMMAND(ch2);
        getString(_ringBuffer);
        
        LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
      }
      if(_stateInit == IDLE_INIT_PRROCESS);
      if(_stateInit == CHECK_SIM);
      if(_stateInit == READ_MODULE);
      if(_stateInit == READ_SIM);
}
    sim_t p_test = {.state =INIT, .stateInit = CHECK_SIM, .fnSimCallBack = fnCallBackUser,.respone = ""};

int main(void)
{
        if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }


    uint8_t ch1[]= "ATE0\r\n";
    //uint8_t ch1[]= "AT+CGMM\r\n";
    lpuart_config_t config;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcSysOscAsync);
    
//    CLOCK_SetIpSrc(kCLOCK_Lpuart2, kCLOCK_IpSrcSysOscAsync);
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps  = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx      = true;
    config.enableRx      = true;
    config.isMsb         = false;

    LPUART_Init(DEMO_LPUART, &config, DEMO_LPUART_CLK_FREQ);
    initSim800C(&p_test);
    LPUART_WriteBlocking(DEMO_LPUART, g_tipString, sizeof(g_tipString) - 1);
    //SIM_SEND_COMMAND(ch1);
    delay(1000);
    LPUART_WriteBlocking(DEMO_LPUART, ch1, sizeof(ch1)-1);

//    while (1)
//    {

          uint8_t ch3[]= "ATE0\r\n";

          SIM_SEND_COMMAND(ch3);
          //delay(20000);
          getString(_ringBuffer);
          LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
          
          if(findString("ERROR",_ringBuffer))
          {
              uint8_t ch2[]= "AT+CIPGSMLOC=?\r\n";
              
              SIM_SEND_COMMAND(ch2);
              
              //delay(50);

              getString(_ringBuffer);
              LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
              
              SIM_SEND_COMMAND(ch3);
              //delay(50);
              getString(_ringBuffer);
              LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);

              SIM_SEND_COMMAND(ch2);
              //delay(50);
              getString(_ringBuffer);
              LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
              
              SIM_SEND_COMMAND(ch3);
              //delay(50);
              getString(_ringBuffer);
              LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);

              SIM_SEND_COMMAND(ch2);
              //delay(50);
              getString(_ringBuffer);
              LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);
              //delay(50);
          }
    
    processSIM();
    p_test.state = GET_CSQ;
    processSIM();
    uint8_t ch2[]= "AT+CIPGSMLOC=?\r\n";
    //uint8_t ch2[]= "AT+GSV\r\n";

    SIM_SEND_COMMAND(ch2);
    
    //delay(10000);
    getString(_ringBuffer);
        
    LPUART_WriteBlocking(DEMO_LPUART, _ringBuffer, sizeof(_ringBuffer)-1);

//    }
}
