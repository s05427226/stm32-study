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
	u16	wramcmd;	//write gram ins
	u16	setxcmd;	//set x pos ins
	u16	setycmd;	//set y pos ins
}_lcd_dev;

extern _lcd_dev lcd_dev;
extern u16 POINT_COLOR;
extern u16 BACK_COLOR;

//////////////////////////////////////////////////////////////////////
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

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

//LCD分辨率设置
#define SSD_HOR_RESOLUTION		800		//LCD水平分辨率
#define SSD_VER_RESOLUTION		480		//LCD垂直分辨率
//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH		1		//水平脉宽
#define SSD_HOR_BACK_PORCH		46		//水平前廊
#define SSD_HOR_FRONT_PORCH		210		//水平后廊

#define SSD_VER_PULSE_WIDTH		1		//垂直脉宽
#define SSD_VER_BACK_PORCH		23		//垂直前廊
#define SSD_VER_FRONT_PORCH		22		//垂直前廊
//如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

extern void my_lcd_set_cursor(u16 xpos,u16 ypos);
extern void my_lcd_draw_point(u16 x,u16 y);
extern void my_lcd_fast_draw_point(u16 x,u16 y,u16 color);
extern void my_lcd_ssd_backlight_set(u8 pwm);
extern void my_lcd_display_dir(u8 dir);
extern void my_lcd_set_window(u16 sx,u16 sy,u16 width,u16 height);
extern u16 my_lcd_read_point(u16 x,u16 y);
extern void my_lcd_show_char(u16 x,u16 y,u8 num,u8 size,u8 mode);
extern void my_lcd_display_on(void);
extern void my_lcd_scan_dir(u8 dir);
extern void my_lcd_clear(u16 color);
extern void my_lcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
extern void my_lcd_color_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
extern void my_lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2);
extern void my_lcd_draw_rect(u16 x1, u16 y1, u16 x2, u16 y2);
extern void my_lcd_draw_cycle(u16 x0,u16 y0,u8 r);
extern void my_lcd_show_num(u16 x,u16 y,u32 num,u8 len,u8 size);
extern void my_lcd_show_x_num(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
extern void my_lcd_show_string(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
extern void LCD_Init(void);
#endif
