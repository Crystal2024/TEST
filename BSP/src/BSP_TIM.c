/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd  
    
   @File name: BSP_TIM.h
   @Author:   Fitz
   @Description:
            介绍:本文件为ESP32上定时器组的驱动文件
                 在ESP32中有2组定时器，每组2个 共四个。
                 本文件基于timer.h文件接口，封装配置函数，控制函数。
            API:
                1.函数:Timer_Config(timer_group_t group_num, 
                                    timer_idx_t timer_num,
                                    timer_isr_t timer_callback)
                    参数：timer_group_t group_num 定时器组 TIMER_GROUP_0   TIMER_GROUP_1
                    参数：timer_idx_t timer_num   定时器   TIMER_0         TIMER_1
                    参数：timer_isr_t timer_callback  回调函数入口 强制无符号（void*）
                    返回值：无
                2.函数：Timer_Start(timer_group_t group_num, timer_idx_t timer_num)
                    参数：timer_group_t group_num 定时器组 TIMER_GROUP_0   TIMER_GROUP_1
                    参数：timer_idx_t timer_num   定时器   TIMER_0         TIMER_1
                    返回值：无
            注意：定时器的中断的使用为回调函数，在Timer_Config()函数中传入回调函数即可
                 回调函数形式 “static bool IRAM_ATTR timer_group_callack()”              
   @Others:
        Data:2022/03/09
        Version:V1.0
        ESP_IDF Version:V4.3.1
   @History:


**********************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/timer.h"
#include "esp_log.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_TIM.h"
#include "BSP_MCPWM.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

/* USER CODE END PD */

/* Private typedef ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const char *TAG = "Timer";
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// EventBits_t timer;
// EventGroupHandle_t Timer_EventGroup;

//回调函数
// static void IRAM_ATTR timer_group_callack()
// {
//     static uint32_t KKCount=0;
//     KKCount++;
//     if(KKCount%2==0)
//     {
//        MCPWM_Start();
//     }
//     else
//     {
//      MCPWM_Stop();
//     }
//     return pdTRUE;
// }


extern bool IRAM_ATTR timer_group_callack(void *arg);


timer_config_t timer_config = {
        .divider = 80,                      //80分配 输出1Mhz 即1us
        .counter_dir = TIMER_COUNT_UP,      //计数器向上计数
        .alarm_en = TIMER_ALARM_EN,         //开启报警
        //.auto_reload = TIMER_AUTORELOAD_EN, //自动重载开启
        .auto_reload =  TIMER_AUTORELOAD_DIS,//不自动重载开启
        .counter_en = TIMER_PAUSE,          //不启动定时器
    };

void TIM_Config(timer_group_t group_num, timer_idx_t timer_num,uint16_t alarm_value,timer_isr_t timer_callback)
{
    ESP_ERROR_CHECK(timer_init(group_num,timer_num,&timer_config));

    ESP_ERROR_CHECK(timer_set_counter_value(group_num,timer_num,0));
    ESP_ERROR_CHECK(timer_set_alarm_value(group_num,timer_num,alarm_value)); //set 100us
    ESP_ERROR_CHECK(timer_enable_intr(group_num,timer_num));         //开启中断

    timer_info_t *timer0_info = calloc(1,sizeof(timer_info_t)); //分配内存
    timer0_info ->timer_group = group_num;
    timer0_info ->timer_idx   = timer_num;
    //timer0_info ->auto_reload = TIMER_AUTORELOAD_EN; //开启报警
    timer0_info ->auto_reload =TIMER_AUTORELOAD_DIS;
    timer0_info ->alarm_interval = 6;
    timer_isr_callback_add(TIMER_GROUP_0,TIMER_0,timer_callback,timer0_info,0);//注册回调函数


}

void TIM_Start(timer_group_t group_num, timer_idx_t timer_num,uint16_t alarm_value)
{
    timer_deinit(group_num,timer_num);
    TIM_Config(TIMER_GROUP_0,TIMER_0,alarm_value,timer_group_callack);
    timer_start(group_num,timer_num);
}

/* USER CODE END 0 */

/*----------------------------------END---------------------------------------*/






