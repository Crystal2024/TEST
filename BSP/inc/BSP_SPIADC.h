/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_SPIADC.h
    @Author:    Fitz
    @Description:
                spi接口外置ADC驱动 
    					1、主要功能
    					2、函数接口，包括参数及返回值
    					3、头文件依赖关系
    @Others:
        Data:2022/03/15
        Version:V1.0
        ESP_IDF Version:V4.3.1
 
    @History:

 
 **********************************************************/

#ifndef _BSP_SPIADC_H
#define _BSP_SPIADC_H

#ifdef _cplusplus
extern "C"{
#endif

/**
 * @brief SPI初始化
 *
 * @note  控制SPI驱动ADC，接口未优化，测试用
 * 
 */
void SPIADC_Config();

/**
 * @brief SPI数据发送
 *
 * @note  控制SPI驱动ADC数据，接口未优化，测试用
 * 
 */
void SPIADC_send();
/**
 * @brief SPI数据读取
 *
 * @note  控制SPI驱动ADC数据，接口未优化，测试用
 * 
 */
uint32_t SPIADC_read();

/**
 * @brief SOFT_SPI_Read
 * @note  软件模拟读取
 * @param Frequency 采样频率
 */
void SOFT_SPI_Config();


/**
 * @brief 采样
 * @note  软件SPI采样
 * @param Num 采样点数
 * @param SampData 采样值存储位置
 */
void Sampling(uint16_t Num,uint16_t * SampData);


/**
 * @brief 基准电压采样
 */
// uint16_t ReferenceVoltageSmp(void);




/**
 * @brief 时钟延时
 * @param ts 节拍数 如240M时,ts=240代表1us
 */
 extern __inline void delay_clock(int ts);
#ifdef _cplusplus
}
#endif

#endif