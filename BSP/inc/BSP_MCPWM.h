/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_MCPWM.h
    @Author:    Fitz
    @Description:
    			MCPWM二次封装，使用MCPWM_Config()或	MCPWMAB_Config()进行初始化
                注意：初始化后将默认开启PWM，若有需求可在mcpwm.c文件中进行修改	

    @Others:
        Data:2022/01/06
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:
 
 **********************************************************/
#ifndef _BSP_MCPWM_H_
#define _BSP_MCPWM_H_



#ifdef _cplusplus
extern "C"{
#endif
uint8_t CapFlag;
#define PWM_TimeStart(m)  TIM_Start(TIMER_GROUP_0,TIMER_0,m)

/**
 * @brief 单端PWM输出初始化
 *
 * @note  初始化MCPWM
 * 
 * @param gpio_num  PWM输出管脚
 * @param frequency PWM频率
 * @param duty      PWM占空比
 */
void MCPWM_Config(int gpio_num, uint32_t frequency,float duty, void (*fn)(void *));

/**
 * @brief 互补PWM输出初始化
 *
 * @note  初始化MCPWM
 * 
 * @param gpioa_num PWM0A输出管脚
 * @param gpiob_num PWM0B输出管脚
 * @param frequency PWM频率
 * @param dutya     PWM0A占空比
 * @param dutyb     PWM0B占空比 
 */
void MCPWMAB_Config(int gpioa_num, int gpiob_num, uint32_t frequency,float dutya,float dutyb);

/**
 * @brief PWM频率设置
 * 
 * @param frequency PWM频率
 * 
 */
void MCPWM_Freq(uint32_t frequency);

/**
 * @brief PWM开启
 * @param Fre 频率
 */
void MCPWM_Start();

/**
 * @brief PWM关闭
 * 
 */
void MCPWM_Stop();

/**
 * @brief PWM发送定频波
 * @param Fre_Mode 频率模式0：低频1：高频
 * @param time 发送时长
 */
void MCPWM_Tran(uint8_t Fre_Mode);


/**
 * @brief 测试API，勿用
 * 
 */
uint32_t MCPWM_REG_TEST();

/**
 * @brief PWM波捕捉
 * @param gpio_num 捕捉引脚号
 */
void MCPWM_Capture_Config(int gpio_num);




void SOFT_PWM(uint8_t A_GPIO_Num,uint8_t B_GPIO_Num,uint32_t Fre,uint8_t Time);
#ifdef _cplusplus
}
#endif

#endif