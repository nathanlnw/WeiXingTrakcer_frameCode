/*
    Tracker  gps  retlate   Part
*/

#include  "Hardware.h"


POS  gps_pos;

static  u8   GPS_Led_timer=0;  //   GPS �صƼ�ʱ��
static  u32  GPS_spd_warning_counter=0;    // ���ٱ��������� 
static  u16  GPS_spd_warn_timer=0; //  100ms
static  u16  Illegal_moveDist_m=0;   // �Ƿ�λ�ƾ��������
static  u16  Illegal__counter=0;  // 100ms ��ʱ��
//==================================================================================================
//������GPS ����ת����غ��� 
//==================================================================================================
void GPS_struct_init(void)
{
    gps_pos.AorV=V;
  
}


void Time_pro(u8 *tmpinfo, u8 hour, u8 min , u8 sec)
{
  	            //---- record  to memory
				GPRMC.utc_hour=hour;
				GPRMC.utc_min=min;
				GPRMC.utc_sec=sec;
                
				CurrentTime[0] = hour;
				CurrentTime[1] = min;
				CurrentTime[2] = sec; 
                
				//-----------  ���ͨЭ�� ------------- 	 
				Temp_Gps_Gprs.Time[0] = hour;
				Temp_Gps_Gprs.Time[1] = ( tmpinfo[2] - 0x30 ) * 10 + tmpinfo[3] - 0x30;
				Temp_Gps_Gprs.Time[2] = ( tmpinfo[4] - 0x30 ) * 10 + tmpinfo[5] - 0x30;  
            
}

void Status_pro(u8 *tmpinfo) 
{
 	                GPRMC.status=tmpinfo[0];
                    Posit_ASCII.AV_ASCII=tmpinfo[0];
					//-------------------------���ͨЭ��-----------------------------
						   if ( tmpinfo[0] == 'V' || tmpinfo[0] == 'v' )
						  {
								UDP_dataPacket_flag=0X03;
								StatusReg_GPS_V();
						  }
						  else if ( tmpinfo[0] == 'A' || tmpinfo[0] == 'a' )
						  {
								UDP_dataPacket_flag=0X02;		
								StatusReg_GPS_A(); 								  
						  }
}

void Latitude_pro(u8 *tmpinfo)
{
  u32  latitude;
  u32  vdr_lati=0;
  
   GPRMC.latitude_value=atof((char *)tmpinfo);
   /*     Latitude  
          ddmm.mmmm
    */
   memset(Posit_ASCII.Lat_ASCII,0,sizeof(Posit_ASCII.Lat_ASCII));
   memcpy(Posit_ASCII.Lat_ASCII,tmpinfo,strlen((const char*)tmpinfo));  
	//--------	808 Э�� --------------------
  if(UDP_dataPacket_flag==0X02)    //��ȷ�������֮һ��
  {  
    
    //------------  dd part   -------- 
	latitude = ( u32 ) ( ( tmpinfo[0] - 0x30 ) * 10 + ( u32 ) ( tmpinfo[1] - 0x30 ) ) * 1000000;
	//------------  mm  part  -----------
	/*    ת���ɰ����֮һ��
	      mm.mmmm   *  1000000/60=mm.mmmm*50000/3=mm.mmmm*10000*5/3
	*/
	latitude = latitude + ( u32 )( (( tmpinfo[2] - 0x30 ) * 100000+ (tmpinfo[3] - 0x30 ) * 10000+(tmpinfo[5] - 0x30 ) * 1000 + ( tmpinfo[6] - 0x30 ) * 100 + ( tmpinfo[7] - 0x30 ) * 10 +( tmpinfo[8] - 0x30 ))*5/3);  

	 if(latitude==0)
	 	{
	 	   GPS_getfirst=0;
	          StatusReg_GPS_V();
	          return;
		}
	Temp_Gps_Gprs.Latitude[0] = ( u8 ) ( latitude >> 24 );
	Temp_Gps_Gprs.Latitude[1] = ( u8 ) ( latitude >> 16 );
	Temp_Gps_Gprs.Latitude[2] = ( u8 ) ( latitude >> 8 );
	Temp_Gps_Gprs.Latitude[3] = ( u8 ) latitude;

    //  �������г���¼�ǵ�ת��
    //                      ��                                           ��large   
    
	vdr_lati=( tmpinfo[0] - 0x30 ) * 10 + ( u32 ) ( tmpinfo[1] - 0x30 );
    vdr_lati= vdr_lati*600000+( tmpinfo[2] - 0x30 ) * 100000+ (tmpinfo[3] - 0x30 ) * 10000+(tmpinfo[5] - 0x30 ) * 1000 + ( tmpinfo[6] - 0x30 ) * 100 + ( tmpinfo[7] - 0x30 ) * 10 +( tmpinfo[8] - 0x30 );

	VdrData.Lati[0]= ( u8 ) ( vdr_lati >> 24 );
    VdrData.Lati[1]= ( u8 ) ( vdr_lati >> 16 );
	VdrData.Lati[2]= ( u8 ) ( vdr_lati >> 8 );
	VdrData.Lati[3]= ( u8 ) vdr_lati;
    
	//-------------------------------------------------------------------------------------------
  } 
  else
  {
    VdrData.Lati[0]= ( u8 ) ( 0x7F );
    VdrData.Lati[1]= ( u8 ) ( 0xFF );
	VdrData.Lati[2]= ( u8 ) (0xFF );
	VdrData.Lati[3]= ( u8 ) 0xFF;

  }
   //----------------------------------------------
}
 
void Lat_NS_pro(u8 *tmpinfo)
{
   GPRMC.latitude=tmpinfo[0];
}

void Longitude_pro(u8 *tmpinfo)
{
 u32  longtitude;
 u32  Longi=0;
 
 GPRMC.longtitude_value=atof((char *)tmpinfo);  
  /*     Latitude  
          dddmm.mmmm
    */

   memset(Posit_ASCII.Longi_ASCII,0,sizeof(Posit_ASCII.Longi_ASCII)); 
   memcpy(Posit_ASCII.Longi_ASCII,tmpinfo,strlen((const char*)tmpinfo));  
 //--------  808Э��  ---------
 if(UDP_dataPacket_flag==0X02)  //��ȷ�������֮һ��
 {
     //------  ddd part -------------------
	 longtitude = ( u32 )( ( tmpinfo[0] - 0x30 ) * 100 + ( tmpinfo[1] - 0x30 ) * 10 + ( tmpinfo[2] - 0x30 ) ) * 1000000;  
	 //------  mm.mmmm --------------------	 
	 /*    ת���ɰ����֮һ��
		   mm.mmmm	 *	1000000/60=mm.mmmm*50000/3=mm.mmmm*10000*5/3
	 */
	 longtitude = longtitude + ( u32 ) (( ( tmpinfo[3] - 0x30 ) * 100000 + ( tmpinfo[4] - 0x30 )*10000+(tmpinfo[6] - 0x30 ) * 1000 + ( tmpinfo[7] - 0x30 ) * 100 + ( tmpinfo[8] - 0x30 ) * 10 + ( tmpinfo[9] - 0x30 ))*5/3);
	 if(longtitude==0)
	 	{GPS_getfirst=0; StatusReg_GPS_V();return;} 
	 
	 Temp_Gps_Gprs.Longitude[0] = ( u8 ) ( longtitude >> 24 );
	 Temp_Gps_Gprs.Longitude[1] = ( u8 ) ( longtitude >> 16 );
	 Temp_Gps_Gprs.Longitude[2] = ( u8 ) ( longtitude >> 8 );
	 Temp_Gps_Gprs.Longitude[3] = ( u8 ) longtitude; 


     
	 //  �������г���¼�ǵ�ת��
	 // 					 �� 										  ��large	
	 Longi=( tmpinfo[0] - 0x30 ) * 100 + ( tmpinfo[1] - 0x30 ) * 10 + ( tmpinfo[2] - 0x30 ); 
    Longi=Longi*600000+ ( tmpinfo[3] - 0x30 ) * 100000 + ( tmpinfo[4] - 0x30 )*10000+(tmpinfo[6] - 0x30 ) * 1000 + ( tmpinfo[7] - 0x30 ) * 100 + ( tmpinfo[8] - 0x30 ) * 10 + ( tmpinfo[9] - 0x30 );

	VdrData.Longi[0]= ( u8 ) ( Longi >> 24 );
    VdrData.Longi[1]= ( u8 ) ( Longi >> 16 );
	VdrData.Longi[2]= ( u8 ) ( Longi >> 8 );
	VdrData.Longi[3]= ( u8 ) Longi;
    
	 
 }		
 else
 {
    VdrData.Longi[0]= ( u8 ) ( 0x7F);
    VdrData.Longi[1]= ( u8 ) ( 0xFF );
	VdrData.Longi[2]= ( u8 ) ( 0xFF);
	VdrData.Longi[3]= ( u8 ) 0xFF;
 }
	 
   //---------------------------------------------------  
}

void Long_WE_pro(u8 *tmpinfo)
{

	GPRMC.longtitude=tmpinfo[0]; 
}


void Speed_pro(u8 *tmpinfo,u8 Invalue,u8 Point)
{ 
  u32	  sp=0,sp_DISP=0;
  u32     reg=0;  

  
    //-------------------------------------------------------------------------------------------------------------
	if(Invalue==INIT)
	{	  
	  return;
	}
	else//---------------------------------------------------------------------------------------------------------
	{
	 GPRMC.speed=atof((char *)tmpinfo);
	 //---------------------------------------------------					  
	 if(UDP_dataPacket_flag==0x02 )
	 {
			  //-----808 Э�� -------------- 
			 //�����ֽڵ�λ0.1 km/h  
			 if ( Point == 1 )	//0.0-9.9=>
			 {
												  //++++++++  Nathan Modify on 2008-12-1   ++++++++++
					if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39))
					 {
					   sp = ( tmpinfo[0] - 0x30 ) * 10 + ( tmpinfo[2] - 0x30 );  //����10��
					 }	   
					else
					   return;			  
				 
			 }
			 else if ( Point == 2 )  //10.0-99.9
			 {
											  //++++++++  Nathan Modify on 2008-12-1   ++++++++++
					if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)) 
					{
					  sp = ( tmpinfo[0] - 0x30 ) * 100 + ( tmpinfo[1] - 0x30 ) * 10 + tmpinfo[3] - 0x30;
					}
					else
					 return; 									 
	 
			 }
			 else if( Point == 3 ) //100.0-999.9
			 {
												   //++++++++  Nathan Modify on 2008-12-1	++++++++++
					if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[4]>=0x30)&&(tmpinfo[4]<=0x39)) 
					 {
					   sp = ( tmpinfo[0] - 0x30 ) * 1000 + ( tmpinfo[1] - 0x30 ) * 100 + ( tmpinfo[2] - 0x30 ) * 10 + tmpinfo[4] - 0x30;
					  }
					else
					  return;  
	 
			 }
		 else		 
			 {
			     if(JT808Conf_struct.Speed_GetType==0)
			          Spd_Using=0;
			 }
	 
		 // --------  sp ��ǰ��0.1 knot------------------	 
		  sp= (u32)(sp * 185.6) ;  //  1 ����=1.856 ǧ��  ������m/h
								 
		  if(sp>220000)   //ʱ�ٴ���220km/h���޳�
			  return;  
	 
		  sp_DISP=sp/100;   //  sp_Disp ��λ�� 0.1km/h 
							  
	         //------------------------------ ͨ��GPSģ�����ݻ�ȡ�����ٶ� --------------------------------
            if(1==Limit_max_SateFlag)
			 {
			   if((sp_DISP>=1200)&&(sp_DISP<1500))
			   	   sp_DISP=1200;     //  �ٶȴ���120 km/h   ��С��150 km/h
			   if(sp_DISP>=1500)
                    Illeagle_Data_kickOUT=1;  // �ٶȴ���150  �޳� 
			   else
			   	   Speed_gps=(u16)sp_DISP;
             }
			 else
			    Speed_gps=(u16)sp_DISP;


        //  Speed_gps=Speed_jiade;//800;  //  �ٵ�Ϊ�˲���    

	         //---------------------------------------------------------------------------
       if(JT808Conf_struct.Speed_GetType)  // ͨ���ٶȴ����� ��ȡ�ٶ�
       	{ 
              K_AdjustUseGPS(Speed_gps);  //  ����Kֵ    
              if(JT808Conf_struct.DF_K_adjustState==0) 
			  {
			     // ---  ��δУ׼ǰ����õ����ٶ���ͨ��GPS����õ���
			      Spd_Using=Speed_gps;      
              }  
			  else
			  	{
				  if((Speed_cacu<50)&&(Speed_gps>150))  
				   //  gps ���� 15 km/h   �Ҵ������ٶ�С��5 ������GPS�ٶȴ��洫�����ٶ�    
				   {
					    Spd_Using=Speed_gps;
				   }
				  else
				        Spd_Using=Speed_cacu;   
			  	}	  
       	}  
	   else
	   	{    // ��GPS ȡ�ٶ�
				Spd_Using=Speed_gps;    // ��GPS���ݼ���õ��ٶ� ��λ0.1km/h
	   	}
	 }
	 else if( UDP_dataPacket_flag == 0x03 )
	 { 
		   Speed_gps=0;
	       if(0==JT808Conf_struct.Speed_GetType)  
			{
				 //  δУ׼����£���GPS δ��λ ��ô��� 0
			     Spd_Using=Speed_gps;	 
	       	} 
	        if((JT808Conf_struct.Speed_GetType)&&(JT808Conf_struct.DF_K_adjustState))  
	       	{   // �Ӵ�����ȡ�ٶ��ң��Ѿ�У׼
	              Spd_Using=Speed_cacu;       
	       	}      
	 }  		
	}

   //---------------------------------------------------
  Delta_Speed_judge();//  �жϴ�������GPS�ٶȵĲ�ֵ  
	
   //---------------------------------------------------
}


void Direction_pro(u8 *tmpinfo,u8 Invalue,u8 Point)
{
   u32	  sp=0;  
  //------------------------------------------------------------------------------------------------
  if(Invalue==INIT)
  {    
    return;
  }
  else//-------------------------------------------------------------------------------------------
  {
   GPRMC.azimuth_angle=atof((char *)tmpinfo);
	
   
   //--------------808 Э��  1 ��------------------------- 	 
   if ( UDP_dataPacket_flag== 0x02 )
   {	  
		  
		
		   if ( Point == 1 )   //5.8 
		   {
					  if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39))
				   sp = ( tmpinfo[0] - 0x30 ) ;
				  else 
				   return;
				   
		   }
		   else if ( Point == 2 )  // 14.7
		   {
					  if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)) 
				   sp = ( tmpinfo[0] - 0x30 ) * 10 + ( tmpinfo[1] - 0x30 );
				  else
				   return;
		   
		   }
		   else    //357.38
		   if ( Point == 3 )
		   {
			      if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[4]>=0x30)&&(tmpinfo[4]<=0x39)) 
				   sp = ( tmpinfo[0] - 0x30 ) * 100 + ( tmpinfo[1] - 0x30 ) * 10 + ( tmpinfo[2] - 0x30 ) ;
				  else
				   return;
   
	       }
		   else
		   	{
               sp=0;   
		   	}
		   GPS_direction=sp;  //  ��λ 1��

	   //----------  �յ㲹�����   ----------
	  // Inflexion_Process();
       
	   
		   
   }
   else if ( UDP_dataPacket_flag == 0x03 )
   {
          GPS_direction=0; 		  
 
   }


    return;
  }
 


}

void Date_pro(u8 *tmpinfo,u8 fDateModify, u8 hour, u8 min , u8 sec)
{
 uint8_t  year=0,mon=0,day=0;  
 TDateTime now, now_bak; 
 int i=0;

 
 day = (( tmpinfo[0] - 0x30 ) * 10 ) + ( tmpinfo[1] - 0x30 );
 mon = (( tmpinfo[2] - 0x30 ) * 10 ) + ( tmpinfo[3] - 0x30 );
 year = (( tmpinfo[4] - 0x30 ) * 10 ) + ( tmpinfo[5] - 0x30 );
 
 if(fDateModify){
	 //sscanf(tmpinfo,"%2d%2d%2d",&day,&mon,&year);
	 day++;
	 if(mon == 2){
		 if ( ( year % 4 ) == 0 ){
			 if ( day == 30 ){day = 1;mon++;}
		 }else if ( day == 29 ){ day = 1;mon++;}
	 }else if (( mon == 4 ) || ( mon == 6 ) || ( mon == 9 ) || ( mon == 11 )){
		 if ( day == 31 ){mon++;day = 1;}
	 }else{
		 if ( day == 32 ){mon++;day = 1;}	 
		 if( mon == 13 ){mon = 1;year++; }
	 }
 }

 //--------  ��ݹ��ˣ� �����λ����ǰʱ�䲻�账��ֱ�ӷ���
 if(year<13)
 {
   //----- ״̬�ĳ� V  
   UDP_dataPacket_flag=0X03;
   StatusReg_GPS_V();

   Year_illigel=1; // ��ݲ��Ϸ�
   
   return ;
 }
 else
 	Year_illigel=0; 
 
 GPRMC.utc_year=year;
 GPRMC.utc_mon=mon;
 GPRMC.utc_day=day;
 if((sec==30)&&(GPRMC.status=='A')){
	 now.year = year;
	 now.month = mon;	   
	 now.day = day;
	 now.hour = hour;
	 now.min = min;
	 now.sec = sec;
	 now.week=1;      
	 Device_RTC_set(now);  

	 //------  ��ȡ����У��---
	 now_bak=Get_RTC();	 
	 i=memcmp((u8*)&now_bak,(u8*)&now,sizeof(now)); 
	 if(i!=0)
	  {
	        RT_Total_Config();		 	
		    Device_RTC_set(now);  
	  }
	 
 }
 //------------------------------------------------ 	
 if(GPRMC.status=='A')     //  ��¼��λʱ��
{
  Time2BCD(A_time);  
  //------- Debug �洢 ÿ��ľ�γ��  || ʵ��Ӧ���� �洢ÿ���ӵ�λ��  -----   
  //  ���ݳ���55��ÿ����£���Ĵ����м�¼������ÿ���������һ����λ�ľ�γ�� ,Ԥ��5�����ڴ洢��һСʱ��λ�� 
  if(sec<55)
  	{
      memcpy(Posit[min].latitude_BgEnd,Gps_Gprs.Latitude,4); //��γ
      memcpy(Posit[min].longitude_BgEnd,Gps_Gprs.Longitude,4); //����	  
      Posit[min].longitude_BgEnd[0]|=0x80;//  ����
  	}
  if((min==59)&&(sec==55))
    { // ÿ��Сʱ��λ����Ϣ               
       NandsaveFlg.MintPosit_SaveFlag=1; 
	}  
 }   
  //---- �洢��ǰ����ʼ���  ����ʱ------------
   if((hour==0)&&(min==0)&&(sec==3))   // �洢3��ȷ���洢�ɹ� 
  	{ 
		  DF_Write_RecordAdd(Distance_m_u32,DayStartDistance_32,TYPE_DayDistancAdd);
		  JT808Conf_struct.DayStartDistance_32=DayStartDistance_32;
		  JT808Conf_struct.Distance_m_u32=Distance_m_u32;		 

          // ������� һ��һ��
		  GB19056.speedlog_Day_save_times=0; //clear
		  VdrData.H_15[0]=0x00;
		  GB19056.start_record_speedlog_flag=0;// clear 
    } 
 
 //-------------------------------------------------
      //---------  ���ͨЭ��  -------
                       
						//if(systemTick_TriggerGPS==0)
 {
   Temp_Gps_Gprs.Date[0] = year;
   Temp_Gps_Gprs.Date[1] = mon;
   Temp_Gps_Gprs.Date[2] = day;   						
  }

}


//---------  GGA --------------------------
void HDop_pro(u8 *tmpinfo)
{ 
 float dop;
 
  dop=atof((char *)tmpinfo);				        
  HDOP_value=dop;		 //  Hdop ��ֵ

}

void  GPS_Delta_DurPro(void)    //��GPS �����ϱ�������
{
   u32  longi_old=0, longi_new=0,lait_old=0 ,lati_new=0;   

 //    1.    ��ʱ�ϱ����
  if((1==JT808Conf_struct.SD_MODE.DUR_TOTALMODE)&&(Year_illigel==0))   // ��ʱ�ϱ�ģʽ
  {
	 	//----- ��һ�����ݼ�¼��ʱ��
		fomer_time_seconds = ( u32 ) ( BakTime[0] * 60 * 60 ) + ( u32 ) ( BakTime[1] * 60 ) + ( u32 ) BakTime[2];  
		
		//-----  ��ǰ���ݼ�¼��ʱ��
		tmp_time_secnonds = ( u32 ) ( CurrentTime[0] * 60 * 60 ) + ( u32 ) ( CurrentTime[1] * 60 ) + ( u32 )  CurrentTime[2];
		
		//һ��86400��
		
		if ( tmp_time_secnonds > fomer_time_seconds )
		{
				delta_time_seconds = tmp_time_secnonds - fomer_time_seconds;
				//systemTickGPS_Clear();
		}
		else if(tmp_time_secnonds < fomer_time_seconds)
		{
				delta_time_seconds = 86400 - fomer_time_seconds + tmp_time_secnonds;
				//systemTickGPS_Clear();
		}
		else 
		 {
			 // systemTickGPS_Set();  
			  UDP_dataPacket_flag=0X03; 
			  StatusReg_GPS_V();
		 }	
		
		if((SleepState==1)&&(delta_time_seconds==(Current_SD_Duration-5)&&(Current_SD_Duration>5)))  //  --  ����ʱ �ȷ���Ȩ
		   {
			  SleepConfigFlag=1;  //����ǰ5 ����һ����Ȩ
		   }            
		
		if((delta_time_seconds >= Current_SD_Duration))//limitSend_idle 
		  {			  

			    Current_State=0;			 
			 if (BD_ISP.ISP_running==0)
			    PositionSD_Enable();  
			 
			  memcpy(BakTime,CurrentTime,3); // update   
		  }
  	}  
    
    //------------------------------ do this every  second-----------------------------------------    

	//----- �¹��ɵ�3  :      �ٶȴ��� 0  km   λ�� û�б仯 
	longi_old=(Gps_Gprs.Longitude[0]<<24)+(Gps_Gprs.Longitude[1]<<16)+(Gps_Gprs.Longitude[2]<<8)+Gps_Gprs.Longitude[3];	
	lait_old=(Gps_Gprs.Latitude[0]<<24)+(Gps_Gprs.Latitude[1]<<16)+(Gps_Gprs.Latitude[2]<<8)+Gps_Gprs.Latitude[3];;
	longi_new=(Temp_Gps_Gprs.Longitude[0]<<24)+(Temp_Gps_Gprs.Longitude[1]<<16)+(Temp_Gps_Gprs.Longitude[2]<<8)+Temp_Gps_Gprs.Longitude[3];
	lati_new=(Temp_Gps_Gprs.Latitude[0]<<24)+(Temp_Gps_Gprs.Latitude[1]<<16)+(Temp_Gps_Gprs.Latitude[2]<<8)+Temp_Gps_Gprs.Latitude[3];
	if(UDP_dataPacket_flag==0x02) 
          GB_doubt_Data3_Trigger(lait_old,longi_old,lati_new,longi_new);   

 
   if(Illeagle_Data_kickOUT==0)
	   memcpy((char*)&Gps_Gprs,(char*)&Temp_Gps_Gprs,sizeof(Temp_Gps_Gprs));   
   else
	   Illeagle_Data_kickOUT=0;  // clear  

 

   // 3.  ��ʻ��¼������ݲ��� ����,  �Ҷ�λ�������   
   // VDR_product_08H_09H_10H();   

   
   //4.   ����Χ�� �ж�  ----------
  /*  if((Temp_Gps_Gprs.Time[2]%20)==0) //   ��֤ʱ�����Բ�ε���Χ��
    {
        CycleRail_Judge(Temp_Gps_Gprs.Latitude,Temp_Gps_Gprs.Longitude);
		//rt_kprintf("\r\n --- �ж�Բ�ε���Χ��");
    }	*/
    // if((Temp_Gps_Gprs.Time[2]==5)||(Temp_Gps_Gprs.Time[2]==25)||(Temp_Gps_Gprs.Time[2]==45)) //  
    if(Temp_Gps_Gprs.Time[2]%2==0)//    ��֤ʱҪ��2 ��
   {
        RectangleRail_Judge(Temp_Gps_Gprs.Latitude,Temp_Gps_Gprs.Longitude);
		//rt_kprintf("\r\n -----�жϾ��ε���Χ��"); 
    }
      if((Temp_Gps_Gprs.Time[2]%5)==0) //    
    {
          // printf("\r\n --- �ж�Բ�ε���Χ��");
          // RouteRail_Judge(Temp_Gps_Gprs.Latitude,Temp_Gps_Gprs.Longitude); 
          ;
    }
	//rt_kprintf("\r\n Delta_seconds %d \r\n",delta_time_seconds);    
  //----------------------------------------------------------------------------------------	
}


//-----------------------------------------------------------------
u8 Process_RMC(u8* packet)  
{
	u8  CommaCount=0,iCount=0,k=0; 
	u8  tmpinfo[15]; // �°汾�ı���ģ�龭�ȸ�����
	                          //$GNRMC,085928.00,A,3920.020977,N,11744.385579,E,0.7,,020113,,,A*67
	                          //$GNRMC,090954.00,A,3920.024800,N,11744.384457,E,0.3,,020113,,,A*65
	                          //           11744.385579    
	u8  hour=0,min=0,sec=0,fDateModify=0;

    //----------------------------------------------

    //----------------- Initial Speed and Direction -----------------    
    GPRMC_Funs.Speed(tmpinfo, INIT, k);	
    GPRMC_Funs.Direction(tmpinfo, INIT, k);  	
    //-------------------------------------------------------------------	
 	while (*packet!=0){ 
		if(*packet==','){
			CommaCount++;
			packet++; 
			if(iCount==0) continue; 
			switch(CommaCount){
				case 2: //ʱ�� 
				   //systemTickGPS_Set();    
				   if ( iCount < 6 ) 	  //  ��ʽ���
				  {    
				      StatusReg_GPS_V();  
					  return false;
				  }

				    if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)&&(tmpinfo[4]>=0x30)&&(tmpinfo[4]<=0x39)&&(tmpinfo[5]>=0x30)&&(tmpinfo[5]<=0x39)) 
				          ;
				   else
					{  StatusReg_GPS_V();   	return false;}

				       
					hour=(tmpinfo[0]-0x30)*10+(tmpinfo[1]-0x30)+7;  // Խ���Ƕ�7 ʱ��
					min=(tmpinfo[2]-0x30)*10+(tmpinfo[3]-0x30);
					sec=(tmpinfo[4]-0x30)*10+(tmpinfo[5]-0x30);
					if(hour>23)
					{ 
						fDateModify=1; 
						hour-=24;
						tmpinfo[0]=(hour/10)+'0';
						tmpinfo[1]=(hour%10)+'0';
					}
					//systemTickGPS_Clear();   
					//----------------------------------------------------
					GPRMC_Funs.Time(tmpinfo, hour, min, sec); 
                    //-----------------------------------------------------  
					break;
				case 3://������Ч��
					GPRMC_Funs.Status(tmpinfo);
					break;
				case 4://γ��
					if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)&&(tmpinfo[5]>=0x30)&&(tmpinfo[5]<=0x39)&&(tmpinfo[6]>=0x30)&&(tmpinfo[6]<=0x39)&&(tmpinfo[7]>=0x30)&&(tmpinfo[7]<=0x39)&&(tmpinfo[8]>=0x30)&&(tmpinfo[8]<=0x39))
						  ;
					else
						 break; 
					GPRMC_Funs.Latitude(tmpinfo);
					break;
				case 5://γ�Ȱ��� 
					GPRMC_Funs.Latitude_NS(tmpinfo);  
					break;
				case 6://���� 
				     if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)&&(tmpinfo[4]>=0x30)&&(tmpinfo[4]<=0x39)&&(tmpinfo[6]>=0x30)&&(tmpinfo[6]<=0x39)&&(tmpinfo[7]>=0x30)&&(tmpinfo[7]<=0x39)&&(tmpinfo[8]>=0x30)&&(tmpinfo[8]<=0x39)&&(tmpinfo[9]>=0x30)&&(tmpinfo[9]<=0x39))
								;
					else
						break; 
					GPRMC_Funs.Longitude(tmpinfo);
					break;
				case 7://���Ȱ��� 
					GPRMC_Funs.Longitude_WE(tmpinfo);
					break;
				case 8://���� 
						   for ( k = 0; k < iCount; k++ )
							{
									if ( tmpinfo[k] == '.' )
									{
											break;
									}
							} 						
							if(k>=iCount)
							{ 
								//break;
								k=0;
							}
                            GPRMC_Funs.Speed(tmpinfo, PROCESS,k);
							 
					break;
					
				case 9://���� 				    
						   
						   if ( iCount < 3 )			// ��ʽ���
							{
							    
								 break; 									
							}
						
							for ( k = 0; k < iCount; k++ )
							{
									if ( tmpinfo[k] == '.' )
									{
											break;
									}
							}
						 if(k>=iCount)
							  break;	
						 GPRMC_Funs.Direction(tmpinfo,PROCESS,k);
					break;		
				case 10://���� 
					 if((tmpinfo[0]>=0x30)&&(tmpinfo[0]<=0x39)&&(tmpinfo[1]>=0x30)&&(tmpinfo[1]<=0x39)&&(tmpinfo[2]>=0x30)&&(tmpinfo[2]<=0x39)&&(tmpinfo[3]>=0x30)&&(tmpinfo[3]<=0x39)&&(tmpinfo[4]>=0x30)&&(tmpinfo[4]<=0x39)&&(tmpinfo[5]>=0x30)&&(tmpinfo[5]<=0x39)) 
				          ;
					 else
					 	break;
					  GPRMC_Funs.Date(tmpinfo,fDateModify,hour,min,sec);
					break;
				default:
					break;
			}	
			iCount=0;
		}
		else
		{
			tmpinfo[iCount++]=*packet++;
			if(iCount<15)
			tmpinfo[iCount]=0;
			if(iCount>15)
			{  
				//return CommaCount;
				break;
			}	
		}
	} 
    //-----------------------------------------------------    
    GPS_Delta_DurPro();   //  ��GPS ��ʱ����  
    //-----------------------------------------------------
	return CommaCount;
} 
//---------------------
u8 Process_GGA(u8* packet)  
{	//�������������,ִ������ת��
	u8 CommaCount=0,iCount=0;
	u8  tmpinfo[12];
	float dop;	
	float Hight1=0,Hight2=0; 
	while (*packet!=0){
		if(*packet==','){
			CommaCount++;
			packet++;
			if(iCount==0)	 
			  { 
			     if(CommaCount==8)
			   	     Satelite_num=0;   
			   	continue;
			   }
			switch(CommaCount){
				case 8:
						        //�ѵ��ǵĸ���$GPGGA,045333,3909.1849,N,11712.3104,E,1,03,4.3,20.9,M,-5.4,M,,*66
						        Satelite_num = ( tmpinfo[0] - 0x30 ) * 10 + ( tmpinfo[1] - 0x30 );
							//	rt_kprintf("\r\n ���ǿ���:%d \r\n",Satelite_num);  
                                                                   //--------- ���� ���ǿ���  --------------
					                 BD_EXT.FJ_SignalValue=BD_EXT.FJ_SignalValue&0xf0;  // ����� GSM �ź�λ
							   BD_EXT.FJ_SignalValue|=	Satelite_num;  // ��4 λ

						        break;             

				case 9:	dop=atof((char *)tmpinfo); 
				        
						HDOP_value=dop;		 //  Hdop ��ֵ	  
				break;
				case 10:// MSL altitude
					Hight1=atof((const char*)tmpinfo);
				break;
				case 12:// Geoid Separation
					Hight2=atof((const char*)tmpinfo);
					GPS_Hight=(u16)(Hight1+Hight2);  	 				
					//printf("\r\n ��ǰ���θ߶�Ϊ:%f,  %f ,%d m\r\n",Hight1,Hight2,GPS_Hight); 
					break;
				default:
					break;
			}
			iCount=0;
		}else{
			tmpinfo[iCount++]=*packet++;
			tmpinfo[iCount]=0;
			if(iCount>12)
				return CommaCount;
		}
	}
	return CommaCount;


}
//------------------------------------------------------------------
void  GPS_ANTENNA_status(void)     //  ���߿���·״̬��� 
{
    // 2013-4-20    ����PCB   ��PD4 : GPS ���߿�·      PB6 : GPS  ���߶�·
   	if(GPIO_ReadOutputDataBit(GPS_PWR_PORT, GPS_PWR_PIN )) // ��GPS �е�ʱ��Ч   
		{
			if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4))//��·���	1:���߿�·
			{
				 if(OutGPS_Flag==0)
				 {
					   if((Warn_Status[3]&0x20)==0)
					         rt_kprintf("\r\n	ANT ��·"); 
					   Warn_Status[3]|=0x20;
					   Warn_Status[3]&=~0x40; 	 
					   GpsStatus.Antenna_Flag=1;
					   Gps_Exception.GPS_circuit_short_couter=0;
					   self_checking_Antenna=1;
					   self_checking_result=2;
			 	}  
			}
			else if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))//��·���  0:���߶�· 
			{
			         if(( Warn_Status[3]&0x40)==0)   
				     {
                        Gps_Exception.GPS_short_keepTimer++;
						if( Gps_Exception.GPS_short_keepTimer>200)     
					    {
					       Gps_Exception.GPS_short_keepTimer=0;  // clear 
					        if(GB19056.workstate==0)
					       rt_kprintf("\r\n	ANT ��·");     
						  // rt_kprintf("\r\n	���ֶ�·�������Ͽ�GPS��Դ");      
						   GPIO_ResetBits( GPS_PWR_PORT, GPS_PWR_PIN ); 
                           self_checking_Antenna=2;
						   self_checking_result=2;

						      //------------------------------------------ 
							  Gps_Exception.GPS_circuit_short_couter++;
							  if(Gps_Exception.GPS_circuit_short_couter>=4)  
							   {
									Gps_Exception.GPS_short_checkFlag=2;
									Gps_Exception.GPS_short_timer=0; // clear  
									rt_kprintf("\r\n   ��·������3�� ��һֱ�Ͽ�GPS ��Դ\r\n");   
									
									//	�Ͽ� GPS ��Դ�󣬵����� ���ض�ʱ �������˼�˵����.NND
									/*
							  
									   */ 
							   }	
							  else
							   {
									  Gps_Exception.GPS_short_checkFlag=1; 
							   } 
			                   //-----------------------------------------------------
                             
							   // set  flag 	
							   Warn_Status[3]&=~0x20;
							   Warn_Status[3]|=0x40;	 
							   //------------------------------------------
					     }
			         } 		
				   

			}
			else
			{
				  if(Warn_Status[3]&0x20)
				  	      rt_kprintf("\r\n	���߻ָ�����");   
	              Warn_Status[3]&=~0x20;
				  Warn_Status[3]&=~0x40;   
				  GpsStatus.Antenna_Flag=0;
				  Gps_Exception.GPS_circuit_short_couter=0;
			} 
			
		}
}

void  GPS_short_judge_timer(void)
{
       if(Gps_Exception.GPS_short_checkFlag==1)
       {
        Gps_Exception.GPS_short_timer++; 
		if(Gps_Exception.GPS_short_timer>100)
		{   //   �ص� ����
		     Gps_Exception.GPS_short_timer=0;  
		     gps_onoff(1);
		     rt_kprintf("\r\n	 �ٴο���GPSģ��\r\n"); 
			 //---------------�ڴ�ģ������-----------
               Warn_Status[3]&=~0x20;
			   Warn_Status[3]&=~0x40; 
			 //----------------
			 Gps_Exception.GPS_short_checkFlag=0; 
		} 
       }



}

void GPS_Rx_Process(void)
{
		 u8  Gps_instr[160];  
		 u8  GPRMC_Enable=0;	 
		 
							 memset(Gps_instr,0,sizeof(Gps_instr)); 	
							 memcpy(Gps_instr,Gps_str,gps_strLen); 
	
					if(GpsStatus.Raw_Output==1) 	  
							   rt_kprintf((const char*)Gps_instr);		  // rt_kprintf((const char*)Gps_str);		   
	
				   //----------------  Mode  Judge	  --------------------- 		 
					if(strncmp((char*)Gps_instr,"$GNRMC,",7)==0)
					{	
						 GpsStatus.Position_Moule_Status=3;
						 GPRMC_Enable=1;
						 Car_Status[1]&=~0x0C; // clear bit3 bit2 
						 Car_Status[1]|=0x0C; // BD+GPS  mode	1100		 
					}
					if(strncmp((char*)Gps_instr,"$BDRMC,",7)==0)
					{	
						   GpsStatus.Position_Moule_Status=1;
						   GPRMC_Enable=1;
						Car_Status[1]&=~0x0C; // clear bit3 bit2 
						Car_Status[1]|=0x08; // BD mode 1000	   
						}
					if(strncmp((char*)Gps_instr,"$GPRMC,",7)==0)	
					{	
						   GpsStatus.Position_Moule_Status=2;	
						   GPRMC_Enable=1;
						   Car_Status[1]&=~0x0C; // clear bit3 bit2 	 1100 
						   Car_Status[1]|=0x04; // Gps mode   0100 
					}		  
					
					//-------------------------------------------------- 
								//----------- Pick up useful  --------------------------
					  if(GPRMC_Enable==1) 
						{
						Process_RMC(Gps_instr); 	
						Gps_Exception.current_datacou+=gps_strLen;
									  return;
						}	
					 if((strncmp((char*)Gps_instr,"$GPGGA,",7)==0)||(strncmp((char*)Gps_instr,"$GNGGA,",7)==0)||(strncmp((char*)Gps_instr,"$BDGGA,",7)==0))  
					 {	 
						   Process_GGA(Gps_instr);	
							return; 
					 }
					if((strncmp((char*)Gps_instr,"$GPGSA,",7)==0)||(strncmp((char*)Gps_instr,"$BDGSA,",7)==0)||(strncmp((char*)Gps_instr,"$GNGSA,",7)==0))
					  {   
						Process_GSA(Gps_instr);
						  return;	
					  }



}

void GPS_service(void)
{
  u8  gps_rx[160]; 
  u8  res;
   // 1. rx data
   res = rt_mq_recv( &mq_gps, (void*)&gps_rx, 124, 2 ); //�ȴ�100ms,ʵ���Ͼ��Ǳ䳤����ʱ,�100ms
   if(res== RT_EOK )													 //�յ�һ������
   {
		//---------------------------------
		   GPS_Rx_Process( gps_rx.body, gps_rx.wr );  
		   GPS_Abnormal_process();//  GPS ģ���쳣��� 
   
   }
   
   //2. GPS_ANTENNA_status	 
		   GPS_ANTENNA_status();	 
		   GPS_short_judge_timer();  

}

void  GPS_Illegal_Movement_checking(void)
{    // 1s timer �Ƿ�λ�Ƽ��
   // u16  dis_per_second=0;  // ÿ����ʻ�ľ���
   Illegal__counter++;
   if(Illegal__counter<10)
   	   return;
   Illegal__counter=0;  //  10 x 100ms  

    //  Below  1s  1   ��
     if((!(work_var.Status_Word&STATUS_ACC))&&(gps_pos.AorV==A)&&(gps_pos.satelateNum>=5))
     {  // ACC �ر�����£� ��λ�� �ѵ��������Ǵ���5 ������ ���ſ�ʼ�Ƿ�λ�Ƶļ���
        if(gps_pos.speed_KmH>5)   // �ٶȴ���5 ����ÿСʱ�������
        {
           //dis_per_second=(gps_pos.speed_KmH*10)/36;  //  ÿ�����˶����� m
		   Illegal_moveDist_m+=(gps_pos.speed_KmH*10)/36;

		   if(Illegal_moveDist_m>SysConfig.illegalMove_dis)  
		   	{  //  �Ƿ�λ�ƴ���
                 work_var.Status_Word|=STATUS_IlLEGALMOV;  
		   	}
		   else
		   	   work_var.Status_Word&=~STATUS_IlLEGALMOV;  // clear
        }   
        else
			{
			   Illegal_moveDist_m=0;  // �ٶ�С��5  �Ƿ�λ�����Ϊ0
			   work_var.Status_Word&=~STATUS_IlLEGALMOV;
        	}   
		

     }
	 else
	 {   
           Illegal_moveDist_m=0;
		   work_var.Status_Word&=~STATUS_IlLEGALMOV;
	 }

}

void  GPS_Speed_Warn_Checking(void)
{     //  1s  timer
    
   GPS_spd_warn_timer++;
   if(GPS_spd_warn_timer<10)
   	   return;
   GPS_spd_warn_timer=0;  //  10 x 100ms  

    //  Below  1s  1   ��

   if(gps_pos.AorV==A)
   {
      if(gps_pos.speed_KmH>SysConfig.speedwarn_Max) // 
      {
           GPS_spd_warning_counter++;
		   if(GPS_spd_warning_counter>30)  // ��������ֵ������������30s  ��ȷ���ж�Ϊ���ٱ���
		   	{
		   	   work_var.Status_Word|=STATUS_SPDWARN;
               work_var.GPRS_Send_Flag=1;  // enable send
		   	}   
      }
	  else
	  { 
	     GPS_spd_warning_counter=0;  
		 work_var.Status_Word&=~STATUS_SPDWARN;   
	  }	  
   }
   else
   {
       GPS_spd_warning_counter=0;  
	   work_var.Status_Word&=~STATUS_SPDWARN;       
   }

}

void  GPS_Module_LED_timer(void)
{   //  100 ms   timer
   u8  dur=0;
    // ģʽ1 ��ģʽ2  �¿صȣ� ����ģʽ�¹رյ� ʡ��
   if((work_var.Mode==MODE1)||(work_var.Mode==MODE2))
   {      
      if(A)
          dur=3;    // ���ƿ�
      else
          dur=5;    // ������
	                
     if((GPS_Led_timer%dur)==0)
	 	 Trker_HwDrv_gpsLED_Ctrl(1);
	 else
	 	 Trker_HwDrv_gpsLED_Ctrl(0);
	 
	 GPS_Led_timer++
	 if(GPS_Led_timer>5)
	 	        GPS_Led_timer=0;  
   }
   else
   	   Trker_HwDrv_gpsLED_Ctrl(0); 
}


