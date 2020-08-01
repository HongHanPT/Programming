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
#include "cool.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPUART LPUART0
#define DEMO_LPUART_CLKSRC kCLOCK_ScgSysOscClk
#define DEMO_LPUART_CLK_FREQ CLOCK_GetFreq(kCLOCK_ScgSysOscClk)
#define ECHO_BUFFER_LENGTH 8

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
extern volatile uint32_t g_timeSysTick;
extern uint8_t temp;

/*******************************************************************************
 * Code
 ******************************************************************************/
   
void SysTick_Handler(void)
{
    if (g_timeSysTick != 0xFFFFFFFF)
    {
        g_timeSysTick++;
    }
    else g_timeSysTick = 0;
}

/* LPUART user callback */
void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    lpuart_transfer_t receiveXfer;
    userData = userData;
    
    receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = 1;

    if (kStatus_LPUART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing    = false;
    }

    if (kStatus_LPUART_RxIdle == status)
    {
        
        temp = *receiveXfer.data;
        LPUART_TransferReceiveNonBlocking(DEMO_LPUART, &g_lpuartHandle, &receiveXfer, NULL);
    }
}
uint32_t dem;
/*!
 * @brief Main function
 */
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
cooling_t p_cooling= {.state = START, .timeStartState = NO_COUT, .fanState = TURNING_OFF, .coolerState = TURNING_OFF};
uint32_t test;
int main(void)
{
    lpuart_config_t config;
    lpuart_transfer_t receiveXfer;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcSysOscAsync);

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
    LPUART_TransferCreateHandle(DEMO_LPUART, &g_lpuartHandle, LPUART_UserCallback, NULL);

    /* Send g_tipString out. */
    txOnGoing     = true;
    LPUART_WriteBlocking(DEMO_LPUART, g_tipString, sizeof(g_tipString) - 1);

    receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = 1;
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }

    LPUART_TransferReceiveNonBlocking(DEMO_LPUART, &g_lpuartHandle, &receiveXfer, NULL);
    while (1)
    {
          procesColling(&p_cooling);
          test = 1 - 0xFFFFFFFF;
    }
}
