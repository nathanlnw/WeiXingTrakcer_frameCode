  //  Tracker    Functions.h

#include "gps.h"
#include "gsm.h"
#include "functions.h"
#include "Hardware.h"


#define   ENABLE      1
#define   FALSE       0


// part1.   sys configuration
typedef struct  _SysConfig   //name:  jt808
{
  u16  Version_ID;  //  系统版本ID  

  u8   AdminCellphoneNum[20];  //  admin
  u8   UserCellphoneNum[20];  //  user   
  u8   ServerIP_main[4];    // main socket
  u16  ServerPort_main;  
  u8   ServerIP_aux[4];     // aux socket
  u16  ServerPort_aux;  
  u8   ServerIP_debug[4];  
  u16  ServerPort_debug;    // debug socket

  u16  Mode1_Dur;      //  工作模式下的发送间隔     10s
  u16  Mode2_Dur;      //  120s
  u16  Mode3_Dur;      //  90 min

  u16  Stolen_CutOil_Dur;  // 开启防盗模式下，到切断油路的间隔 默认为0
  u8   speedwarn_Max;  //  超速门限   KM/h       60 default 
  u16  illegalMove_dis; //  非法位移   200米
  u16  Speaker_warn_keep_Dur; //  蜂鸣器 报警器持续时间   5min=300s

  u8   Anti_theft_enable;   //  防盗功能  默认关闭
  u8   auto2Anti_theft_enable; //  自动切换到防盗状态 默认: 关闭
  u8   cutpowerWarn_enable;   // 断电报警使能  默认开启
  u8   lowVoltWarn_enable;  // 欠压报警  --开启
  u8   nomoneyWarn_enable;  // 欠费报警 --开启  
  u8   illegalMove_enable;  // 非法位移报警    ---开启
  u8   abnormalMove_cutpower_enable;  //  异动情况下 切断引擎  : 开启
  u8   shakeWarn_enable;// 震动报警:开启  
  u8   speedWarn_enable; // 超速报警使能  关闭
  u8   gprs_work_enable;  //  gprs 功能:开启      
  u8   SMS_work_enable;   //   sms 功能 : 关闭 
  
   u8   current_userstate; //    当前用户状态       管理员 或 用户
   u8   superPassword[20];  //  超级管理员密码
}SYS_CONFIG;


//  防盗功能相关
typedef struct _ANTI_TH
{
  u8   anti_working;   //  是否处于防盗状态下
  u8   start_execut_warning;  //开始执行报警指示     0: idle    1: executing     2:  execute  over  
  u8   anti_workStep;   //  0 : init
  u8   speaker_enable; //  使能控制speaker
  u32  speaker_run_counter; // speaktimer
  u8   rx_enable_anti_theft_sms;  //  接收到开启防盗模式短信息

  u8   cutoff_oil_delay_enable; // 使能延时切断
  u16  cutoff_oil_delay_counter;//  使能切断定时器



}ANTI_TH;

extern ANTI_TH  Anti_theft;  // 防盗功能 变量


extern  u8  DeviceID[30];  //  终端ID 单独存储

extern  SYS_CONFIG   SysConfig;
extern  SYS_CONFIG   SysConfig_BAK1;
extern  SYS_CONFIG   SysConfig_BAK2;

extern void Trker_SysConfiguration(void);


// part2.  working   

#define MODE1         1
#define MODE2         2
#define MODE3         3

//  Warn   Status   Bits
#define  WARN_EMERG       0x00001       //  bit 0   紧急报警
#define  WARN_STOLEN      0x00002       //  bit1   防盗报警
#define  WARN_POWERCUT    0x00004       //  bit 2  断电报警
#define  WARN_LOWPOWER    0x00008       //  bit 3   低电压报警
#define  WARN_NOMONEY     0x00010       //  bit 4  欠费报警
#define  WARN_FORBIDMOVE  0x00020       //  bit 5   非法位移报警
#define  WARN_SHAKE       0x00040       //  bit 6    震动报警
#define  WARN_EXPSPD      0x00080       //  bit 7   超时报警 


//  Woking Status  Bits      //  工作状态
#define  STATUS_ACC         0x00001       //  bit 0     ACC            1:on  0:0ff 
#define  STATUS_LOWVOLT     0x00002       //  bit 1     欠压    1: on
#define  STATUS_SHAKING     0x00004       //  bit 2     震动中  1: on
#define  STATUS_RELAY       0x00008       //  bit 3     继电器   1: 断开
#define  STATUS_BATTERY     0x00010       //  bit 4     电池供电   1: on
#define  STATUS_EMERGBUTTON 0x00020       //  bit 5     紧急报警按下  (2  次)
#define  STATUS_SPDWARN     0x00040       //  bit  6    超速报警
#define  STATUS_IlLEGALMOV  0x00080       //  bit  7    非法位移报警

//  ACC  action
#define   AccAction_IDLE        0
#define   AccAction_Off2On      1
#define   AccAction_On2Off      2 




typedef struct  _WORK  
{
  //  position
  POS  position;  

  //  GSM
  u8   CSQ_value;	//	csq 信号强度
  u8   GPRS_State;  //   1: GPRS  online    0:  SMS state
  
  u32  GPRS_Duration_Counter;   //   发送计时器
  u16  GPRS_Duration_DoorValue; //    发送门限值
  u8   GPRS_Send_Flag;  //  发送位置信息标志位


  
  // work  sate 
  char	first_power;  // 首次上电状态监测 0: 外部供电  1 : 电池供电  
  u16   Status_Word;    /*  车辆状态     ACC              bit0: 1on  0:0ff    
                                                                欠压         bit1:   
                                                                震动         BIT2
                                                                继电器   BIT3
                                                                电子锁    BIT4
                                                                电池供电BIT5 
                                */
  u16	 Warn_Word; // 报警 方式	   
				    //  bit 0   紧急报警
					//  bit1   防盗报警
					//  bit 2  断电报警
					//  bit 3   低电压报警
					//  bit 4  欠费报警
					//  bit 5   非法位移报警
					//  bit 6    震动报警
					//  bit 7   超速报警 
  u8      Acc_change_Action;  //    0: ilde        1:  off2on     2 :  on2off 
					
  u8	 illegalMoveWarn; // 非法位移报警
  u8	 nomoneyWarn; //欠费报警
  u8     stolenWarn;  //  防盗报警
  u8     CutPowerWarn; //  断电报警
  u8     shakeWarn; //  震动报警
  u8     speedWarn;  // 超速报警
                                
  
  u8   Mode;        // 工作模式   Mode1   Mode2   Mode3
  u8   Mode3_Status_Change_Mode1;  //  模式3 情况下  ，外部状态发生变化 
  u8   Normal_Battery_working;  //  正常模式下电子工作
  u32  Mode2_working_counter_seconds;  // 模式2 工作状态计时器 
  u32  Mode1_from_mode3_no_SMS_and_call_counter; //     从mode3   进入 mode1  后，没有电话短信定时器
  
  u16  timer_counter;  //  定时器计数器   



}WORK;

extern WORK  work_var;  // 工作状态  



extern  void  Trker_Mode_service(void);
extern  void  Trker_Function_warnChecking(void);


//  Anti  theft  related  below   
extern void  Anti_theft_init(void);
extern void  Anti_theft_Enable_Executing(void);
extern void  Anti_theft_Disable_Executing(void);

extern void Trker_OutPowerChecking_after_BattteryOn_1st_timer(void);

