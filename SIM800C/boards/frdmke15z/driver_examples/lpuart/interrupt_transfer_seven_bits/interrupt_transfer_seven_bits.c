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

/* LPUART user callback */
void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
lpuart_handle_t g_lpuartHandle;

uint8_t g_tipString[] =
    "Lpuart interrupt example with seven data bits\r\nBoard receives 8 characters then sends them out\r\nNow please "
    "input:\r\n";

uint8_t g_txBuffer[ECHO_BUFFER_LENGTH] = {0};
uint8_t g_rxBuffer[ECHO_BUFFER_LENGTH] = {0};
volatile bool rxBufferEmpty            = true;
volatile bool txBufferFull             = false;
volatile bool txOnGoing                = false;
volatile bool rxOnGoing                = false;
uint8_t ringBuffer[70] = "";
int8_t ringBufferTail=0, ringBufferHead=0, lengthString;
uint8_t string[16]={0};

volatile uint32_t g_systickCounter=0;
uint32_t last_g_systickCounter=0;
uint8_t *string1;


lpuart_transfer_t receiveXfer;

    
/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
    }
    else g_systickCounter = 0;
}

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

bool isUpdate=0, isNewString =0;
/* LPUART user callback */
void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;
    static uint8_t stringIndex=0;
    if (kStatus_LPUART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing    = false;
    }

    if (kStatus_LPUART_RxIdle== status)
    {
        if(g_systickCounter>10)
        {
                  //LPUART_WriteBlocking(DEMO_LPUART, ringBuffer , sizeof(ringBuffer));
                  stringIndex=0;
                  for (uint8_t i= 0; i<70; i++)
                  {
                       ringBuffer[i] = NULL;
                  }
        }
        ringBuffer[stringIndex++] = *receiveXfer.data;
//        //stringIndex++;
        g_systickCounter = 0;
//        LPUART_WriteBlocking(DEMO_LPUART, receiveXfer.data , receiveXfer.dataSize);
        LPUART_TransferReceiveNonBlocking(GSM_LPUART, &g_lpuartHandle, &receiveXfer, NULL);

    }
}
void getString(uint8_t *_string)
{
    //memcpy(_string, ringBuffer, sizeof(ringBuffer));
}
uint32_t dem;
/*!
 * @brief Main function
 */
typedef enum
{
    RESET = 0,
    INIT,
    SET_MIC_GAIN,
    GET_CSQ,
}state_sim_t;

typedef enum
{
    IDE = 0,
    CHECK_SIM,
    READ_MODULE,
    READ_SIM,
}state_init_t;

typedef struct{
    state_sim_t state;    
    uint8_t str;
    void (*fnCallBack)(void);
}sim_t;
sim_t *sim, *init;

void processInit(void)
{
    switch(init -> state)
    {   
    case IDE:   break;
    case CHECK_SIM: break;
    case READ_MODULE: break;
    case READ_SIM: break;
    }
}

void processSIM(void)
{
    switch (sim ->state)
    {
    case RESET: break;
    case INIT:  processInit();break;
    case SET_MIC_GAIN: break;
    case GET_CSQ: break;
    }
}

int main(void)
{
        if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }
    receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = 1;

    //uint8_t ch1[]= "ATE0\r\n";
    uint8_t ch1[]= "AT+CGMM\r\n";
    lpuart_config_t config;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcSysOscAsync);
    
    CLOCK_SetIpSrc(kCLOCK_Lpuart2, kCLOCK_IpSrcSysOscAsync);
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
    LPUART_Init(GSM_LPUART, &config, DEMO_LPUART_CLK_FREQ);
    LPUART_TransferCreateHandle(GSM_LPUART, &g_lpuartHandle, LPUART_UserCallback, NULL);


    txOnGoing     = true;
    LPUART_WriteBlocking(DEMO_LPUART, g_tipString, sizeof(g_tipString) - 1);

//    LPUART_WriteBlocking(GSM_LPUART, (uint8_t*) "ATE0\r\n", 6);
    delay(1000);
    LPUART_WriteBlocking(DEMO_LPUART, ch1, sizeof(ch1)-1);
    LPUART_WriteBlocking(GSM_LPUART, ch1, sizeof(ch1)-1);
    LPUART_TransferReceiveNonBlocking(GSM_LPUART, &g_lpuartHandle, &receiveXfer, NULL);
//    delay(10000);
    while (1)
    {
//          dem=dem +1;
//          LPUART_ReadBlocking(DEMO_LPUART, g_txBuffer, 1);
//          

//          LPUART_WriteBlocking(GSM_LPUART, g_txBuffer, 1);
          
          delay(1000);
          LPUART_WriteBlocking(DEMO_LPUART, ringBuffer, sizeof(ringBuffer)-1);
          uint8_t ch3[]= "AT+GSV\r\n";
          LPUART_WriteBlocking(GSM_LPUART, ch3, sizeof(ch3)-1);
          delay(1000);
          LPUART_WriteBlocking(DEMO_LPUART, ringBuffer, sizeof(ringBuffer)-1);
          uint8_t ch2[]= "AT+CIPGSMLOC=?\r\n";
          LPUART_WriteBlocking(GSM_LPUART, ch2, sizeof(ch2)-1);
          delay(1000);
          LPUART_WriteBlocking(DEMO_LPUART, ringBuffer, sizeof(ringBuffer)-1);
          LPUART_WriteBlocking(GSM_LPUART, ch1, sizeof(ch1)-1);
          
    }
}
