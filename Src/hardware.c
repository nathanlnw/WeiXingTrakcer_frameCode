
#include "Hardware.h"
#include  "main.h"


GSM  gsm_module;  //  gsm ģ��������

// EmergenButton  related
static u16   EmergenButton_keep_counter=0; // �������¼�����
static u8    EmergenButton_count_in_1_second=0;  //   2 s  �ڰ��� �����Ĵ���
static u8    EmergenButton_Total_limit_counter=0;  //  Ҫ����ָ����Чʱ������1s=10x100ms ��ɰ���


//  Shake keep   related
static u16   Shake_keep_counter=0;   //    ����Ч��
static u16   Shake_confirm_times=0;   //  ȷ�� �𶯵Ĵ���

//  Normal  Cut Out power related 
static u16   Normal_CutMainPower_counter=0;  // ������Դ��������£��жϵ�Դ���

void Hardware_init(u8 value)     //  value  0: basic  init      1:  init  later    
{
	 if(value==0)
	 {
		   // 0.   mcu frequence

		   // 1.  I/O  PIN  init 
	 }
		 
		   // 2. hard   timer  init 

		   // 3.  Trker_gps_init()
}


//  HardDrv    check    -------------------------------------------------------------------------------------------------
//   Chip Drivers
u16  Driver_AD_volt(void)
{    // chip   AD  check     get  Voltage




}

u8  Driver_OutPower_Pin(void)
{  //  �ⲿ�����ѹ ���ܽż��

     if("�ⲿ����")
	 	 return true;
	 else
	 	 return false;
}

u8  Driver_ACC_Pin(void) 
{     //  ������״̬


}


//   Part1:  ����״̬���
u8  Trker_HwDrv_InitpowerCheck(void)  // 1. ��ʼ���ⲿ��Դ���
{
   if("��⵽�ⲿ����" )
       return  false;
   else
   	   return  true;
}

void  Trker_HwDrv_CutpowerCheck(void)  // 1. �ⲿ������
{   // 100ms
   if(work_var.first_power)
 	 return;

   	
   if(Driver_OutPower_Pin) 
   	{
   	  if(!(work_var.Status_Word&STATUS_BATTERY))
   	  {
   	    Normal_CutMainPower_counter++;
		if(Normal_CutMainPower_counter>10);  //1s
		{
	   	    work_var.Normal_Battery_working=1;
		    work_var.Status_Word|=STATUS_BATTERY;  //bit 5
		    Normal_CutMainPower_counter=0;
		}    
   	  }
   	}
   else 
    {     
		 if(work_var.Status_Word&STATUS_BATTERY)
		 {
			 Normal_CutMainPower_counter++;
			if(Normal_CutMainPower_counter>10);  //1s
			{
		      work_var.Normal_Battery_working=0;
			  work_var.Status_Word&=~STATUS_BATTERY;  //bit 5       
			  Normal_CutMainPower_counter=0;
		    }  
			  
		 }
   	}
   
}


u8  Trker_HwDrv_AccCheck(void)   // �ǲ��Ǿ���ACC
{
    if("��������")      // Acc  ��
     {
       // checking action change
       if(!(work_var.Status_Word&STATUS_ACC))
       {
          work_var.Acc_change_Action=AccAction_Off2On;
          // send duration
          work_var.GPRS_Send_Flag=1;
		  work_var.GPRS_Duration_DoorValue=SysConfig.Mode1_Dur;
		  work_var.GPRS_Duration_Counter=0;
       }	  
	   
       work_var.Status_Word|=STATUS_ACC;  //bit 4
       work_var.Mode=MODE1;
	   work_var.Mode3_Status_Change_Mode1=0;   //  clear    �������� Mode1
       work_var.Mode2_working_counter_seconds=0;


	   
       return true;
     }
	else
	  {
        // checking action change
       if(work_var.Status_Word&STATUS_ACC)
       {
         work_var.Acc_change_Action=AccAction_On2Off;  
         // send duration
          work_var.GPRS_Send_Flag=1;
		  work_var.GPRS_Duration_DoorValue=SysConfig.Mode2_Dur;
		  work_var.GPRS_Duration_Counter=0;
		 
       }	 
	  
       work_var.Status_Word&=~STATUS_ACC;  //bit 4          
       
       if( work_var.Mode=MODE1)&&( work_var.Mode3_Status_Change_Mode1==0)     //  ֻ�д�ģʽ1 ���, �Ҳ��Ǵ�mode3 ��mode1 ����²� �л���ģʽ2 
       {
           work_var.Mode=MODE2;
		   work_var.Mode2_working_counter_seconds=0; 
       }
	   
       return false;
     }	
}

void  Trker_HwDrv_shake_checking(void)
{
      if("��")    
     {
       if(!(work_var.Status_Word&STATUS_SHAKING))  // ��δ��״̬�����
       {
	        Shake_confirm_times++;
			if(Shake_confirm_times>5)    // ���ȷ���𶯲������𶯴���
			{
		       work_var.Status_Word|=STATUS_SHAKING;  //bit 1
			   Shake_keep_counter=0;  // clear
			}   
       }		
     }
	else
	 {
	    if(work_var.Status_Word&STATUS_SHAKING)
	    {
	      Shake_keep_counter++;
		  if(Shake_keep_counter>100)   // ����10s
		  {  
		     Shake_keep_counter=0;
             work_var.Status_Word&=~STATUS_SHAKING;  //bit 1
		  }
	    }	       
     }	

}

void  Trker_HwDrv_EmergenButton_checking(void)   
{    //   ��������      in   100 ms  timer

   //  0.  û�п�����������ֱ�ӷ���
   if(FALSE==SysConfig.Anti_theft_enable)
   	     return ;

   //  1.   �����������ܽ��м��
	if"(WARN_StatusGet() I/O �ܽż��") 
	{
	   EmergenButton_keep_counter++;
	   if(EmergenButton_keep_counter>=2)		   
	  {
		  EmergenButton_keep_counter=0;
		  EmergenButton_count_in_1_second++;  // ���¼��μ��
		  if(EmergenButton_count_in_1_second>=2)  // ���ڵ�2 ��
		  {
             //------
             if(Anti_theft.anti_working==0)  // ����״̬�Ƿ�ִ����
			  {     // start warning
			     //Anti_theft.anti_working==1;
				 //Anti_theft.start_execut_warning=1;
				 //Anti_theft.anti_workStep=1; 
				 Anti_theft_Work_Start();   
              }
			 else
			  {     //  recover normal 
			     Anti_theft.anti_working=0;
				 Anti_theft.start_execut_warning=0;
				 Anti_theft_Disable_Executing(); 
			  }		  
             //-------
		  }
		  else
		    EmergenButton_Total_limit_counter=0;  // ��һ�ΰ���ʱ�� ��� ��ʱ��
	  }
	
	   //------------------------------------------------- 
	} 
	 else
	{		 
		 EmergenButton_keep_counter=0;	
	}	

    // 3.  limit counter
    EmergenButton_Total_limit_counter++;
	if(EmergenButton_Total_limit_counter>10)    // 1 s ��
		{
             EmergenButton_Total_limit_counter=0;
			 EmergenButton_count_in_1_second=0;     // �����������ͳ��
		}

    //------------------
}



u8   Trker_HwDrv_LowPower_Warning(void)
{
    if(Driver_AD_volt()<"3.7V")
	    return true;
	else
	     return false;	
}

 //Part2:  �������
void   Trker_HwDrv_Relay_Ctrl(u8  value) // �̵������
{
     if(value)
	 	"���ƿ����̵���"	 	
	 else
	 	"���Ʊպϱռ̵���"
}

void   Trker_HwDrv_Speaker_Ctrl(u8  value) //  ����������
{
     if(value)
	 	"��������"	 	
	 else
	 	"�������ر�"
}

void   Trker_HwDrv_gsmLED_Ctrl(u8  value) //  GSM �ƿ���
{
     if(value)
	 	"gsm led  on"	 	
	 else
	 	"gsm led off"
}

void   Trker_HwDrv_gpsLED_Ctrl(u8  value) //  gps �ƿ���
{    
	if(value)
	   "gps led  on"	   
	else
	   "gps led off"
    
}




//  gps    Part        ---------------------------------------------------------
void Trker_gps_init(void)
{ // gpio   usart  init



}
 

