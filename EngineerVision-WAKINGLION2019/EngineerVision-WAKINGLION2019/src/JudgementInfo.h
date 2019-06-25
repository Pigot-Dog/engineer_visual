//-----------------------------------【裁判系统信息结构体】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------

#ifndef __JUDGEMENT_INFO_H__
#define __JUDGEMENT_INFO_H__

#include "Protocol.h"

#ifdef USE_SERIAL

#define JUDGE_FIFO_BUFLEN 500

/** 
  * @brief  judgement data command id
  */
typedef enum
{
  GAME_INFO_ID       = 0x0001,  //10Hz
  REAL_BLOOD_DATA_ID = 0x0002,
  REAL_SHOOT_DATA_ID = 0x0003,
  REAL_POWER_DATA_ID = 0x0004,   //实时功率和热量数据
  REAL_FIELD_DATA_ID = 0x0005,  //10hZ
  GAME_RESULT_ID     = 0x0006,
  GAIN_BUFF_ID       = 0x0007,
  POSITION_DATA_ID   = 0x0008,  //机器人位置和枪口朝向信息，50HZ频率周期发送
  
  STU_CUSTOM_DATA_ID = 0x0100,
  ROBOT_TO_CLIENT_ID = 0x0101,
  CLIENT_TO_ROBOT_ID = 0x0102,
} judge_data_id_e;


/** 
  * @brief  GPS state structures definition
  */
typedef   struct //__packed
{
  u8 valid_flag;
  float x;
  float y;
  float z;
  float yaw;
}__attribute__((packed))  position_t;

/** 
  * @brief  game information structures definition(0x0001)
  *         this package send frequency is 50Hz
  */
typedef   struct  //__packed
{
  u16   stage_remain_time;
  u8    game_process;
  /* current race stage
   0 not start
   1 preparation stage
   2 self-check stage
   3 5 seconds count down
   4 fighting stage
   5 result computing stage */
 // u8    reserved;
  u8    robotLevel;
  u16   remain_hp;
  u16   max_hp;
 // position_t position;
} game_robot_state_t;

/** 
  * @brief  real time blood volume change data(0x0002)
  */
typedef  struct //__packed
{
  u8 armor_type:4;
 /* 0-3bits: the attacked armor id:
    0x00: 0 front
    0x01：1 left
    0x02：2 behind
    0x03：3 right
    others reserved*/
  u8 hurt_type:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01：module offline
    0x02: bullet over speed
    0x03: bullet over frequency */
} robot_hurt_data_t;

/** 
  * @brief  real time shooting data(0x0003)
  */
typedef  struct //__packed
{
 // u8 reserved1;
  u8 bulletType;  //弹丸类型
  u8 bullet_freq;
  float   bullet_spd;
 // float   reserved2;
	
} real_shoot_t;

/*********design by li********/
/** 
  * @brief  power_heat_data(0x0004)
  */
typedef  struct  //__packed
{
 float chassisVolt;    //底盘输出电压
 float chassisCurrent; //底盘输出电流
 float chassisPower;   //底盘输出功率
 float chassisPowerBuffer; //地盘功率缓冲
 u16 shootHeat0;      //17mm枪口热量
 u16 shootHeat1;      //42mm枪口热量
}extPowerHeatData_t;
/*****************************/

/** 
  * @brief  rfid detect data(0x0005)
  */
typedef  struct  //__packed
{
  u8 card_type;
  u8 card_idx;
} rfid_detect_t;

/** 
  * @brief  game result data(0x0006)
  */
typedef  struct   //__packed
{
  u8 winner;
} game_result_t;

/** 
  * @brief  the data of get field buff(0x0007)
  */
typedef  struct   //__packed
{
	/*
  u8 buff_type;
  u8 buff_addition;
	*/
	u16 buffMusk;
} get_buff_t;

/**********design by li**************/
/** 
  * @brief  position(0x0008)
  */
typedef  struct  //__packed
{
  //u8 valid_flag;
  float x;  //位置x坐标值
  float y;	//位置y坐标值
  float z;	//位置z坐标值
  float yaw; //枪口朝向角度值
} extGameRobotPos_t;
/*********************************/

/** 
  * @brief  student custom data
  */
typedef  struct  //__packed
{
  float data1;
  float data2;
  float data3;
  u8 mask;  //自定义数据4
} client_show_data_t;

typedef  struct  //__packed
{
  u8  data[64];
} user_to_server_t;

typedef  struct  //__packed
{
  u8  data[32];
} server_to_user_t;

/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  game_robot_state_t game_information;
  robot_hurt_data_t    blood_changed_data;
  real_shoot_t              real_shoot_data;
  extPowerHeatData_t  power_heat_data; //
  rfid_detect_t             rfid_data;
  game_result_t            game_result_data;
  get_buff_t                    get_buff_data;
  extGameRobotPos_t  position;        //
  client_show_data_t      client_data;     //用户自定义数据
  server_to_user_t          student_download_data;
} receive_judge_t;//接收来自主控 转发给妙算的信息

#endif // USE_SERIAL

#endif

