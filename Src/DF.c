//  Dataflash  32 Mbits    Operation

#include "main.h"

static unsigned char  OneSectorReg[4096]; 
unsigned char	  reg_4096[4096];  








// part 1:   Devce  Driver     器件级别驱动


void SST25V_DBSY(void); 



void SST25V_DBSY(void)
{ 
  SST25V_CS_LOW();
  SPI_Flash_SendByte(DBSY);
  SST25V_CS_HIGH();
}

 void SST25V_Init(void)
 {
       SPI__HARD_INIT()
	   SST25V_DBSY(); 
 }

unsigned char SPI_Flash_SendByte(unsigned char data)
{
  #if  0
    //Wait until the transmit buffer is empty
    while (SPI_I2S_GetFlagStatus(DF_SPI, SPI_I2S_FLAG_TXE) == RESET);
    // Send the byte
    SPI_I2S_SendData(DF_SPI, data);

    //Wait until a data is received
    while (SPI_I2S_GetFlagStatus(DF_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    // Get the received data
    data = SPI_I2S_ReceiveData(DF_SPI);

    // Return the shifted data
#endif	
    return data;
}
unsigned char SPI_Flash_ReceiveByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}


unsigned char SST25V_ByteRead(u32 ReadAddr)
{
  unsigned char Temp = 0;
  SST25V_CS_LOW();
  SPI_Flash_SendByte(Read_Data);
  SPI_Flash_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((ReadAddr& 0xFF00) >> 8);
  SPI_Flash_SendByte(ReadAddr & 0xFF);
  
  Temp = SPI_Flash_ReceiveByte();
  SST25V_CS_HIGH();
  return Temp;
}

void SST25V_BufferRead(unsigned char* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
     u32  timeout=50000;
   // note:  befor read  operation, please  check state
   while((SST25V_ReadStatusRegister()&0x01)&&(timeout--))       
   	   ;
  SST25V_CS_LOW();
  SPI_Flash_SendByte(Read_Data);
  SPI_Flash_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((ReadAddr& 0xFF00) >> 8);
  SPI_Flash_SendByte(ReadAddr & 0xFF);

  while(NumByteToRead--)
  {
    *pBuffer = SPI_Flash_ReceiveByte();
    pBuffer++;
  }
  SST25V_CS_HIGH();
}

/************************************************************************/
/* 名称: Byte_Program						*/
/* 功能: 写一个字节数据,被写的地址必须被擦除及未被保护		*/				
/* 输入:								*/
/*		Dst:		(目标地址 000000H - 1FFFFFH)	*/
/*		byte:		数据			*/
/* 返回:								*/
/*		Nothing							*/
/************************************************************************/
void SST25V_ByteWrite(unsigned char Byte, u32 WriteAddr)
{
  SST25V_WriteEnable();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(Byte_Program);
  SPI_Flash_SendByte((WriteAddr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((WriteAddr & 0xFF00) >> 8);
  SPI_Flash_SendByte(WriteAddr & 0xFF);
  
  SPI_Flash_SendByte(Byte);
  SST25V_CS_HIGH();  
  SST25V_WaitForWriteEnd(); 
}

void SST25V_BufferWrite(unsigned char* pBuffer, u32 WriteAddr, u16 NumByteToRead)
{
  u32  i=0;
 
  for(i=0;i<NumByteToRead;i++)
  {
     SST25V_ByteWrite(*pBuffer,WriteAddr+i);
     pBuffer++;
  }
}


void SST25V_strWrite(unsigned char *p, u32 WriteAddr,u16 length)
{
  SST25V_WriteEnable();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(Byte_Program);
  SPI_Flash_SendByte((WriteAddr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((WriteAddr & 0xFF00) >> 8);
  SPI_Flash_SendByte(WriteAddr & 0xFF);
  while(length--)
    {  
       SPI_Flash_SendByte(*p); 
        p++;
    }
  
  
  SST25V_CS_HIGH();  
  SST25V_WaitForWriteEnd(); 
}
void SST25V_SectorErase_4KByte(u32 Addr)
{
  SST25V_WriteEnable();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(SectorErace_4KB);
  SPI_Flash_SendByte((Addr & 0xFF0000) >> 16); 
  SPI_Flash_SendByte((Addr & 0xFF00) >> 8);
  SPI_Flash_SendByte(Addr & 0xFF);
  
  SST25V_CS_HIGH();
  SST25V_WaitForWriteEnd();
}

void SST25V_BlockErase_32KByte(u32 Addr)
{
  SST25V_WriteEnable();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(BlockErace_32KB);
  SPI_Flash_SendByte((Addr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((Addr & 0xFF00) >> 8);
  SPI_Flash_SendByte(Addr & 0xFF);
  
  SST25V_CS_HIGH();
  SST25V_WaitForWriteEnd();
}

void SST25V_BlockErase_64KByte(u32 Addr)
{
  SST25V_WriteEnable();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(BlockErace_64KB);
  SPI_Flash_SendByte((Addr & 0xFF0000) >> 16);
  SPI_Flash_SendByte((Addr & 0xFF00) >> 8);
  SPI_Flash_SendByte(Addr & 0xFF);
  
  SST25V_CS_HIGH();
  SST25V_WaitForWriteEnd();
}

unsigned char SST25V_ReadStatusRegister(void)
{
  unsigned char StatusRegister = 0;
  SST25V_CS_LOW();
  SPI_Flash_SendByte(ReadStatusRegister);
  StatusRegister = SPI_Flash_ReceiveByte();
  SST25V_CS_HIGH();
  return StatusRegister;
}

void SST25V_WriteEnable(void)
{
  SST25V_CS_LOW();
  SPI_Flash_SendByte(WriteEnable);
  SST25V_CS_HIGH();
}

void SST25V_WriteDisable(void)
{
  SST25V_CS_LOW();
  SPI_Flash_SendByte(WriteDisable);
  SST25V_CS_HIGH();
}

void SST25V_EnableWriteStatusRegister(void)
{
  SST25V_CS_LOW();
  SPI_Flash_SendByte(EnableWriteStatusRegister);
  SST25V_CS_HIGH();
}

void SST25V_WriteStatusRegister(unsigned char Byte)
{
  SST25V_EnableWriteStatusRegister();
  SST25V_CS_LOW();
  SPI_Flash_SendByte(WriteStatusRegister);
  SPI_Flash_SendByte(Byte);
  SST25V_CS_HIGH();
}

void SST25V_WaitForWriteEnd(void)
{
  unsigned char FLASH_Status = 0;
  u32 count=0x80000;//0x250000;   
  
  SST25V_CS_LOW();
  SPI_Flash_SendByte(ReadStatusRegister);
  do
  {
    FLASH_Status = SPI_Flash_SendByte(Dummy_Byte);

  } while(((FLASH_Status & WriteStatusRegister) == SET)&& (--count)); 

  SST25V_CS_HIGH(); 
}

/*
      note:  使用前提是   addr+len 不允许超过  本扇区
*/
unsigned char  SST25V_OneSector_Write(unsigned char *p,  u32  addr,  u32 len)
{ 
   #if 0
   u32   SectorStartAddr=addr&0xFFFFFFFFFFFFF000,i=0;  //  获取起始扇区的起始地址
   u32   insector_offset=addr&0xFFF; // 取扇区内偏移地址 



      //  1.  get   4096 to buf
       WatchDog_Feed();
       SST25V_BufferRead(OneSectorReg,SectorStartAddr,4096);
       WatchDog_Feed();
	   
	  // printf("\r\n insector_offset=0x%X  \r\n",insector_offset);   
	   //OutPrint_HEX("\r\n 读取前",OneSectorReg,4096);  
       delay_ms(35);
       WatchDog_Feed();
	  
	  // 2. 把更新的内容给填进去
	   memcpy(OneSectorReg+insector_offset,p,len);  

	  // 3.  erase  Sector
	  	 SST25V_SectorErase_4KByte(SectorStartAddr);	
	     WatchDog_Feed();
	     DF_delay_ms(130); 	 
		// OutPrint_HEX("\r\n 擦完写前",OneSectorReg,4096);  
	  // 4.  write  buf  to  DF
	  // SST25V_strWrite(OneSectorReg,SectorStartAddr,4096);
	   SST25V_BufferWrite(OneSectorReg,SectorStartAddr,4096); 
	   DF_delay_ms(130);    
      

         //---------------- add   for  debug----------------       
	    WatchDog_Feed();
	    SST25V_BufferRead(reg_4096,SectorStartAddr,4096); 
		DF_delay_ms(50);    
      
	     //  rt_thread_delay(2);	  
		//   OutPrint_HEX("\r\n 读取后",OneSectorReg,4096);     
		   for(i=0;i<4096;i++)
		   {
		       if(OneSectorReg[i]!=reg_4096[i])
		       	{
		       	    printf("\r\n Error at =0x%X \r\n",i);   
	                break;
		       	}
		   }	   
	 
       //----------- debug --------------------------------------------------  
					
		   WatchDog_Feed();
	      delay_ms(5);
	  //--------------------------------
	  #endif
	  return  true;
}








//  part2 :    API    Driver    应用级别的驱动

void DF_init(void)
{
     u32 device_id=0;	

      SST25V_Init();    

     SST25V_CS_LOW();
     SPI_Flash_SendByte(WriteDisable);
     SST25V_CS_HIGH();

     SST25V_CS_LOW();

    //-----erase chip-------------------
    //	DF_delay_ms(350);      
    //--------------------------------	
	
     SPI_Flash_SendByte( ReadJedec_ID  );
    device_id  = SPI_Flash_SendByte(0xFF)<<16;
    device_id = device_id | SPI_Flash_SendByte(0xFF)<<8;
    device_id = device_id | SPI_Flash_SendByte(0xFF);	 
    SST25V_CS_HIGH();

    //if(device_id == 0xBF254A)//SST25VF032B = 0xBF254A,
    {
        SST25V_CS_LOW();
        SPI_Flash_SendByte( DBSY );
        SST25V_CS_HIGH();

        SST25V_CS_LOW();
        SPI_Flash_SendByte( EnableWriteStatusRegister );
        SST25V_CS_HIGH();

        SST25V_CS_LOW();
        SPI_Flash_SendByte( WriteStatusRegister );
        SPI_Flash_SendByte( 0 );
        SST25V_CS_HIGH();
    }

    delay_ms(700); 

}
void DF_delay_us(u16 j)
{
u8 i;
  while(j--)
  	{
  	i=3;
  	while(i--);
  	}  
}

void DF_delay_ms(u16 j)
{
  while(j--)
  	{
           DF_delay_us(2000);// 1000
  	} 
   
}

u8 DF_Spi_Tranbyte(u8 BYTE)
{
//USART_TxStr(USART1,"\r\nSpi_Tranbyte\r\n");
   /*
   BSPI1->TXR=(u16)(BYTE<<8);
   while(((BSPI1->CSR2)&0x10)!=0x10);
   return (BSPI1->RXR)>>8;
   */
//Set_AT45_CLK_1;

   while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
//USART_TxStr(USART1,"\r\nSpi_Tranbyte111111111\r\n");
//GPIO_SetBits(GPIOB,GPIO_Pin_3);
   SPI_I2S_SendData(SPI3,(u16)BYTE);
//USART_TxStr(USART1,"\r\nSpi_Tranbyte2222222222\r\n");
//   DF_delay_us(400);

//Set_AT45_CLK_0;
//GPIO_ResetBits(GPIOB,GPIO_Pin_3);
   while ( (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET ) );
   return SPI_I2S_ReceiveData(SPI3);
//   DF_delay_us(400);
//USART_TxStr(USART1,"\r\nSpi_Tranbyte_over\r\n");
}


 
void DF_ReadFlash(u16 page_counter,u16 page_offset,u8 *p,u16 length)
{   
	u16 i=0;
	u32 ReadAddr=((u32)page_counter*PageSIZE)+(u32)page_offset;
   
	//------ Byte Read-------
	#if 0
	for(i=0;i<length;i++)
	{
		 *p=SST25V_ByteRead(((u32)page_counter*PageSIZE)+(u32)(page_offset+i));//512bytes 一个单位
		 p++;
	}	
    //DF_delay_us(200);   
	#endif
    //----  Addr +  content 内容对比----- 
    SST25V_BufferRead(p,ReadAddr,length);  	
    DF_delay_ms(5);  

}

void DF_WriteFlash(u16 page_counter,u16 page_offset,u8 *p,u16 length)//512bytes 一个单位 
{
   u16 i=0,j=0,k=0;   //写补报

	for(i=0;i<8;i++)
	{
	    DF_ReadFlash((8*(page_counter/8))+i,0,SectorBuf_save[i],DFBakSize);//PageSIZE 
	}	
	
	SST25V_SectorErase_4KByte((8*((u32)page_counter/8))*PageSIZE);
	DF_delay_ms(100);          
	for(j=0;j<8;j++)
	{
	    if(j==(page_counter%8))
	    {
			for(i=0;i<length;i++)
			{			 
				SectorBuf_save[j][page_offset+i]=*p;    
				p++; 
			}			
	    } 	    	
		for(k=0;k<DFBakSize;k++)              
		{
			SST25V_ByteWrite(SectorBuf_save[j][k],((8*(page_counter/8))+j)*PageSIZE+k);  
		}		
	}  
	DF_delay_ms(20);  
} 
void DF_WriteFlashSector(u16 page_counter,u16 page_offset,u8 *p,u16 length) //512bytes 直接存储
{
	u16 i=0;
    
    // 要擦除所在的sector(要求pagecounter 为所在sector的第一个page) ，然后该sector的第一个page写
	SST25V_SectorErase_4KByte((8*((u32)page_counter/8))*PageSIZE);	
	DF_delay_ms(200); 
	
	for(i=0;i<length;i++)
	{
		SST25V_ByteWrite(*p,page_counter*512+i+page_offset);
		p++;
	}
	DF_delay_ms(30);  
	
}
void DF_Erase(void)
{
    u16 i=0;
      /*
           1. 从0x1000    4K  开始擦除28K     7 sector
           2.擦除56个扇区   7 个32k block     
        */

        printf("\r\n  ISP erase start\r\n");
        //  -----erase        28 K   area  -------------
         for(i=0;i<7;i++)   // 7  secotor
        {
            WatchDog_Feed();
            SST25V_SectorErase_4KByte(ISP_StartArea+i*0x1000);
	        DF_delay_ms(220);  
	        WatchDog_Feed();		
        }
      //----------- erase  32k     
        for(i=0;i<7;i++)    // 56sector
        {
        WatchDog_Feed();	
            SST25V_BlockErase_32KByte(0x8000+i*0x8000);
			DF_delay_ms(800); 
			WatchDog_Feed();	
        }	   
       printf("\r\n  ISP erase OK DFReady\r\n"); 
}
//FINSH_FUNCTION_EXPORT(DF_Erase, DF_Erase);      

void DF_WriteFlashDirect(u16 page_counter,u16 page_offset,u8 *p,u16 length)//512bytes 直接存储
{
    u16 i=0;
      
	for(i=0;i<length;i++)
	{
		SST25V_ByteWrite(*p,(u32)page_counter*PageSIZE+(u32)(page_offset+i));
		p++;
	}
	DF_delay_ms(5);  
}

// part3:   sysconfigure 


