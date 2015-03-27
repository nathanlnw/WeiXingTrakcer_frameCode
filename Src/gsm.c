/*
      Tracker  GSM  part         GPRS +  SMS  
*/
#include  "Hardware.h"

static  u8  GSM_Led_timer=0;  //   GPS 控灯计时器



void  GSM_init(void)
{


}


//-----------------GPRS  Part   ------------------------------
void Send_gprs(u8 *Str , u16 len )
{   //  发送信息给Server


}

void  GPRS_part(void)
{    //   Default -----   开启


 


}
//---------------- SMS  Part  -------------------------------
void  Send_Msg(u8 *Str, u16 Str_len,u8 *dest)
{  //    给 dest 发送  str_len  长度的 Str 信息



}

void  Receive_Msg(void)
{
    if( content ==  "开启防盗命令")
		   Anti_theft.rx_enable_anti_theft_sms=1；
    if( content ==  "取消防盗命令防盗命令")
		   Anti_theft_Disable_Executing();   


	
	if((work_var.Mode==MODE1)&&(work_var.Mode3_Status_Change_Mode1==1))
	{ 
		//	模式1 下，且是从Mode3  触发上来的， 如果收到短息，那么清除计时
			work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;  
    }
}

void  Phone_Call(u8 *Str, u16 len)
{   //    拨打电话号码
     if((work_var.Mode==MODE1)&&(work_var.Mode3_Status_Change_Mode1==1))
	{ 
		//	模式1 下，且是从Mode3  触发上来的， 如果收到来电，那么清除计时
			work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;  
    }
}

void  SMS_part(void)
{    //  Default    ------关闭





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
		case   GSM_FEECheck:   //  欠费状态监测

		              break;
		case   GSM_OFFLINE:      //  初始化完成但没在线

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
    // 模式1 和模式2  下控等， 其他模式下关闭灯 省电
   if((work_var.Mode==MODE1)||(work_var.Mode==MODE2))
   {      
      if(work_var.GPRS_State==1)  //  GPRS  在线
          dur=3;    // 闪灯快
      else
          dur=5;    // 闪灯慢 
	                
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



















