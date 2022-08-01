/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_ADC.h
    @Author:    Fitz
    @Description: 
    			BSP驱动层 对idf框架的ADC驱动进一步封装

                ADC使用DMA
                
    @Others:
        Data:2021/11/19
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_
#include "esp_adc_cal.h"
#ifdef _cplusplus
extern "C"{
#endif
const adc_channel_t AD_Temp;        //GPIO32 if ADC4
const adc_channel_t AD_SAMP_VREF;   //GPIO34 if ADC6
const adc_channel_t AD_Battery;     //GPIO35 if ADC7
void ADC_Init(void);
uint32_t Get_ADC_Date(uint8_t num,adc_channel_t ChannelNum);

#ifdef _cplusplus
}
#endif

#endif