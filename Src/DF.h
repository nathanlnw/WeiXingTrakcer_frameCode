//   DF   .h

// part 1:   Devce  Driver     ������������

//  1.   Total  Define 
#define   DF_SPI           SPI2
//------------------------------------------------------------------
#define Read_Data                     0x03       //��ȡ�洢������
#define HighSpeedReadData             0x0B       //���ٶ�ȡ�洢������
#define SectorErace_4KB               0x20       //��������
#define BlockErace_32KB               0x52       //32KB�����
#define BlockErace_64KB               0xD8       //64KB�����
#define ChipErace                     0xC7       //Ƭ����
 
#define Byte_Program                  0x02       //ҳ����--д����  
#define AAI_WordProgram               0xAD
#define ReadStatusRegister            0x05       //��״̬�Ĵ���
#define EnableWriteStatusRegister     0x50
#define WriteStatusRegister           0x01       //д״̬�Ĵ���

#define WriteEnable                   0x06       //дʹ�ܣ�����״̬�Ĵ���
#define WriteDisable                  0x04       //д��ֹ
#define SST25_WRDI		              0x04       //  �˳�AAI ģʽ
#define ReadDeviceID                  0xAB       //��ȡ�豸ID��Ϣ

#define ReadJedec_ID                  0x9F       //JEDEC��ID��Ϣ

#define EBSY                          0X70
#define DBSY                          0X80

#define Dummy_Byte                    0xA5 //0xFF

//  3.   PIN   
#define SST25V_CS_LOW()      GPIO_ResetBits(GPIOD,GPIO_Pin_14)  
#define SST25V_CS_HIGH()     GPIO_SetBits(GPIOD,GPIO_Pin_14)

#define SST25V_WP_HIGH()     GPIO_SetBits(GPIOD,GPIO_Pin_15)
#define SST25V_WP_LOW()      GPIO_SetBits(GPIOD,GPIO_Pin_15) 


extern void SST25V_Init(void);
extern unsigned char   SST25V_ByteRead(u32 ReadAddr);
extern void SST25V_strWrite(unsigned char *p, u32 WriteAddr,u16 length);    
extern unsigned char  SST25V_OneSector_Write(unsigned char *p,  u32  addr,  u32 len);

void SST25V_BufferRead(unsigned char* pBuffer, u32 ReadAddr, u16 NumByteToRead);

//extern unsigned char SPI_Flash_SendByte(unsigned char byte);
//extern unsigned char SPI_Flash_ReceiveByte(void);
extern void SST25V_ByteWrite(unsigned char Byte, u32 WriteAddr);
extern void SST25V_BufferWrite(unsigned char* pBuffer, u32 WriteAddr, u16 NumByteToRead); 

extern void SST25V_SectorErase_4KByte(u32 Addr); 
void SST25V_BlockErase_32KByte(u32 Addr);
void SST25V_BlockErase_64KByte(u32 Addr);

unsigned char SST25V_ReadStatusRegister(void);
void SST25V_WriteEnable(void);
void SST25V_WriteDisable(void);

void SST25V_EnableWriteStatusRegister(void);
void SST25V_WriteStatusRegister(unsigned char Byte);
void SST25V_WaitForWriteEnd(void);



extern unsigned char SPI_Flash_SendByte(unsigned char data);
extern unsigned char SPI_Flash_ReceiveByte(void);   





//  part2 :    API    Driver    Ӧ�ü��������

extern void DF_init(void);