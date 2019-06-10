#include "pwr.h"
#include "led.h"
#include "delay.h"
#include "pwr.h"

extern RTC_HandleTypeDef RTC_Handle;

//PH3_n
void sleep_mode_init(void){
	
	//set PH3(KEY0) IT with falling mode
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOA时钟	
    GPIO_Initure.Pin=GPIO_PIN_3;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //中断,上升沿
    GPIO_Initure.Pull=GPIO_PULLUP;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_NVIC_SetPriority(EXTI3_IRQn,0x02,0x02);//抢占优先级2，子优先级2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	
}

void EXTI3_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin){
	
	if(GPIO_Pin == GPIO_PIN_3){
		LED_1_Ctrl(1);
		delay_ms(500);
		delay_ms(500);
		/* enter standby mode */
		
		while(HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3) == 0);
		PWR_goto_standby_mode();
	}
}

void PWR_goto_standby_mode(void){
	
	__HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
//    __HAL_RCC_BACKUPRESET_FORCE();      //复位备份区域
    HAL_PWR_EnableBkUpAccess();         //后备区域访问使能  
	
	//STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
	//RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handle);//关闭RTC写保护
    
    //关闭RTC相关中断，可能在RTC实验打开了
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RTC_Handle,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&RTC_Handle,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&RTC_Handle,RTC_IT_ALRA|RTC_IT_ALRB);
    
    //清除RTC相关中断标志位
    __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_WUTF);
    
    __HAL_RCC_BACKUPRESET_RELEASE();                    //备份区域复位结束
    __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handle);     //使能RTC写保护
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志
	
		
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //设置WKUP用于唤醒
    HAL_PWR_EnterSTANDBYMode();                         //进入待机模式 
}
