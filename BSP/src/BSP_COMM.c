/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_COMM.h
    @Author:    kk
    @Description: 
    			
                
    @Others:
        Data:2022/04/21
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/i2s.h"
#include "driver/dac_common.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_COMM.h"
#include "BSP_WIFI.h"
#include "BSP_SPIADC.h"
#include "BSP_MCPWM.h"
#include "BSP_GPS.h"
#include "BSP_TVG.h"
#include "BSP_ADC.h"
#include "BSP_FLASH.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Time10msTos          100
#define Time10msTous         10000

/* USER CODE END PD */

/* Private typedef ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const char *TAG = "comm";
static const char *GainCommDate_key="GainConfig";
static const char *TargetCommDate_key="TargetConfig";
uint8_t sta_ssid[32];   
uint8_t sta_password[64];  


ACK_DATA_t g_ack_data=
  {
    .head=ACK_DATA_HEAD,
    .len=ACK_DATA_LEN,
    .term=ACK_DATA_TERM
  };
FIRST_FISH_DATA_t g_fish_dataFirst=
{
    .head=0xa8e6,
    .cmd=FISH_DATA_CMD,
    .comm_num=1,//开机后累加不清零
    .temp=-100,
    .battery=-1,
    .range_depth=6,
    .coord_depth=0,
    .status=1,
    .lat=91*60*100000,
    .lon=181*60*100000,
    .timestamp=0,
    .V_reference=0
};
OTHER_FISH_DATA_t g_fish_dataOther=
{
    .head=0xa8e6,  
    .cmd=FISH_DATA_CMD,
    .comm_num=1,//开机后累加不清零
    .V_reference=0
};
GAIN_Off_t          g_gain_offset=
{
    .Fre_L=0,
    .Fre_H=0
};
TARGET_Base_t       g_target_base=
{
    .Fre_L=0,
    .Fre_H=0
};


uint8_t  g_LastRange=10,g_LastLink=0,g_LastCMD_OFF=0,g_LastFreMode=0,g_LastSumPacks=0,g_intervalPacks=0,g_Last_GAIN=0,g_LastGainBase=Default_TVG,g_LastWifiMode=AP_MODE;
static   uint8_t  LastGainOff=0;
int8_t LastBatteryPer=0;
uint16_t g_LastSamplNum=0,g_LastPwm_Time=100,g_LastTVGnum=0;;
uint32_t g_LastSampRate=45000;
// int16_t g_LastGAIN_Off=0;

uint32_t g_LastFre=FRE_PWML;
int64_t CheckTime=0;  
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
ACK_DATA_t* ACK_Pack(uint8_t cmd,uint16_t status);
void SetTimestamp(int64_t Num);
void GetTimestamp(int64_t *time);
void update_TimesTamp(void);
void BatteryToPer(void);
/* USER CODE END 0 */
void Socket_Force_Connect(int socketNum)
{ 
	//  xEventGroupClearBits(PACKEvent_Group,packbackbit|PwmTranbit|Samplingbit|unpackbit);
  ESP_LOGI(TAG,"g_LastLink=%d",g_LastLink);
	if(g_LastLink==0&&g_switch_data.cmd_status==0x01)//未连接
	{  
	   connect_socket=socketNum;
     ESP_LOGI(TAG,"connect_socket=%d",connect_socket);
	  //  xEventGroupSetBits(PACKEvent_Group,unpackbit);
	}
   
	else if (g_LastLink!=0&&g_switch_data.cmd_status==0x01&&connect_socket!=socketNum)//已有连接，他设备申请连接
	{
    ESP_LOGI(TAG,"再申请,socketNum=%d",socketNum);

    send(socketNum,ACK_Pack(Version_ASK,LAST_Version),sizeof(g_ack_data),0);//软件版本号
	  send(socketNum,ACK_Pack(LINK_CHANGE,0x03),sizeof(g_ack_data),0);//设备占用
    
	}
	else if (g_LastLink!=0&&g_switch_data.cmd_status==0x04&&socketNum!=connect_socket)//强制断开,通讯状态设备不允许强制断开
	{
     connect_socket=socketNum; //0404
      // if(recive_handle1!=NULL)//0402
      if(recive_handle1!=NULL&&socketNum!=WIFI_1.connect_socket) //已建立连接并创建socket//0404且非申请强连设备
      {
        send(WIFI_1.connect_socket,ACK_Pack(g_switch_data.cmd,0X02),sizeof(g_ack_data),0);//先返回ACK包  
        
        if(g_LastWifiMode==STA_MODE)            //避免sta模式下断wifi占资源
        {
          vTaskDelay(10/portTICK_RATE_MS); 
          close(WIFI_1.connect_socket);
        }
     }

      // if(recive_handle2!=NULL)//0402
      if(recive_handle2!=NULL&&socketNum!=WIFI_2.connect_socket) //已建立连接并创建socket//0404
      {
        send(WIFI_2.connect_socket,ACK_Pack(g_switch_data.cmd,0X02),sizeof(g_ack_data),0);//先返回ACK包
        if(g_LastWifiMode==STA_MODE)
        {
          vTaskDelay(10/portTICK_RATE_MS);
          close(WIFI_2.connect_socket);
        }
      }
      // if(recive_handle3!=NULL)//0402
      if(recive_handle3!=NULL&&socketNum!=WIFI_3.connect_socket) //已建立连接并创建socket//0404
      {
        send(WIFI_3.connect_socket,ACK_Pack(g_switch_data.cmd,0X02),sizeof(g_ack_data),0);//先返回ACK包 
        if(g_LastWifiMode==STA_MODE)
        {
          vTaskDelay(10/portTICK_RATE_MS);
          close(WIFI_3.connect_socket);
        }
      }
      
      g_switch_data.cmd_status=0x01;
	}

}

void  Recive_Data1()
{
    int len = 0;
    //    ESP_LOGI(TAG,"Recv Start");
    while (1)
    { 
        //清零
        /*接受数据 并获取数据长度*/
        len = recv(WIFI_1.connect_socket,&g_switch_data,sizeof(g_switch_data),0);

        if(len >0)
        {            
            if((g_switch_data.head==SWITCH_DATA_HEAD
            &&g_switch_data.term==SWITCH_DATA_TERM
            // &&g_switch_data.len==SWITCH_DATA_LEN
            ))
            { 
              if(g_switch_data.cmd==LINK_CHANGE)
              {
                Socket_Force_Connect(WIFI_1.connect_socket); 
              }
              if(connect_socket==WIFI_1.connect_socket)
              {
                xEventGroupSetBits(PACKEvent_Group,unpackbit);
              }        
            }
         
        }
        else
        {   /*链接断开 未接受到数据 跳出循环进入重连状态*/
            close(WIFI_1.connect_socket);
            // vTaskDelay(100/portTICK_RATE_MS);  
            recive_handle1=NULL;  
            break;           
        }
    } 
    ESP_LOGI(TAG,"R1C");
    if(WIFI_1.connect_socket==(uint8_t)connect_socket)//通讯设备断连
    {

      g_LastLink=0;
      g_LastCMD_OFF=0;
      connect_socket=0;
    }
    WIFI_1.connect_socket=0;
    xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);    
    vTaskDelete(NULL);
}

void  Recive_Data2()
{
    int len = 0;
    //    ESP_LOGI(TAG,"Recv Start");
    while (1)
    { 
        //清零
        /*接受数据 并获取数据长度*/
        len = recv(WIFI_2.connect_socket,&g_switch_data,sizeof(g_switch_data),0);
        if(len >0)
        {               
            if((g_switch_data.head==SWITCH_DATA_HEAD
            &&g_switch_data.term==SWITCH_DATA_TERM
            // &&g_switch_data.len==SWITCH_DATA_LEN
            ))
            { 
              if(g_switch_data.cmd==LINK_CHANGE)
              {
                Socket_Force_Connect(WIFI_2.connect_socket); 
              }
              if(connect_socket==(uint8_t)WIFI_2.connect_socket)
              {
                xEventGroupSetBits(PACKEvent_Group,unpackbit);
              } 
            }
         
        }
        else
        {   /*链接断开 未接受到数据 跳出循环进入重连状态*/
             close(WIFI_2.connect_socket);
            //  vTaskDelay(100/portTICK_RATE_MS);  
            recive_handle2=NULL;  
            
            break;           
        }
    } 
    ESP_LOGI(TAG,"R2C");
    if(WIFI_2.connect_socket==(uint8_t)connect_socket)//通讯设备断连
    {
      ESP_LOGI(TAG,"W2C");
      g_LastLink=0;
      g_LastCMD_OFF=0;
      connect_socket=0;
    }
    WIFI_2.connect_socket=0;
    xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);    
    vTaskDelete(NULL);
}
void  Recive_Data3()
{
    int len = 0;
    //    ESP_LOGI(TAG,"Recv Start");
    while (1)
    { 
        //清零
        /*接受数据 并获取数据长度*/
        len = recv(WIFI_3.connect_socket,&g_switch_data,sizeof(g_switch_data),0);

        if(len >0)
        {               
            if((g_switch_data.head==SWITCH_DATA_HEAD
            &&g_switch_data.term==SWITCH_DATA_TERM
            // &&g_switch_data.len==SWITCH_DATA_LEN
            ))
            { 
              if(g_switch_data.cmd==LINK_CHANGE)
              {
                Socket_Force_Connect(WIFI_3.connect_socket); 
              }
              if(connect_socket==WIFI_3.connect_socket)
              {
                xEventGroupSetBits(PACKEvent_Group,unpackbit);
              } 
            }        
        }
        else
        {   /*链接断开 未接受到数据 跳出循环进入重连状态*/           
            
            close(WIFI_3.connect_socket);
            // vTaskDelay(100/portTICK_RATE_MS);  
            recive_handle3=NULL;
            
            break;           
        }
    } 
    ESP_LOGI(TAG,"R3C");
    if(WIFI_3.connect_socket==connect_socket)//通讯设备断连
    {
      ESP_LOGI(TAG,"W3C");
      g_LastLink=0;
      g_LastCMD_OFF=0;
      connect_socket=0;
    }
    WIFI_3.connect_socket=0;  
    xEventGroupSetBits(WIFIevent_group,TCP_Listenbit);    
    vTaskDelete(NULL);
}




//解包
void COMM_Switcm(void)
{
   uint8_t ssid_len=0,pass_len=0;
    switch (g_switch_data.cmd)
    {
      case CMD_OFF:
       if(g_switch_data.cmd_status==1)    //打开
        {                  
            update_TimesTamp();
            //  ReferenceVoltageSmp();
             uart_enable_rx_intr(EX_UART_NUM);//接收GPS定位信号
            g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
            BatteryToPer();  
            ADtoTEMP();        
        }
        g_LastCMD_OFF=g_switch_data.cmd_status;
 
      break;
      case RANGE_CHANGE:
          update_TimesTamp();
          g_LastRange=g_switch_data.cmd_status; 
          
          // ReferenceVoltageSmp(); 
          g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
          
      break; 
      case FER_CHANGE:
      {
           update_TimesTamp();
           g_LastFreMode=g_switch_data.cmd_status;
           if(g_LastFreMode==0)
           {
              g_LastFre=FRE_PWML;
              LastGainOff=g_gain_offset.Fre_L;
              g_fish_dataFirst.TargetBase=g_target_base.Fre_L;
           }
           else 
           {
              g_LastFre=FRE_PWMH;
              LastGainOff=g_gain_offset.Fre_H;
              g_fish_dataFirst.TargetBase=g_target_base.Fre_H;
           }

          //  ReferenceVoltageSmp();
              g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
        

        //    MCPWM_Stop();
        //    MCPWM_Freq(g_LastFre);
      break; 
      }
      case LINK_CHANGE:
           g_LastLink=g_switch_data.cmd_status;
           if(g_LastLink!=0)
           {
              CommPramInit();
           }
           else
           {
              g_LastLink=0;
              connect_socket=0;
              // g_LastCMD_OFF=0;
           }
            // uart_enable_rx_intr(EX_UART_NUM);//接收GPS定位信号
            // ReferenceVoltageSmp();
            // g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
            
      break; 
      case WIFI_CHANGE:
           if(g_LastWifiMode!=g_switch_data.cmd_status)//状态改变
           {
              g_LastWifiMode=g_switch_data.cmd_status;
              send(connect_socket,ACK_Pack(g_switch_data.cmd,g_switch_data.cmd_status),sizeof(g_ack_data),0);//先返回ACK包 
              if(g_LastWifiMode==STA_MODE)
              {
                    memset( sta_ssid,0x00,sizeof(sta_ssid));
                    memset( sta_password,0x00,sizeof(sta_password));
                    ESP_LOGI(TAG,"WIFI_info=%s",g_switch_data.content);
                    for(uint8_t i=0;i<sizeof(g_switch_data.content);i++)
                    {
                      if(g_switch_data.content[i]==',')
                      {
                        if(ssid_len==0)
                        ssid_len=i;
                        else 
                        {
                          pass_len=i-ssid_len-1;
                          break;
                        }
                      }
                    }
                      memcpy( sta_ssid,g_switch_data.content,ssid_len);
                      memcpy( sta_password,&g_switch_data.content[ssid_len+1],pass_len);
     
                      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, ap_sta_ip));
                      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, ap_any_id));
                      
                      esp_wifi_stop();
                      esp_wifi_deinit();
                      Wifi_STAConfig();
              }
              else 
              {
                    esp_restart();
              }
           }
      break; 
      case Target_Base:
           if(g_switch_data.cmd_status==1)//高频
            {
             g_target_base.Fre_H=(g_switch_data.content[0]<<8)+g_switch_data.content[1];
                       
             g_fish_dataFirst.TargetBase=g_target_base.Fre_H;          
            }
           else 
           {
             g_target_base.Fre_L=(g_switch_data.content[0]<<8)+g_switch_data.content[1];
             g_fish_dataFirst.TargetBase=g_target_base.Fre_L;
           }

           nvs_Write_Date(sizeof(g_target_base),TargetCommDate_key,(uint8_t*)&g_target_base);

      break; 

      
      case GAIN_CHANGE:           
            g_LastGainBase=g_switch_data.content[0];
            g_Last_GAIN=g_LastGainBase+LastGainOff;
       
            ESP_LOGI(TAG,"g_Last_GAIN=%d",g_Last_GAIN);
            if(g_LastGainBase!=0)
              {   
                i2s_set_dac_mode( I2S_DAC_CHANNEL_DISABLE);
                SET_GainNum(g_Last_GAIN);
              }
            else
            {
                dac_output_disable(DAC_CHANNEL_1);
                I2S_DAC_init();
            }
            // ReferenceVoltageSmp(); 
            g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
      break;
      case AutoRANGE_CHANGE:
           g_LastRange=g_switch_data.content[0];         
          //  g_LastGainBase=g_switch_data.content[1];  
           g_LastPwm_Time=(g_switch_data.content[1]<<8)+g_switch_data.content[2];        
           g_LastSampRate=g_switch_data.content[3]*1000;
           g_Last_GAIN=g_LastGainBase+LastGainOff;
           if(g_LastGainBase!=0)
            {   
                i2s_set_dac_mode( I2S_DAC_CHANNEL_DISABLE);
                SET_GainNum(g_Last_GAIN);
            }
            else
            {
                dac_output_disable(DAC_CHANNEL_1);
                I2S_DAC_init();
            }
          //  ESP_LOGI(TAG,"g_LastRange=%d",g_LastRange);
          //  ESP_LOGI(TAG,"g_Last_GAIN=%d",g_Last_GAIN);
          //  ESP_LOGI(TAG,"g_LastSampRate=%d",g_LastSampRate);
          //  ESP_LOGI(TAG,"g_LastPwm_Time=%d",g_LastPwm_Time);

      break; 
      case SetGAIN_Off_CHANGE:
          if(g_LastFreMode==1)//高频
            {
             g_gain_offset.Fre_H=(g_switch_data.content[0]<<8)+g_switch_data.content[1];                       
             LastGainOff=g_gain_offset.Fre_H;
             g_Last_GAIN=g_LastGainBase+LastGainOff;
            }
           else 
           {
             g_gain_offset.Fre_L=(g_switch_data.content[0]<<8)+g_switch_data.content[1];
             LastGainOff=g_gain_offset.Fre_L;
             g_Last_GAIN=g_LastGainBase+LastGainOff;
           }
            if(g_LastGainBase!=0)
              {   
                i2s_set_dac_mode( I2S_DAC_CHANNEL_DISABLE);
                SET_GainNum(g_Last_GAIN);
              }
            else
            {
                dac_output_disable(DAC_CHANNEL_1);
                I2S_DAC_init();
            }
           nvs_Write_Date(sizeof(g_gain_offset),GainCommDate_key,(uint8_t*)&g_gain_offset);
          //  ESP_LOGI(TAG," g_gain_offset.Fre_L=%d", g_gain_offset.Fre_L);
          //  ESP_LOGI(TAG," g_gain_offset.Fre_H=%d", g_gain_offset.Fre_H);
      break; 
      case PWM_Time_CHANGE:
           g_LastPwm_Time=(g_switch_data.content[0]<<8)+g_switch_data.content[1];
          //  ESP_LOGI(TAG,"g_LastPwm_Time=%d",g_LastPwm_Time);
      break;
      } 
      g_fish_dataOther.V_reference=g_fish_dataFirst.V_reference;
      g_LastTVGnum=g_LastRange*20000/750+5;
      // ESP_LOGI(TAG,"g_LastTVGnum=%d",g_LastTVGnum);     
      g_LastSamplNum=g_LastSampRate*g_LastRange/750;//采样个数   
      // ESP_LOGI(TAG,"g_LastSamplNum=%d",g_LastSamplNum);
    // g_LastSumPacks=ceil(g_LastSamplNum/(sizeof(g_fish_dataFirst.data)/sizeof(g_fish_dataFirst.data[0])));//精度丢失
    g_LastSumPacks=g_LastSamplNum/(sizeof(g_fish_dataFirst.data)/sizeof(g_fish_dataFirst.data[0]));
    if(g_LastSamplNum%(sizeof(g_fish_dataFirst.data)/sizeof(g_fish_dataFirst.data[0]))!=0)
       g_LastSumPacks++;
    if(g_LastRange<=36)
       g_intervalPacks=1000/10;
    else if(g_LastRange==60)
       g_intervalPacks=1000/6;
    else if(g_LastRange==120)
       g_intervalPacks=1000/4;
}
//ACK包
ACK_DATA_t* ACK_Pack(uint8_t cmd,uint16_t status)
{
    g_ack_data.cmd=cmd;
    g_ack_data.link_status=status;
    return &g_ack_data;
}

//声呐数据包
FIRST_FISH_DATA_t* FirstFish_DataPack(uint8_t sum_pack,uint8_t num_pack,uint16_t num_data,uint16_t* FishData,uint8_t freMode )
{
  g_fish_dataFirst.len=num_data*2+FirstFISH_DATA_LEN;
  g_fish_dataFirst.sum_packets=sum_pack;
  g_fish_dataFirst.num_packets=num_pack;
 
  memcpy(g_fish_dataFirst.data,FishData,sizeof(g_fish_dataFirst.data));
//   send(connect_socket,g_fish_dataFirst.data,sizeof(g_fish_dataFirst.data),0);
  g_fish_dataFirst.fre_mode=freMode;
  g_fish_dataFirst.range_depth=g_LastRange;
  
  GetTimestamp(&g_fish_dataFirst.timestamp);
  // ESP_LOGI(TAG,"纬度=%d",g_fish_dataFirst.lat);
  // ESP_LOGI(TAG,"经度=%d",g_fish_dataFirst.lon);
  return &g_fish_dataFirst;
}
OTHER_FISH_DATA_t* OtherFish_DataPack(uint8_t sum_pack,uint8_t num_pack,uint16_t num_data,uint16_t* FishData)
{
  g_fish_dataOther.len=num_data*2+OtherFISH_DATA_LEN;
  g_fish_dataOther.sum_packets=sum_pack;
  g_fish_dataOther.num_packets=num_pack;

  memcpy(g_fish_dataOther.data,FishData,sizeof(g_fish_dataOther.data));
  return &g_fish_dataOther;
}

void SetTimestamp(int64_t Num)
{
    struct timeval tv_now;
    tv_now.tv_sec=Num/Time10msTos;
    tv_now.tv_usec=Num%Time10msTos*Time10msTous;

    settimeofday(&tv_now, NULL);

}
void GetTimestamp(int64_t *time)
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    *time=((int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec)/Time10msTous;

}
void CommPramInit(void)
{
        nvs_Read_Date(sizeof(g_gain_offset),GainCommDate_key,(uint8_t*)&g_gain_offset);
        nvs_Read_Date(sizeof(g_target_base),TargetCommDate_key,(uint8_t*)&g_target_base);
        if(g_target_base.Fre_H==0||g_target_base.Fre_L==0)
        {
          if(g_target_base.Fre_H==0)
          {
            g_target_base.Fre_H=default_target_baseH;          
          }
          else if(g_target_base.Fre_L==0)
          {
            g_target_base.Fre_L=default_target_baseL;  
          }
          nvs_Write_Date(sizeof(g_target_base),TargetCommDate_key,(uint8_t*)&g_target_base);
        }
    
        g_fish_dataFirst.TargetBase=g_target_base.Fre_L;
        g_fish_dataFirst.comm_num=0;
        ESP_LOGI(TAG,"g_gain_offset.Fre_L=%d",g_gain_offset.Fre_L);
        ESP_LOGI(TAG,"g_gain_offset.Fre_H=%d",g_gain_offset.Fre_H);
        ESP_LOGI(TAG,"g_target_base.Fre_L=%d",g_target_base.Fre_L);
        ESP_LOGI(TAG,"g_target_base.Fre_H=%d",g_target_base.Fre_H);   
     
        g_LastFreMode=0;  
        g_LastFre=FRE_PWML; //低频
        g_LastRange=6;      //6米
        // g_LastLink=0;      //未连接
        g_LastCMD_OFF=0;   //未打开
        LastGainOff=g_gain_offset.Fre_L;
        g_Last_GAIN=g_LastGainBase+LastGainOff;     //增益倍数
        g_LastSampRate=100000;
        g_LastPwm_Time=100;
}




void update_TimesTamp(void)
{
    for(uint8_t i=0;i<8;i++)
    {
      CheckTime=(CheckTime<<8)+g_switch_data.content[i];
    }
    SetTimestamp(CheckTime);
} 


void BatteryToPer(void)
{
  g_fish_dataFirst.battery=100-(2050-Get_ADC_Date(10,AD_Battery))*100/650;
  if(LastBatteryPer!=0&&LastBatteryPer<g_fish_dataFirst.battery)
  {
    g_fish_dataFirst.battery=LastBatteryPer;
  }
  else
  {
    LastBatteryPer=g_fish_dataFirst.battery;
  }
  if(g_fish_dataFirst.battery>100)
       g_fish_dataFirst.battery=100;
  else if(g_fish_dataFirst.battery<1)
       g_fish_dataFirst.battery=0;
//  ESP_LOGI(TAG,"g_fish_dataFirst.battery=%d",g_fish_dataFirst.battery);
}
void ADtoTEMP(void)
{
   float Rt=0;
   float Rp=4990;
   float T0=273.15+25;
   float Bx=3950;
   float Ka=273.15;
   float vol=0;
  //  float temp=0;
   if(g_fish_dataFirst.V_reference==0)
    g_fish_dataFirst.V_reference=Get_ADC_Date(10,AD_SAMP_VREF);
   vol=Get_ADC_Date(10,AD_Temp);
   Rt=(g_fish_dataFirst.V_reference-vol)*10000/vol;
   g_fish_dataFirst.temp=10*(1/(1/T0+log(Rp/Rt)/Bx)-Ka);
  //  ESP_LOGI(TAG,"温度=%d℃",g_fish_dataFirst.temp);
}


/* USER CODE END 0 */


/*----------------------------------END---------------------------------------*/