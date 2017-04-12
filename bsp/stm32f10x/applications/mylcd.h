#ifndef __H_MY_LCD
#define __H_MY_LCD

//----------head file is here----------
#include <board.h>
#include "delay.h"

//----------struct define is here------
typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
}LCD_TypeDef;

#define LCD_BASE	((u32)(0x6c000000 | 0x000007FE))
#define LCD	((LCD_TypeDef*)LCD_BASE)
//--------how to read/write lcd
//for write:
//	LCD->LCD_REG = CMD;	LCD->LCD_RAM = DATA;
//for read:
//	CMD = LCD->LCD_REG; DATA = LCD->LCD_RAM;


typedef struct 
{
	u16 width;		//lcd width
	u16 height;		//LCD height
	u16 id;				//lcd id
	u8	dir;			//0:vertical,1:horizable
	u8	wramcmd;	//write gram ins
	u8	setxcmd;	//set x pos ins
	u8	setycmd;	//set y pos ins
}_lcd_dev;

extern _lcd_dev lcd_dev;
u16 POINT_COLOR;
u16 BACK_COLOR;

__STATIC_INLINE	void my_lcd_wr_reg(u16 reg_val)
{
	LCD->LCD_REG = reg_val;
}

__STATIC_INLINE	void my_lcd_wr_data(u16 data)
{
	LCD->LCD_RAM = data;
}

__STATIC_INLINE	u16 my_lcd_rd_data()
{
	return LCD->LCD_RAM;
}

__STATIC_INLINE	void my_lcd_write_reg(u16 lcd_reg,u16 reg_value)
{
	LCD->LCD_REG = lcd_reg;
	LCD->LCD_RAM = reg_value;
}

__STATIC_INLINE	u16 my_lcd_read_reg(u16 lcd_reg)
{
	my_lcd_wr_reg(lcd_reg);
	delay_us(5);
	return my_lcd_rd_data();
}

__STATIC_INLINE	void my_lcd_write_ram_prepare()
{
	LCD->LCD_REG = lcd_dev.wramcmd;
}

extern void my_lcd_set_cursor(u16 xpos,u16 ypos);
extern void my_lcd_draw_point(u16 x,u16 y);
extern u16 my_lcd_read_point(u16 x,u16 y);
extern void my_lcd_show_char(u16 x,u16 y,u8 num,u8 size,u8 mode);
extern void LCD_Init(void);
#endif
