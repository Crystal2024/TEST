/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_GPS.h
    @Author:    kk
    @Description: 
    			
                
    @Others:
        Data:2022/03/07
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"




/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_GPS.h"
#include "BSP_COMM.h"


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RXD_PIN            GPIO_NUM_9
#define TXD_PIN            GPIO_NUM_10

#define PATTERN_CHR_NUM     (1)


#define BUF_SIZE            (1024)

#define GPS_CMD_Num         (0)
#define GPS_Statue          (2)
#define GPS_latDir          (4)//纬度半球
#define GPS_lonDir          (6)//经度半球





/* USER CODE END PD */

/* Private typedef ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
  uint8_t  dtmp[200];
  uint8_t  GPS_AddrOff[15]={0};
  uint32_t GPS_Addr_Head=0;
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static const char *TAG = "GPS";
static QueueHandle_t Uart1_Queue;
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

uint8_t GPS_Com(uint8_t *Data,uint8_t Len);
/* USER CODE END PFP */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void uart_event_task(void)
{
    uart_event_t event;
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(Uart1_Queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            memset(&dtmp,0x00,sizeof(dtmp));//数据清零
            if(event.type==UART_DATA) {          
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY); 
                GPS_Com(dtmp,event.size);    
            }
        }
    }
    vTaskDelete(NULL);
}


//GPS_AddrOff[i]为第i个与首地址偏移值
//即第i个数据首地址为GPS_Addr_Head+GPS_AddrOff[i]
//长度为GPS_AddrOff[i+1]-GPS_AddrOff[i]
uint8_t GPS_Com(uint8_t *GPSData,uint8_t Len)
{
    uint8_t CMD_Num=0;
    GPS_Addr_Head=(uint32_t)GPSData;
   CMD_Num=0;
   while(Len>0)
   {     
    if(*GPSData=='$')
    {
     if(*(GPSData+1)=='G'&&*(GPSData+2)=='N'&&*(GPSData+3)=='R')
     {
      uart_disable_rx_intr(EX_UART_NUM);
      memset(&GPS_AddrOff,0x00,sizeof(GPS_AddrOff));//数据清零
      GPS_Addr_Head=(uint32_t)GPSData;
      GPS_AddrOff[0]=0;
      CMD_Num=1;
     }
     else
       return 0; 
    }
    else if(*GPSData==','&&CMD_Num>0)
    {
      
      if(CMD_Num==GPS_Statue&&*(GPSData+1)=='V')//定位失败
        return 0; 
      else if (CMD_Num==GPS_Statue&&*(GPSData+1)=='A')//定位成功
      {      
        g_fish_dataFirst.lat=0;
        g_fish_dataFirst.lon=0;
      }
      else if(CMD_Num==GPS_latDir&&*(GPSData+1)=='S')//纬度半球//南为-
         g_fish_dataFirst.lat=-1*g_fish_dataFirst.lat;
      else if(CMD_Num==GPS_lonDir&&*(GPSData+1)=='W')//经度半球//西为-
         g_fish_dataFirst.lon=-1*g_fish_dataFirst.lon;
      GPS_AddrOff[CMD_Num]=GPSData-GPS_Addr_Head;
      CMD_Num++;
    } 
    GPSData++;
   if(CMD_Num==GPS_latDir&&(*GPSData)!=',')  //纬度
    {
      if(*GPSData=='.')
        g_fish_dataFirst.lat=60*(g_fish_dataFirst.lat/100)+g_fish_dataFirst.lat%100;
      else    
        g_fish_dataFirst.lat=(g_fish_dataFirst.lat*10)+(*GPSData-'0'); 
    }
    else if(CMD_Num==GPS_lonDir&&(*GPSData)!=',')  //经度
    {   
      if(*GPSData=='.')
        g_fish_dataFirst.lon=60*(g_fish_dataFirst.lon/100)+g_fish_dataFirst.lon%100;
      else
        g_fish_dataFirst.lon=(g_fish_dataFirst.lon*10)+(*GPSData-'0');
    }
   
    else if(CMD_Num>GPS_lonDir)//只解包到经度半球
    {      

      break;
    }
    Len--;
   }
   uart_enable_rx_intr(EX_UART_NUM);
return 1; 

}

void Uart1_Init(void) {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 10, &Uart1_Queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', 1, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(EX_UART_NUM, 10);
        //Create a task to handler UART event from ISR
    uart_disable_rx_intr(EX_UART_NUM);        //激活不接收    
    xTaskCreatePinnedToCore(&uart_event_task, "uart_event_task", 1024, NULL, 1, NULL,0);  
}

/* USER CODE END 0 */

/*----------------------------------END---------------------------------------*/