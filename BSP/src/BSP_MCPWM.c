/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_MCPWM.h
    @Author:    Fitz
    @Description:

                
                由于需要的频率较高，因此在mcpwm.c文件第59，60行
                的宏定义MCPWM_CLK_PRESCL，TIMER_CLK_PRESCALE均
                设置为 0 。
    @Others:
        Data:2022/01/06
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:
 
 **********************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "driver/timer.h"
#include "driver/gpio.h"
#include "hal\gpio_ll.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_MCPWM.h"
#include "BSP_TIM.h"
#include "BSP_COMM.h"
#include "BSP_SPIADC.h"




/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const char *TAG = "PWM";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void SET_GainNum(uint8_t Num);
/* USER CODE END PFP */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};
uint8_t CapFlag=0;
// mcpwm event
static void IRAM_ATTR Cap_isr_handler(void *arg)
{
    MCPWM_Stop();
    MCPWM[MCPWM_UNIT_0]->int_clr.val=MCPWM_LL_INTR_CAP0;
    // mcpwm_capture_disable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0);
    //  xEventGroupSetBits(PACKEvent_Group,Samplingbit); 

}
/* USER CODE END 0 */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MCPWMAB_Config(int gpioa_num, int gpiob_num, uint32_t frequency,float dutya,float dutyb)
{
    mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0A,gpioa_num);//set PWM0A GPIO_number
    mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0B,gpiob_num);//set PWM0B GPIO_number

    mcpwm_config_t pwm_init;
    pwm_init.frequency = frequency; //set frequency 
    pwm_init.cmpr_a = dutya;       //PWMxA duty 
    pwm_init.cmpr_b = dutyb;       //PWMxB duty 
    pwm_init.duty_mode = MCPWM_DUTY_MODE_0;//
    pwm_init.counter_mode = MCPWM_UP_COUNTER;

    /* set deadtime */
    mcpwm_deadtime_enable(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE,48,48);//set MCPWM0-TIMER0 
    
    mcpwm_init(MCPWM_UNIT_0,MCPWM_TIMER_0,&pwm_init);

}   

void MCPWM_Config(int gpio_num, uint32_t frequency,float duty, void (*fn)(void *))
{
    mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0A,gpio_num);//set pwm output gpio nummber
    mcpwm_config_t pwm_init;
    pwm_init.frequency = frequency; //set frequency
    pwm_init.cmpr_a = duty;         //set PWM0A duty
    pwm_init.duty_mode = MCPWM_DUTY_MODE_0;
    pwm_init.counter_mode = MCPWM_UP_COUNTER;
    mcpwm_init(MCPWM_UNIT_0,MCPWM_TIMER_0,&pwm_init);
    mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
    // pcnt_isr_register

}
void MCPWM_Capture_Config(int gpio_num)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, gpio_num);
    gpio_pulldown_en(gpio_num);    //Enable pull down on CAP0   signal
    MCPWM[MCPWM_UNIT_0]->int_ena.val = MCPWM_LL_INTR_CAP0 ; //Enable interrupt on  CAP0
    mcpwm_isr_register(MCPWM_UNIT_0, Cap_isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);  //Set ISR Handler

}
void MCPWM_ISRSet(mcpwm_unit_t pwm_unit, void (*fn)(void *))
{
    mcpwm_isr_register(pwm_unit,fn,NULL,ESP_INTR_FLAG_IRAM,NULL);
}

void MCPWM_Freq(uint32_t frequency)
{
    mcpwm_set_frequency(MCPWM_UNIT_0,MCPWM_TIMER_0,frequency);
}

void MCPWM_Start()
{
    mcpwm_start(MCPWM_UNIT_0,MCPWM_TIMER_0);
    // mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
    // mcpwm_set_duty_type(MCPWM_UNIT_0,MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void MCPWM_Stop()
{
    mcpwm_stop(MCPWM_UNIT_0,MCPWM_TIMER_0);
    // mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
    
}

uint32_t MCPWM_REG_TEST()
{
    MCPWM0.int_ena.timer0_tep_int_ena =1;
    MCPWM0.update_cfg.global_up_en =1;
    return MCPWM0.int_ena.timer0_tep_int_ena;
}

void MCPWM_Tran(uint8_t PulseNum)
{
    // PWM_TimeStart(100*timeMode+100);
    mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE,PulseNum);
    MCPWM_Start();
   
}

void SOFT_PWM(uint8_t A_GPIO_Num,uint8_t B_GPIO_Num,uint32_t Fre,uint8_t Time)
{
   float FreDelay=240*(1.0*1000000/(Fre*2)-0.1)-33;
   uint8_t PulseNum=Fre*Time/1000000;
   uint16_t FreDelay1=FreDelay*0.9;
   uint16_t FreDelay2=FreDelay*0.05;
   gpio_dev_t *dev= (gpio_dev_t *)GPIO_LL_GET_HW(GPIO_PORT_0);
 
   for(uint8_t i=0;i<PulseNum;i++)
   {   
     dev->out_w1ts = 1 << A_GPIO_Num;//拉高
     dev->out_w1tc = 1 << B_GPIO_Num;//拉低
     delay_clock((uint16_t)FreDelay);
     dev->out_w1tc = 1 << A_GPIO_Num;//拉低
     //5%
     delay_clock((uint16_t)FreDelay2); 
     dev->out_w1ts = 1 << B_GPIO_Num;//拉高
     //90%
     delay_clock((uint16_t)FreDelay1); 
     dev->out_w1tc = 1 << B_GPIO_Num;//拉低 
     //5%
     delay_clock((uint16_t)FreDelay2); 
   }  
}

/* USER CODE END 0 */
/*----------------------------------END---------------------------------------*/