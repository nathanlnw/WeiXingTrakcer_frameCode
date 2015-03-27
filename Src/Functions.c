//  Tracker  功能 实现相关
#include "main.h"
#include "gps.h"
#include "gsm.h"


// part1.   sys configuration
#define  SYSID      0x0011

u8 DeviceID[30];  //  终端ID 单独存储

SYS_CONFIG   SysConfig; 
SYS_CONFIG   SysConfig_BAK1;
SYS_CONFIG   SysConfig_BAK2;



// part2.  working   
WORK  work_var;  // 工作状态  
ANTI_TH  Anti_theft;  // 防盗功能 变量

static  u16  OutPowerchecking_underBATTon_counter=0;  

// part1.   sys configuration

/*
       系统配置信息写入
*/
u8  Trker_SysConfig_init(void)
{
	  
       //  1. Stuff
	                 //   系统版本
	      SysConfig.Version_ID=SYSID; 
         
		  SysConfig.AdminCellphoneNum[20]="8613012345678";	//	admin
		  SysConfig.UserCellphoneNum[20];  //  user   
		  SysConfig.ServerIP_main[4];	  // main socket
		  SysConfig.ServerPort_main;  
		  SysConfig.ServerIP_aux[4];	  // aux socket
		  SysConfig.ServerPort_aux;  
		  SysConfig.ServerIP_debug[4]={60,28,50,210};   // udp
		  SysConfig.ServerPort_debug=7850;	  // debug socket
		 
		  SysConfig.Mode1_Dur=10;	  //  工作模式下的发送间隔
		  SysConfig.Mode2_Dur=120;
		  SysConfig.Mode3_Dur=0;// 不发送数据
		 
		  SysConfig.Stolen_CutOil_Dur=0;  // 开启防盗模式下，到切断油路的间隔 默认为0
		  SysConfig.speedwarn_Max=60;  //  超速门限	 KM/h		60 default 
		  SysConfig.illegalMove_dis=200; //  非法位移   200米
		  SysConfig.Speaker_warn_keep_Dur=30; //  蜂鸣器 报警器持续时间   30s  spec  文档要求是30s

		   // Default  work state
			SysConfig.Anti_theft_enable=1;   //  防盗功能  默认开启
			SysConfig.auto2Anti_theft_enable=0; //  自动切换到防盗状态 默认: 关闭
			SysConfig.cutpowerWarn_enable=1;   // 断电报警使能  默认开启
			SysConfig.lowVoltWarn_enable=1;  // 欠压报警  --开启
			SysConfig.nomoneyWarn_enable=1;  // 欠费报警 --开启  
			SysConfig.illegalMove_enable=1;  // 非法位移报警    ---开启
			SysConfig.abnormalMove_cutpower_enable=1;  //  异动情况下 切断引擎  : 开启
			SysConfig.shakeWarn_enable=1;// 震动报警:开启  
			SysConfig.speedWarn_enable=0; // 超速报警使能  关闭
			SysConfig.gprs_work_enable=0;  //  gprs 功能:开启      
			SysConfig.SMS_work_enable=0;   //   sms 功能 : 关闭 
		 
		  SysConfig.current_userstate=0; //	当前用户状态	  0:  普通用户     1: 管理员
		  SysConfig.superPassword[20]="";  //  超级管理员密码
   //    2. Operate  Save
            return(Api_Config_write(config,ID_CONF_SYS,(u8*)&SysConfig,sizeof(SysConfig)));       

}

void  Trker_DeviceID_write( u8 *instr)
{

    Wirte_deviceid(DeviceID);
}

void  Trker_DeviceID_read(void)
{
    Read_deviceid(DeviceID);
}



void Trker_SysConfig_Read(void)
{
        if( Api_Config_read(config,ID_CONF_SYS,(u8*)&SysConfig,sizeof(SysConfig))==false)   //读取系统配置信息
                      rt_kprintf("\r\nConfig_ Read Error\r\n");   

			
 		memset((u8*)APN_String,0 ,sizeof(APN_String)); 
		memcpy((u8*)APN_String,SysConfig.APN_str,strlen((const char*)SysConfig.APN_str));  
	                        //   域名
		memset((u8*)DomainNameStr,0 ,sizeof(DomainNameStr));
		memcpy((u8*)DomainNameStr,SysConfig.DNSR,strlen((const char*)SysConfig.DNSR)); 
	                        //   域名aux
		memset((u8*)DomainNameStr_aux,0 ,sizeof(DomainNameStr_aux));
		memcpy((u8*)DomainNameStr_aux,SysConfig.DNSR_Aux,strlen((const char*)SysConfig.DNSR_Aux)); 

		
			 
	        //   主 IP   +  端口
	        memcpy((u8*)RemoteIP_main,SysConfig.IP_Main,4); 
	        RemotePort_main=SysConfig.Port_main;
	       //   备用 IP   +  端口
	        memcpy((u8*)RemoteIP_aux,SysConfig.IP_Aux,4); 
	        RemotePort_aux=SysConfig.Port_Aux;	 		
		   //  Link2  
	        memcpy((u8*)Remote_Link2_IP,SysConfig.Link2_IP,4); 
	        Remote_Link2_Port=SysConfig.Link2_Port;	 				 

	       //  传感器触发上报状态
	        TriggerSDsatus=SysConfig.TriggerSDsatus;
		//   ACC  on   off  设置
		ACC_on_sd_Duration=SysConfig.AccOn_Dur;
		ACC_off_sd_Duration=SysConfig.AccOff_Dur;              


}


void  Trker_FirstRun_Config_Write(void)
{     //   程序首次更新是写配置操作 


          //   写入终端ID  
          Trker_DeviceID_write( u8 *instr);         



		  //  写入默认配置信息
          Trker_SysConfig_init();   //  写入系统配置信息
 		  //---- add special -----------  
		  DF_WriteFlashSector(DF_LOGIIN_Flag_offset,0,&Login_Menu_Flag,1); 
}
//-----------------------------------------------------------------
void Trker_SetConfig(void)
{
	//u8 i=0;//,len_write=0;
//	u32 j=0;
	
       printf("\r\nSave Config\r\n");
	// 1.  读取config 操作      0 :成功    1 :  失败
	Api_Config_read(config,ID_CONF_SYS,(u8*)&SysConfig,sizeof(SysConfig));           
       //printf("\r\nRead Save SYSID\r\n");
       //  2. 读取成功  ，判断  版本ID 
	if(SysConfig.Version_ID!=SYSID)//SYSID)   //  check  wether need  update  or not 
	{
	       printf("\r\n ID not Equal   Saved==0x%X ,  Read==0x%X !\r\n",SYSID,SysConfig.Version_ID);	
	        SysConfig.Version_ID=SYSID;  // update  ID 
         //  2.1  播报使能
         
		 DF_WriteFlashSector(DF_WARN_PLAY_Page,0,&Warn_Play_controlBit,1); 
              
	     //  2.2  重新写入		 
           Trker_FirstRun_Config_Write();   // 里边更新了 SYSID           
		   vdr_erase(); 

	}
	else			
		   printf("\r\n Config Already Exist!\r\n"); 
}

 void Trker_ReadConfig(void) 
{
    u16   res[3];
	
   
             //   写入终端ID           很重要
          Trker_DeviceID_read( u8 *instr);      







		   DF_delay_ms(500);  

		  //  1.   read  read to  compare  judge
         //-------- JT808 参数配置读取测试，操作频繁而且重要所以需要特殊处理 
           DF_ReadFlash(JT808Start_offset, 0,(u8*)&SysConfig,sizeof(SysConfig)); 
		   DF_delay_ms(80); 	// large content delay	

		   DF_ReadFlash(JT808_BakSetting_offset, 0,(u8*)&SysConfig_BAK1,sizeof(SysConfig_BAK1)); 
		   DF_delay_ms(80); 	// large content delay	 

		   DF_ReadFlash(JT808_Bak2Setting_offset, 0,(u8*)&SysConfig_BAK2,sizeof(SysConfig_BAK2)); 
		   DF_delay_ms(80); 	// large content delay	

		   // 2. compare
		   /*
		           note:   res[0] == org cmp  bak    res[1]== bak  cmp  bak2    res[2]== bak2  cmp  org		  

		           ---org --<seg1>--  bak ---<seg2>----bak2 ---<seg3>---
		           |-----------<---------------<----------------------|
		    */
		   res[0]=memcmp((u8*)&SysConfig,(u8*)&SysConfig_BAK1,sizeof(SysConfig_BAK1));
		   res[1]=memcmp((u8*)&SysConfig_BAK1,(u8*)&SysConfig_BAK2,sizeof(SysConfig_BAK1));
		   res[2]=memcmp((u8*)&SysConfig_BAK2,(u8*)&SysConfig,sizeof(SysConfig_BAK1));

           // 3. judge 
           if(res[0]&&res[1]&&res[2])   // 全有问题
           	{
           	   printf("\r\n JT808 全部失败!  need all recover\r\n"); 	
	           JT808_Conf_init();	
           	}
		   else
		   if(res[0]&&res[1])   //    seg1  seg2  有问题说明  BAK error
		   	{    
		   	    // org  bak2 ---ok      bak---error
		   	   if((u8)(SysConfig.DURATION.Default_Dur>>24)!=0xFF) // 判断正确的是不是 FF
		   	   	{ 
		   	   	 
				  DF_WriteFlashSector(JT808_BakSetting_offset,0,(u8*)&SysConfig,sizeof(SysConfig_BAK1));
				  printf("\r\n JT808 BAK error ,correct ok"); 			

		   	   	}
			   else
			   	{ 
			   	  printf("\r\n need all recover 1"); 
				  JT808_Conf_init();
			   	}

		   	}
		   else
		   if(res[0]&&res[2])  //  seg1  seg3    有问题说明 BAK2  error
		   	{
		   	   // org  bak  ---ok       bak2 -----error
		   	   if((u8)(SysConfig.DURATION.Default_Dur>>24)!=0xFF) // 判断正确的是不是 FF
		   	   	{ 
		   	   	 
				  DF_WriteFlashSector(JT808_Bak2Setting_offset,0,(u8*)&SysConfig,sizeof(SysConfig_BAK1));
				  printf("\r\n JT808 BAK2 error ,correct ok"); 			

		   	   	}
			   else
			   	{ 
			   	  printf("\r\n need all recover 2"); 
				  JT808_Conf_init();
			   	}

		   	}
		   else
		   if(res[1]&&res[2])  //  seg2  seg3	 有问题说明 org  error
			{
			   //  bak  bak2 --ok     org---error
		         if((u8)(SysConfig_BAK1.DURATION.Default_Dur>>24)!=0xFF) // 判断正确的是不是 FF
		   	   	{ 
		   	   	 
				  DF_WriteFlashSector(JT808Start_offset,0,(u8*)&SysConfig_BAK1,sizeof(SysConfig_BAK1));
				  printf("\r\n JT808 org error ,correct ok"); 	 		

		   	   	}
			   else
			   	{ 
			   	  printf("\r\n need all recover 3"); 
				  JT808_Conf_init();
			   	}
		   
			} 
		   else
		   	printf("\r\n JT808 读取校验成功! \r\n"); 
         //-------------------------------------------------------------------------------------
		   
		   Trker_SysConfig_Read();  //读取系统配置信息	                 

                 
    printf("\r\n Read Config Over \r\n");   
}
void Trker_DefaultConfig(void)
{
   u32 DriveCode32=0;
   u8  reg_str[30],i=0;

       printf("\r\n         SYSTEM ID=0x%X ",SysConfig.Version_ID);   
	printf("\r\n		   设置的鉴权码为: ");
       printf(" %s\r\n		   鉴权码长度: %d\r\n",SysConfig.ConfirmCode,strlen((const char*)SysConfig.ConfirmCode));   
					   
  if(SysConfig.Regsiter_Status)
  	   printf("\r\n		   该终端已经注册过!    %d \r\n",SysConfig.Regsiter_Status);  
  else
  	   printf("\r\n		   该终端被尚未被注册!\r\n");   
  
  
	  printf("\r\n中心控制断电= %d (1 : 断电  0 : 正常)\r\n", SysConfig.relay_flag);     
   if(SysConfig.relay_flag==1)
		  {
	  Enable_Relay();
	   Car_Status[2]|=0x08; 	// 需要控制继电器
	   }
  else
	  {
	  Disable_Relay();
	  Car_Status[2]&=~0x08;    // 需要控制继电器
	  printf("\r\n继电器闭合");
	  }
        
        // APN 设置
	     printf("\r\n		   APN 设置 :%s	 \r\n",APN_String); 
         DataLink_APN_Set(APN_String,1); 
            //     域名
          memset(reg_str,0,sizeof(reg_str));
          memcpy(reg_str,DomainNameStr,strlen((char const*)DomainNameStr));
          printf("\r\n		  域名设置 :	 %s\r\n",reg_str); 
          KorH_check(); 
		 
		  
	      //    域名aux
          memset(reg_str,0,sizeof(reg_str));
          memcpy(reg_str,DomainNameStr_aux,strlen((char const*)DomainNameStr_aux));
          printf("\r\n		aux  域名设置 :	 %s\r\n",reg_str); 	    
					  
         // 数据中心IP 地址(4Bytes)  UDP端口号码(2Bytes) TCP端口号码(2Bytes)
         printf("\r\n		  主IP: %d.%d.%d.%d : %d \r\n",RemoteIP_main[0],RemoteIP_main[1],RemoteIP_main[2],RemoteIP_main[3],RemotePort_main);   
	  DataLink_MainSocket_set(RemoteIP_main, RemotePort_main,0);
	//  printf("\r\n		   备用IP: %d.%d.%d.%d : %d \r\n",RemoteIP_aux[0],RemoteIP_aux[1],RemoteIP_aux[2],RemoteIP_aux[3],RemotePort_aux);   
         DataLink_AuxSocket_set(RemoteIP_aux, RemotePort_main,0);

	 // printf("\r\n		   IC 卡IP: %d.%d.%d.%d  TCP: %d  UDP:%d\r\n",SysConfig.BD_IC_main_IP[0],SysConfig.BD_IC_main_IP[1],SysConfig.BD_IC_main_IP[2],SysConfig.BD_IC_main_IP[3],SysConfig.BD_IC_TCP_port,SysConfig.BD_IC_UDP_port);   
	 // printf("\r\n		   IC 卡备用IP: %d.%d.%d.%d \r\n",SysConfig.BD_IC_Aux_IP[0],SysConfig.BD_IC_Aux_IP[1],SysConfig.BD_IC_Aux_IP[2],SysConfig.BD_IC_Aux_IP[3]);   
	  DataLink_IC_Socket_set(SysConfig.BD_IC_main_IP,SysConfig.BD_IC_TCP_port,0); 
	  	 
	  //  ACC On 上报间隔(2Bytes)  ACC Off 上报间隔(2Bytes)
	  printf("\r\n		   ACC on 发送间隔为: %d S\r\n		   ACC Off 发送间隔为: %d S\r\n",ACC_on_sd_Duration,ACC_off_sd_Duration);
	  printf("\r\n  播放状态: 0x%2X 最小休息时间: %d s   疲劳驾驶门限: %d s  \r\n",Warn_Play_controlBit,TiredConf_struct.TiredDoor.Door_MinSleepSec,TiredConf_struct.TiredDoor.Door_DrvKeepingSec);  		  
	  
          //  超长停车报警(2Bytes)	
	//  printf("\r\n		   超长停车报警: %d S\r\n",StopLongDuration);
	   //  定距离回传使能标志位(1Bytes) ， 定距离上传距离(4Bytes)  单位: m 米  
         printf("\r\n         监听号码: %s \r\n",SysConfig.LISTEN_Num);    


	   
          //   记录仪唯一ID
         printf("\r\n		   车辆唯一性编号: %35s \r\n",SysConfig.DeviceOnlyID);    
         printf("\r\n		   上报速度获取方式: %d :",SysConfig.Speed_GetType);	  
         if(SysConfig.Speed_GetType)							
                printf("车速通过车辆速度传感器获取!\r\n");
         else
                printf("车速通过GPS模块获取!\r\n"); 

         printf("\r\n		   特征系数校准状态: %d :",SysConfig.DF_K_adjustState);	    
         if(SysConfig.DF_K_adjustState)					  	 
                  printf(" 特征系数--校准成功!\r\n"); 
         else
                  printf("特征系数--尚未校准!\r\n");   

	  //   里程
		  SysConfig.DayStartDistance_32=DayStartDistance_32;
		  SysConfig.Distance_m_u32=Distance_m_u32;		
         printf("\r\n		   累计里程: %d  米   ,  当日里程:   %d米\r\n",SysConfig.Distance_m_u32,SysConfig.Distance_m_u32-SysConfig.DayStartDistance_32);  	
         //  速度限制
         printf("		   允许最大速度: %d  Km/h    超速报警持续时间门限: %d  s \r\n", SysConfig.Speed_warn_MAX,SysConfig.Spd_Exd_LimitSeconds);  		 

     //    printf("\r\n		  国家标准版本: %14s \r\n",SysConfig.StdVersion.stdverStr);					  
       //  printf("\r\n		  国家标准版本修改单号: %d \r\n",SysConfig.StdVersion.MdfyID); 


         printf("\r\n		  特征系数(速度脉冲系数): %d \r\n",SysConfig.Vech_Character_Value); 					  
         printf("\r\n		  初次安装日期: %X-%X-%X %X:%X:%X \r\n",SysConfig.FirstSetupDate[0],SysConfig.FirstSetupDate[1],SysConfig.FirstSetupDate[2],SysConfig.FirstSetupDate[3],SysConfig.FirstSetupDate[4],SysConfig.FirstSetupDate[5]);  


         DriveCode32=(SysConfig.Driver_Info.Effective_Date[0]<<16)+(SysConfig.Driver_Info.Effective_Date[1]<<8)+SysConfig.Driver_Info.Effective_Date[2];
         printf("\r\n		  驾驶员代码: %d \r\n",DriveCode32);  					  
         printf("\r\n		  机动车驾驶证号: %18s \r\n",SysConfig.Driver_Info.DriverCard_ID);  					  
         printf("\r\n		  驾驶员姓名: %s \r\n",SysConfig.Driver_Info.DriveName); 					  
         printf("\r\n		  驾驶员从业资格证: %20s \r\n",SysConfig.Driver_Info.Drv_CareerID); 
         printf("\r\n		  发证机构: %s \r\n",SysConfig.Driver_Info.Comfirm_agentID);   



         printf("\r\n		  车辆VIN号: %17s \r\n",Vechicle_Info.Vech_VIN);   
         printf("\r\n		  车牌号码: %12s \r\n",Vechicle_Info.Vech_Num);  
         printf("\r\n		  车牌分类: %12s \r\n",Vechicle_Info.Vech_Type);  
         printf("\r\n        车辆所在省ID: %d \r\n",Vechicle_Info.Dev_ProvinceID);
         printf("\r\n        车辆所在市ID: %d \r\n",Vechicle_Info.Dev_CityID); 
         printf("\r\n        车辆颜色:   JT415    1  蓝 2 黄 3 黑 4 白 9其他----当前颜色 %d \r\n",Vechicle_Info.Dev_Color);  


         printf("\r\n        触发上报传感器为  TriggerSDsatus=%X    \r\n",TriggerSDsatus);   
         printf("\r\n        Max_picNum =  %d   Max_CycleNum = %d   Max_DrvRcdNum=%d \r\n",Max_PicNum,Max_CycleNum,Max_RecoderNum); 

         printf("\r\n\r\n	  车辆负载状态: ");
         switch(SysConfig.LOAD_STATE)
		{  
		case 1:
		   printf("空车\r\n"); 
		  break;
		case 2:
		   printf("半载\r\n"); 
		  break;		  
		case 3:
		   printf("满载\r\n"); 
		  break;
		default:
		       SysConfig.LOAD_STATE=1;
		       Api_Config_Recwrite_Large(jt808,0,(u8*)&SysConfig,sizeof(SysConfig));
		   printf("空车2\r\n");  
		  break;
		}
         printf("\r\n\r\n  起始流水号: %d \r\n", SysConfig.Msg_Float_ID); 
	     printf("\r\n\r\n             cyc_read:   %d ,     cyc_write :%d\r\n  \r\n",cycle_read,cycle_write);     		
         
         //=====================================================================
         //API_List_Directories();
         //-----------  北斗模块相关  ---------
	     BD_list(); 

		 // ----    首次连接类型  --------------------
		 if(Vechicle_Info.Link_Frist_Mode==1)  
		 	{
                 printf("\r\n\r\n   首次连接模式:   MainLink");    
		 	}
		 else
		 	{
                  printf("\r\n\r\n   首次连接模式:  DNSR域名");     
		 	}

		if(DeviceNumberID[0]==0xFF)
			printf("\r\n  =======> 尚未设置设备编号，请重新设置\r\n" );  
		else
			{
			printf("\r\n 读取终端ID为 : "); 
			for(i=0;i<12;i++)
				printf("%c",DeviceNumberID[i]);
			printf("\r\n");
			}  
       //---------- SIM ID -----------------------------  
        	if(SimID_12D[0]==0xFF)
			printf("\r\n  =======> 尚未设置SIMID_入网编号，请重新设置\r\n" );  
		else
			{
			printf("\r\n 读取设备SIMID 入网ID 为 : "); 
			for(i=0;i<12;i++)
				printf("%c",SimID_12D[i]);
			printf("\r\n");
			} 
	  	// 短息中心号码-----------------------------
	  	// printf("\r\n		   短息中心号码 :%s	 \r\n",SysConfig.SMS_RXNum);

		 // ---  硬件版本信息-------------
		  HardWareVerion=HardWareGet();		 
		  printf("\r\n		        -------硬件版本:%X        B : %d %d %d\r\n",HardWareVerion,(HardWareVerion>>2)&0x01,(HardWareVerion>>1)&0x01,(HardWareVerion&0x01));   
		  if(HardWareVerion==3) 
		  	  printf("\r\n		       车台型号: TW705   \r\n"); 

          //------- 自动接听方式 -----------           
		  printf("\r\n		        -------自动接听方式:%d    \r\n",SysConfig.Auto_ATA_flag);      

}

void Trker_SysConfiguration(void)
{
		Trker_SetConfig();  
		Trker_ReadConfig();
		Trker_DefaultConfig();	 
}


//-------------------------------------------------------------------------------------------------------------------------------
//                                                                              BLANK
//-------------------------------------------------------------------------------------------------------------------------------



//  application    related    
/*
Three mode of operating modes:
+Mode 1:   Turn on vehicle's eletric lock. Device send regular messages to server. Default frequency= 10sec.
+ Mode 2:  Turn off vehicle's eletric lock. Device send regular messages to server with default frequency = 120sec.
+ Mode 3:  After device works in Mode 2 for more than 90mins（Viesleept）, it will automatically setup both GPRS and GPS Module at Standby mode (do not send mesages).If there are any changes in the status of the vehicle, the device will activate the operating mode under Mode 1. If there is no vibration, SMS and call in 10minues, it will return to Mode3. If user turns on ACC, device will run into Mode1 immediately.  

*/

void Trker_Mode1_service(void)
{
	 if(work_var.Mode!=MODE1)
	 	 return;
	 
     if(work_var.GPRS_State==1)
     {

     }

}
void Trker_Mode2_service(void)
{

}

void Trker_Mode3_service(void)
{


}

void  Trker_Mode_service(void)
{
   // 不同模式下 区分处理
  switch(work_var.Mode)
  	{
  	   case MODE1:
                  Trker_Mode1_service();
		          break;
	   case  MODE2:
                  Trker_Mode2_service();
                  break;
	   case  MODE3:
                  Trker_Mode3_service();
                  break;
	   default:
	   	          break;
  	}
   //  共用处理方式 	   



}


void Trker_Mode_convert(void)
{  // 工作模式切换
    
	// 0 . 电子锁检测 ACC
	Trker_HwDrv_AccCheck();  // 电子锁检测

	  /*
	     Mode3: If there are any changes in the status of the vehicle, the device will activate the operating mode under Mode 1
	  */
	  if(work_var.Mode==MODE3)
	  { 
	      //  模式3  下， 如果车辆发生变化
		   if(work_var.Status_Word&STATUS_SHAKING)  
		   {  
			  work_var.Mode3_Status_Change_Mode1=1;
			  work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;  
			  work_var.work_var.Mode2_working_counter_seconds=0;
			  work_var.Mode=MODE1;
		   
		   }
	  }

}

void  Trker_Mode_related_timer(void)
{     //----------- 100ms  
     if((work_var.Mode==MODE1)||(work_var.Mode==MODE2))
     { //    在模式1  和模式下按照指定间隔发送数据   ,  Mode3   不发送数据
     			   work_var.GPRS_Duration_Counter++;
					if(work_var.GPRS_Duration_Counter>=(work_var.GPRS_Duration_DoorValue*10))
					{
                       work_var.GPRS_Duration_Counter=0;
					   work_var.GPRS_Send_Flag=1;
					}

     }

	 //-----------------------------------------------------------------------
	 switch(work_var.Mode)
	  {
		 case MODE1:
                    if(work_var.Mode3_Status_Change_Mode1==1)
                    {  //   从mode3  激活的情况下，累计计算没有电话和短息的时间
                       work_var.Mode1_from_mode3_no_SMS_and_call_counter++;
					   if(work_var.Mode1_from_mode3_no_SMS_and_call_counter>=600*10) //10分钟
					   	{
					   	  work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;
						  work_var.Mode2_working_counter_seconds=0;
                          work_var.Mode=MODE3;
					   	}
 
                    }
					break;
		 case  MODE2:   //  进入MOD2  情况下，
		 	        work_var.Mode2_working_counter_seconds++;
					if(work_var.Mode2_working_counter_seconds>=54000) //  90 分钟=5400*10
					{
					  work_var.Mode2_working_counter_seconds=0;
                      work_var.Mode=MODE3;
					}
					break;
		 case  MODE3:
					Trker_Mode3_service();
					break;
		 default:
					break;
	  }
  

     //------ convert mode  according to ACC sates
     Trker_Mode_convert();

}

void Trker_Function_warnChecking(void)
{    //  Tracker     功能  ， 报警状态检测
    
    //  0. 防盗报警-- 关闭
    /*    Description of  this:         
	   When vehicle is robbed/stolen, users can call/sms/gprs to the device or press SOS button (panic button, double times press to cause), device will:
	   - whistle SPEAKERT minutes (default 30 seconds).
	   - send SMS with position to user, send T6 message to server and call user.
	   - If no vibration, no need to cut off fuel supply.
		 If there is vibration and acc off, no need cut off relay. 
		 If there is vibration and ACC on, after TIMETORELAY seconds (default 0s) cut off fuel supply.
	   So that vehicle will move in inertia , robber cannot start the engine again.
       */
    if(ENABLE==SysConfig.Anti_theft_enable)
    {                                       //  防盗功能开启情况下
       // or vibration alarm or        
	   if((work_var.Status_Word&STATUS_SHAKING)&&((work_var.Status_Word&STATUS_ACC)==0x0000))
	   	{  //       ACC 关 且 有震动
	   	   Anti_theft_Work_Start();
	   	}
	   //illegal ACC happens (In anti-theft lock, turn on ACC. 
	   if((Anti_theft.anti_working==1)&&(Anti_theft.start_execut_warning==2)&&( work_var.Acc_change_Action==AccAction_Off2On))
	   	{  //   处于防盗模式下，且ACC  从off  to   ON   ,那么开始报警
	   	   Anti_theft_Work_Start();
           work_var.Acc_change_Action=AccAction_IDLE; // clear state
	   	}    
	   //Or When ACC on, activate anti-theft lock, by SMS(lock), Call from user, [,S12] message),
	   if((work_var.Status_Word&STATUS_ACC)&&(Anti_theft.rx_enable_anti_theft_sms))
	   	{  //  ACC on   且 收到  开启防盗短信息
           Anti_theft_Work_Start();
           Anti_theft.rx_enable_anti_theft_sms=0; // clear
	   	}

    }
    // 1. 自动切换到防盗功能--关闭
     if(ENABLE==SysConfig.auto2Anti_theft_enable)
     {                                    //  自动切换到防盗功能 
      
     }
	// 2. 断电报警  ---- 开启
	if(ENABLE==SysConfig.cutpowerWarn_enable)
     {   
        
		//	外部断电电源检测
		Trker_HwDrv_CutpowerCheck();	 
     }
	
	// 3. 欠压报警----开启
	if(ENABLE==SysConfig.lowVoltWarn_enable)
     {                                     
	    Trker_HwDrv_LowPower_Warning();
     }
	

	// 4.欠费报警---开启
	if(ENABLE==SysConfig.nomoneyWarn_enable)
     {                                   
       ;
     }
	
	
	// 5.非法位移报警---开启
	if(ENABLE==SysConfig.illegalMove_enable)
     {                                    
        GPS_Illegal_Movement_checking();
     }

	
	// 6. 震动报警---开启
	if(ENABLE==SysConfig.shakeWarn_enable)
     {          
		//	 传感器检测
		Trker_HwDrv_shake_checking();	  //   震动传感器
     }

	// 7. 超速报警---关闭
	if(ENABLE==SysConfig.speedWarn_enable)
     {       
		//	超速报警检测
		GPS_Speed_Warn_Checking(); 
     }

    // 8.Allow remote shutdown engine. Default ON.	  运动中允许切断引擎报警---开启
    // 在  Anti_theft_Relay_Contrl_timer  函数中有处理了 

	//  9 . 电话报警  --拨打电话


    // 10. 紧急报警按下   紧急报警按键处理
	  Trker_HwDrv_EmergenButton_checking();  
   
}


void Trker_Function_Recover_Defalut(void)
{
   //  恢复出厂设置




}

void  Trker_Device_Reset(u8  value)
{  //  终端复位

    if(value)
        "终端复位" 
	    
  
}

//  Anti  theft  related  below   
void  Anti_theft_init(void)
{
   Anti_theft.anti_working=0;
   Anti_theft.start_execut_warning=0;
   Anti_theft.anti_workStep=0;   //  0 : init
   Anti_theft.speaker_enable=0; //  使能控制speaker
   Anti_theft.speaker_run_counter=0; // speaktimer
   Anti_theft.rx_enable_anti_theft_sms=0;  //  接收到开启防盗模式短信息

   Anti_theft.cutoff_oil_delay_enable=0; // 使能延时切断
   Anti_theft.cutoff_oil_delay_counter=0;//  使能切断定时器
}

void Anti_theft_Relay_Contrl_timer(void)
{
  if(0==Anti_theft.cutoff_oil_delay_enable)
  	return;

  Anti_theft.cutoff_oil_delay_counter++;  
  if(Anti_theft.cutoff_oil_delay_counter>=(SysConfig.Stolen_CutOil_Dur *10))
  	{
       Anti_theft.cutoff_oil_delay_counter=0;
       Anti_theft.cutoff_oil_delay_enable=0;// clear
       //-------------------------------
      if(gps_pos.speed_KmH>5) // 如果再运动中，则检测是否允许运动中切断引擎
      {
		   // 8.Allow remote shutdown engine. Default ON.	  运动中允许切断引擎报警---开启
		   if(ENABLE==SysConfig.abnormalMove_cutpower_enable)
			{									
			    Trker_HwDrv_Relay_Ctrl(1); 
			}
		  
      }
	   else
	          Trker_HwDrv_Relay_Ctrl(1);    // 没在运动中
	  
  	}

}

void Anti_theft_Speaker_timer(void)
{                                      //   100 ms  timer
   if(0==Anti_theft.speaker_enable)
   	 return;
   
   Anti_theft.speaker_run_counter++;
   if(Anti_theft.speaker_run_counter<(SysConfig.Speaker_warn_keep_Dur*10))
   	{
   	   if(Anti_theft.speaker_run_counter%3==0)  // 响1  停 2 
            Trker_HwDrv_Speaker_Ctrl(1);
	   else
	   	    Trker_HwDrv_Speaker_Ctrl(0);
   	}
   else
   	   Trker_HwDrv_Speaker_Ctrl(0);
}
void Anti_theft_Work_Start(void)
{
	Anti_theft.anti_working==1;
	Anti_theft.start_execut_warning=1;
	Anti_theft.anti_workStep=1; 
	Anti_theft.speaker_enable=1;  // 使能蜂鸣器响
}

void Anti_theft_Enable_Executing(void)
{  //   执行 防盗报警下的 操作
   /*
   When vehicle is robbed/stolen, users can call/sms/gprs to the device or press SOS button (panic button, double times press to cause), device will:
- whistle SPEAKERT minutes (default 30 seconds).
- send SMS with position to user, send T6 message to server and call user.
- If no vibration, no need to cut off fuel supply.
  If there is vibration and acc off, no need cut off relay. 
  If there is vibration and ACC on, after TIMETORELAY seconds (default 0s) cut off fuel supply.
  So that vehicle will move in inertia , robber cannot start the engine again.
    */
   if((Anti_theft.anti_working==1)&&(Anti_theft.start_execut_warning==1))
   {
     //---------------------------------------------------
     switch(Anti_theft.anti_workStep)
     {
       case 1:
            // 1. Send position SMS to User
            Send_Msg(PosInfo,infolen,SysConfig.UserCellphoneNum);
			Anti_theft.anti_workStep++;
	          break;
	   case 2:		  
	        // 2.  Send T6 msg to server
	        Send_gprs(T6msg,len);
			Anti_theft.anti_workStep++;
			  break;
	   case 3:		
	        // 3.  call  user 
            Phone_Call(SysConfig.UserCellphoneNum,strlen(SysConfig.UserCellphoneNum));
			Anti_theft.anti_workStep++;

			Anti_theft.start_execut_warning=2;  // excuting  over
			break;
	   default:
	   	        break;
			
     }
     /*     
	 - If no vibration, no need to cut off fuel supply.
	   If there is vibration and acc off, no need cut off relay. 
	   If there is vibration and ACC on, after TIMETORELAY seconds (default 0s) cut off fuel supply.
	  */
     if((work_var.Status_Word&STATUS_SHAKING)&&(work_var.Status_Word&STATUS_ACC))
     	{
     	     Anti_theft.cutoff_oil_delay_enable=1;
			 Anti_theft.cutoff_oil_delay_counter=0;
             //Trker_HwDrv_Relay_Ctrl(1); 
     	}
	 	
   }
}

void Anti_theft_Disable_Executing(void)
{   //  还原正常  



   Trker_HwDrv_Relay_Ctrl(0);  // recover oil

   Trker_HwDrv_Speaker_Ctrl(0);
   Anti_theft.speaker_enable=0;
   Anti_theft.speaker_run_counter=0;   	
   Anti_theft.rx_enable_anti_theft_sms=0;
}

void Trker_OutPowerChecking_after_BattteryOn_1st_timer(void)
{   //  100ms                    电池先开电工作的状态下  检测外部电源状态
  if(work_var.first_power)
  {
    if(Trker_HwDrv_InitpowerCheck())
	{
	  OutPowerchecking_underBATTon_counter++;
	  if(OutPowerchecking_underBATTon_counter>10)  //1s 以上
	  	{
           OutPowerchecking_underBATTon_counter=0;
		   work_var.first_power=0;  // 外部供电使能
	  	}
    }  

  }
  
}
