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
#ifndef _BSP_COMM_H_
#define _BSP_COMM_H_
#include "freertos/event_groups.h"
#ifdef _cplusplus
extern "C"{
#endif


#define packbackbit   (0X01 << 0)
#define PwmTranbit    (0X01 << 1)
#define Samplingbit   (0X01 << 2)
#define unpackbit     (0X01 << 3)



#define SWITCH_DATA_HEAD     0xa8e6
#define SWITCH_DATA_TERM     0xb9d5
#define SWITCH_DATA_LEN      0x22

#define ACK_DATA_HEAD        0xb9d5
#define ACK_DATA_TERM        0xa8e6
#define ACK_DATA_LEN         0x09

#define FISH_DATA_HEAD       0xa8e6
#define FirstFISH_DATA_LEN   0x2E
#define OtherFISH_DATA_LEN   0x13
#define AllDataLen           sizeof(g_fish_dataOther.data)/sizeof(g_fish_dataOther.data[0])
#define FISH_DATA_CMD        0x10     //波形数据

#define CMD_OFF              0X01
#define RANGE_CHANGE         0X02
#define FER_CHANGE           0X03
#define LINK_CHANGE          0X04
#define Version_ASK          0X05
#define WIFI_CHANGE          0X06
#define Target_Base          0X07

#define AP_MODE              0X00
#define STA_MODE             0X01

#define GAIN_CHANGE          0X20
#define AutoRANGE_CHANGE     0X21
#define SetGAIN_Off_CHANGE   0X22
#define PWM_Time_CHANGE      0X23





#define LAST_Version         2000
#define Default_TVG          0
// #define FRE_SAMP             116000
#define FRE_PWML             110000
#define FRE_PWMH             310000

#define default_target_baseL  1000
#define default_target_baseH  1500


#pragma pack(push,1)
typedef struct
{
    uint16_t head;
    uint16_t len;
    uint8_t  cmd;
    uint8_t  cmd_status;
    char     content[26];
    uint16_t term;

}SWITCH_DATA_t;
typedef struct
{
    uint16_t head;
    uint16_t len;
    uint8_t  cmd;
    uint16_t link_status;
    uint16_t term;
}ACK_DATA_t;
typedef struct
{
    uint16_t head;
    uint16_t len;
    uint8_t  cmd;
    uint32_t comm_num;//连接后累加不清零
    uint8_t  sum_packets;
    uint8_t  num_packets;
    uint8_t  fre_mode;
    uint16_t temp;
    int8_t  battery;
    uint8_t  range_depth;
    uint16_t coord_depth;
    uint16_t status;
    int32_t lat;//纬度
    int32_t lon;//经度
    int64_t timestamp;
    uint16_t TargetBase;//目标基准电压
    uint16_t V_reference;  //参考电压
    uint8_t  reserved[6];
    uint16_t data[700];
}FIRST_FISH_DATA_t;


typedef struct
{
    uint16_t head;
    uint16_t len;
    uint8_t  cmd;
    uint32_t comm_num;//开机后累加不清零
    uint8_t  sum_packets;
    uint8_t  num_packets;
    uint16_t V_reference;  //参考电压
    uint8_t  reserved[6];
    uint16_t data[700];
}OTHER_FISH_DATA_t;

typedef struct  GAIN_Off
{
	int16_t Fre_L;
    int16_t Fre_H;
}GAIN_Off_t;
typedef struct  TARGET_Base
{
	int16_t Fre_L;
    int16_t Fre_H;
}TARGET_Base_t;
#pragma pack(pop)


SWITCH_DATA_t       g_switch_data;
ACK_DATA_t          g_ack_data;
FIRST_FISH_DATA_t   g_fish_dataFirst;
OTHER_FISH_DATA_t   g_fish_dataOther;
GAIN_Off_t          g_gain_offset;
TARGET_Base_t       g_target_base;


extern uint8_t  g_LastRange,g_LastFreMode,g_LastSumPacks,g_LastLink,g_LastCMD_OFF,g_intervalPacks,g_Last_GAIN,g_LastGainBase,g_LastWifiMode;
extern uint16_t g_LastSamplNum,g_LastPwm_Time,g_LastTVGnum;
extern uint32_t g_LastSampRate;
extern uint32_t g_LastFre;
extern uint8_t sta_ssid[32];   
extern uint8_t sta_password[64];  
EventGroupHandle_t PACKEvent_Group;

/**
 * @brief Recive_Data
 * @note  接收数据
 *
 */
void  Recive_Data1(void);
void  Recive_Data2(void);
void  Recive_Data3(void);
/**
 * @brief ACK_Pack
 * @note  应答包
 *
 */

ACK_DATA_t* ACK_Pack(uint8_t cmd,uint16_t status);
/**
 * @brief 数据采集回包
 * @param  sum_pack 总包数
 * @param  num_pack 包序号
 * @param  num_data 数据有效长度
 * @param  freMode  频率模式
 */
FIRST_FISH_DATA_t* FirstFish_DataPack(uint8_t sum_pack,uint8_t num_pack,uint16_t num_data,uint16_t* Data,uint8_t freMode );
OTHER_FISH_DATA_t* OtherFish_DataPack(uint8_t sum_pack,uint8_t num_pack,uint16_t num_data,uint16_t* Data);
/**
 * @brief 数据接收
 */
void  Recive_Data(void);


/**
 * @note  解包
 */
void COMM_Switcm(void);
/**
 * @note  参数初始化
 */
void CommPramInit(void);

/**
 * @note  电量转化成百分比
 */
void BatteryToPer(void);
/**
 * @note  AD转换成温度
 */
void ADtoTEMP(void);
#ifdef _cplusplus
}
#endif

#endif