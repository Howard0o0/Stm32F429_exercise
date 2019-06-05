#ifndef STM32F4_RTC
#define STM32F4_RTC

#include "sys.h"

typedef struct{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}RTC_TimeTypedef;

typedef struct{
	uint8_t WeekDay;
	uint8_t Month;
	uint8_t Date;
	uint8_t Year;
}RTC_CalendarTypedef;



void rtc_domain_access(void);
HAL_StatusTypeDef rtc_Init(void);
HAL_StatusTypeDef RTC_setTime(u8 hour,u8 min,u8 second,u8 timeFormat);
RTC_TimeTypedef* RTC_read_Time(void);

#endif
