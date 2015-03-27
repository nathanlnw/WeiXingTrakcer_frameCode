#include "main.h"

extern unsigned  Trker_HwDrv_InitpowerCheck(void);



#define   GSM_RAW        0
#define   GSM_POWERON    1
#define   GSM_SIMcheck   2
#define   GSM_FEECheck   3  //  欠费状态监测
#define   GSM_OFFLINE    4  //  初始化完成但没在线
#define   GSM_Dial       5
#define   GSM_ONline     6


typedef struct  _GSM
{
  u8 GSM_STATUS;  //   电源开启 



}GSM;

extern GSM  gsm_module;  //  gsm 模块操作相关


// HwDrv  
extern void   Trker_HwDrv_gsmLED_Ctrl(u8  value) ;
extern void   Trker_HwDrv_gpsLED_Ctrl(u8  value);

#define LED_gsm_ON     Trker_HwDrv_gsmLED_Ctrl(1)
#define LED_gsm_OFF    Trker_HwDrv_gsmLED_Ctrl(0)
#define LED_gps_ON     Trker_HwDrv_gpsLED_Ctrl(1)
#define LED_gps_OFF    Trker_HwDrv_gpsLED_Ctrl(0) 



//   State  checking  
extern  void  Trker_HwDrv_Total_checking(void)；   //  状态检测


