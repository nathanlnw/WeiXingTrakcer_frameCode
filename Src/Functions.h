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
  u16  Version_ID;  //  ϵͳ�汾ID  

  u8   AdminCellphoneNum[20];  //  admin
  u8   UserCellphoneNum[20];  //  user   
  u8   ServerIP_main[4];    // main socket
  u16  ServerPort_main;  
  u8   ServerIP_aux[4];     // aux socket
  u16  ServerPort_aux;  
  u8   ServerIP_debug[4];  
  u16  ServerPort_debug;    // debug socket

  u16  Mode1_Dur;      //  ����ģʽ�µķ��ͼ��     10s
  u16  Mode2_Dur;      //  120s
  u16  Mode3_Dur;      //  90 min

  u16  Stolen_CutOil_Dur;  // ��������ģʽ�£����ж���·�ļ�� Ĭ��Ϊ0
  u8   speedwarn_Max;  //  ��������   KM/h       60 default 
  u16  illegalMove_dis; //  �Ƿ�λ��   200��
  u16  Speaker_warn_keep_Dur; //  ������ ����������ʱ��   5min=300s

  u8   Anti_theft_enable;   //  ��������  Ĭ�Ϲر�
  u8   auto2Anti_theft_enable; //  �Զ��л�������״̬ Ĭ��: �ر�
  u8   cutpowerWarn_enable;   // �ϵ籨��ʹ��  Ĭ�Ͽ���
  u8   lowVoltWarn_enable;  // Ƿѹ����  --����
  u8   nomoneyWarn_enable;  // Ƿ�ѱ��� --����  
  u8   illegalMove_enable;  // �Ƿ�λ�Ʊ���    ---����
  u8   abnormalMove_cutpower_enable;  //  �춯����� �ж�����  : ����
  u8   shakeWarn_enable;// �𶯱���:����  
  u8   speedWarn_enable; // ���ٱ���ʹ��  �ر�
  u8   gprs_work_enable;  //  gprs ����:����      
  u8   SMS_work_enable;   //   sms ���� : �ر� 
  
   u8   current_userstate; //    ��ǰ�û�״̬       ����Ա �� �û�
   u8   superPassword[20];  //  ��������Ա����
}SYS_CONFIG;


//  �����������
typedef struct _ANTI_TH
{
  u8   anti_working;   //  �Ƿ��ڷ���״̬��
  u8   start_execut_warning;  //��ʼִ�б���ָʾ     0: idle    1: executing     2:  execute  over  
  u8   anti_workStep;   //  0 : init
  u8   speaker_enable; //  ʹ�ܿ���speaker
  u32  speaker_run_counter; // speaktimer
  u8   rx_enable_anti_theft_sms;  //  ���յ���������ģʽ����Ϣ

  u8   cutoff_oil_delay_enable; // ʹ����ʱ�ж�
  u16  cutoff_oil_delay_counter;//  ʹ���ж϶�ʱ��



}ANTI_TH;

extern ANTI_TH  Anti_theft;  // �������� ����


extern  u8  DeviceID[30];  //  �ն�ID �����洢

extern  SYS_CONFIG   SysConfig;
extern  SYS_CONFIG   SysConfig_BAK1;
extern  SYS_CONFIG   SysConfig_BAK2;

extern void Trker_SysConfiguration(void);


// part2.  working   

#define MODE1         1
#define MODE2         2
#define MODE3         3

//  Warn   Status   Bits
#define  WARN_EMERG       0x00001       //  bit 0   ��������
#define  WARN_STOLEN      0x00002       //  bit1   ��������
#define  WARN_POWERCUT    0x00004       //  bit 2  �ϵ籨��
#define  WARN_LOWPOWER    0x00008       //  bit 3   �͵�ѹ����
#define  WARN_NOMONEY     0x00010       //  bit 4  Ƿ�ѱ���
#define  WARN_FORBIDMOVE  0x00020       //  bit 5   �Ƿ�λ�Ʊ���
#define  WARN_SHAKE       0x00040       //  bit 6    �𶯱���
#define  WARN_EXPSPD      0x00080       //  bit 7   ��ʱ���� 


//  Woking Status  Bits      //  ����״̬
#define  STATUS_ACC         0x00001       //  bit 0     ACC            1:on  0:0ff 
#define  STATUS_LOWVOLT     0x00002       //  bit 1     Ƿѹ    1: on
#define  STATUS_SHAKING     0x00004       //  bit 2     ����  1: on
#define  STATUS_RELAY       0x00008       //  bit 3     �̵���   1: �Ͽ�
#define  STATUS_BATTERY     0x00010       //  bit 4     ��ع���   1: on
#define  STATUS_EMERGBUTTON 0x00020       //  bit 5     ������������  (2  ��)
#define  STATUS_SPDWARN     0x00040       //  bit  6    ���ٱ���
#define  STATUS_IlLEGALMOV  0x00080       //  bit  7    �Ƿ�λ�Ʊ���

//  ACC  action
#define   AccAction_IDLE        0
#define   AccAction_Off2On      1
#define   AccAction_On2Off      2 




typedef struct  _WORK  
{
  //  position
  POS  position;  

  //  GSM
  u8   CSQ_value;	//	csq �ź�ǿ��
  u8   GPRS_State;  //   1: GPRS  online    0:  SMS state
  
  u32  GPRS_Duration_Counter;   //   ���ͼ�ʱ��
  u16  GPRS_Duration_DoorValue; //    ��������ֵ
  u8   GPRS_Send_Flag;  //  ����λ����Ϣ��־λ


  
  // work  sate 
  char	first_power;  // �״��ϵ�״̬��� 0: �ⲿ����  1 : ��ع���  
  u16   Status_Word;    /*  ����״̬     ACC              bit0: 1on  0:0ff    
                                                                Ƿѹ         bit1:   
                                                                ��         BIT2
                                                                �̵���   BIT3
                                                                ������    BIT4
                                                                ��ع���BIT5 
                                */
  u16	 Warn_Word; // ���� ��ʽ	   
				    //  bit 0   ��������
					//  bit1   ��������
					//  bit 2  �ϵ籨��
					//  bit 3   �͵�ѹ����
					//  bit 4  Ƿ�ѱ���
					//  bit 5   �Ƿ�λ�Ʊ���
					//  bit 6    �𶯱���
					//  bit 7   ���ٱ��� 
  u8      Acc_change_Action;  //    0: ilde        1:  off2on     2 :  on2off 
					
  u8	 illegalMoveWarn; // �Ƿ�λ�Ʊ���
  u8	 nomoneyWarn; //Ƿ�ѱ���
  u8     stolenWarn;  //  ��������
  u8     CutPowerWarn; //  �ϵ籨��
  u8     shakeWarn; //  �𶯱���
  u8     speedWarn;  // ���ٱ���
                                
  
  u8   Mode;        // ����ģʽ   Mode1   Mode2   Mode3
  u8   Mode3_Status_Change_Mode1;  //  ģʽ3 �����  ���ⲿ״̬�����仯 
  u8   Normal_Battery_working;  //  ����ģʽ�µ��ӹ���
  u32  Mode2_working_counter_seconds;  // ģʽ2 ����״̬��ʱ�� 
  u32  Mode1_from_mode3_no_SMS_and_call_counter; //     ��mode3   ���� mode1  ��û�е绰���Ŷ�ʱ��
  
  u16  timer_counter;  //  ��ʱ��������   



}WORK;

extern WORK  work_var;  // ����״̬  



extern  void  Trker_Mode_service(void);
extern  void  Trker_Function_warnChecking(void);


//  Anti  theft  related  below   
extern void  Anti_theft_init(void);
extern void  Anti_theft_Enable_Executing(void);
extern void  Anti_theft_Disable_Executing(void);

extern void Trker_OutPowerChecking_after_BattteryOn_1st_timer(void);

