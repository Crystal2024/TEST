/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd  
    
   @File name: mian.c
   @Description: 
   				程序应用逻辑层文件
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "sys/time.h"
#include "esp_log.h"
#include "hal/timer_hal.h"
#include <sys/socket.h>
#include "driver/mcpwm.h"
#include "driver/i2s.h"

#include "nvs_flash.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_TIM.h"
#include "BSP_WIFI.h"
#include "BSP_ADC.h"
#include "BSP_GPIO.h"
#include "BSP_MCPWM.h"
#include "BSP_SPIADC.h"
#include "BSP_COMM.h"
#include "BSP_GPS.h"
#include "BSP_TVG.h"
#include "BSP_ADC.h"

/* USER CODE END Includes */


/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Bottom_Base      300       //砍底基准
#define Bottom_count     6



/********Power switch*******/
#define SW_PWM      GPIO_NUM_18       //升压开关
#define SW_5VDC     GPIO_NUM_5     //模拟电路开关
#define SW_GPS      GPIO_NUM_22
#define WORK_LED    GPIO_NUM_13
   

/********spi adc**********/
#define SPI_CS    12
#define SPI_CLK   15
#define SPI_MISO  14
/********PWM**********/
#define SOFTPWM_A     23
#define SOFTPWM_B     19

/********GPS Uart*********/
#define Gps_tx    10
#define Gps_rx    9
/********GPS Uart*********/




#define SW_ON     0
#define SW_OFF    1

/* USER CODE END PD */

/* Private typedef ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*typedef struct
{

}Sonar_Data;*/
 uint16_t Data[16100]={0};
// #define Timer_ISR (0x01 << 0)
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const char *TAG = "mian";

 
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*定时器回调函数*/
bool IRAM_ATTR timer_group_callack(void *arg)
{  
  
  // xEventGroupSetBits(PACKEvent_Group,Samplingbit);
  return pdTRUE;
}

void BSPInit()
{ 
  
  //  CommPramInit();
    /*初始化nvs*/
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    Wifi_APConfig();
   


   GPIO_LEDConfig(SOFTPWM_A);
   gpio_set_level(SOFTPWM_A,0);
   GPIO_LEDConfig(SOFTPWM_B);
   gpio_set_level(SOFTPWM_B,0);
   GPIO_LEDConfig(SW_PWM);
   gpio_set_level(SW_PWM,SW_ON);
   GPIO_LEDConfig(SW_5VDC);
   gpio_set_level(SW_5VDC,SW_ON);
   GPIO_LEDConfig(SW_GPS);
   gpio_set_level(SW_GPS,SW_OFF);
   GPIO_LEDConfig(WORK_LED);
   gpio_set_level(WORK_LED,SW_ON);
   ADC_Init();
  //  g_fish_dataFirst.V_reference=Get_ADC_Date(10,SAMP_VREF);
  //  SET_GainNum(0);
   SET_GainNum(g_Last_GAIN);
   //  SPIADC_Config();
  //  I2S_DAC_init();
   SOFT_SPI_Config();
  //  TIM_Config(TIMER_GROUP_0,TIMER_0,100,timer_group_callack);
  //  MCPWM_Config(GPIO_NUM_23,g_LastFre,50,NULL);   //默认初始化低频
  //  MCPWM_Capture_Config(GPIO_NUM_26);
   Uart1_Init();
   delay_clock(240*100000);



   
   
}


/* USER CODE END 0 */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*static void IRAM_ATTR mcpwm_isr()
{
  ESP_LOGI(TAG,"MCPWM_isr");

}*/

void DataBackEvent(void)
{
  uint8_t  PackNum=0;

  uint16_t DataLenEnd=0;
  uint16_t NowDataLen=0;
  EventBits_t Packbits;
  TickType_t xLastWakeTime;

  while (1)
  {  
    Packbits=xEventGroupWaitBits(PACKEvent_Group,
                       unpackbit,
                       pdTRUE,
                       pdFALSE,
                       portMAX_DELAY);            
    if(Packbits>=unpackbit)  
    {   
       
       if(g_switch_data.cmd_status==0x01&&g_switch_data.cmd==LINK_CHANGE)
          {     //连接前先回版本号
            send(connect_socket,ACK_Pack(Version_ASK,LAST_Version),sizeof(g_ack_data),0);//软件版本号
          }
       send(connect_socket,ACK_Pack(g_switch_data.cmd,g_switch_data.cmd_status),sizeof(g_ack_data),0);//先返回ACK包 
       //if()//连接成功则返回版本号
       COMM_Switcm();
       if(g_LastLink==1&&g_LastCMD_OFF==1)
          xLastWakeTime=xTaskGetTickCount();
    }
    while(g_LastLink!=0&&g_LastCMD_OFF==1)
    {
      if(xEventGroupGetBits(PACKEvent_Group)==unpackbit)
      break;
      memset(&Data,0x00,sizeof(Data));//数据清零
      

     

      if(g_LastGainBase==0)
      {
        if(g_LastFreMode==0)
        TVG_Set(tvg_tableL,g_LastTVGnum);
          else 
        TVG_Set(tvg_tableH,g_LastTVGnum);
      }
      delay_clock(240*100);
      SOFT_PWM(SOFTPWM_A,SOFTPWM_B,g_LastFre,g_LastPwm_Time);
    
      Sampling(g_LastSamplNum,Data);//内存充足
      if(g_LastGainBase==0)
        i2s_zero_dma_buffer(0);
     DataLenEnd=g_LastSamplNum%(sizeof(g_fish_dataFirst.data)/sizeof(g_fish_dataFirst.data[0]));
     if(DataLenEnd==0)
         DataLenEnd=AllDataLen;
      NowDataLen=AllDataLen;
      do{
          if(PackNum==g_LastSumPacks-1)//最后一包
               NowDataLen=DataLenEnd;
          if(PackNum==0) //第一包
          {     
           send(connect_socket,FirstFish_DataPack(g_LastSumPacks,PackNum,NowDataLen,(uint16_t *)(Data+(PackNum*(sizeof(g_fish_dataFirst.data)/sizeof(g_fish_dataFirst.data[0])))),g_LastFreMode),NowDataLen*2+FirstFISH_DATA_LEN,0);
          
          }
          else
          {
           send(connect_socket,OtherFish_DataPack(g_LastSumPacks,PackNum,NowDataLen,(uint16_t *)(Data+(PackNum*(sizeof(g_fish_dataOther.data)/sizeof(g_fish_dataOther.data[0]))))),NowDataLen*2+OtherFISH_DATA_LEN,0);
         
          }
             PackNum++;
        } while(PackNum<g_LastSumPacks);
         PackNum=0;
          
          g_fish_dataFirst.comm_num++;
          g_fish_dataOther.comm_num=g_fish_dataFirst.comm_num;

        // vTaskDelay(g_intervalPacks/portTICK_RATE_MS); 
        vTaskDelayUntil(&xLastWakeTime,(g_intervalPacks/portTICK_RATE_MS));     
   }
  }        
  
}

// uint8_t Looking_Bottom(uint16_t SampNum,uint16_t* SampData)
// {
//   uint8_t Group_Count=0,Bottom_flag=0;
//   uint16_t LastSampData=5000;
//   for(uint16_t i=0;i<SampNum;i++,SampData++)//历遍当前量程所有值
//    {
//       if(*SampData<Bottom_Base&&Bottom_flag==0)//找到一个值小于设定底值（负检波）
//       {
//          for(uint16_t j=i;j<i+10;j++)//往后找10个个点
//          {
//             if(*SampData<Bottom_Base)//如果N个点中有M个点小于设定值
//             {
//               Group_Count++;             
//               if(Group_Count>=Bottom_count)
//               {
//                i=j;
//                Bottom_flag=1;
//                break;
//               }
//             }
//          }
//       }
//       if(Bottom_flag==1)//已找到底
//       {
//         //峰值
//         if(*SampData<LastSampData)
//         {
//           LastSampData=*SampData;
//         }
//         else
//         {
//           Bottom_flag=0;
//           return i;
//         }
//       }
//    }
// // //未找到底
// // //修改量程?修改TVG?
//    return 0;
// }
/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval none
  */
void app_main(void)
{
  BSPInit();
  xTaskCreatePinnedToCore(&DataBackEvent,"BackEvent",1024*20,NULL,20,NULL,1);
  PACKEvent_Group = xEventGroupCreate();
  while (1)
  {
    vTaskDelay(3000/portTICK_RATE_MS);  
    if( g_LastLink==1&&g_LastCMD_OFF==1)
    {
        BatteryToPer();
        ADtoTEMP();
    }
    
    

  /* code */     
  }

    
}
