#include "main.h"

extern unsigned  Trker_HwDrv_InitpowerCheck(void);



#define   GSM_RAW        0
#define   GSM_POWERON    1
#define   GSM_SIMcheck   2
#define   GSM_FEECheck   3  //  Ƿ��״̬���
#define   GSM_OFFLINE    4  //  ��ʼ����ɵ�û����
#define   GSM_Dial       5
#define   GSM_ONline     6


typedef struct  _GSM
{
  u8 GSM_STATUS;  //   ��Դ���� 



}GSM;

extern GSM  gsm_module;  //  gsm ģ��������


// HwDrv  
extern void   Trker_HwDrv_gsmLED_Ctrl(u8  value) ;
extern void   Trker_HwDrv_gpsLED_Ctrl(u8  value);

#define LED_gsm_ON     Trker_HwDrv_gsmLED_Ctrl(1)
#define LED_gsm_OFF    Trker_HwDrv_gsmLED_Ctrl(0)
#define LED_gps_ON     Trker_HwDrv_gpsLED_Ctrl(1)
#define LED_gps_OFF    Trker_HwDrv_gpsLED_Ctrl(0) 



//   State  checking  
extern  void  Trker_HwDrv_Total_checking(void)��   //  ״̬���


