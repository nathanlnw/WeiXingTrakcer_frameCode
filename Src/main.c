

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
        // ��������
	    Trker_Function_warnChecking(); 
 
        //---  ��������Speaker  Timer 
        Anti_theft_Speaker_timer();
        //----   �������������м̵���
		Anti_theft_Relay_Contrl_timer();

		//---- gps led ----
		GPS_Module_LED_timer();
		//---- gsm led -----
		GSM_Module_LED_timer();

		//  ģʽ����л����
		Trker_Mode_related_timer();

		//  �ⲿ����ȹ������� ������ⲿ��Դ����״̬ 
        Trker_OutPowerChecking_after_BattteryOn_1st_timer(); 

		


}


void main(void)
{
  Hardware_init();
  
  // �ϵ��ⲿ��ѹ����ⲿ
  Trker_delay(100);// �ȴ���ѹ�ȶ�
  work_var.first_power==Trker_HwDrv_InitpowerCheck();
  if(work_var.first_power)
  {
  	 while(1)
	  {
	      "recover"
          "�رյ�ʡ�磬��������";
		  "ι��";
		  //----- �ָ���������---��������ִ��
		  if(work_var.first_power==0)
		  	break;
	  }     //  ֱ�ӵ���ϵ粻�ɻ�
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

