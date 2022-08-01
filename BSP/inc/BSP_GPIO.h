/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_GPIO.h
    @Author:    Fitz
    @Description: 
    			
                
    @Others:
        Data:2022/03/07
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#ifdef _cplusplus
extern "C"{
#endif

/**
 * @brief GPIO_LEDConfig
 *
 * @note 初始化LED
 *
 * @param gpio_num：GPIO管脚 GPIO_NUM_XX       
 *
 * @note 默认上拉
 *
 */
void GPIO_LEDConfig(gpio_num_t gpio_num);


#ifdef _cplusplus
}
#endif

#endif