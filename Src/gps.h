/*
     Trakcer   gps  part   
*/

#include  "Hardware.h"

#define  A       1
#define  V       0


typedef struct  _POS  
{
 u16  speed_original;
 u16  speed_KmH;       //  km/h
 u8   AorV;            //  ÓÐÐ§×´Ì¬
 u32  lati;
 u32  longi;
 u16  direction; 
 u8   year;
 u8   month;
 u8   day;
 u8   hour;
 u8   min;
 u8   sec;
 u8   satelateNum;  // ÎÀÐÇ¿ÅÊý

}POS;

extern  POS  gps_pos;

extern  void  GPS_service(void);
extern  void  GPS_Module_LED_timer(void);
extern  void  GPS_Speed_Warn_Checking(void);
extern  void  GPS_Illegal_Movement_checking(void);


