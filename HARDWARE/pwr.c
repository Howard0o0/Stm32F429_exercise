#include "pwr.h"
#include "led.h"
#include "delay.h"
#include "pwr.h"

extern RTC_HandleTypeDef RTC_Handle;

//PH3_n
void sleep_mode_init(void){
	
	//set PH3(KEY0) IT with falling mode
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOH_CLK_ENABLE();			//����GPIOAʱ��	
    GPIO_Initure.Pin=GPIO_PIN_3;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //�ж�,������
    GPIO_Initure.Pull=GPIO_PULLUP;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	HAL_NVIC_SetPriority(EXTI3_IRQn,0x02,0x02);//��ռ���ȼ�2�������ȼ�2
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
	
	__HAL_RCC_PWR_CLK_ENABLE();         //ʹ��PWRʱ��
//    __HAL_RCC_BACKUPRESET_FORCE();      //��λ��������
    HAL_PWR_EnableBkUpAccess();         //���������ʹ��  
	
	//STM32F4,��������RTC����жϺ�,�����ȹر�RTC�ж�,�����жϱ�־λ,Ȼ����������
	//RTC�ж�,�ٽ������ģʽ�ſ�����������,�����������.	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handle);//�ر�RTCд����
    
    //�ر�RTC����жϣ�������RTCʵ�����
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RTC_Handle,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&RTC_Handle,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&RTC_Handle,RTC_IT_ALRA|RTC_IT_ALRB);
    
    //���RTC����жϱ�־λ
    __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handle,RTC_FLAG_WUTF);
    
    __HAL_RCC_BACKUPRESET_RELEASE();                    //��������λ����
    __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handle);     //ʹ��RTCд����
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //���Wake_UP��־
	
		
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //����WKUP���ڻ���
    HAL_PWR_EnterSTANDBYMode();                         //�������ģʽ 
}
