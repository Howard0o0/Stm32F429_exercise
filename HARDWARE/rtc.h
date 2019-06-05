#ifndef STM32F4_RTC
#define STM32F4_RTC

#include "sys.h"

typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}RTC_GetTimeTypedef;

typedef struct{
	uint8_t WeekDay;
	uint8_t Month;
	uint8_t Date;
	uint8_t Year;
}RTC_GetCalendarTypedef;



/*
@func: RTC Init , call before use following API
@return: HAL_OK or HAL_ERROR
*/
HAL_StatusTypeDef RTC_Init(void);

/*
@func: set RTC time
@para: hour,min,second
       timeFormate -> RTC_HOURFORMAT12_AM || RTC_HOURFORMAT12_PM
@return: HAL_OK || HAL_ERROR
*/
HAL_StatusTypeDef RTC_setTime(u8 hour,u8 min,u8 second,u8 timeFormat);

/*
@func: read RTC time
@return: RTC_TimeTypedef*
*/
HAL_StatusTypeDef RTC_read_Time(void);

/*
@func: set RTC date
@para: Year,month,date,weekday
@return: HAL_OK || HAL_ERROR
*/
HAL_StatusTypeDef RTC_setDate(u8 Year,u8 Month,u8 Date,u8 WeekDay);

/*
@func: read RTC date
@return: RTC_CalendarTypedef*
*/
HAL_StatusTypeDef RTC_read_Date(void);



static void rtc_domain_access(void);

#endif
