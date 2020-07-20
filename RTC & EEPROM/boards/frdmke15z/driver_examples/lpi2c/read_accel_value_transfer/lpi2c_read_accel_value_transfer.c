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

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_BASEADDR LPI2C1
#define LPI2C_CLOCK_FREQUENCY CLOCK_GetIpFreq(kCLOCK_Lpi2c1)
#define I2C_BAUDRATE 100000U

/* Accelerometer Reset PIN */
#define BOARD_ACCEL_RESET_GPIO GPIOB
#define BOARD_ACCEL_RESET_PIN 9U

#define RTC_ADDR               (0x68)                   //K dich bit
#define E2PROM0_ADDR           (0x50)
#define E2PROM1_ADDR           (0x51)

#define RTC_SECONDS_ADDR       0x00U
#define RTC_MINUTES_ADDR       0x01U
#define RTC_HOURS_ADDR         0x02U
#define RTC_DAY_ADDR           0x03U
#define RTC_DATE_ADDR          0x04U
#define RTC_MONTH_ADDR         0x05U
#define RTC_YEAR_ADDR          0x06U
#define RTC_CONTROL_ADDR       0x07U


#define I2C_BAUDRATE 100000U
#define FXOS8700_WHOAMI 0xC7U
#define MMA8451_WHOAMI 0x1AU
#define ACCEL_STATUS 0x00U
#define ACCEL_XYZ_DATA_CFG 0x0EU
#define ACCEL_CTRL_REG1 0x2AU
/* FXOS8700 and MMA8451 have the same who_am_i register address. */
#define ACCEL_WHOAMI_REG 0x0DU
#define ACCEL_READ_TIMES 10U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static bool LPI2C_WriteReg(LPI2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value);
static bool LPI2C_ReadRegs(
    LPI2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);

//static bool LPI2C_WriteRTCReg(uint8_t reg_addr, uint8_t value);
//static bool LPI2C_ReadRTCRegs(uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);
//
//static bool LPI2C_WriteE2prom0Reg(uint8_t reg_addr, uint8_t value);
//static bool LPI2C_ReadE2prom0Regs(uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);

typedef enum
{
    RTC_MODE_24H = 0x40,
    RTC_MODE_12AM = 0x02,
    RTC_MODE_12PM = 0x00,
}time_mode_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*  FXOS8700 and MMA8451 device address */
const uint8_t g_accel_address[] = {0x1CU, 0x1DU, 0x1EU, 0x1FU};

lpi2c_master_handle_t g_m_handle;

uint8_t g_accel_addr_found = 0x00U;

volatile bool completionFlag = false;
volatile bool nakFlag        = false;

/*******************************************************************************
 * Code
 ******************************************************************************/




static bool LPI2C_WriteReg(LPI2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value)
{
    lpi2c_master_transfer_t masterXfer;
    status_t reVal = kStatus_Fail;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress   = device_addr;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = &value;
    masterXfer.dataSize       = 1;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

    //reVal = LPI2C_MasterTransferNonBlocking(I2C_BASEADDR, &g_m_handle, &masterXfer);
    reVal = LPI2C_MasterTransferBlocking(I2C_BASEADDR, &masterXfer);
    if (reVal != kStatus_Success)
    {
        return false;
    }
    else
    {
        return true;
    }

}

static bool LPI2C_ReadRegs(
    LPI2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    lpi2c_master_transfer_t masterXfer;
    status_t reVal = kStatus_Fail;

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress   = device_addr;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxSize;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

    //reVal = LPI2C_MasterTransferNonBlocking(base, &g_m_handle, &masterXfer);
    reVal = LPI2C_MasterTransferBlocking(I2C_BASEADDR,  &masterXfer);
    if (reVal != kStatus_Success)
    {
        return false;
    }
        else
    {
        return true;
    }
}

static bool LPI2C_WriteRTCReg(uint8_t reg_addr, uint8_t value)
{
    return LPI2C_WriteReg(I2C_BASEADDR, RTC_ADDR, reg_addr, value);
}
static bool LPI2C_ReadRTCRegs(uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    return LPI2C_ReadRegs(I2C_BASEADDR, RTC_ADDR, reg_addr, rxBuff, rxSize);
}


static bool LPI2C_WriteE2prom0Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Write))
    {
        /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + regHigh_Address + regLowAddress + length of data buffer + data buffer + stop*/
        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);
        
        uint8_t g_master_txBuff[1];
        g_master_txBuff[0] = value;
        LPI2C_MasterSend(I2C_BASEADDR, g_master_txBuff, 1);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    return true;
}
static bool LPI2C_ReadE2prom0Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Write))
    {

        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);

        LPI2C_MasterRepeatedStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Read);

        LPI2C_MasterReceive(I2C_BASEADDR, rxBuff, rxSize);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    else return false;
    return true;
}

static bool LPI2C_WriteE2prom1Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Write))
    {
        /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + regHigh_Address + regLowAddress + length of data buffer + data buffer + stop*/
        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);
        
        uint8_t g_master_txBuff[1];
        g_master_txBuff[0] = value;
        LPI2C_MasterSend(I2C_BASEADDR, g_master_txBuff, 1);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    return true;
}
static bool LPI2C_ReadE2prom1Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Write))
    {

        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);

        LPI2C_MasterRepeatedStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Read);

        LPI2C_MasterReceive(I2C_BASEADDR, rxBuff, rxSize);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    else return false;
    return true;
}

uint8_t BCD(uint8_t data)
{       
    return ((data/10)*16) + (data%10);
}

uint8_t DEC(uint8_t data)
{
    return (data>>4)*10 + (data&0x0f);
}
void setRTC(uint8_t sec, uint8_t min, uint8_t hour, time_mode_t mode)
{
    LPI2C_WriteRTCReg(0x00, BCD(sec));
    LPI2C_WriteRTCReg(0x01, BCD(min));
    LPI2C_WriteRTCReg(0x02, (BCD(hour)| mode));
}
void getRTC(uint8_t *p_sec, uint8_t *p_min, uint8_t *p_hour, time_mode_t *mode)
{
    uint8_t rxBuff[3];
    LPI2C_ReadRTCRegs(RTC_SECONDS_ADDR, rxBuff, 3);
    *p_sec = DEC(rxBuff[0]);
    *p_min = DEC(rxBuff[1]);
    *p_hour = rxBuff[2];
    if ((*p_hour) & 0x40)               //Mode 24H
    {
        *p_hour = DEC(*p_hour & 0x3F);
        *mode = RTC_MODE_24H;
    }
    else                                //Mode 12H
    {
        if ((*p_hour)&0x20)
        {
          *mode = RTC_MODE_12PM;
        }
        else 
        {
          *mode = RTC_MODE_12PM;
        }
        *p_hour = DEC(*p_hour & 0x1F);
    }
}


int main(void)
{
    lpi2c_master_config_t masterConfig;

    BOARD_InitPins();                                   //Can define lai (co the tu viet)
    BOARD_BootClockRUN();                               //Default    
    //BOARD_ACCEL_Reset();                               //Ham moi        
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



    setRTC(30, 57, 16, RTC_MODE_24H);

    uint8_t second, min, hour;   
    time_mode_t Mode;
    uint8_t rxdata[2] = {0};
    uint8_t rxdata1[2]= {0};
//    LPI2C_WriteE2prom0Reg(0x00, 0x00, 0x15);
    LPI2C_ReadE2prom0Regs(0x00, 0x00, rxdata, 1);
//    LPI2C_WriteE2prom1Reg(0x00, 0x00, 0x2E);
    LPI2C_ReadE2prom1Regs(0x00, 0x00, rxdata1, 1);
    while (1)
    {             
          getRTC(&second, &min, &hour, &Mode);
    }
}
