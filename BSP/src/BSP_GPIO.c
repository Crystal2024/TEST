/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_GPIO.h
    @Author:    Fitz
    @Description: 
    			
                
    @Others:
        Data:2022/03/07
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "driver/gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_GPIO.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private typedef ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


/* Private user Task ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void GPIO_LEDConfig(gpio_num_t gpio_num)
{
    
    gpio_config_t led_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = (1UL << gpio_num)
    };

    ESP_ERROR_CHECK(gpio_config(&led_conf));
}



/* USER CODE END 0 */

/*----------------------------------END---------------------------------------*/