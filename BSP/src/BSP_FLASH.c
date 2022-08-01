/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_FLASH.h
    @Author:    Fitz
    @Description: 
    			
                
    @Others:
        Data:2022/07/05
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nvs_flash.h"
#include "nvs.h"
/* USER CODE END Includes */
#include "BSP_FLASH.h"
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static const char *StorageName="storage";
nvs_handle_t my_handle;
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
void nvs_Read_Date(uint8_t len,const char* key,uint8_t* Date)
{ 
    // Initialize NVS
    nvs_flash_init();
    // Open
    nvs_open(StorageName, NVS_READWRITE, &my_handle);//分配一个空间
    for(;len>0;len--,key++,Date++)
    {
       nvs_get_u8(my_handle,key, Date);             //key为主键即地址
    }
    nvs_close(my_handle);
}
void nvs_Write_Date(uint8_t len,const char* key,uint8_t *Date)
{
   // Initialize NVS
    nvs_flash_init();
    // Open
 
    nvs_open(StorageName, NVS_READWRITE, &my_handle);
    for(;len>0;len--,key++,Date++)
    {
       nvs_set_u8(my_handle, key, *Date);  
    }
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

/* USER CODE END 0 */

/*----------------------------------END---------------------------------------*/