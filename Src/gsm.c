/*
      Tracker  GSM  part         GPRS +  SMS  
*/
#include  "Hardware.h"

static  u8  GSM_Led_timer=0;  //   GPS �صƼ�ʱ��



void  GSM_init(void)
{


}


//-----------------GPRS  Part   ------------------------------
void Send_gprs(u8 *Str , u16 len )
{   //  ������Ϣ��Server


}

void  GPRS_part(void)
{    //   Default -----   ����


 


}
//---------------- SMS  Part  -------------------------------
void  Send_Msg(u8 *Str, u16 Str_len,u8 *dest)
{  //    �� dest ����  str_len  ���ȵ� Str ��Ϣ



}

void  Receive_Msg(void)
{
    if( content ==  "������������")
		   Anti_theft.rx_enable_anti_theft_sms=1��
    if( content ==  "ȡ�����������������")
		   Anti_theft_Disable_Executing();   


	
	if((work_var.Mode==MODE1)&&(work_var.Mode3_Status_Change_Mode1==1))
	{ 
		//	ģʽ1 �£����Ǵ�Mode3  ���������ģ� ����յ���Ϣ����ô�����ʱ
			work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;  
    }
}

void  Phone_Call(u8 *Str, u16 len)
{   //    ����绰����
     if((work_var.Mode==MODE1)&&(work_var.Mode3_Status_Change_Mode1==1))
	{ 
		//	ģʽ1 �£����Ǵ�Mode3  ���������ģ� ����յ����磬��ô�����ʱ
			work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;  
    }
}

void  SMS_part(void)
{    //  Default    ------�ر�





}

void  A_GPS_part(void)
{


}

void  GSM_Service(void)
{  
    switch(gsm_module.GSM_STATUS)
    {
		case  GSM_RAW://        0

		              break;
		case GSM_POWERON :  
			          break;   
		case   GSM_SIMcheck:

			          break;
		case   GSM_FEECheck:   //  Ƿ��״̬���

		              break;
		case   GSM_OFFLINE:      //  ��ʼ����ɵ�û����

		              break;
		case   GSM_Dial:    
			
			          break;
		case   GSM_ONline:    

                      break;
		default:
			          break;


    	}




}


void  GSM_Module_LED_timer(void)
{   //  100 ms   timer
   u8  dur=0;
    // ģʽ1 ��ģʽ2  �¿صȣ� ����ģʽ�¹رյ� ʡ��
   if((work_var.Mode==MODE1)||(work_var.Mode==MODE2))
   {      
      if(work_var.GPRS_State==1)  //  GPRS  ����
          dur=3;    // ���ƿ�
      else
          dur=5;    // ������ 
	                
	     if((GSM_Led_timer%dur)==0)
		 	 Trker_HwDrv_gsmLED_Ctrl(1);
		 else
		 	 Trker_HwDrv_gsmLED_Ctrl(0);	
		 
		 GSM_Led_timer++
		 if(GSM_Led_timer>5)
		 	        GSM_Led_timer=0;
   }
   else
   	   Trker_HwDrv_gsmLED_Ctrl(0);    
}



















