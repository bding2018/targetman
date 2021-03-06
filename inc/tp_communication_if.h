
#ifndef _NXP_TP_PP_H
#define _NXP_TP_PP_H



typedef unsigned short     UINT16;     /* 16-bit quantity */
typedef unsigned int       UINT32;     /* 32-bit quantity */
typedef unsigned long long UINT64;     /* 64-bit quantity */
typedef unsigned char      UINT8;      /* 8-bit quantity  */
typedef unsigned char      UCHAR;      /* 8-bit quantity  */

typedef signed char     INT8;           /* 8-bit quantity  */
typedef signed short    INT16;      /* 16-bit quantity */
typedef signed int         INT32;      /* 32-bit quantity */
typedef long long          INT64;          /* 64-bit quantity */
typedef float FLOAT32;
typedef double FLOAT64;

//typedef void   VOID;
typedef enum {False = 0, True = 1} GT_BOOL;
typedef signed int GT_STATUS;

#define UINT32_BIT_SIZE    32

#define FALSE  -1
#define TRUE   0

#define  ENABLE_ACT_UP_DATA  1   //  Communication mode  Device actively uploads data
#define  DISABLE_ACT_UP_DATA  0   //  Communication mode  Device actively uploads data

typedef enum target_device_e
{
    LIGHT_WEAPON = 1,  /* Light weapon 轻武器靶标系列 */
    HEAVY_WEAPON = 2,  /*  Heavy weapon 重武器靶标系列  */

    TP_DEVICE_MAX
} target_device_e;

#define    INDIVIDUAL_SOLDIER              0x01    /* 单兵起倒靶 Individual soldier */
#define    INDIVIDUAL_PRECIS_SOLDIER       0x02   /* 单兵精度起倒靶  */
#define    PORTABLE_INDI_SOLDIER           0x11  /* 便携式起倒靶 Individual precision reverse target*/
#define    PORTABLE_INDI_SOLDIER_SIDE      0x12        /* 便携式侧转靶 Portable  side  */
#define    PORTABLE_INDI_SOLDIER_SWING     0x13      /*  便携式摇摆靶*/


#define TG_RUN_FLG_STILL_L            0 //  静止
#define TG_RUN_FLG_UP_L               1 //  起动作
#define TG_RUN_FLG_DOWN_L             2 //  倒动作
#define TG_RUN_FLG_UNKOWN_H            0 //  未知位置
#define TG_RUN_FLG_UP_H               1 //  起位置
#define TG_RUN_FLG_DOWN_H             2 //  倒位置

#define ALARM_FLG_MSENSOR            0x1  //  电机传感器故障
#define ALARM_FLG_MOTO               0x2  //  电机堵转
#define ALARM_FLG_DRIVER             0x4  //  驱动电路故障
#define ALARM_FLG_SAMP               0x8  // 电流采样电路故障
#define ALARM_FLG_FHOS               0x10  //  Photoelectric Sensors 光电传感器
#define ALARM_FLG_SFAULT             0x80  //Short circuit fault  导电靶板短路故障

#define TG_CMD_QUERY_STATUS           0x01 //  设备状态（故障、起、倒、命中弹数等）查询
#define TG_CMD_QUERY_POSISION         0x02  //  定位坐标查询
#define TG_CMD_CFG_DEVTYPE            0x30  //  配置设备类型
#define TG_CMD_CFG_DEVNUMB            0x31 //  配置设备号
#define TG_CMD_CFG_DEVCMU             0x32 // 配置通信模式
#define TG_CMD_CFG_MODE               0x50 //  （命中后是否倒等）
#define TG_CMD_CTL_RESET              0x80 //  reset  
#define TG_CMD_CTL_UPDOWN             0x81 // 起倒/侧转控制
#define TG_CMD_CTL_SWING              0x82 // 摇摆靶控制
#define TG_CMD_CTL_LIGHT              0xa0 // 灯光控制
#define TG_CMD_CTL_HIT_NUM            0xa1 // 设置命中弹数
#define TG_CMD_COM_CONNECT            0xE1 //连接
#define TG_CMD_COM_DISCONNECT         0xE2 //断开

#define TARGET_DATA_LEN        256  //
#define PP_TX_LEN              512  //

// target communication mode 
#define TG_CM_DISBLE_SEND             0x00 //  禁止靶标设备主动上传数据（当状态发生改变时）
#define TG_CM_ENABLE_SEND             0x01 //  允许靶标设备主动上传数据（当状态发生改变时）
#define TG_CM_CYCLE_SEND              0x03 // 允许靶标设备周期上传数据

// target hit action
#define TG_RUN_HIT_DISBLE_DOWN            0x00 // 命中禁止倒  运动中禁止射击
#define TG_RUN_HIT_ENABLE_DOWN            0x01 //  命中倒靶    运动中禁止射击
#define TG_RUN_ENABLE_HIT                 0x02 // 命中禁止倒 运动中允许射击
#define TG_RUN_ENABLE_HIT_DOWN            0x03 // 命中倒靶   运动中允许射击

// target run action
#define TG_RUN_TO_DOWN                    0x01 // 起倒靶 倒靶 
#define TG_RUN_TO_UP                      0x02 // 起倒靶 起靶 
#define TG_RUN_TO_HID                     0x01 // 侧转靶 隐靶 
#define TG_RUN_TO_DISPLAY_FRONT           0x02 // 侧转靶 显示正面靶 
#define TG_RUN_TO_DISPLAY_BACK            0x02 // 侧转靶 显示反面靶 

#pragma pack (1)


typedef struct contrl_cmd_s
{
	char  head;
	char  lenth;
	char  cmd_serial;
	char  device;
	char  sub_dev;   //5
	char user_id_h;
	char user_id_l;
	char dev_id_h;  //
	char dev_id_l;  //
	char  cmd_type;
	char  ctl_data[4];
	char  checksum;
}contrl_cmd;

typedef struct target_data_s
{
	char  head;
	char  lenth;
	char  cmd_serial;
	char  device;
	char  sub_dev;   //5
	char user_id_h;
	char user_id_l;
	char dev_id_h;  //
	char dev_id_l;  //
	char  cmd_type;   //10
	char  data[TARGET_DATA_LEN];
}target_data;

typedef struct coordinates_param_
{
    char status;  /*定位与否及经纬度标识  BIT7： 0，表示未定位；1，表示已定位  BIT1：0，表示北纬；1，表示南纬 BIT0：0，表示东经；1，表示西经*/
    char longitude;
    char longitude1;
    char l_y;
    char l_x;
    char latitude;
    char latitude1;
    char ll_x;
    char ll_y;
}positon_s;

typedef struct hit_info_
{
    char direction:4;
    char grade:4;
}hit_info_s;

typedef struct pp_data_
{
    char fault_h;  
    char fault_l;
    char rev1;
    char run_state;
    char battery;
    char total_times_h;
    char total_times_l;
    char cycle_times;
    char data[TARGET_DATA_LEN - 10]; 
}pp_data;



#pragma pack()

typedef enum dev_type
{
    DB_QD     = 1,   //单兵 起倒靶
    DB_JD_QD  = 2, //  单兵 精度 起倒靶
    BX_QD     = 3,    //  便携 起倒靶
    BX_CZ = 4,    //   便携 侧转靶
    BX_YB = 5,    //   便携 摇摆靶
    DEV_TYPE_MAX
}dev_type_e;



typedef struct target_ctrl_cfg_s
{
    int targetId;
    int userId;
    dev_type_e dev_type;
    int op_type;  
    int data;
    int data2;
    int times;
}tg_ctrl_cfg;

typedef struct positon_info_
{
    unsigned short flag;  /*定位与否及经纬度标识  BIT7： 0，表示未定位；1，表示已定位  BIT1：0，表示北纬；1，表示南纬 BIT0：0，表示东经；1，表示西经*/
    unsigned short longitude;   //Degree 
    unsigned short lgde_minute;
    unsigned short latitude;    //Degree
    unsigned short ltde_minute; 
}positon_info_s;

typedef struct dev_status_
{
    unsigned short fault;     // 故障信息  ALARM_FLG_XXX定义信息
    unsigned short run_state;  // 运行状态  静止：起、倒、未知 运动：起过程中，倒过程中  ，摇摆靶时为摇摆位置的角度
    unsigned short battery;    //电池电量
    unsigned short hit_total;  // 总命中数
    unsigned short cycle_hits;  // 一个查询周期的命中数
    hit_info_s hit[TARGET_DATA_LEN];   //弹点坐标
}dev_status;


typedef struct target_query_s
{
    int targetId;    
    int userId;
    dev_type_e dev_type;
    int op_type;  
    union{
        positon_info_s ps_info;
        dev_status stat_info;
    }data;
}target_query;

#include "target_com_global.h"
/* 精度靶 :  工作模式、参数对应关系、状态信息(位置信息、运行信息)、设备信息(电量、故障信息)*/
/* 战术靶 :  工作模式、参数对应关系、状态信息(位置信息、运行信息)、设备信息(电量、故障信息)*/
/*命令类型-- 配置、控制、查询 */
/*********************************************************************
* func:        target_cfg_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号?
?              userId    用户编号   如:以班、排、连为单位时为班号、 排号、 连号；以人员为单位时为人员编号
*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                          #define TG_CMD_CFG_DEVTYPE            0x30  //  配置设备类型  --data:设备大类，data2:设备小类
*                          #define TG_CMD_CFG_DEVNUMB            0x31 //  配置设备号 --data:用户id，data2:设备 ID
*                          #define TG_CMD_CFG_DEVCMU             0x32 // 配置通信模式  --data
*                                    #define TG_CM_DISBLE_SEND  0x00 //  禁止靶标设备主动上传数据（当状态发生改变时）
*                                    #define TG_CM_ENABLE_SEND  0x01 //  允许靶标设备主动上传数据（当状态发生改变时）
*                                    #define TG_CM_CYCLE_SEND   0x03 // 允许靶标设备周期上传数据
                                         --data2 周期上送时的周期参数
*                          #define TG_CMD_CFG_MODE               0x50 //  （命中后是否倒等）
*              data:    参数数值
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int TARGET_COM_EXPORT  target_cfg_command(tg_ctrl_cfg* cmd,char* buff);

/*********************************************************************
* func:        target_query_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号；
?              userId    用户编号   如:以班、排、连为单位时为班号、 排号、 连号；以人员为单位时为人员编号
*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                          #define TG_CMD_CTL_RESET              0x80 //  reset  
*                          #define TG_CMD_CTL_UPDOWN             0x81 // 起倒/侧转控制
                                        data: 起倒侧转控制
                                        data2: 电机转速  0x1~0x5  共5档
*                          #define TG_CMD_CTL_SWING              0x50 // 摇摆靶控制
                                        data: BYTE1:表示循环摇摆时的间隔时间： （0-15，单位：200毫秒）
                                              BYTE0:表示速度等级，共5档  0 静止，1 ~5共5档
                                        data2: BYTE1:表示摇摆起始位置： （0-12，单位：15度）
                                              BYTE0:表示摇摆结束位置：  （0-12，单位：15度）
                                        times: 摇摆次数
*                          #define TG_CMD_CTL_LIGHT              0xa0 // 灯光控制  data:0x1~0x5  共5档
*                          #define TG_CMD_CTL_HIT_NUM            0x50 // 设置命中弹数  data:
*              data:    坐标值:经纬度、分 定位与否及经纬度标识  BIT7： 0，表示未定位；1，表示已定位  BIT1：0，表示北纬；1，表示南纬 BIT0：0，表示东经；1，表示西经
*                       状态:  dev_status
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int TARGET_COM_EXPORT target_ctrl_command(tg_ctrl_cfg* cmd,char* buff);

/*********************************************************************
* func:        target_ctrl_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号；  //全ff时为广播消息，仅用于控制
?              userId    用户编号   如:以班、排、连为单位时为班号、 排号、 连号；以人员为单位时为人员编号

*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                       #define TG_CMD_QUERY_STATUS           0x01 //  设备状态（故障、起、倒、命中弹数等）查询
*                       #define TG_CMD_QUERY_POSISION         0x02  //  定位坐标查询
*              data:    参数数值
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int TARGET_COM_EXPORT target_query_command(target_query* cmd,char* buff);


int TARGET_COM_EXPORT target_query_return_msg(char* msg, int msg_len, target_query* info);
#endif

