/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_FLASH.h
    @Author:    Fitz
    @Description: 
    			
                
    @Others:
        Data:2022/07/06
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_

#ifdef _cplusplus
extern "C"{
#endif
/**
 * @brief        nvs模式读取数据
 *
 * @note         适用于少量数据4000字节以下
 * 
 * @param len    数据长度
 * @param key    主键（地址）
 * @param Date   读取数据存储地址
 */
void nvs_Read_Date(uint8_t len,const char* key,uint8_t* Date);



/**
 * @brief        nvs模式写入数据
 *
 * @note         适用于少量数据4000字节以下
 * 
 * @param len    数据长度
 * @param key    主键（地址）
 * @param Date   写入数据存储地址
 */
void nvs_Write_Date(uint8_t len,const char* key,uint8_t *Date);



/**
 * @brief        nvs模式读取16位数据
 *
 * @note         适用于少量数据4000字节以下
 * 
 * @param key    主键（地址）
 * @param Date   读取数据存储地址
 */
void nvs_Read_i16(const char* key,int16_t* Date);



/**
 * @brief        nvs模式写入16位数据
 *
 * @note         适用于少量数据4000字节以下
 * 
 * @param key    主键（地址）
 * @param Date   写入数据存储地址
 */
void nvs_Write_i16(const char* key,int16_t *Date);
#ifdef _cplusplus
}
#endif

#endif