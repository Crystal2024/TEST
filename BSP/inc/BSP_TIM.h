/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd  
    
   @File name: BSP_TIM.h
   @Author:   Fitz
   @Description:
            介绍:本文件为ESP32上定时器组的驱动文件
                 在ESP32中有2组定时器，每组2个 共四个。
                 本文件基于timer.h文件接口，封装配置函数，控制函数。
            API:
                1.函数:TIM_Config(timer_group_t group_num, 
                                    timer_idx_t timer_num,
                                    timer_isr_t timer_callback)
                    参数：timer_group_t group_num 定时器组 TIMER_GROUP_0   TIMER_GROUP_1
                    参数：timer_idx_t timer_num   定时器   TIMER_0         TIMER_1
                    参数：timer_isr_t timer_callback  回调函数入口 强制无符号（void*）
                    返回值：无
                2.函数：TIM_Start(timer_group_t group_num, timer_idx_t timer_num)
                    参数：timer_group_t group_num 定时器组 TIMER_GROUP_0   TIMER_GROUP_1
                    参数：timer_idx_t timer_num   定时器   TIMER_0         TIMER_1
                    返回值：无
            注意：定时器的中断的使用为回调函数，在TIM_Config()函数中传入回调函数即可
                 回调函数形式 “static bool IRAM_ATTR timer_group_callack()”              
   @Others:
        Data:2021/11/03
        Version:V1.0
        ESP_IDF Version:V4.3.1
   @History:


**********************************************************/
#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_

#ifdef _cplusplus
extern "C"{
#endif
#define Timer_ISR (0x01 << 0)
// EventBits_t timer;
// EventGroupHandle_t Timer_EventGroup;
typedef struct 
{
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
    char *adc_add;
}timer_info_t;


/**
 * @brief 定时器配置
 *
 * @param group_num 定时器组 TIMER_GROUP_x
 * @param timer_num 定时器号 TIMER_x      
 * @param timer_callback 回调函数
 *  
 * @note
 *
 */
void TIM_Config(timer_group_t group_num, timer_idx_t timer_num,uint16_t alarm_value,timer_isr_t timer_callback);

/**
 * @brief 开启定时器
 *
 * @param group_num 定时器组 TIMER_GROUP_x
 * @param timer_num 定时器号 TIMER_x        
 *  
 * @note
 *
 */

void TIM_Start(timer_group_t group_num, timer_idx_t timer_num,uint16_t alarm_value);

#ifdef _cplusplus
}
#endif

#endif