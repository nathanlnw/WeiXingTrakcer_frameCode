

#include "main.h"
#include "gps.h"
#include "gsm.h" 


void Trker_delay(u32  value)
{
   u32  valuelocal=value;
   u32  sub_value=100;

   while(valuelocal)
   	{
       while(sub_value)
	   	     sub_value--;
       valuelocal--;
	   sub_value=100;
   	}
}

void Trker_timer_100ms(void) 
{
   work_var.timer_counter++;
   if(work_var.timer_counter>10)
   {  //  1 s  timer
      work_var.timer_counter=0;


	  
   }  
        // 报警处理
	    Trker_Function_warnChecking(); 
 
        //---  防盗报警Speaker  Timer 
        Anti_theft_Speaker_timer();
        //----   防盗报警控制切继电器
		Anti_theft_Relay_Contrl_timer();

		//---- gps led ----
		GPS_Module_LED_timer();
		//---- gsm led -----
		GSM_Module_LED_timer();

		//  模式检测切换相关
		Trker_Mode_related_timer();

		//  外部电池先供电的情况 ，检测外部电源供电状态 
        Trker_OutPowerChecking_after_BattteryOn_1st_timer(); 

		


}


void main(void)
{
  Hardware_init();
  
  // 上电外部电压检测外部
  Trker_delay(100);// 等待电压稳定
  work_var.first_power==Trker_HwDrv_InitpowerCheck();
  if(work_var.first_power)
  {
  	 while(1)
	  {
	      "recover"
          "关闭电省电，进入休眠";
		  "喂狗";
		  //----- 恢复正常工作---继续往下执行
		  if(work_var.first_power==0)
		  	break;
	  }     //  直接电池上电不干活
  }	  

  //-----  Normal   run  below  -----	 
  DF_init();
  Trker_SysConfiguration();   

  GPS_struct_init();
  GSM_init(); 
  
  // normal  
  while(1)
  	{

	   GSM_Service();
	   Trker_Mode_service();
	   GPS_service();

       
	   
  	}
}

