#include "mylcd.h"
#include "font.h"
#include "rtthread.h"

_lcd_dev lcd_dev;

void my_lcd_set_cursor(u16 xpos,u16 ypos)
{
	switch(lcd_dev.id)
	{
		case 0x9341:
			my_lcd_wr_reg(lcd_dev.setxcmd);
			my_lcd_wr_data(xpos >> 8);
			my_lcd_wr_data(xpos &0xff);
			my_lcd_wr_reg(lcd_dev.setycmd);
			my_lcd_wr_data(ypos >> 8);
			my_lcd_wr_data(ypos &0xff);
			break;
		default:
			if(lcd_dev.dir == 1) 
				xpos = lcd_dev.width - 1 - xpos;
			my_lcd_write_reg(lcd_dev.setxcmd,xpos);
			my_lcd_write_reg(lcd_dev.setycmd,ypos);
			break;
	}
}

void my_lcd_draw_point(u16 x,u16 y)
{
	my_lcd_set_cursor(x,y);
	my_lcd_write_ram_prepare();
	LCD->LCD_RAM = POINT_COLOR;
}

u16 my_lcd_read_point(u16 x,u16 y)
{
	u16 r,g,b;
	my_lcd_set_cursor(x,y);
	
	if(lcd_dev.id == 0x9341)
		my_lcd_wr_reg(0x2e);
	else
		my_lcd_wr_reg(0x22);	//R34
	
	if(lcd_dev.id == 0x9320)
		delay_us(2);
	
	if(LCD->LCD_RAM)		// dummy read
		r = 0;
	
	delay_us(2);
	
	if(lcd_dev.id == 0x9341)
	{
		delay_us(2);
		b = LCD->LCD_RAM;
		g = r & 0xFF;
		g <<= 8;
	}
	else
		my_lcd_wr_reg(0x22);	//R34
	
	if(lcd_dev.id == 0x4535 || lcd_dev.id == 0x4531 || lcd_dev.id == 0x8989 || lcd_dev.id == 0xb505)
		return r;
	else if(lcd_dev.id == 0x9341)
		return ( ((r >> 11)<<11) | ((g >> 10) << 5) |(b >> 11));
//	else
//		return LCD_BGR2RGB(r);
	return 1;
}

void my_lcd_show_char(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
	u8 temp,t1,t;
	u16 y0 = y;
	u16 color_tmp = POINT_COLOR;

	num = num - '0';
	
	if(!mode)
	{
		for(t=0;t<size;t++)
		{
			if(size == 12)
				temp = asc2_1206[num][t];
			else
				temp = asc2_1608[num][t];
			
			for(t1=0;t1<8;t1++)
			{
				if(temp & 0x80)
					POINT_COLOR = color_tmp;
				else
					POINT_COLOR = BACK_COLOR;
				
				my_lcd_draw_point(x,y);
				temp <<= 1;
				y++;
				
				if(y >= lcd_dev.height) return;
				if(y - y0 == size)
				{
					y = y0;
					x++;
					if(x >= lcd_dev.width) return;
					break;
				}
			}
		}
	}
	else
	{
		for(t=0;t<size;t++)
		{
			if(size == 12)
				temp = asc2_1206[num][t];
			else
				temp = asc2_1608[num][t];
			
			for(t1=0;t1<8;t1++)
			{
				if(temp & 0x80)
					my_lcd_draw_point(x,y);
			
				temp <<= 1;
				y++;
				
				if(y >= lcd_dev.height) return;
				if(y - y0 == size)
				{
					y = y0;
					x++;
					if(x >= lcd_dev.width) return;
					break;
				}
			}
		}
	}
	POINT_COLOR = color_tmp;
}

void LCD_Init()
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;
	FSMC_NORSRAMTimingInitTypeDef	Read_Write_Timing;
	FSMC_NORSRAMTimingInitTypeDef Write_Timing;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD 
		| RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 
		| GPIO_Pin_5 | GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 
		| GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_12;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	Read_Write_Timing.FSMC_AddressSetupTime = 0x01;
	Read_Write_Timing.FSMC_AddressHoldTime = 0x00;
	Read_Write_Timing.FSMC_DataSetupTime = 0x0f;
	Read_Write_Timing.FSMC_BusTurnAroundDuration = 0x00;
	Read_Write_Timing.FSMC_CLKDivision = 0x00;
	Read_Write_Timing.FSMC_DataLatency = 0x00;
	Read_Write_Timing.FSMC_AccessMode = FSMC_AccessMode_A;
	
	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct = &Read_Write_Timing;
	FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct = &Write_Timing;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4,ENABLE);
	
	delay_ms(50);
	
	my_lcd_write_reg(0x0000,0x0001);
	delay_ms(50);
	lcd_dev.id = my_lcd_read_reg(0x0000);
	
	if(lcd_dev.id == 0 || lcd_dev.id == 0xFFFF)
	{
		my_lcd_wr_reg(0xd3);
		my_lcd_rd_data();
		my_lcd_rd_data();
		lcd_dev.id = my_lcd_rd_data();
		lcd_dev.id <<= 8;
		lcd_dev.id |= my_lcd_rd_data();
	}
	
	rt_kprintf("LCD ID:%x\n",lcd_dev.id);
}

