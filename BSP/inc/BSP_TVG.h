/**********************************************************
    Copyright © 2021 Bohai Shenheng Technology Co.Ltd   
     
    @File name: BSP_TVG.h
    @Author:    KK
    @Description: 
    			
                
    @Others:
        Data:2022/05/17
        Version:V1.0
        ESP_IDF Version:V4.3.1
    @History:

 **********************************************************/
#ifndef _BSP_TVG_H_
#define _BSP_TVG_H_

#ifdef _cplusplus
extern "C"{
#endif

const unsigned char tvg_tableH[1700],tvg_tableL[1700];
typedef struct
{
    uint8_t rang_6;
    uint8_t rang_12;
    uint8_t rang_24;
    uint8_t rang_36;
    uint8_t rang_60;
    uint8_t rang_90;
    uint8_t rang_120;
}TVG_DATA_t;
const  TVG_DATA_t TVG_DATE;
extern void I2S_DAC_init(void);
/**
 * @brief TVG_Set
 * @note  设置TVG
 * @param *Data:表格首地址，Len长度
 */
extern void TVG_Set(uint8_t *Data,uint32_t Len);

/**
 * @brief SET_GainNum
 * @note  设置增益 
 * @param Num 增益值 mV=3300*Num/256
 */
void SET_GainNum(uint8_t Num);
#ifdef _cplusplus
}
#endif

#endif