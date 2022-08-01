/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_WIFI.h
    @Author:    Fitz
    @Description: 
    			BSP驱动层 对idf框架的WiFi驱动进一步封装
                实现了 WiFi的AP与STA配置、开关、TCP的开启、数据的接收
                
                首先使用 wifi_APConfig() 函数进行配置，若成功将创建 WIFI_Event_Group() 任务
                WIFI_Event_Group() 任务 实现了一个类状态机的效果，响应不同的事件标志位，如wifi配置成功，重启TCP等
                配置完毕 可以使用Wifi_Start() 和 Wifi_Stop() 函数 开启和关闭WIFI。
                AP与sta模式切换 
                
                当有设备连上指定的TCP端口后，将开启Recive_Data()任务，接收数据。

                注意：
                    1、WiFi的SSID、Password和TCP端口号 均在源文件的宏定义当中修改
                    2、当使用WiFi后 会产生两个任务，一个时间处理。
            
    @Others:
        Data:2021/11/11
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:
        1.Date:2022/02/22
           Author:Fitz
           Modification: 修改TCP重启相关逻辑，暂未完成测试，指令解析暂由接收任务处理
 **********************************************************/
#ifndef _BSP_WIFI_H_
#define _BSP_WIFI_H_

#include "esp_wifi.h"

#ifdef _cplusplus
extern "C"{
#endif

#define WIFI_Creatbit   (0X01 << 0)
#define TCP_Listenbit   (0x01 << 1)



extern int connect_socket;

extern TaskHandle_t recive_handle1,recive_handle2,recive_handle3;
extern EventGroupHandle_t WIFIevent_group;
typedef struct MyWifiPara
{
	uint32_t IP;;      //IP
	uint8_t  mac[6];;         //MAC
	uint8_t  connect_socket;         //socket
}MyWifiPara_t;
extern MyWifiPara_t WIFI_1,WIFI_2,WIFI_3;
extern wifi_config_t wifi_config ;
extern esp_event_handler_instance_t ap_any_id;
extern esp_event_handler_instance_t ap_sta_ip;
extern esp_event_handler_instance_t sta_any_id;
extern esp_event_handler_instance_t sta_got_ip;
/**
 * @brief Wifi_APConfig
 *
 * @note AP模式初始化，包括AP模式设置、WIFI的SSID及Password配置
 *       注册WIFI的事件处理任务
 *
 */
extern void Wifi_APConfig();
/**
 * @brief Wifi_STAConfig
 *
 * @note AP模式初始化，包括AP模式设置、WIFI的SSID及Password配置
 *       注册WIFI的事件处理任务
 *
 */
extern void Wifi_STAConfig();

/**
 * @brief Wifi_Start
 *
 * @note  开启WIFI
 *
 */
extern void Wifi_Start();

/**
 * @brief Wifi_Stop
 *
 * @note  关闭WIFI
 *
 */
extern void Wifi_Stop();

/**
 * @brief Wifi_send
 *
 * @note  WIFI数据发送
 * 
 * @param send_data 待发送数据指针
 * @param data_len  待发送数据长度 
 *
 */
extern void Wifi_send(void *send_data,uint16_t data_len);

/**
 * @brief Wifi_send
 * @note  关闭Socket
 */
extern void Socket_close();


#ifdef _cplusplus
}
#endif

#endif