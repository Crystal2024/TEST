/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_SPIADC.c
    @Author:    Fitz
    @Description:
                spi接口外置ADC驱动 
    					1、主要功能
    					2、函数接口，包括参数及返回值
    					3、头文件依赖关系
    @Others:
 
    @History:

 
 **********************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "hal\gpio_ll.h"
#include "esp_log.h"

#include "soc/spi_struct.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_SPIADC.h"
#include "BSP_COMM.h"


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MISO_GPIO 14
#define CLK_GPIO  15
#define CS_GPIO   12 
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
spi_device_handle_t spiadc;//SPI句柄
/* USER CODE END PV */
static const char *TAG = "BSP_SPIADC";

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


 __inline void delay_clock(int ts)
{
uint32_t start, curr;

__asm__ __volatile__("rsr %0, ccount" : "=r"(start));
do
__asm__ __volatile__("rsr %0, ccount" : "=r"(curr));
while (curr - start <= ts);
}
/* USER CODE END 0 */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void SPIADC_Config()
{
    spi_bus_config_t buscfg ={
        .miso_io_num = MISO_GPIO,
        .sclk_io_num = CLK_GPIO,
        .mosi_io_num = -1,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 16*320*2+8
    };

    spi_device_interface_config_t interfacecfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits   = 0,
        .input_delay_ns=0,
        .mode =2,
        .spics_io_num = CS_GPIO,
        .clock_speed_hz = SPI_MASTER_FREQ_8M,
        // .clock_speed_hz =100000,

        .queue_size = 7,
        
    };
    // SPI2.user.rd_byte_order=1;//大小端配置
    SPI2.ctrl2.cs_delay_mode=3;
    SPI2.ctrl2.cs_delay_num=1;
 
     spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &interfacecfg, &spiadc);
}

void SPIADC_send()
{
    spi_transaction_t send = {
        .length = 16,
        .tx_data = {8},
        .flags = SPI_TRANS_USE_TXDATA
    };

    spi_device_polling_transmit(spiadc,&send);
}

uint32_t SPIADC_read()
{
    spi_transaction_t read = {
        .length = 16,
        .rx_data = {0},
        .flags = SPI_TRANS_USE_RXDATA
    };

    spi_device_polling_transmit(spiadc,&read);
    return *(uint32_t *)read.rx_data;
}

/*----------------------------------END---------------------------------------*/
void SOFT_SPI_Config()
{
    gpio_config_t MISO_conf = {
    .mode = GPIO_MODE_DEF_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pin_bit_mask = (1UL << MISO_GPIO)
    };
    gpio_config(&MISO_conf);
    gpio_reset_pin(CS_GPIO);
    gpio_reset_pin(CLK_GPIO);
   
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(CS_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(CLK_GPIO, GPIO_MODE_OUTPUT);

   gpio_set_level(CLK_GPIO,1);
   gpio_set_level(CS_GPIO,1);
}

//    gpio_set_level(CLK_GPIO,1);//拉高
//    gpio_set_level(CS_GPIO,0);//拉低CS 
//    for(uint8_t i=0;i<Byte;i++)
//    {  
    //  Data<<=1; 
    //  gpio_set_level(CLK_GPIO,0);//拉低
    //  if(gpio_get_level(MISO_GPIO))//采样
    //  Data++; 
    //  gpio_set_level(CLK_GPIO,1);//拉高
    // }
    //  gpio_set_level(CS_GPIO,1);//拉高CS 
    //  return Data;
uint16_t SOFT_SPI_Read(void)
{
   uint16_t Data=0;
   
   
   gpio_dev_t *dev= (gpio_dev_t *)GPIO_LL_GET_HW(GPIO_PORT_0);
   dev->out_w1tc = 1 << CS_GPIO;//拉低CS
   for(uint8_t i=0;i<16;i++)
   {   
    Data<<=1; 
    dev->out_w1tc = 1 << CLK_GPIO;//拉低CLK
    if(((dev->in)>> MISO_GPIO)& 0x1)
      Data++; 
     dev->out_w1ts = 1 << CLK_GPIO;//拉高CLK
   } 
   dev->out_w1ts = 1 << CS_GPIO;//拉高CS 
   return Data;
}


void Sampling(uint16_t Num,uint16_t* SampData)
{    
     uint16_t Data=0;
    
     float Clk_Num=240*(1.0*1000000/g_LastSampRate-3.42)-33;  
    //  ESP_LOGI(TAG,"g_LastSampRate=%d",g_LastSampRate);
     for(uint16_t i=0;i<Num;i++,SampData++)
    {   
      Data=(uint16_t)SOFT_SPI_Read(); 
      delay_clock((uint16_t)Clk_Num); 
       *SampData=Data*3000/4095;
    }
}
// uint16_t ReferenceVoltageSmp(void)
// {
//     g_fish_data.V_reference=0;
//     for(uint8_t i=0;i<10;i++) 
//     {
//       g_fish_data.V_reference=g_fish_data.V_reference+(uint16_t)SOFT_SPI_Read() ;
//     }
//     g_fish_data.V_reference=3000*(g_fish_data.V_reference/10)/4095;
//     ESP_LOGI(TAG,"g_fish_data.V_reference=%d",g_fish_data.V_reference);
//     return g_fish_data.V_reference;

// }



