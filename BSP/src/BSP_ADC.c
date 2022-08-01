/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_ADC.h
    @Author:    Fitz
    @Description: 
    			BSP驱动层 对idf框架的ADC驱动进一步封装
                
    @Others:
        Data:2021/11/19
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_ADC.h"


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_VREF    3300        //Use adc2_vref_to_gpio() to obtain a better estimate
//临界段
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static esp_adc_cal_characteristics_t *adc_chars;

const adc_channel_t AD_Temp = ADC_CHANNEL_4;        //GPIO32 if ADC4
const adc_channel_t AD_SAMP_VREF = ADC_CHANNEL_6;   //GPIO34 if ADC6
const adc_channel_t AD_Battery = ADC_CHANNEL_7;     //GPIO35 if ADC7


static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void ADC_Init(void)
{
    //Configure ADC
    adc1_config_width(width);

    adc1_config_channel_atten(AD_Temp, atten);
    adc1_config_channel_atten(AD_SAMP_VREF, atten);
    adc1_config_channel_atten(AD_Battery, atten);
    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
}
uint32_t Get_ADC_Date(uint8_t num,adc_channel_t ChannelNum)
{
   uint32_t adc_reading=0;
  for (uint8_t i = 0; i < num; i++) 
  {   
   adc_reading+=adc1_get_raw((adc1_channel_t)ChannelNum);

  }
   
  adc_reading/= num;
  uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
//    printf("adc_reading%d: %d\tVoltage: %dmV\n",ChannelNum, adc_reading, voltage);
   return voltage;
}


/* USER CODE END PFP */