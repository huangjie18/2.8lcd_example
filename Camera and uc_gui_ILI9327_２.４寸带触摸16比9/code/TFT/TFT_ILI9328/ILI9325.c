#include "stm32f10x.h"
#include "ili9325.h"
 
void DataToWrite(u16 data) 
{
	GPIOB->ODR=((GPIOB->ODR&0x00ff)|(data&0xff00));	
	Clr_nWr;
	Set_nWr;
	GPIOB->ODR=((GPIOB->ODR&0x00ff)|(data<<8));
}

/*
 * ��������LCD_GPIO_Config
 * ����  ������FSMC����LCD��I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* config tft BL gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  		   
    
    /* config tft data lines base on FSMC
	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /* tft control gpio init */	 
    GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // RST				
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
		GPIO_SetBits(GPIOD, GPIO_Pin_11);		 // RS
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1

		GPIO_SetBits(GPIOA, GPIO_Pin_1);	 	 //	BL = 1 
		GPIO_SetBits(GPIOD, GPIO_Pin_12);		 //	FLASH = 1��û�õ�SPI FlashҪʧ����
		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//���� OK
    GPIO_Init(GPIOA, &GPIO_InitStructure);
																// right          up           left        down
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6;		
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*
 * ��������LCD_FSMC_Config
 * ����  ��LCD  FSMC ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */

static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //��ַ����ʱ��
    p.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ��
    p.FSMC_DataSetupTime = 0x05;		 //���ݽ���ʱ��
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // һ��ʹ��ģʽB������LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

volatile void LCD_delay(u16 j)
{
volatile u16 i;	
	while(j--)
for(i=7200;i>0;i--);
}
static void LCD_Rst(void)
{			
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    LCD_delay(10);					   
    GPIO_SetBits(GPIOD, GPIO_Pin_13 );		 	 
    LCD_delay(10);	
}
static void WriteComm(u16 CMD)
{			
	*(__IO u16 *) (Bank1_LCD_C) = CMD;
}
static void WriteData(u16 tem_data)
{			
	*(__IO u16 *) (Bank1_LCD_D) = tem_data>>8;
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}

static void WriteCMDData(u16 tem_data)
{			
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}
/**********************************************
Lcd��ʼ������
***********************************************/
void Lcd_Initialize(void)
{	
LCD_GPIO_Config();
LCD_FSMC_Config();
LCD_delay(20);
LCD_Rst();
	
WriteComm(0xE9);
WriteCMDData(0x20);
WriteComm(0x11); //Exit Sleep
LCD_delay(100);
WriteComm(0xD1);
WriteCMDData(0x00);
WriteCMDData(0x79);
WriteCMDData(0x1E);

WriteComm(0xD0);
WriteCMDData(0x05);
WriteCMDData(0x01);
WriteCMDData(0x01);

WriteComm(0x36);         //������ʾ����
WriteCMDData(0x28);

WriteComm(0x3A);
WriteCMDData(0x55);

WriteComm(0xC1);
WriteCMDData(0x10);
WriteCMDData(0x10);
WriteCMDData(0x02);
WriteCMDData(0x02);
WriteComm(0xC0); //Set Default Gamma
WriteCMDData(0x00);
WriteCMDData(0x35);
WriteCMDData(0x00);
WriteCMDData(0x00);
WriteCMDData(0x01);
WriteCMDData(0x02);

WriteComm(0xC5); //Set frame rate
WriteCMDData(0x00);
WriteComm(0xD2); //power setting
WriteCMDData(0x01);
WriteCMDData(0x22);

WriteComm(0xC8); //Set Gamma
WriteCMDData(0x04);
WriteCMDData(0x67);
WriteCMDData(0x35);
WriteCMDData(0x04);
WriteCMDData(0x08);
WriteCMDData(0x06);
WriteCMDData(0x24);
WriteCMDData(0x01);
WriteCMDData(0x37);
WriteCMDData(0x40);
WriteCMDData(0x03);
WriteCMDData(0x10);
WriteCMDData(0x08);
WriteCMDData(0x80);
WriteCMDData(0x00);
WriteComm(0x2A); 
WriteCMDData(0x00);
WriteCMDData(0x00);
WriteCMDData(0x00);
WriteCMDData(0xeF);
WriteComm(0x2B); 
WriteCMDData(0x00);
WriteCMDData(0x00);
WriteCMDData(0x01);
WriteCMDData(0x8F);
WriteComm(0x29); //display on 
LCD_delay(0x200);	
	Lcd_Light_ON;
	
	Lcd_ColorBox(0,0,400,240,YELLOW);

}
/******************************************
��������Lcdд�����
���ܣ���Lcdָ��λ��д��Ӧ�����������
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
��������Lcdд��ʼ����
���ܣ�����Lcd�������� ִ��д����
************************************************/
void Lcd_WR_Start(void)
{
*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//HX8352-C

	WriteComm(0x2a);   
	WriteCMDData(Xstart>>8);
	WriteCMDData(Xstart&0xff);
	WriteCMDData(Xend>>8);
	WriteCMDData(Xend&0xff);

	WriteComm(0x2b);   
	WriteCMDData(Ystart>>8);
	WriteCMDData(Ystart&0xff);
	WriteCMDData(Yend>>8);
	WriteCMDData(Yend&0xff);
	
	WriteComm(0x2c);
}
u16 GetPoint(u16 x,u16 y)
{
volatile u16 a;
	
	WriteComm(0x2a);   
	WriteData(x>>8);
	WriteData(x&0xff);

	WriteComm(0x2b);   
	WriteData(y>>8);
	WriteData(y&0xff);
	a=0;a=0;a=0;a=0;a=0;
	WriteComm(0x2E);
	a=0;a=0;a=0;a=0;a=0;a=0;
	a = *(__IO u16 *) (Bank1_LCD_D);
	a=0;a=0;a=0;a=0;a=0;
	a = *(__IO u16 *) (Bank1_LCD_D);
	return a;
// 	 return (a&0xf800)|((a&0x03f0)<<1)|((a&0x3)<<3)|(b>>13);  
}
/******************************************
��������Lcdͼ�����100*100
���ܣ���Lcdָ��λ�����ͼ��
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;

// 	WriteComm(0x36); //Set_address_mode
// 	WriteInitData(0x20); //�����������½ǿ�ʼ�������ң����µ���
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = pic[i];
	}
// 	WriteComm(0x36); //Set_address_mode
// 	WriteInitData(0x60);
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = Color>>8;
		*(__IO u16 *) (Bank1_LCD_D) = Color;
	}
}




/************************************************
��������Lcdд��ʼ����
���ܣ�����Lcd�������� ִ��д����
��ڲ�������
����ֵ����
************************************************/
void Lcd_CMD_WR_Start(void)
{
	Clr_Cs;
	Clr_Rs;
	DataToWrite(0x0000);
	Clr_nWr;
	Set_nWr;
	Set_Rs;
}



/**********************************************
��������Lcdȫ����������
���ܣ���Lcd������Ϊָ����ɫ
��ڲ�����color ָ��Lcdȫ����ɫ RGB(5-6-5)
����ֵ����
***********************************************/
void Lcd_Clear(u16 Color)
{
	u32 temp;
	
	Lcd_Start();
	if((Color>>8)==(Color&0x00ff))
	{
		DataToWrite(Color);
		for (temp = 0; temp < 76800*2-1; temp++)
		{
			Clr_nWr;
			Set_nWr;
		}
	}	
	else
	{
		for (temp = 0; temp < 76800; temp++)
		{
			DataToWrite(Color);
			Clr_nWr;
			Set_nWr;
		}
	}

}
/*************************************************
��������Lcd�����㶨λ����
���ܣ�ָ��320240Һ���ϵ�һ����Ϊд���ݵ���ʼ��
��ڲ�����x ���� 0~239
          y ���� 0~319
����ֵ����
*************************************************/
void Lcd_SetCursor(u16 x,u16 y)
{ 
	//�����ת
	LCD_WR_REG(0x20,y);//ˮƽ����
	LCD_WR_REG(0x21,x);//��ֱ����  
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/


void Lcd_SetBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 x_offset,u16 y_offset)
{
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset);
	LCD_WR_REG(0x0050,yStart+y_offset);//ˮƽGRAM��ʼλ��
	LCD_WR_REG(0x0051,yStart+yLong-1+y_offset);//ˮƽGRAM��ֹλ�� 

    //x,y����Ե� 
	LCD_WR_REG(0x0052,xStart+x_offset);//��ֱ GRAM��ʼλ��
	LCD_WR_REG(0x0053,xStart+xLong-1+x_offset);//��ֱGRAM��ֹλ��

	
}

void Lcd_ClearCharBox(u16 x,u16 y,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(x*8,y*16,8,16,0,0); 
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp < 128; temp++)
	{
		DataToWrite(Color); 
		Clr_nWr;
		Set_nWr;
	}
}

////////////////////////////////////////////////////////////////////////////////
void DrawPixel(u16 x, u16 y, int Color)
{
	BlockWrite(x,x,y,y);
	*(__IO u16 *) (Bank1_LCD_D) = Color>>8;
	*(__IO u16 *) (Bank1_LCD_D) = Color;	
}

//��ʼȫ��д,���õ�ַ
void Lcd_Start(void)
{  
	Lcd_SetCursor(0,239);     
	LCD_WR_REG(0x0050,0x00);//��ֱ GRAM��ʼλ��
	LCD_WR_REG(0x0051,239);//��ֱGRAM��ֹλ��
	LCD_WR_REG(0x0052,0x00);//ˮƽGRAM��ʼλ��
	LCD_WR_REG(0x0053,319);//ˮƽGRAM��ֹλ��   
	Lcd_WR_Start();
}


/****************************************************************************
* ��    �ƣ�void Put32_32HZ(u16 x,u16 y,const unsigned char *c,u16 charColor,u16 bkColor,u8 flag)
* ��    �ܣ���ָ��������ʾһ������ĺ���
* ��ڲ�����x          ������
*           y          ������
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ʾ��ascii��
* ���÷�����ssd1289_PutChar(10,10,'a',0x0000,0xffff, 1);
****************************************************************************/
// void Put32_32HZ(u16 x,u16 y,const unsigned char *c,u16 charColor,u16 bkColor,u8 flag)
// {
//   u8 i,j,k; 
//   u8 tmp_char=0;

//   Lcd_SetBox(x,y,32,32,0,0);
//   Lcd_WR_Start();
//   Set_Rs;

//   for (i=31;i>=0;i--)//32��
//   {
//     for (j=3;j>=0;j--)//ÿ��4�ֽ�
//     {
//       tmp_char=c[4*i+j];
//       for (k=0;k<8;k++)//ÿ�ֽ�8λ
//       {
//         if (((tmp_char << k) & 0x80) == 0x80)
//         {
// 		  DataToWrite(charColor);
// 		  Clr_nWr;
// 	  	  Set_nWr;
//         }
//         else
//         {
// 		   if(flag){
// 			 DataToWrite(bkColor);
// 		     Clr_nWr;
// 	  	     Set_nWr;
// 		   }
//         }
//       }
//     }
//   }

//   Set_Cs;

// }

/****************************************************************************
* ��    �ƣ�void Put16_16HZ(u16 x,u16 y,const unsigned char *c,u16 charColor,u16 bkColor,u8 flag)
* ��    �ܣ���ָ��������ʾһ������ĺ���
* ��ڲ�����x          ������
*           y          ������
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ʾ��ascii��
* ���÷�����ssd1289_PutChar(10,10,'a',0x0000,0xffff, 1);
****************************************************************************/
void Put16_16HZ(u16 x,u16 y,const unsigned char *c,u16 charColor,u16 bkColor,u8 flag)
{
  char i,j,k;
  
  u8 tmp_char=0;

  Lcd_SetBox(x,y,16,16,0,0);
  Lcd_SetCursor(x,y+15);
  Lcd_WR_Start();
  Set_Rs;

  for (i=0;i<16;i++)//16��
  {
    for (j=0;j<2;j++)//ÿ��2�ֽ�
    {
      tmp_char=c[2*(15-i)+j];
      for (k=0;k<8;k++)//ÿ�ֽ�8λ
      {
        if (((tmp_char << k) & 0x80) == 0x80)
        {
		  DataToWrite(charColor);
		  Clr_nWr;
		  Set_nWr;
        }
        else
        {
			 DataToWrite(bkColor);
		     Clr_nWr;
		     Set_nWr;
        }
      }
    }
  }
  Set_Cs;

}


void Put8_16Char(u16 x,u16 y,const unsigned char *ascii_8x16,u8 ch,u16 charColor,u16 bkColor,u8 flag)
{
  char i=0,j=0;
  u8 tmp_char=0;

  Lcd_SetBox(x,y,8,16,0,0);
  Lcd_SetCursor(x,y+15);
  Lcd_WR_Start();
  Set_Rs;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((ch-0x20)*16)+(15-i)];
    for (j=0;j<8;j++)
    {
        if(((tmp_char<<j) & 0x80) == 0x80)
        {
		  DataToWrite(charColor);
		  Clr_nWr;
		  Set_nWr;
        }
        else
        {
		   	 DataToWrite(bkColor);  //0x7BEF
		     Clr_nWr;
		     Set_nWr;
        }
    }
  }
  Set_Cs;
}



void Put16x32Num(u16 x,u16 y,const unsigned char *num_16x32,u8 ch,u16 charColor,u16 bkColor,u8 flag)
{
  char i=0,j=0,k=0;
  u8 tmp_char=0;

  Lcd_SetBox(x,y,16,32,0,0);
  Lcd_SetCursor(x,y+31);
  Lcd_WR_Start();
  for (i=0;i<32;i++)
  {
    for (k=0;k<2;k++)
	{
      tmp_char=num_16x32[ch*32*2+(31-i)*2+k];
      for (j=0;j<8;j++)
      {
        if(((tmp_char<<j) & 0x80) == 0x80)
        {
		  DataToWrite(charColor);
		  Clr_nWr;
		  Set_nWr;
        }
        else
        {
		   	 DataToWrite(bkColor);  //0x7BEF
		     Clr_nWr;
		     Set_nWr;

        }
      }
	}
  }
}

void LCD_Scan_Set(char i)
{
	
  if(i==0)	{LCD_WR_REG(0x0003,0x1038);Lcd_SetCursor(0,239);}//
	if(i==1)	{LCD_WR_REG(0x0003,0x1028);Lcd_SetCursor(0,239);}//
  if(i==2)	{LCD_WR_REG(0x0003,0x1008);Lcd_SetCursor(319,0);}//
	if(i==3)	{LCD_WR_REG(0x0001,0x0100);return;}//
	LCD_WR_REG(0x0050,0x00);//��ֱGRAM��ʼλ��
	LCD_WR_REG(0x0051,239);//��ֱGRAM��ֹλ��
	LCD_WR_REG(0x0052,0x00);//ˮƽGRAM��ʼλ��
	LCD_WR_REG(0x0053,319);//ˮƽGRAM��ֹλ��   
	Lcd_WR_Start();
}
/*---------------------------------------------------------------------------
���ܣ�	��ֱ����
��ڲ�����line_num  ��������
---------------------------------------------------------------------------*/
void lcd_scrol_screen(unsigned int line_num)
{
	 LCD_WR_REG(0x006a, line_num);// set scrolling line	
	 LCD_WR_REG(0x0061, 0x0003);//enable scrol

//	 LCD_WR_REG(0x006a, line_num);// set scrolling line	
//	 LCD_WR_REG(0x0061, 0x0001);////disable scrol
}

/*---------------------------------------------------------------------------
���ܣ�	��ֹ��ֱ����
---------------------------------------------------------------------------*/
void lcd_scrol_disable(void)
{
	 LCD_WR_REG(0x006a, 0x0000);// set scrolling line	
	 LCD_WR_REG(0x0061, 0x0001);//disable scrol
}
