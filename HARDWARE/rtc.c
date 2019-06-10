#include "rtc.h"
#include "sys.h"
#include <stdlib.h>

/* global variable */
RTC_HandleTypeDef RTC_Handle;
RTC_TimeTypeDef RTC_TIME_Handle;
RTC_DateTypeDef RTC_DATE_Handle;
RTC_GetTimeTypedef RTC_GetTime;
RTC_GetCalendarTypedef RTC_GetCalendar;




HAL_StatusTypeDef RTC_Init(void){

	RTC_Handle.Instance = RTC;	
	RTC_Handle.Init.HourFormat =  RTC_HOURFORMAT_24;
	RTC_Handle.Init.AsynchPrediv = 0x7F;  //127
	RTC_Handle.Init.SynchPrediv = 0xFF; //255
	RTC_Handle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTC_Handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RTC_Handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	HAL_StatusTypeDef rtc_init_result = HAL_RTC_Init(&RTC_Handle);
	
	
	if(rtc_init_result != HAL_OK)
		return HAL_ERROR;
	
	uint32_t rtc_backup_R0 = HAL_RTCEx_BKUPRead(&RTC_Handle,RTC_BKP_DR0);
	if(rtc_backup_R0 != 0x0505){ //first time configuration, need to set time
		RTC_setTime(17,27,0,RTC_HOURFORMAT12_PM);
		RTC_setDate(19,6,5,3);
		HAL_RTCEx_BKUPWrite(&RTC_Handle,RTC_BKP_DR0,0x0505); //write a conf-flag into BACKUO_0_REG		
	}
	return HAL_OK;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE配置
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;                  //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
	
	
//	__HAL_RCC_PWR_CLK_ENABLE();
//	HAL_PWR_EnableBkUpAccess();
//	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
//	__HAL_RCC_RTC_ENABLE();
}

//timeFormat : RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
HAL_StatusTypeDef RTC_setTime(u8 hour,u8 min,u8 second,u8 timeFormat){
	
	RTC_TIME_Handle.Hours = hour;
	RTC_TIME_Handle.Minutes = min;
	RTC_TIME_Handle.Seconds = second;
	RTC_TIME_Handle.TimeFormat = timeFormat;
	return HAL_RTC_SetTime(&RTC_Handle,&RTC_TIME_Handle,RTC_FORMAT_BIN);
}

HAL_StatusTypeDef RTC_read_Time(void){
	HAL_StatusTypeDef result = HAL_RTC_GetTime(&RTC_Handle, &RTC_TIME_Handle, RTC_FORMAT_BIN);
	if (result == HAL_ERROR)
		return HAL_ERROR;
	else{
		RTC_GetTime.hour = RTC_TIME_Handle.Hours;
		RTC_GetTime.min = RTC_TIME_Handle.Minutes;
		RTC_GetTime.sec = RTC_TIME_Handle.Seconds;
		return HAL_OK;
	}
}

HAL_StatusTypeDef RTC_setDate(u8 Year,u8 Month,u8 Date,u8 WeekDay){
	
	RTC_DATE_Handle.Year = Year;
	RTC_DATE_Handle.Month = Month;
	RTC_DATE_Handle.Date = Date;
	RTC_DATE_Handle.WeekDay = WeekDay;
	return HAL_RTC_SetDate (&RTC_Handle, &RTC_DATE_Handle, RTC_FORMAT_BIN);
}

HAL_StatusTypeDef RTC_read_Date(void){
	
	HAL_StatusTypeDef result = HAL_RTC_GetDate(&RTC_Handle, &RTC_DATE_Handle,RTC_FORMAT_BIN);
	if (result == HAL_ERROR)
		return HAL_ERROR;
	else{
		RTC_GetCalendar.Year = RTC_DATE_Handle.Year;
		RTC_GetCalendar.Month = RTC_DATE_Handle.Month;
		RTC_GetCalendar.Date = RTC_DATE_Handle.Date;
		RTC_GetCalendar.WeekDay = RTC_DATE_Handle.WeekDay;
		return HAL_OK;
	}
}

HAL_StatusTypeDef RTC_SetAlarm_A(u8 Date_WeekDay,u8 Hour,u8 Minute,u32 DateWeekDaySel){
	
	RTC_AlarmTypeDef rtc_alarm_init_structure;
	rtc_alarm_init_structure.AlarmMask = RTC_ALARMMASK_NONE;
	rtc_alarm_init_structure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	rtc_alarm_init_structure.AlarmDateWeekDaySel = DateWeekDaySel;
	rtc_alarm_init_structure.AlarmDateWeekDay = Date_WeekDay;
	rtc_alarm_init_structure.Alarm = RTC_ALARM_A;
	
	rtc_alarm_init_structure.AlarmTime.Hours = Hour;
	rtc_alarm_init_structure.AlarmTime.Minutes = Minute;
	rtc_alarm_init_structure.AlarmTime.Seconds = 0;
	rtc_alarm_init_structure.AlarmTime.SubSeconds = 0;
	if(Hour <= 12)
		rtc_alarm_init_structure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
	else
		rtc_alarm_init_structure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_PM;
	
	
	HAL_StatusTypeDef set_alarm_result = HAL_RTC_SetAlarm_IT(&RTC_Handle, &rtc_alarm_init_structure,RTC_FORMAT_BIN);
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn,0x01,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	
	return set_alarm_result;
}

void RTC_Alarm_IRQHandler(void){
	 HAL_RTC_AlarmIRQHandler(&RTC_Handle);
}

void HAL_RTC_AlarmAEventCallback (RTC_HandleTypeDef *hrtc){
	printf("RTC_Alarm_A triggered!!\r\n");
}

