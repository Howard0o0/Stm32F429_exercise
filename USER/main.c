#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "usmart.h"
#include "rtc.h"
#include "rng.h"




//LED状态设置函数
void led_set(u8 sta)
{
	LED1=sta;
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
}     

int main(void)
{
	extern RTC_GetTimeTypedef RTC_GetTime;
	extern RTC_GetCalendarTypedef RTC_GetCalendar;
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART	
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
    LCD_Init();                     //初始化LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");	
	LCD_ShowString(30,70,200,16,16,"USMART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2015/12/10");

	//Init RTC
	HAL_StatusTypeDef rtc_init_result = RTC_Init();
	if(rtc_init_result != HAL_OK){
		printf("RTC Init error!\r\n");
		return 0;
	}
	//Set RTC_Alarm_A
	if(RTC_SetAlarm_A(6,16,25,RTC_ALARMDATEWEEKDAYSEL_DATE) != HAL_OK)
		printf("RTC_ALARM set failed!\r\n");
	
	//Init RNG
	if(RNG_Init() != HAL_OK){
		printf("RNG Init Failed! \r\n");
		return 0;
	}
		

  	while(1) 
	{		
		RTC_read_Time();
		RTC_read_Date();
		printf("RTC_Time: %d:%d:%d  ",RTC_GetTime.hour,RTC_GetTime.min,RTC_GetTime.sec);
		printf("%d-%d-%d week[%d] \r\n",RTC_GetCalendar.Year,RTC_GetCalendar.Month,RTC_GetCalendar.Date,RTC_GetCalendar.WeekDay);
		LED0=!LED0;		
			
		int range_random = RNG_GetRangeRandom(-5,-1);
		printf("RNG_Random data: %d\r\n",range_random);
		delay_ms(500);
		delay_ms(500);			
	} 
	return 0;
}
