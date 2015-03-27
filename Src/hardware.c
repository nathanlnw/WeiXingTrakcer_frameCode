
#include "Hardware.h"
#include  "main.h"


GSM  gsm_module;  //  gsm 模块操作相关

// EmergenButton  related
static u16   EmergenButton_keep_counter=0; // 按键按下计数器
static u8    EmergenButton_count_in_1_second=0;  //   2 s  内按下 按键的次数
static u8    EmergenButton_Total_limit_counter=0;  //  要求在指定有效时间内如1s=10x100ms 完成按键


//  Shake keep   related
static u16   Shake_keep_counter=0;   //    震动有效后
static u16   Shake_confirm_times=0;   //  确定 震动的次数

//  Normal  Cut Out power related 
static u16   Normal_CutMainPower_counter=0;  // 正常电源供电情况下，切断电源检测

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
{  //  外部供电电压 ，管脚检测

     if("外部供电")
	 	 return true;
	 else
	 	 return false;
}

u8  Driver_ACC_Pin(void) 
{     //  电子锁状态


}


//   Part1:  输入状态检测
u8  Trker_HwDrv_InitpowerCheck(void)  // 1. 初始化外部电源检测
{
   if("监测到外部供电" )
       return  false;
   else
   	   return  true;
}

void  Trker_HwDrv_CutpowerCheck(void)  // 1. 外部供电检测
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


u8  Trker_HwDrv_AccCheck(void)   // 是不是就是ACC
{
    if("电子锁开")      // Acc  开
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
	   work_var.Mode3_Status_Change_Mode1=0;   //  clear    真正的是 Mode1
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
       
       if( work_var.Mode=MODE1)&&( work_var.Mode3_Status_Change_Mode1==0)     //  只有纯模式1 情况, 且不是从mode3 到mode1 情况下才 切换到模式2 
       {
           work_var.Mode=MODE2;
		   work_var.Mode2_working_counter_seconds=0; 
       }
	   
       return false;
     }	
}

void  Trker_HwDrv_shake_checking(void)
{
      if("震动")    
     {
       if(!(work_var.Status_Word&STATUS_SHAKING))  // 在未震动状态情况下
       {
	        Shake_confirm_times++;
			if(Shake_confirm_times>5)    // 多次确认震动才是震动震动触发
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
		  if(Shake_keep_counter>100)   // 保持10s
		  {  
		     Shake_keep_counter=0;
             work_var.Status_Word&=~STATUS_SHAKING;  //bit 1
		  }
	    }	       
     }	

}

void  Trker_HwDrv_EmergenButton_checking(void)   
{    //   紧急报警      in   100 ms  timer

   //  0.  没有开启防盗功能直接返回
   if(FALSE==SysConfig.Anti_theft_enable)
   	     return ;

   //  1.   开启防盗功能进行检测
	if"(WARN_StatusGet() I/O 管脚检测") 
	{
	   EmergenButton_keep_counter++;
	   if(EmergenButton_keep_counter>=2)		   
	  {
		  EmergenButton_keep_counter=0;
		  EmergenButton_count_in_1_second++;  // 按下几次检测
		  if(EmergenButton_count_in_1_second>=2)  // 大于等2 次
		  {
             //------
             if(Anti_theft.anti_working==0)  // 防盗状态是否执行中
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
		    EmergenButton_Total_limit_counter=0;  // 第一次按下时候 清除 计时器
	  }
	
	   //------------------------------------------------- 
	} 
	 else
	{		 
		 EmergenButton_keep_counter=0;	
	}	

    // 3.  limit counter
    EmergenButton_Total_limit_counter++;
	if(EmergenButton_Total_limit_counter>10)    // 1 s 内
		{
             EmergenButton_Total_limit_counter=0;
			 EmergenButton_count_in_1_second=0;     // 清除按键次数统计
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

 //Part2:  控制输出
void   Trker_HwDrv_Relay_Ctrl(u8  value) // 继电器输出
{
     if(value)
	 	"控制开启继电器"	 	
	 else
	 	"控制闭合闭继电器"
}

void   Trker_HwDrv_Speaker_Ctrl(u8  value) //  蜂鸣器控制
{
     if(value)
	 	"蜂鸣器响"	 	
	 else
	 	"蜂鸣器关闭"
}

void   Trker_HwDrv_gsmLED_Ctrl(u8  value) //  GSM 灯控制
{
     if(value)
	 	"gsm led  on"	 	
	 else
	 	"gsm led off"
}

void   Trker_HwDrv_gpsLED_Ctrl(u8  value) //  gps 灯控制
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
 

