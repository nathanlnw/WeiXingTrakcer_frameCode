//  Tracker  ���� ʵ�����
#include "main.h"
#include "gps.h"
#include "gsm.h"


// part1.   sys configuration
#define  SYSID      0x0011

u8 DeviceID[30];  //  �ն�ID �����洢

SYS_CONFIG   SysConfig; 
SYS_CONFIG   SysConfig_BAK1;
SYS_CONFIG   SysConfig_BAK2;



// part2.  working   
WORK  work_var;  // ����״̬  
ANTI_TH  Anti_theft;  // �������� ����

static  u16  OutPowerchecking_underBATTon_counter=0;  

// part1.   sys configuration

/*
       ϵͳ������Ϣд��
*/
u8  Trker_SysConfig_init(void)
{
	  
       //  1. Stuff
	                 //   ϵͳ�汾
	      SysConfig.Version_ID=SYSID; 
         
		  SysConfig.AdminCellphoneNum[20]="8613012345678";	//	admin
		  SysConfig.UserCellphoneNum[20];  //  user   
		  SysConfig.ServerIP_main[4];	  // main socket
		  SysConfig.ServerPort_main;  
		  SysConfig.ServerIP_aux[4];	  // aux socket
		  SysConfig.ServerPort_aux;  
		  SysConfig.ServerIP_debug[4]={60,28,50,210};   // udp
		  SysConfig.ServerPort_debug=7850;	  // debug socket
		 
		  SysConfig.Mode1_Dur=10;	  //  ����ģʽ�µķ��ͼ��
		  SysConfig.Mode2_Dur=120;
		  SysConfig.Mode3_Dur=0;// ����������
		 
		  SysConfig.Stolen_CutOil_Dur=0;  // ��������ģʽ�£����ж���·�ļ�� Ĭ��Ϊ0
		  SysConfig.speedwarn_Max=60;  //  ��������	 KM/h		60 default 
		  SysConfig.illegalMove_dis=200; //  �Ƿ�λ��   200��
		  SysConfig.Speaker_warn_keep_Dur=30; //  ������ ����������ʱ��   30s  spec  �ĵ�Ҫ����30s

		   // Default  work state
			SysConfig.Anti_theft_enable=1;   //  ��������  Ĭ�Ͽ���
			SysConfig.auto2Anti_theft_enable=0; //  �Զ��л�������״̬ Ĭ��: �ر�
			SysConfig.cutpowerWarn_enable=1;   // �ϵ籨��ʹ��  Ĭ�Ͽ���
			SysConfig.lowVoltWarn_enable=1;  // Ƿѹ����  --����
			SysConfig.nomoneyWarn_enable=1;  // Ƿ�ѱ��� --����  
			SysConfig.illegalMove_enable=1;  // �Ƿ�λ�Ʊ���    ---����
			SysConfig.abnormalMove_cutpower_enable=1;  //  �춯����� �ж�����  : ����
			SysConfig.shakeWarn_enable=1;// �𶯱���:����  
			SysConfig.speedWarn_enable=0; // ���ٱ���ʹ��  �ر�
			SysConfig.gprs_work_enable=0;  //  gprs ����:����      
			SysConfig.SMS_work_enable=0;   //   sms ���� : �ر� 
		 
		  SysConfig.current_userstate=0; //	��ǰ�û�״̬	  0:  ��ͨ�û�     1: ����Ա
		  SysConfig.superPassword[20]="";  //  ��������Ա����
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
        if( Api_Config_read(config,ID_CONF_SYS,(u8*)&SysConfig,sizeof(SysConfig))==false)   //��ȡϵͳ������Ϣ
                      rt_kprintf("\r\nConfig_ Read Error\r\n");   

			
 		memset((u8*)APN_String,0 ,sizeof(APN_String)); 
		memcpy((u8*)APN_String,SysConfig.APN_str,strlen((const char*)SysConfig.APN_str));  
	                        //   ����
		memset((u8*)DomainNameStr,0 ,sizeof(DomainNameStr));
		memcpy((u8*)DomainNameStr,SysConfig.DNSR,strlen((const char*)SysConfig.DNSR)); 
	                        //   ����aux
		memset((u8*)DomainNameStr_aux,0 ,sizeof(DomainNameStr_aux));
		memcpy((u8*)DomainNameStr_aux,SysConfig.DNSR_Aux,strlen((const char*)SysConfig.DNSR_Aux)); 

		
			 
	        //   �� IP   +  �˿�
	        memcpy((u8*)RemoteIP_main,SysConfig.IP_Main,4); 
	        RemotePort_main=SysConfig.Port_main;
	       //   ���� IP   +  �˿�
	        memcpy((u8*)RemoteIP_aux,SysConfig.IP_Aux,4); 
	        RemotePort_aux=SysConfig.Port_Aux;	 		
		   //  Link2  
	        memcpy((u8*)Remote_Link2_IP,SysConfig.Link2_IP,4); 
	        Remote_Link2_Port=SysConfig.Link2_Port;	 				 

	       //  �����������ϱ�״̬
	        TriggerSDsatus=SysConfig.TriggerSDsatus;
		//   ACC  on   off  ����
		ACC_on_sd_Duration=SysConfig.AccOn_Dur;
		ACC_off_sd_Duration=SysConfig.AccOff_Dur;              


}


void  Trker_FirstRun_Config_Write(void)
{     //   �����״θ�����д���ò��� 


          //   д���ն�ID  
          Trker_DeviceID_write( u8 *instr);         



		  //  д��Ĭ��������Ϣ
          Trker_SysConfig_init();   //  д��ϵͳ������Ϣ
 		  //---- add special -----------  
		  DF_WriteFlashSector(DF_LOGIIN_Flag_offset,0,&Login_Menu_Flag,1); 
}
//-----------------------------------------------------------------
void Trker_SetConfig(void)
{
	//u8 i=0;//,len_write=0;
//	u32 j=0;
	
       printf("\r\nSave Config\r\n");
	// 1.  ��ȡconfig ����      0 :�ɹ�    1 :  ʧ��
	Api_Config_read(config,ID_CONF_SYS,(u8*)&SysConfig,sizeof(SysConfig));           
       //printf("\r\nRead Save SYSID\r\n");
       //  2. ��ȡ�ɹ�  ���ж�  �汾ID 
	if(SysConfig.Version_ID!=SYSID)//SYSID)   //  check  wether need  update  or not 
	{
	       printf("\r\n ID not Equal   Saved==0x%X ,  Read==0x%X !\r\n",SYSID,SysConfig.Version_ID);	
	        SysConfig.Version_ID=SYSID;  // update  ID 
         //  2.1  ����ʹ��
         
		 DF_WriteFlashSector(DF_WARN_PLAY_Page,0,&Warn_Play_controlBit,1); 
              
	     //  2.2  ����д��		 
           Trker_FirstRun_Config_Write();   // ��߸����� SYSID           
		   vdr_erase(); 

	}
	else			
		   printf("\r\n Config Already Exist!\r\n"); 
}

 void Trker_ReadConfig(void) 
{
    u16   res[3];
	
   
             //   д���ն�ID           ����Ҫ
          Trker_DeviceID_read( u8 *instr);      







		   DF_delay_ms(500);  

		  //  1.   read  read to  compare  judge
         //-------- JT808 �������ö�ȡ���ԣ�����Ƶ��������Ҫ������Ҫ���⴦�� 
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
           if(res[0]&&res[1]&&res[2])   // ȫ������
           	{
           	   printf("\r\n JT808 ȫ��ʧ��!  need all recover\r\n"); 	
	           JT808_Conf_init();	
           	}
		   else
		   if(res[0]&&res[1])   //    seg1  seg2  ������˵��  BAK error
		   	{    
		   	    // org  bak2 ---ok      bak---error
		   	   if((u8)(SysConfig.DURATION.Default_Dur>>24)!=0xFF) // �ж���ȷ���ǲ��� FF
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
		   if(res[0]&&res[2])  //  seg1  seg3    ������˵�� BAK2  error
		   	{
		   	   // org  bak  ---ok       bak2 -----error
		   	   if((u8)(SysConfig.DURATION.Default_Dur>>24)!=0xFF) // �ж���ȷ���ǲ��� FF
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
		   if(res[1]&&res[2])  //  seg2  seg3	 ������˵�� org  error
			{
			   //  bak  bak2 --ok     org---error
		         if((u8)(SysConfig_BAK1.DURATION.Default_Dur>>24)!=0xFF) // �ж���ȷ���ǲ��� FF
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
		   	printf("\r\n JT808 ��ȡУ��ɹ�! \r\n"); 
         //-------------------------------------------------------------------------------------
		   
		   Trker_SysConfig_Read();  //��ȡϵͳ������Ϣ	                 

                 
    printf("\r\n Read Config Over \r\n");   
}
void Trker_DefaultConfig(void)
{
   u32 DriveCode32=0;
   u8  reg_str[30],i=0;

       printf("\r\n         SYSTEM ID=0x%X ",SysConfig.Version_ID);   
	printf("\r\n		   ���õļ�Ȩ��Ϊ: ");
       printf(" %s\r\n		   ��Ȩ�볤��: %d\r\n",SysConfig.ConfirmCode,strlen((const char*)SysConfig.ConfirmCode));   
					   
  if(SysConfig.Regsiter_Status)
  	   printf("\r\n		   ���ն��Ѿ�ע���!    %d \r\n",SysConfig.Regsiter_Status);  
  else
  	   printf("\r\n		   ���ն˱���δ��ע��!\r\n");   
  
  
	  printf("\r\n���Ŀ��ƶϵ�= %d (1 : �ϵ�  0 : ����)\r\n", SysConfig.relay_flag);     
   if(SysConfig.relay_flag==1)
		  {
	  Enable_Relay();
	   Car_Status[2]|=0x08; 	// ��Ҫ���Ƽ̵���
	   }
  else
	  {
	  Disable_Relay();
	  Car_Status[2]&=~0x08;    // ��Ҫ���Ƽ̵���
	  printf("\r\n�̵����պ�");
	  }
        
        // APN ����
	     printf("\r\n		   APN ���� :%s	 \r\n",APN_String); 
         DataLink_APN_Set(APN_String,1); 
            //     ����
          memset(reg_str,0,sizeof(reg_str));
          memcpy(reg_str,DomainNameStr,strlen((char const*)DomainNameStr));
          printf("\r\n		  �������� :	 %s\r\n",reg_str); 
          KorH_check(); 
		 
		  
	      //    ����aux
          memset(reg_str,0,sizeof(reg_str));
          memcpy(reg_str,DomainNameStr_aux,strlen((char const*)DomainNameStr_aux));
          printf("\r\n		aux  �������� :	 %s\r\n",reg_str); 	    
					  
         // ��������IP ��ַ(4Bytes)  UDP�˿ں���(2Bytes) TCP�˿ں���(2Bytes)
         printf("\r\n		  ��IP: %d.%d.%d.%d : %d \r\n",RemoteIP_main[0],RemoteIP_main[1],RemoteIP_main[2],RemoteIP_main[3],RemotePort_main);   
	  DataLink_MainSocket_set(RemoteIP_main, RemotePort_main,0);
	//  printf("\r\n		   ����IP: %d.%d.%d.%d : %d \r\n",RemoteIP_aux[0],RemoteIP_aux[1],RemoteIP_aux[2],RemoteIP_aux[3],RemotePort_aux);   
         DataLink_AuxSocket_set(RemoteIP_aux, RemotePort_main,0);

	 // printf("\r\n		   IC ��IP: %d.%d.%d.%d  TCP: %d  UDP:%d\r\n",SysConfig.BD_IC_main_IP[0],SysConfig.BD_IC_main_IP[1],SysConfig.BD_IC_main_IP[2],SysConfig.BD_IC_main_IP[3],SysConfig.BD_IC_TCP_port,SysConfig.BD_IC_UDP_port);   
	 // printf("\r\n		   IC ������IP: %d.%d.%d.%d \r\n",SysConfig.BD_IC_Aux_IP[0],SysConfig.BD_IC_Aux_IP[1],SysConfig.BD_IC_Aux_IP[2],SysConfig.BD_IC_Aux_IP[3]);   
	  DataLink_IC_Socket_set(SysConfig.BD_IC_main_IP,SysConfig.BD_IC_TCP_port,0); 
	  	 
	  //  ACC On �ϱ����(2Bytes)  ACC Off �ϱ����(2Bytes)
	  printf("\r\n		   ACC on ���ͼ��Ϊ: %d S\r\n		   ACC Off ���ͼ��Ϊ: %d S\r\n",ACC_on_sd_Duration,ACC_off_sd_Duration);
	  printf("\r\n  ����״̬: 0x%2X ��С��Ϣʱ��: %d s   ƣ�ͼ�ʻ����: %d s  \r\n",Warn_Play_controlBit,TiredConf_struct.TiredDoor.Door_MinSleepSec,TiredConf_struct.TiredDoor.Door_DrvKeepingSec);  		  
	  
          //  ����ͣ������(2Bytes)	
	//  printf("\r\n		   ����ͣ������: %d S\r\n",StopLongDuration);
	   //  ������ش�ʹ�ܱ�־λ(1Bytes) �� �������ϴ�����(4Bytes)  ��λ: m ��  
         printf("\r\n         ��������: %s \r\n",SysConfig.LISTEN_Num);    


	   
          //   ��¼��ΨһID
         printf("\r\n		   ����Ψһ�Ա��: %35s \r\n",SysConfig.DeviceOnlyID);    
         printf("\r\n		   �ϱ��ٶȻ�ȡ��ʽ: %d :",SysConfig.Speed_GetType);	  
         if(SysConfig.Speed_GetType)							
                printf("����ͨ�������ٶȴ�������ȡ!\r\n");
         else
                printf("����ͨ��GPSģ���ȡ!\r\n"); 

         printf("\r\n		   ����ϵ��У׼״̬: %d :",SysConfig.DF_K_adjustState);	    
         if(SysConfig.DF_K_adjustState)					  	 
                  printf(" ����ϵ��--У׼�ɹ�!\r\n"); 
         else
                  printf("����ϵ��--��δУ׼!\r\n");   

	  //   ���
		  SysConfig.DayStartDistance_32=DayStartDistance_32;
		  SysConfig.Distance_m_u32=Distance_m_u32;		
         printf("\r\n		   �ۼ����: %d  ��   ,  �������:   %d��\r\n",SysConfig.Distance_m_u32,SysConfig.Distance_m_u32-SysConfig.DayStartDistance_32);  	
         //  �ٶ�����
         printf("		   ��������ٶ�: %d  Km/h    ���ٱ�������ʱ������: %d  s \r\n", SysConfig.Speed_warn_MAX,SysConfig.Spd_Exd_LimitSeconds);  		 

     //    printf("\r\n		  ���ұ�׼�汾: %14s \r\n",SysConfig.StdVersion.stdverStr);					  
       //  printf("\r\n		  ���ұ�׼�汾�޸ĵ���: %d \r\n",SysConfig.StdVersion.MdfyID); 


         printf("\r\n		  ����ϵ��(�ٶ�����ϵ��): %d \r\n",SysConfig.Vech_Character_Value); 					  
         printf("\r\n		  ���ΰ�װ����: %X-%X-%X %X:%X:%X \r\n",SysConfig.FirstSetupDate[0],SysConfig.FirstSetupDate[1],SysConfig.FirstSetupDate[2],SysConfig.FirstSetupDate[3],SysConfig.FirstSetupDate[4],SysConfig.FirstSetupDate[5]);  


         DriveCode32=(SysConfig.Driver_Info.Effective_Date[0]<<16)+(SysConfig.Driver_Info.Effective_Date[1]<<8)+SysConfig.Driver_Info.Effective_Date[2];
         printf("\r\n		  ��ʻԱ����: %d \r\n",DriveCode32);  					  
         printf("\r\n		  ��������ʻ֤��: %18s \r\n",SysConfig.Driver_Info.DriverCard_ID);  					  
         printf("\r\n		  ��ʻԱ����: %s \r\n",SysConfig.Driver_Info.DriveName); 					  
         printf("\r\n		  ��ʻԱ��ҵ�ʸ�֤: %20s \r\n",SysConfig.Driver_Info.Drv_CareerID); 
         printf("\r\n		  ��֤����: %s \r\n",SysConfig.Driver_Info.Comfirm_agentID);   



         printf("\r\n		  ����VIN��: %17s \r\n",Vechicle_Info.Vech_VIN);   
         printf("\r\n		  ���ƺ���: %12s \r\n",Vechicle_Info.Vech_Num);  
         printf("\r\n		  ���Ʒ���: %12s \r\n",Vechicle_Info.Vech_Type);  
         printf("\r\n        ��������ʡID: %d \r\n",Vechicle_Info.Dev_ProvinceID);
         printf("\r\n        ����������ID: %d \r\n",Vechicle_Info.Dev_CityID); 
         printf("\r\n        ������ɫ:   JT415    1  �� 2 �� 3 �� 4 �� 9����----��ǰ��ɫ %d \r\n",Vechicle_Info.Dev_Color);  


         printf("\r\n        �����ϱ�������Ϊ  TriggerSDsatus=%X    \r\n",TriggerSDsatus);   
         printf("\r\n        Max_picNum =  %d   Max_CycleNum = %d   Max_DrvRcdNum=%d \r\n",Max_PicNum,Max_CycleNum,Max_RecoderNum); 

         printf("\r\n\r\n	  ��������״̬: ");
         switch(SysConfig.LOAD_STATE)
		{  
		case 1:
		   printf("�ճ�\r\n"); 
		  break;
		case 2:
		   printf("����\r\n"); 
		  break;		  
		case 3:
		   printf("����\r\n"); 
		  break;
		default:
		       SysConfig.LOAD_STATE=1;
		       Api_Config_Recwrite_Large(jt808,0,(u8*)&SysConfig,sizeof(SysConfig));
		   printf("�ճ�2\r\n");  
		  break;
		}
         printf("\r\n\r\n  ��ʼ��ˮ��: %d \r\n", SysConfig.Msg_Float_ID); 
	     printf("\r\n\r\n             cyc_read:   %d ,     cyc_write :%d\r\n  \r\n",cycle_read,cycle_write);     		
         
         //=====================================================================
         //API_List_Directories();
         //-----------  ����ģ�����  ---------
	     BD_list(); 

		 // ----    �״���������  --------------------
		 if(Vechicle_Info.Link_Frist_Mode==1)  
		 	{
                 printf("\r\n\r\n   �״�����ģʽ:   MainLink");    
		 	}
		 else
		 	{
                  printf("\r\n\r\n   �״�����ģʽ:  DNSR����");     
		 	}

		if(DeviceNumberID[0]==0xFF)
			printf("\r\n  =======> ��δ�����豸��ţ�����������\r\n" );  
		else
			{
			printf("\r\n ��ȡ�ն�IDΪ : "); 
			for(i=0;i<12;i++)
				printf("%c",DeviceNumberID[i]);
			printf("\r\n");
			}  
       //---------- SIM ID -----------------------------  
        	if(SimID_12D[0]==0xFF)
			printf("\r\n  =======> ��δ����SIMID_������ţ�����������\r\n" );  
		else
			{
			printf("\r\n ��ȡ�豸SIMID ����ID Ϊ : "); 
			for(i=0;i<12;i++)
				printf("%c",SimID_12D[i]);
			printf("\r\n");
			} 
	  	// ��Ϣ���ĺ���-----------------------------
	  	// printf("\r\n		   ��Ϣ���ĺ��� :%s	 \r\n",SysConfig.SMS_RXNum);

		 // ---  Ӳ���汾��Ϣ-------------
		  HardWareVerion=HardWareGet();		 
		  printf("\r\n		        -------Ӳ���汾:%X        B : %d %d %d\r\n",HardWareVerion,(HardWareVerion>>2)&0x01,(HardWareVerion>>1)&0x01,(HardWareVerion&0x01));   
		  if(HardWareVerion==3) 
		  	  printf("\r\n		       ��̨�ͺ�: TW705   \r\n"); 

          //------- �Զ�������ʽ -----------           
		  printf("\r\n		        -------�Զ�������ʽ:%d    \r\n",SysConfig.Auto_ATA_flag);      

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
+ Mode 3:  After device works in Mode 2 for more than 90mins��Viesleept��, it will automatically setup both GPRS and GPS Module at Standby mode (do not send mesages).If there are any changes in the status of the vehicle, the device will activate the operating mode under Mode 1. If there is no vibration, SMS and call in 10minues, it will return to Mode3. If user turns on ACC, device will run into Mode1 immediately.  

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
   // ��ͬģʽ�� ���ִ���
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
   //  ���ô���ʽ 	   



}


void Trker_Mode_convert(void)
{  // ����ģʽ�л�
    
	// 0 . ��������� ACC
	Trker_HwDrv_AccCheck();  // ���������

	  /*
	     Mode3: If there are any changes in the status of the vehicle, the device will activate the operating mode under Mode 1
	  */
	  if(work_var.Mode==MODE3)
	  { 
	      //  ģʽ3  �£� ������������仯
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
     { //    ��ģʽ1  ��ģʽ�°���ָ�������������   ,  Mode3   ����������
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
                    {  //   ��mode3  ���������£��ۼƼ���û�е绰�Ͷ�Ϣ��ʱ��
                       work_var.Mode1_from_mode3_no_SMS_and_call_counter++;
					   if(work_var.Mode1_from_mode3_no_SMS_and_call_counter>=600*10) //10����
					   	{
					   	  work_var.Mode1_from_mode3_no_SMS_and_call_counter=0;
						  work_var.Mode2_working_counter_seconds=0;
                          work_var.Mode=MODE3;
					   	}
 
                    }
					break;
		 case  MODE2:   //  ����MOD2  ����£�
		 	        work_var.Mode2_working_counter_seconds++;
					if(work_var.Mode2_working_counter_seconds>=54000) //  90 ����=5400*10
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
{    //  Tracker     ����  �� ����״̬���
    
    //  0. ��������-- �ر�
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
    {                                       //  �������ܿ��������
       // or vibration alarm or        
	   if((work_var.Status_Word&STATUS_SHAKING)&&((work_var.Status_Word&STATUS_ACC)==0x0000))
	   	{  //       ACC �� �� ����
	   	   Anti_theft_Work_Start();
	   	}
	   //illegal ACC happens (In anti-theft lock, turn on ACC. 
	   if((Anti_theft.anti_working==1)&&(Anti_theft.start_execut_warning==2)&&( work_var.Acc_change_Action==AccAction_Off2On))
	   	{  //   ���ڷ���ģʽ�£���ACC  ��off  to   ON   ,��ô��ʼ����
	   	   Anti_theft_Work_Start();
           work_var.Acc_change_Action=AccAction_IDLE; // clear state
	   	}    
	   //Or When ACC on, activate anti-theft lock, by SMS(lock), Call from user, [,S12] message),
	   if((work_var.Status_Word&STATUS_ACC)&&(Anti_theft.rx_enable_anti_theft_sms))
	   	{  //  ACC on   �� �յ�  ������������Ϣ
           Anti_theft_Work_Start();
           Anti_theft.rx_enable_anti_theft_sms=0; // clear
	   	}

    }
    // 1. �Զ��л�����������--�ر�
     if(ENABLE==SysConfig.auto2Anti_theft_enable)
     {                                    //  �Զ��л����������� 
      
     }
	// 2. �ϵ籨��  ---- ����
	if(ENABLE==SysConfig.cutpowerWarn_enable)
     {   
        
		//	�ⲿ�ϵ��Դ���
		Trker_HwDrv_CutpowerCheck();	 
     }
	
	// 3. Ƿѹ����----����
	if(ENABLE==SysConfig.lowVoltWarn_enable)
     {                                     
	    Trker_HwDrv_LowPower_Warning();
     }
	

	// 4.Ƿ�ѱ���---����
	if(ENABLE==SysConfig.nomoneyWarn_enable)
     {                                   
       ;
     }
	
	
	// 5.�Ƿ�λ�Ʊ���---����
	if(ENABLE==SysConfig.illegalMove_enable)
     {                                    
        GPS_Illegal_Movement_checking();
     }

	
	// 6. �𶯱���---����
	if(ENABLE==SysConfig.shakeWarn_enable)
     {          
		//	 ���������
		Trker_HwDrv_shake_checking();	  //   �𶯴�����
     }

	// 7. ���ٱ���---�ر�
	if(ENABLE==SysConfig.speedWarn_enable)
     {       
		//	���ٱ������
		GPS_Speed_Warn_Checking(); 
     }

    // 8.Allow remote shutdown engine. Default ON.	  �˶��������ж����汨��---����
    // ��  Anti_theft_Relay_Contrl_timer  �������д����� 

	//  9 . �绰����  --����绰


    // 10. ������������   ����������������
	  Trker_HwDrv_EmergenButton_checking();  
   
}


void Trker_Function_Recover_Defalut(void)
{
   //  �ָ���������




}

void  Trker_Device_Reset(u8  value)
{  //  �ն˸�λ

    if(value)
        "�ն˸�λ" 
	    
  
}

//  Anti  theft  related  below   
void  Anti_theft_init(void)
{
   Anti_theft.anti_working=0;
   Anti_theft.start_execut_warning=0;
   Anti_theft.anti_workStep=0;   //  0 : init
   Anti_theft.speaker_enable=0; //  ʹ�ܿ���speaker
   Anti_theft.speaker_run_counter=0; // speaktimer
   Anti_theft.rx_enable_anti_theft_sms=0;  //  ���յ���������ģʽ����Ϣ

   Anti_theft.cutoff_oil_delay_enable=0; // ʹ����ʱ�ж�
   Anti_theft.cutoff_oil_delay_counter=0;//  ʹ���ж϶�ʱ��
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
      if(gps_pos.speed_KmH>5) // ������˶��У������Ƿ������˶����ж�����
      {
		   // 8.Allow remote shutdown engine. Default ON.	  �˶��������ж����汨��---����
		   if(ENABLE==SysConfig.abnormalMove_cutpower_enable)
			{									
			    Trker_HwDrv_Relay_Ctrl(1); 
			}
		  
      }
	   else
	          Trker_HwDrv_Relay_Ctrl(1);    // û���˶���
	  
  	}

}

void Anti_theft_Speaker_timer(void)
{                                      //   100 ms  timer
   if(0==Anti_theft.speaker_enable)
   	 return;
   
   Anti_theft.speaker_run_counter++;
   if(Anti_theft.speaker_run_counter<(SysConfig.Speaker_warn_keep_Dur*10))
   	{
   	   if(Anti_theft.speaker_run_counter%3==0)  // ��1  ͣ 2 
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
	Anti_theft.speaker_enable=1;  // ʹ�ܷ�������
}

void Anti_theft_Enable_Executing(void)
{  //   ִ�� ���������µ� ����
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
{   //  ��ԭ����  



   Trker_HwDrv_Relay_Ctrl(0);  // recover oil

   Trker_HwDrv_Speaker_Ctrl(0);
   Anti_theft.speaker_enable=0;
   Anti_theft.speaker_run_counter=0;   	
   Anti_theft.rx_enable_anti_theft_sms=0;
}

void Trker_OutPowerChecking_after_BattteryOn_1st_timer(void)
{   //  100ms                    ����ȿ��繤����״̬��  ����ⲿ��Դ״̬
  if(work_var.first_power)
  {
    if(Trker_HwDrv_InitpowerCheck())
	{
	  OutPowerchecking_underBATTon_counter++;
	  if(OutPowerchecking_underBATTon_counter>10)  //1s ����
	  	{
           OutPowerchecking_underBATTon_counter=0;
		   work_var.first_power=0;  // �ⲿ����ʹ��
	  	}
    }  

  }
  
}
