#include "rtc.h"
static int16_t DaysInMonth365[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static int16_t DaysInMonth366[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

static void RTC_InitPins(void)
{
    /* Clock Control: 0x01u */
    CLOCK_EnableClock(kCLOCK_PortD);

    const port_pin_config_t portd8_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as LPI2C1_SDA */
                                                   kPORT_MuxAlt2,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTA2 (pin 73) is configured as LPI2C0_SDA */
    PORT_SetPinConfig(PORTD, 8U, &portd8_config);

    const port_pin_config_t portd9_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as LPI2C1_SCL */
                                                   kPORT_MuxAlt2,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTD9 (pin 72) is configured as LPI2C0_SCL */
    PORT_SetPinConfig(PORTD, 9U, &portd9_config);
}

static void RTC_ConfigI2C(void)
{
	lpi2c_master_config_t masterConfig; CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcFircAsync);  //Chua biet lam gi

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
}

void RTC_Init(void)
{
	RTC_InitPins();
	RTC_ConfigI2C();
}

bool RTC_WriteReg(uint8_t reg_addr, uint8_t value)
{
    lpi2c_master_transfer_t masterXfer;
    status_t reVal = kStatus_Fail;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress   = RTC_ADDR;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = &value;
    masterXfer.dataSize       = 1;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

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

static bool RTC_ReadRegs(uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    lpi2c_master_transfer_t masterXfer;
    status_t reVal = kStatus_Fail;

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress   = RTC_ADDR ;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxSize;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

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

static uint8_t BCD(uint8_t data)
{       
    return ((data/10)*16) + (data%10);
}

static uint8_t DEC(uint8_t data)
{
    return (data>>4)*10 + (data&0x0f);
}

void setRTC(uint8_t sec, uint8_t min, uint8_t hour)
{
    RTC_WriteReg(0x00, BCD(sec));
    RTC_WriteReg(0x01, BCD(min));
    RTC_WriteReg(0x02, BCD(hour));
}
                      
void getRTC(uint8_t *p_sec, uint8_t *p_min, uint8_t *p_hour)
{
    uint8_t rxBuff[3];
    RTC_ReadRegs(RTC_SECONDS_ADDR, rxBuff, 3);
    *p_sec = DEC(rxBuff[0]);
    *p_min = DEC(rxBuff[1]);
    *p_hour = DEC(rxBuff[2]);

}

void RTC_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t date, uint8_t month, uint8_t year)
{
    RTC_WriteReg(0x00 , BCD(sec));
    RTC_WriteReg(0x01, BCD(min));
    RTC_WriteReg(0x02, BCD(hour));

    RTC_WriteReg(0x04, BCD(date));
    RTC_WriteReg(0x05, BCD(month));
    RTC_WriteReg(0x06, BCD(year));
}


void RTC_GetTime(uint8_t *p_sec, uint8_t *p_min, uint8_t *p_hour, uint8_t *p_date, uint8_t *p_month, uint8_t *p_year)
{
  uint8_t rxBuff1[3] = {0};
    RTC_ReadRegs(0x00, rxBuff1, 3);
    *p_sec = DEC(rxBuff1[0]);
    *p_min = DEC(rxBuff1[1]);
    *p_hour = DEC(rxBuff1[2]);
    
    uint8_t rxBuff2[3] = {0};
    RTC_ReadRegs(0x04, rxBuff2, 3);
    *p_date = DEC(rxBuff2[0]);
    *p_month = DEC(rxBuff2[1]);
    *p_year = DEC(rxBuff2[2]);
}


static bool isLeapYear(int year)
{
    if ((year % 4) != 0)
        return false;
    return true;
}

static uint8_t numberLeapYear(int year)
{
    uint8_t demLeapYear;
    for (uint8_t i = 0; i<year; i++)
      if (isLeapYear(year)) demLeapYear++;
    return demLeapYear;
}

static inline uint64_t DateToTicks(int year, int month, int date)
{
   if (year == 0)
        return (year*365 + DaysInMonth366[month-1] + (date-1)+ numberLeapYear(year) - 1);
   else if (isLeapYear(year) && year >0)  
        return (year*365 + DaysInMonth366[month-1] + (date-1)+ numberLeapYear(year));
   else return (year*365 + DaysInMonth365[month-1] + (date-1)+ numberLeapYear(year));
}

static inline uint64_t TimeToTicks(int hour, int minute, int second)
{
    return (hour*3600+minute*60+second);
}

uint64_t RTC_GetUnixTime()
{
     uint8_t sec, min, hour, date, month, year;
     RTC_GetTime(&sec, &min, &hour, &date, &month, &year);
     return (DateToTicks(year, month, date)*24*3600 + TimeToTicks(hour, min, sec));     
}

bool RTC_Compare(time_t time1, time_t time2, uint16_t valueCompare)
{
    if (time2.year != time1.year) return true;
    if (time2.month != time1.month) return true;
    if (time2.date != time1.date) return true;
    if (((time2.hour-time1.hour)*3600 + (time2.min - time1. min)*60 + (time2.sec -time1.sec)) > valueCompare) return true;
    return false;
}
