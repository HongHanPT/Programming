/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#include "e2prom.h"
#include "rtc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_BASEADDR LPI2C1
#define LPI2C_CLOCK_FREQUENCY CLOCK_GetIpFreq(kCLOCK_Lpi2c1)


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


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


uint8_t second = 0, min = 0, hour = 0, date = 0, year = 0;  
uint8_t month = 0;
uint8_t sec1, min1, hour1;
uint64_t value;
time_t time1, time2;
int main(void)
{
    //LPI2C_MasterDeinit(I2C_BASEADDR);
    lpi2c_master_config_t masterConfig;
    //RTC_Init();
    //E2PROM_Init();
    //BOARD_InitPins();                                   //Can define lai (co the tu viet)
    BOARD_BootClockRUN();                               //Default  
    BOARD_I2C_ConfigurePins();                          //Can define lai
    BOARD_InitDebugConsole();                           //Default

    CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcFircAsync);  //Chua biet lam gi


    /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * masterConfig.baudRate_Hz = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);        //Khai bao mac dinh cua I2C, co the thay doi thong so, nhu ben duoi

    masterConfig.baudRate_Hz = I2C_BAUDRATE;            // Thay doi toc do BAURATE

    LPI2C_MasterInit(I2C_BASEADDR, &masterConfig, LPI2C_CLOCK_FREQUENCY);   //Khoi tao I2C

    
      //setRTC(55, 59, 23);
      //RTC_SetTime(0, 0, 0, 1, 1, 0);


    
    uint8_t rxdata[2] = {0};
    uint8_t rxdata1[2]= {0};
    E2PROM_WriteE2prom0Reg(0x00, 0x00, 0x35);
//    LPI2C_WriteE2prom0Reg(0x00, 0x00, 0x35);
    delay(10);
    E2PROM_ReadE2prom0Regs(0x00, 0x00, rxdata, 1);
//    LPI2C_ReadE2prom0Regs(0x00, 0x00, rxdata, 1);
   E2PROM_WriteE2prom1Reg(0x00, 0x00, 0x2E);
    delay(10);
    E2PROM_ReadE2prom1Regs(0x00, 0x00, rxdata1, 1);
    while (1)
    {             
         RTC_GetTime(&second, &min, &hour, &date, &month, &year);
         getRTC(&sec1, &min1, &hour1);
         time1.sec = second;
         time1.min = min;
         time1.hour = hour;
         time1.date = date;
         time1.month = month;
         time1.year = year;
         //delay(100);
         
         value = RTC_GetUnixTime();
         RTC_GetTime(&second, &min, &hour, &date, &month, &year);
         time2.sec = second;
         time2.min = min;
         time2.hour = hour;
         time2.date = date;
         time2.month = month;
         time2.year = year;
         
         bool test = RTC_Compare(time1, time2, 10);
         
    }
}
