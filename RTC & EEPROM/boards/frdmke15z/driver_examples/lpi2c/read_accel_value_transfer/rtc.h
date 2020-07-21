#ifndef __RTC_H__
#define __RTC_H__


#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_lpi2c.h"

#define I2C_BASEADDR LPI2C1
#define LPI2C_CLOCK_FREQUENCY CLOCK_GetIpFreq(kCLOCK_Lpi2c1)
#define I2C_BAUDRATE 100000U

#define RTC_ADDR               (0x68)                   //K dich bit


#define RTC_SECONDS_ADDR       0x00U
#define RTC_MINUTES_ADDR       0x01U
#define RTC_HOURS_ADDR         0x02U
#define RTC_DAY_ADDR           0x03U
#define RTC_DATE_ADDR          0x04U
#define RTC_MONTH_ADDR         0x05U
#define RTC_YEAR_ADDR          0x06U
#define RTC_CONTROL_ADDR       0x07U

typedef enum
{
    RTC_MODE_24H = 0x40,
    RTC_MODE_12AM = 0x02,
    RTC_MODE_12PM = 0x00,
}time_mode_t;

typedef enum
{
	SUN_DAY	 = 1,
	MON_DAY  = 2,
	TUS_DAY  = 3,
	WED_DAY  = 4,
	THUR_DAY = 5,
	FRI_DAY	 = 6,
	SAT_DAY	 = 7,
}day_t;

typedef enum 
{
	JANUARY		= 1,
	FEBRUARY	= 2,
	MARCH		= 3,
	APRIL		= 4,
	MAY		= 5,
	JUNE		= 6,
	JULY		= 7,
	AUGUST		= 8,
	SEPTEMBER	= 9,
	OCTOBER		= 10,
	NOVEMBER	= 11,
	DECEMBER	= 12,
}month_t;

typedef struct
{
        uint8_t sec;
        uint8_t min; 
        uint8_t hour; 
        uint8_t date;
        uint8_t month;
        uint8_t year;
}time_t;

void RTC_Init(void);
void setRTC(uint8_t sec, uint8_t min, uint8_t hour);
void RTC_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t date, uint8_t month, uint8_t year);
void getRTC(uint8_t *p_sec, uint8_t *p_min, uint8_t *p_hour);
void RTC_GetTime(uint8_t *p_sec, uint8_t *p_min, uint8_t *p_hour, uint8_t *p_date, uint8_t *p_month, uint8_t *p_year);
uint64_t RTC_GetUnixTime(void);
void getRTC(uint8_t *sec, uint8_t *min, uint8_t *hour);
bool RTC_Compare(time_t time1, time_t time2, uint16_t valueCompare);

#endif /* __RTC_H__ */