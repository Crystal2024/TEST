/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_GPIO.h
    @Author:    KK
    @Description: 
    			GPS模块解析
                
    @Others:
        Data:2022/03/07
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
#ifndef _BSP_GPS_H_
#define _BSP_GPS_H_

#ifdef _cplusplus
extern "C"{
#endif
#define EX_UART_NUM         UART_NUM_1 
uint8_t  dtmp[200];
/**
 * @brief GPS串口初始化 
 *
 */
void Uart1_Init(void);
#ifdef _cplusplus
}
#endif

#endif