#include "rtc.h"
#include "sys.h"
#include <stdlib.h>

/* global variable */
RTC_HandleTypeDef RTC_Handle;
RTC_TimeTypeDef RTC_TIME_Handle;


void rtc_domain_access(void){
	
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
	__HAL_RCC_RTC_ENABLE();
}


HAL_StatusTypeDef rtc_Init(void){
	
	rtc_domain_access();

	RTC_Handle.Instance = RTC;	
	RTC_Handle.Init.HourFormat =  RTC_HOURFORMAT_24;
	RTC_Handle.Init.AsynchPrediv = 0X7F;  //127
	RTC_Handle.Init.SynchPrediv = 0xFF; //255
	RTC_Handle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTC_Handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RTC_Handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	HAL_StatusTypeDef rtc_init_result = HAL_RTC_Init(&RTC_Handle);
	
	
	if(rtc_init_result != HAL_OK)
		return HAL_ERROR;
	else{
		uint32_t rtc_backup_R0 = HAL_RTCEx_BKUPRead(&RTC_Handle,RTC_BKP_DR0);
		if(rtc_backup_R0 == 0x0505){ //first time configuration, need to set time
			RTC_setTime(17,27,0,RTC_HOURFORMAT12_PM);
			HAL_RTCEx_BKUPWrite(&RTC_Handle,RTC_BKP_DR0,0x0505); //write a conf-flag into BACKUO_0_REG		
		}
	}
	return HAL_OK;
}

//timeFormat : RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
HAL_StatusTypeDef RTC_setTime(u8 hour,u8 min,u8 second,u8 timeFormat){
	
	RTC_TIME_Handle.Hours = hour;
	RTC_TIME_Handle.Minutes = min;
	RTC_TIME_Handle.Seconds = second;
	RTC_TIME_Handle.TimeFormat = timeFormat;
	return HAL_RTC_SetTime(&RTC_Handle,&RTC_TIME_Handle,RTC_FORMAT_BIN);
}

RTC_TimeTypedef* RTC_read_Time(void){
	HAL_StatusTypeDef result = HAL_RTC_GetTime(&RTC_Handle, &RTC_TIME_Handle, RTC_FORMAT_BIN);
	if (result == HAL_ERROR)
		return NULL;
	else{
		RTC_TimeTypedef* RTC_Time = (RTC_TimeTypedef*)malloc(sizeof(RTC_TimeTypedef));
		RTC_Time->hour = RTC_TIME_Handle.Hours;
		RTC_Time->min = RTC_TIME_Handle.Minutes;
		RTC_Time->sec = RTC_TIME_Handle.Seconds;
		return RTC_Time;
	}
}






