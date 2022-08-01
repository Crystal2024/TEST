/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_WIFI.c
    @Author:    Fitz
    @Description: 
    			BSP驱动层 对idf框架的WiFi驱动进一步封装
                实现了 WiFi的AP与STA配置、开关、TCP的开启、数据的接收
    @Others:
        Data:2021/11/11
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/uart.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_WIFI.h"
#include "BSP_COMM.h"
#include "BSP_GPS.h"
#include "BSP_FLASH.h"
/* USER CODE END Includes */


/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//WIFI_Config

#define Wifi_SSID       "Dolphin-ESP-"
#define Wifi_Password   "12345678"
#define  DefaultLocalPort 2222

static const char *WIFI_Tail_key="WifiTail";
const char *AP_Local_ip="192.168.20.1";
const char *AP_Local_gw="192.168.20.1";


#define EXAMPLE_ESP_WIFI_SSID      "B283822"
#define EXAMPLE_ESP_WIFI_PASS      "12345678"

#define EXAMPLE_ESP_MAXIMUM_RETRY   20
const char *STA_Local_ip="192.168.137.101";
const char *STA_Local_gw="192.168.137.1";


//Event bit 
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
/* USER CODE END PD */


/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//LOG
static const char *TAG = "BSP_WIFI";

wifi_config_t wifi_config ;
esp_event_handler_instance_t ap_any_id;
esp_event_handler_instance_t ap_sta_ip;
esp_event_handler_instance_t sta_any_id;
esp_event_handler_instance_t sta_got_ip;
//wifi
uint32_t WifiTail_val=0; 
//socket
uint8_t LATS_AID=0;
static int server_socket = 0;

int connect_socket = 0,Lastconnect_socket=0;
static struct sockaddr_in server_addr;
static struct sockaddr_in clint_addr;
char addr_str[128];
static unsigned int clintlen = sizeof(clint_addr);

MyWifiPara_t WIFI_1,WIFI_2,WIFI_3;
TaskHandle_t recive_handle1 = NULL,recive_handle2 = NULL,recive_handle3 = NULL;


//Event Group
 EventGroupHandle_t WIFIevent_group;

/* USER CODE END PV */



/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void Recive_Data();
void Socket_close();
esp_err_t Creat_TCP_Server();
esp_err_t TCP_Listen();

/* USER CODE END PFP */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static int s_retry_num = 0;
static void wifi_ap_event_handler(void *arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{ 
  
   uart_disable_rx_intr(EX_UART_NUM);
    static uint16_t i=0;
    //AP模式
    if(event_id == WIFI_EVENT_AP_STACONNECTED)
    {    
          //有设备连接AP
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG,"station "MACSTR"join,AID=%d",MAC2STR(event->mac),event->aid);
        LATS_AID=event->aid;
        switch(LATS_AID)
        {
            case 1:
              memcpy(WIFI_1.mac,event->mac,sizeof(WIFI_1.mac));
            break;
            case 2:
              memcpy(WIFI_2.mac,event->mac,sizeof(WIFI_2.mac));
            break;
            case 3:
              memcpy(WIFI_3.mac,event->mac,sizeof(WIFI_3.mac));
            break;
        }
          uart_enable_rx_intr(EX_UART_NUM); 
        //   xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);
    }
    else if(event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {   //有设备与AP断开
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG,"station "MACSTR"leave,AID=%d",MAC2STR(event->mac),event->aid);
        xEventGroupClearBitsFromISR(WIFIevent_group,WIFI_Creatbit|TCP_Listenbit);
        
        switch(event->aid)
            {
                case 1:
                 if(recive_handle1!=NULL) //已建立连接并创建socket
                 {
                   close(WIFI_1.connect_socket);
             
                 }  
                break;
                case 2:
                if(recive_handle2!=NULL) //已建立连接并创建socket
                 {
                   close(WIFI_2.connect_socket);
              
                 }      
                break;
                case 3:
                 if(recive_handle3!=NULL) //已建立连接并创建socket
                 {
                   close(WIFI_3.connect_socket);
                 }  
                break;
            }
        // if(recive_handle!=NULL)
        // {
        //   vTaskDelete(recive_handle);
        //   recive_handle=NULL;
        // } 
        uart_enable_rx_intr(EX_UART_NUM);     
    }

    else if (event_id == IP_EVENT_AP_STAIPASSIGNED ) 
       {
            ip_event_ap_staipassigned_t* event = (ip_event_ap_staipassigned_t*) event_data;
            
            switch(LATS_AID)
            {
                case 1:
                    memcpy(&WIFI_1.IP,&event->ip,sizeof(WIFI_1.IP));      
                    //  ESP_LOGI(TAG, "LATS_AID:%d,IP:%d,mac:"MACSTR"" ,LATS_AID,WIFI_1.IP,MAC2STR(WIFI_1.mac));
                break;
                case 2:
                    memcpy(&WIFI_2.IP,&event->ip,sizeof(WIFI_2.IP));
                break;
                case 3:
                    memcpy(&WIFI_3.IP,&event->ip,sizeof(WIFI_3.IP));
                break;
            }
            // xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);
            uart_enable_rx_intr(EX_UART_NUM); 
       }

}
static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
     {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            WIFI_1.IP=0;
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else 
        ESP_LOGI(TAG,"connect to the AP fail");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
    }
}
static void Wifi_Event_Group()
{
    EventBits_t WIFIbits;
 
    while (1)
    {
        WIFIbits=xEventGroupWaitBits(WIFIevent_group,
                                      WIFI_Creatbit|TCP_Listenbit,
                                      pdTRUE,
                                      pdFALSE,
                                      portMAX_DELAY);                      
        switch (WIFIbits)
        {
             case WIFI_Creatbit:
               Creat_TCP_Server(); 
               break;
             case TCP_Listenbit:   
                TCP_Listen();

                break;
             default:
                break;       
        }   
   }
    vTaskDelete(NULL);
}

void Wifi_APConfig()
{ 
    /*初始化wifi的AP模式*/
    ESP_ERROR_CHECK(esp_netif_init());//netif初始化
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *eth_netif_ap=esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_ap_event_handler,
                                                        NULL,
                                                        &ap_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_AP_STAIPASSIGNED,
                                                        &wifi_ap_event_handler,
                                                        NULL,
                                                        &ap_sta_ip));
   

    esp_netif_dhcps_stop(eth_netif_ap);
    esp_netif_ip_info_t info_t;
    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    info_t.ip.addr = esp_ip4addr_aton(AP_Local_ip);
    info_t.gw.addr = esp_ip4addr_aton(AP_Local_gw);
    info_t.netmask.addr = esp_ip4addr_aton((const char *)"255.255.255.0");
    esp_netif_set_ip_info(eth_netif_ap, &info_t);
    esp_netif_dhcps_start(eth_netif_ap);


    /*AP参数设置*/                                                   
    wifi_config.ap=(wifi_ap_config_t)
    {
        .ssid = Wifi_SSID, //WiFi SSID
        .ssid_len = strlen(Wifi_SSID)+6,
        .password = Wifi_Password,//WiFi Password
        .max_connection = 3,
        .authmode = WIFI_AUTH_WPA2_PSK
    };
      nvs_Read_Date(sizeof(WifiTail_val),WIFI_Tail_key,(uint8_t*)&WifiTail_val);
      if(WifiTail_val==0)
      {
            WifiTail_val=esp_random()/256;               
            nvs_Write_Date(sizeof(WifiTail_val),WIFI_Tail_key,(uint8_t*)&WifiTail_val);
      }
      utoa(WifiTail_val,(char *)(&wifi_config.ap.ssid[strlen(Wifi_SSID)]),16);

    /*密码长短判断，若无则设为无密码*/
    if(strlen(Wifi_Password) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    /*设置AP模式*/
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&wifi_config));
    
    xTaskCreate(&Wifi_Event_Group,"WIFI_Event",2048,NULL,1,NULL);
    WIFIevent_group = xEventGroupCreate();
    Wifi_Start();
    Creat_TCP_Server();

}

void Wifi_STAConfig()
{
    /*初始化wifi的STA模式*/
    ESP_ERROR_CHECK(esp_netif_init());//netif初始化
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *eth_netif_sta =esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_sta_event_handler,
                                                        NULL,
                                                        &sta_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_sta_event_handler,
                                                        NULL,
                                                        &sta_got_ip));
    esp_netif_dhcpc_stop(eth_netif_sta);
    esp_netif_ip_info_t info_t;
    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    info_t.ip.addr = esp_ip4addr_aton(STA_Local_ip);
    info_t.netmask.addr = esp_ip4addr_aton((const char *)"255.255.255.0");
    info_t.gw.addr = esp_ip4addr_aton(STA_Local_gw);
    esp_netif_set_ip_info(eth_netif_sta, &info_t);
    
    wifi_config.sta = (wifi_sta_config_t )
                     {
                        // .ssid =EXAMPLE_ESP_WIFI_SSID,
                        // .password = EXAMPLE_ESP_WIFI_PASS,
                        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
                        .pmf_cfg = 
                        {
                            .capable = true,
                            .required = false
                        },
                    };
                               
     memcpy(wifi_config.sta.ssid, sta_ssid,sizeof(sta_ssid));
     memcpy(wifi_config.sta.password, sta_password,sizeof(sta_password));  
     ESP_LOGI(TAG,"wifi_config.sta.ssid=%s,wifi_config.sta.password=%s",wifi_config.sta.ssid,wifi_config.sta.password);                 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    // xTaskCreate(&Wifi_Event_Group,"WIFI_Event",2048,NULL,1,NULL);
    // WIFIevent_group = xEventGroupCreate();
    Wifi_Start();
    // Creat_TCP_Server();
    // /* The event will not be processed after unregister */

  


}

void Wifi_Start()
{
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG,"wifi Start ok");  
}

void Wifi_Stop()
{
    ESP_ERROR_CHECK(esp_wifi_stop());
    Socket_close();
}



esp_err_t Creat_TCP_Server()
{
  
        ESP_LOGI(TAG,"Creat_TCP_Server Start");
        server_socket = socket(AF_INET,SOCK_STREAM,0);

        if(server_socket < 0)
        {
            ESP_LOGI(TAG,"Creat_TCP_Server socket Fail");
            return ESP_FAIL;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(DefaultLocalPort);//端口号

        /*绑定信息*/
        if(bind(server_socket,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        { 
            close(server_socket);
            ESP_LOGI(TAG,"Creat_TCP_Server  bind Fail");
            return ESP_FAIL;
        }
    xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);

    return ESP_OK;
}

esp_err_t TCP_Listen()
{
    /*开启监听*/
    while(1)
    {
    ESP_LOGI(TAG,"TCP_Listen Start");  
     uart_enable_rx_intr(EX_UART_NUM); 
    if(listen(server_socket,5) < 0)
    {
        close(server_socket);
        ESP_LOGI(TAG,"TCP_Listen socke Fail");
        return ESP_FAIL;
    }

    /*建立连接*/
    Lastconnect_socket = accept(server_socket,(struct sockaddr *)&clint_addr,&clintlen);
    if(Lastconnect_socket <0)
     {
        close(server_socket);
        ESP_LOGI(TAG,"TCP_Listen accept Fail");
        return ESP_FAIL;
    } 
    // ESP_LOGI(TAG,"clint ip=%d",(uint32_t) (clint_addr.sin_addr.s_addr&0xFFFFFFFF));
    //AP模式
    if(g_LastWifiMode==AP_MODE)
    {
        if(clint_addr.sin_addr.s_addr==WIFI_1.IP)
        {  
        // if(recive_handle1!=NULL) //已建立连接并创建socket
        // {
        //   ESP_LOGI(TAG,"WIFI_1.connect_socket:%d ",WIFI_1.connect_socket);
        //   close(WIFI_1.connect_socket);
        //   vTaskDelay(100/portTICK_RATE_MS); 
        // }  
        WIFI_1.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"WIFI_1.connect_socket:%d ",WIFI_1.connect_socket);
        xTaskCreate(Recive_Data1,"Recv_Data1",2048*5,NULL,1,&recive_handle1);
        }
        else if(clint_addr.sin_addr.s_addr==WIFI_2.IP)
        {
        // if(recive_handle2!=NULL) //已建立连接并创建socket
        // {
        //   ESP_LOGI(TAG,"WIFI_2.connect_socket:%d ",WIFI_2.connect_socket);
        //   close(WIFI_2.connect_socket);
        //   vTaskDelay(100/portTICK_RATE_MS); 
        // } 
        WIFI_2.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"WIFI_2.connect_socket:%d ",WIFI_2.connect_socket);
        xTaskCreate(Recive_Data2,"Recv_Data2",2048*5,NULL,1,&recive_handle2);

        }
        else if(clint_addr.sin_addr.s_addr==WIFI_3.IP)
        {
        // if(recive_handle3!=NULL) //已建立连接并创建socket
        // {
        //   ESP_LOGI(TAG,"WIFI_3.connect_socket:%d ",WIFI_3.connect_socket);
        //   close(WIFI_3.connect_socket);
        //   vTaskDelay(100/portTICK_RATE_MS);  
        // } 
        WIFI_3.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"WIFI_3.connect_socket:%d ",WIFI_3.connect_socket);
        xTaskCreate(Recive_Data3,"Recv_Data3",2048*5,NULL,1,&recive_handle3);
        }
    }   
    //STA模式
    if(g_LastWifiMode==STA_MODE)
   {
    //  

      if(recive_handle1==NULL)
      {   
        WIFI_1.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"STAWIFI_1.connect_socket:%d ",WIFI_1.connect_socket);
        xTaskCreate(Recive_Data1,"Recv_Data1",2048*5,NULL,1,&recive_handle1);
      }
      else if(recive_handle2==NULL)
      {   
        WIFI_2.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"STAWIFI_2.connect_socket:%d ",WIFI_2.connect_socket);
        xTaskCreate(Recive_Data2,"Recv_Data1",2048*5,NULL,1,&recive_handle2);
      }
      else if(recive_handle3==NULL)
      {   
        WIFI_3.connect_socket=Lastconnect_socket;
        ESP_LOGI(TAG,"STAWIFI_3.connect_socket:%d ",WIFI_3.connect_socket);
        xTaskCreate(Recive_Data3,"Recv_Data3",2048*5,NULL,3,&recive_handle3);
      }
      else
        close(Lastconnect_socket);

   }

    
    // return ESP_OK;
    }
     return ESP_OK;
}

void Wifi_send(void *send_data,uint16_t data_len)
{
    send(connect_socket,send_data,data_len,-3);
}




void Socket_close()
{
    /*关闭socket连接*/
    close(WIFI_1.connect_socket);
    close(WIFI_2.connect_socket);
    close(WIFI_3.connect_socket);
    // close(server_socket);
    ESP_LOGI(TAG,"Socket Close");
}
/* USER CODE END 0 */

/*----------------------------------END---------------------------------------*/