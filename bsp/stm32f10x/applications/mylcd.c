#include "mylcd.h"
#include "font.h"
#include "rtthread.h"

_lcd_dev lcd_dev;
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色

void my_lcd_set_cursor(u16 xpos,u16 ypos)
{
	switch(lcd_dev.id)
	{
		case 0x9341:
			my_lcd_wr_reg(lcd_dev.setxcmd);
			my_lcd_wr_reg(xpos >> 8);
			my_lcd_wr_reg(xpos &0xff);
			my_lcd_wr_reg(lcd_dev.setycmd);
			my_lcd_wr_reg(ypos >> 8);
			my_lcd_wr_reg(ypos &0xff);
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
	my_lcd_wr_data(POINT_COLOR);
}

void my_lcd_fast_draw_point(u16 x,u16 y,u16 color)
{
	if(lcd_dev.id==0X9341||lcd_dev.id==0X5310)
	{
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(x>>8);
		my_lcd_wr_reg(x&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(y>>8);
		my_lcd_wr_reg(y&0XFF);
	}else if(lcd_dev.id==0X5510)
	{
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(x>>8);
		my_lcd_wr_reg(lcd_dev.setxcmd+1);
		my_lcd_wr_reg(x&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(y>>8);
		my_lcd_wr_reg(lcd_dev.setycmd+1);
		my_lcd_wr_reg(y&0XFF);
	}else if(lcd_dev.id==0X1963)
	{
		if(lcd_dev.dir==0)x=lcd_dev.width-1-x;
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(x>>8);
		my_lcd_wr_reg(x&0XFF);
		my_lcd_wr_reg(x>>8);
		my_lcd_wr_reg(x&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(y>>8);
		my_lcd_wr_reg(y&0XFF);
		my_lcd_wr_reg(y>>8);
		my_lcd_wr_reg(y&0XFF);
	}else if(lcd_dev.id==0X6804)
	{
		if(lcd_dev.dir==1)x=lcd_dev.width-1-x;//横屏时处理
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(x>>8);
		my_lcd_wr_reg(x&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(y>>8);
		my_lcd_wr_reg(y&0XFF);
	}else
	{
		if(lcd_dev.dir==1)x=lcd_dev.width-1-x;//横屏其实就是调转x,y坐标
		my_lcd_write_reg(lcd_dev.setxcmd,x);
		my_lcd_write_reg(lcd_dev.setycmd,y);
	}
	my_lcd_write_ram_prepare();
	my_lcd_wr_reg(color);		//写数据
}

void my_lcd_ssd_backlight_set(u8 pwm)
{
	my_lcd_wr_reg(0xBE);	//配置PWM输出
	my_lcd_wr_reg(0x05);	//1设置PWM频率
	my_lcd_wr_reg(pwm*2.55);//2设置PWM占空比
	my_lcd_wr_reg(0x01);	//3设置C
	my_lcd_wr_reg(0xFF);	//4设置D
	my_lcd_wr_reg(0x00);	//5设置E
	my_lcd_wr_reg(0x00);	//6设置F
}

void my_lcd_display_dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcd_dev.dir=0;	//竖屏
		lcd_dev.width=232;
		lcd_dev.height=328;
		if(lcd_dev.id == 0x9328)
		{
			lcd_dev.wramcmd=0X22;
	 		lcd_dev.setxcmd=0X20;
			lcd_dev.setycmd=0X21;
		}
		if(lcd_dev.id==0X9341||lcd_dev.id==0X6804||lcd_dev.id==0X5310)
		{
			lcd_dev.wramcmd=0X2C;
	 		lcd_dev.setxcmd=0X2A;
			lcd_dev.setycmd=0X2B;
			if(lcd_dev.id==0X6804||lcd_dev.id==0X5310)
			{
				lcd_dev.width=320;
				lcd_dev.height=480;
			}
		}else if(lcd_dev.id==0x5510)
		{
			lcd_dev.wramcmd=0X2C00;
			lcd_dev.setxcmd=0X2A00;
			lcd_dev.setycmd=0X2B00;
			lcd_dev.width=480;
			lcd_dev.height=800;
		}else if(lcd_dev.id==0X1963)
		{
			lcd_dev.wramcmd=0X2C;	//设置写入GRAM的指令
			lcd_dev.setxcmd=0X2B;	//设置写X坐标指令
			lcd_dev.setycmd=0X2A;	//设置写Y坐标指令
			lcd_dev.width=480;		//设置宽度480
			lcd_dev.height=800;		//设置高度800
		}else
		{
			lcd_dev.wramcmd=0X22;
			lcd_dev.setxcmd=0X20;
			lcd_dev.setycmd=0X21;
		}
	}else 				//横屏
	{
		lcd_dev.dir=1;	//横屏
		lcd_dev.width=328;
		lcd_dev.height=232;
		if(lcd_dev.id == 0x9328)
		{
			lcd_dev.wramcmd=0X22;
	 		lcd_dev.setxcmd=0X20;
			lcd_dev.setycmd=0X21;
		}
		lcd_dev.width=320;
		lcd_dev.height=240;
		if(lcd_dev.id==0X9341||lcd_dev.id==0X5310)
		{
			lcd_dev.wramcmd=0X2C;
			lcd_dev.setxcmd=0X2A;
			lcd_dev.setycmd=0X2B;
		}else if(lcd_dev.id==0X6804)
		{
			lcd_dev.wramcmd=0X2C;
			lcd_dev.setxcmd=0X2B;
			lcd_dev.setycmd=0X2A;
		}else if(lcd_dev.id==0x5510)
		{
			lcd_dev.wramcmd=0X2C00;
	 		lcd_dev.setxcmd=0X2A00;
			lcd_dev.setycmd=0X2B00;
			lcd_dev.width=800;
			lcd_dev.height=480;
		}else if(lcd_dev.id==0X1963)
		{
			lcd_dev.wramcmd=0X2C;	//设置写入GRAM的指令
			lcd_dev.setxcmd=0X2A;	//设置写X坐标指令
			lcd_dev.setycmd=0X2B;	//设置写Y坐标指令
			lcd_dev.width=800;		//设置宽度800
			lcd_dev.height=480;		//设置高度480
		}else
		{
			lcd_dev.wramcmd=0X22;
	 		lcd_dev.setxcmd=0X21;
			lcd_dev.setycmd=0X20;
		}
		if(lcd_dev.id==0X6804||lcd_dev.id==0X5310)
		{
			lcd_dev.width=480;
			lcd_dev.height=320;
		}
	}
	my_lcd_scan_dir(DFT_SCAN_DIR);	//默认扫描方向
}

void my_lcd_set_window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval;
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	if(lcd_dev.id==0X9341||lcd_dev.id==0X5310||lcd_dev.id==0X6804||(lcd_dev.dir==1&&lcd_dev.id==0X1963))
	{
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(sx>>8);
		my_lcd_wr_reg(sx&0XFF);
		my_lcd_wr_reg(twidth>>8);
		my_lcd_wr_reg(twidth&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(sy>>8);
		my_lcd_wr_reg(sy&0XFF);
		my_lcd_wr_reg(theight>>8);
		my_lcd_wr_reg(theight&0XFF);
	}else if(lcd_dev.id==0X1963)//1963竖屏特殊处理
	{
		sx=lcd_dev.width-width-sx;
		height=sy+height-1;
		my_lcd_wr_reg(lcd_dev.setxcmd);
		my_lcd_wr_reg(sx>>8);
		my_lcd_wr_reg(sx&0XFF);
		my_lcd_wr_reg((sx+width-1)>>8);
		my_lcd_wr_reg((sx+width-1)&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);
		my_lcd_wr_reg(sy>>8);
		my_lcd_wr_reg(sy&0XFF);
		my_lcd_wr_reg(height>>8);
		my_lcd_wr_reg(height&0XFF);
	}else if(lcd_dev.id==0X5510)
	{
		my_lcd_wr_reg(lcd_dev.setxcmd);my_lcd_wr_reg(sx>>8);
		my_lcd_wr_reg(lcd_dev.setxcmd+1);my_lcd_wr_reg(sx&0XFF);
		my_lcd_wr_reg(lcd_dev.setxcmd+2);my_lcd_wr_reg(twidth>>8);
		my_lcd_wr_reg(lcd_dev.setxcmd+3);my_lcd_wr_reg(twidth&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd);my_lcd_wr_reg(sy>>8);
		my_lcd_wr_reg(lcd_dev.setycmd+1);my_lcd_wr_reg(sy&0XFF);
		my_lcd_wr_reg(lcd_dev.setycmd+2);my_lcd_wr_reg(theight>>8);
		my_lcd_wr_reg(lcd_dev.setycmd+3);my_lcd_wr_reg(theight&0XFF);
	}else	//其他驱动IC
	{
		if(lcd_dev.dir==1)//横屏
		{
			//窗口值
			hsaval=sy;
			heaval=theight;
			vsaval=lcd_dev.width-twidth-1;
			veaval=lcd_dev.width-sx-1;	
		}else
		{
			hsaval=sx;	
			heaval=twidth;
			vsaval=sy;
			veaval=theight;
		}
		hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
		vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器
		//设置寄存器值
		my_lcd_write_reg(hsareg,hsaval);
		my_lcd_write_reg(heareg,heaval);
		my_lcd_write_reg(vsareg,vsaval);
		my_lcd_write_reg(veareg,veaval);
		my_lcd_set_cursor(sx,sy);	//设置光标位置
	}
}

static u16 LCD_BGR2RGB(u16 c)
{
	u16 r,g,b,rgb;
	b = (c>>0) & 0x1f;
	g = (c>>5) & 0x3F;
	r = (c>>11) & 0x1F;
	rgb=(b<<11)+(g<<5)+(r<<0);
	return rgb;
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
	else
		return LCD_BGR2RGB(r);
}

void my_lcd_show_char(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
	u8 temp,t1,t;
	u16 x0=x;
	u16 color_tmp = POINT_COLOR;

	num = num - ' ';
	
	if(!mode)
	{
		for(t=0;t<size;t++)
		{
			if(size == 12)
				temp = asc2_1206[num][t];
			else
				temp = asc2_1608[num][t];
			
			for(t1=0;t1<size/2;t1++)
			{
				if(temp & 0x01)
					POINT_COLOR = color_tmp;
				else
					POINT_COLOR = BACK_COLOR;
				
				my_lcd_draw_point(x,y);
				temp >>= 1;
				x++;
			}
			x=x0;
			y++;
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
			
			for(t1=0;t1<size/2;t1++)
			{
				if(temp & 0x01);
					my_lcd_draw_point(x+t1,y+t);
			
				temp >>= 1;
			}
		}
	}
	POINT_COLOR = color_tmp;
}

void my_lcd_display_on(void)
{
	if(lcd_dev.id==0X9341||lcd_dev.id==0X6804||lcd_dev.id==0X5310||lcd_dev.id==0X1963)
		my_lcd_wr_reg(0X29);
	else if(lcd_dev.id==0X5510)my_lcd_wr_reg(0X2900);
	else my_lcd_write_reg(0x07,0x0173);
}

void my_lcd_display_off(void)
{
	if(lcd_dev.id==0X9341||lcd_dev.id==0X6804||lcd_dev.id==0X5310||lcd_dev.id==0X1963)
		my_lcd_wr_reg(0X28);
	else if(lcd_dev.id==0X5510)my_lcd_wr_reg(0X2800);
	else my_lcd_write_reg(0x07,0);
}

void my_lcd_scan_dir(u8 dir)
{
	u16 reg_val;
	u16 dir_reg;
	u16 tmp;

	if((lcd_dev.dir==1 && lcd_dev.id!=0X6804&&lcd_dev.id!=0X1963)||(lcd_dev.dir==0&&lcd_dev.id==0X1963))
	{
		switch(dir)
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break; 
		}
	}
	if(lcd_dev.id==0x9341||lcd_dev.id==0X6804||lcd_dev.id==0X5310||lcd_dev.id==0X5510||lcd_dev.id==0X1963)
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				reg_val|=(0<<7)|(0<<6)|(0<<5);
				break;
			case L2R_D2U://从左到右,从下到上
				reg_val|=(1<<7)|(0<<6)|(0<<5);
				break;
			case R2L_U2D://从右到左,从上到下
				reg_val|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				reg_val|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				reg_val|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				reg_val|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				reg_val|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				reg_val|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcd_dev.id==0X5510)dir_reg=0X3600;
		else dir_reg=0X36;
 		if((lcd_dev.id!=0X5310)&&(lcd_dev.id!=0X5510)&&(lcd_dev.id!=0X1963))reg_val|=0X08;//5310/5510/1963不需要BGR   
		if(lcd_dev.id==0X6804)reg_val|=0x02;//6804的BIT6和9341的反了	 
		my_lcd_write_reg(dir_reg,reg_val);
		if(lcd_dev.id!=0X1963)//1963不做坐标处理
		{
			if(reg_val&0X20)
			{
				if(lcd_dev.width<lcd_dev.height)//交换X,Y
				{
					tmp=lcd_dev.width;
					lcd_dev.width=lcd_dev.height;
					lcd_dev.height=tmp;
				}
			}else  
			{
				if(lcd_dev.width>lcd_dev.height)//交换X,Y
				{
					tmp=lcd_dev.width;
					lcd_dev.width=lcd_dev.height;
					lcd_dev.height=tmp;
				}
			}  
		}
		if(lcd_dev.id==0X5510)
		{
			my_lcd_wr_reg(lcd_dev.setxcmd);
			my_lcd_wr_reg(0); 
			my_lcd_wr_reg(lcd_dev.setxcmd+1);
			my_lcd_wr_reg(0); 
			my_lcd_wr_reg(lcd_dev.setxcmd+2);
			my_lcd_wr_reg((lcd_dev.width-1)>>8); 
			my_lcd_wr_reg(lcd_dev.setxcmd+3);
			my_lcd_wr_reg((lcd_dev.width-1)&0XFF); 
			my_lcd_wr_reg(lcd_dev.setycmd);
			my_lcd_wr_reg(0); 
			my_lcd_wr_reg(lcd_dev.setycmd+1);
			my_lcd_wr_reg(0); 
			my_lcd_wr_reg(lcd_dev.setycmd+2);
			my_lcd_wr_reg((lcd_dev.height-1)>>8); 
			my_lcd_wr_reg(lcd_dev.setycmd+3);
			my_lcd_wr_reg((lcd_dev.height-1)&0XFF);
		}else
		{
			my_lcd_wr_reg(lcd_dev.setxcmd); 
			my_lcd_wr_reg(0);
			my_lcd_wr_reg(0);
			my_lcd_wr_reg((lcd_dev.width-1)>>8);
			my_lcd_wr_reg((lcd_dev.width-1)&0XFF);
			my_lcd_wr_reg(lcd_dev.setycmd); 
			my_lcd_wr_reg(0);
			my_lcd_wr_reg(0);
			my_lcd_wr_reg((lcd_dev.height-1)>>8);
			my_lcd_wr_reg((lcd_dev.height-1)&0XFF);  
		}
	}
	else 
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				reg_val|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://从左到右,从下到上
				reg_val|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://从右到左,从上到下
				reg_val|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://从右到左,从下到上
				reg_val|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				reg_val|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://从上到下,从右到左
				reg_val|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://从下到上,从左到右
				reg_val|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://从下到上,从右到左
				reg_val|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		} 
		dir_reg=0X03;
		reg_val|=1<<12; 
		my_lcd_write_reg(dir_reg,reg_val);
	}
		
}

void my_lcd_clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcd_dev.width;
	totalpoint*=lcd_dev.height; 			//得到总点数
	if((lcd_dev.id==0X6804)&&(lcd_dev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcd_dev.dir=0;	 
 		lcd_dev.setxcmd=0X2A;
		lcd_dev.setycmd=0X2B;  	 			
		my_lcd_set_cursor(0x00,0x0000);		//设置光标位置  
 		lcd_dev.dir=1;	 
  	lcd_dev.setxcmd=0X2B;
		lcd_dev.setycmd=0X2A;  	 
 	}else my_lcd_set_cursor(0x00,0x0000);	//设置光标位置 
	my_lcd_write_ram_prepare();     		//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++)my_lcd_wr_data(color);	
}  

void my_lcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	u16 temp;
	if((lcd_dev.id==0X6804)&&(lcd_dev.dir==1))	//6804横屏的时候特殊处理  
	{
		temp=sx;
		sx=sy;
		sy=lcd_dev.width-ex-1;	  
		ex=ey;
		ey=lcd_dev.width-temp-1;
 		lcd_dev.dir=0;	 
 		lcd_dev.setxcmd=0X2A;
		lcd_dev.setycmd=0X2B;  	 			
		my_lcd_fill(sx,sy,ex,ey,color);  
 		lcd_dev.dir=1;	 
  	lcd_dev.setxcmd=0X2B;
		lcd_dev.setycmd=0X2A;  	 
 	}else
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	my_lcd_set_cursor(sx,i);      				//设置光标位置 
			my_lcd_write_ram_prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)my_lcd_wr_data(color);	//设置光标位置 	    
		}
	}
}  

void my_lcd_color_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		my_lcd_set_cursor(sx,sy+i);   	//设置光标位置 
		my_lcd_write_ram_prepare();     //开始写入GRAM
		for(j=0;j<width;j++)my_lcd_wr_data(color[i*width+j]);//写入数据 
	}	  
}

void my_lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		my_lcd_draw_point(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void my_lcd_draw_rect(u16 x1, u16 y1, u16 x2, u16 y2)
{
	my_lcd_draw_line(x1,y1,x2,y1);
	my_lcd_draw_line(x1,y1,x1,y2);
	my_lcd_draw_line(x1,y2,x2,y2);
	my_lcd_draw_line(x2,y1,x2,y2);
}

void my_lcd_draw_cycle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		my_lcd_draw_point(x0+a,y0-b);             //5
 		my_lcd_draw_point(x0+b,y0-a);             //0           
		my_lcd_draw_point(x0+b,y0+a);             //4               
		my_lcd_draw_point(x0+a,y0+b);             //6 
		my_lcd_draw_point(x0-a,y0+b);             //1       
 		my_lcd_draw_point(x0-b,y0+a);             
		my_lcd_draw_point(x0-a,y0-b);             //2             
  		my_lcd_draw_point(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 								

//m^n函数
//返回值:m^n次方.
static  u32 my_lcd_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

void my_lcd_show_num(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/my_lcd_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				my_lcd_show_char(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	my_lcd_show_char(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void my_lcd_show_x_num(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/my_lcd_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)my_lcd_show_char(x+(size/2)*t,y,'0',size,mode&0X01);  
				else my_lcd_show_char(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	my_lcd_show_char(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void my_lcd_show_string(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
	while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
	{       
			if(x>=width){x=x0;y+=size;}
			if(y>=height)break;//退出
			my_lcd_show_char(x,y,*p,size,0);
			x+=size/2;
			p++;
	}  
}

void LCD_Init()
{
	GPIO_InitTypeDef	GPIO_InitStruct;
//	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;
//	FSMC_NORSRAMTimingInitTypeDef	Read_Write_Timing;
//	FSMC_NORSRAMTimingInitTypeDef Write_Timing;
	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(GPIOB, &GPIO_InitStruct); //GPIOB 
	GPIO_SetBits(GPIOB,GPIO_Pin_All);
	
//	Read_Write_Timing.FSMC_AddressSetupTime = 0x01;
//	Read_Write_Timing.FSMC_AddressHoldTime = 0x00;
//	Read_Write_Timing.FSMC_DataSetupTime = 0x0f;
//	Read_Write_Timing.FSMC_BusTurnAroundDuration = 0x00;
//	Read_Write_Timing.FSMC_CLKDivision = 0x00;
//	Read_Write_Timing.FSMC_DataLatency = 0x00;
//	Read_Write_Timing.FSMC_AccessMode = FSMC_AccessMode_A;
//	
//	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM4;
//	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_NOR;
//	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
//	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//	FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct = &Read_Write_Timing;
//	FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct = &Write_Timing;
//	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
//	
//	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4,ENABLE);
	
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
	
	if(lcd_dev.id==0X9341)	//9341初始化
	{	 
		my_lcd_wr_reg(0xCF);  
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0xC1); 
		my_lcd_wr_reg(0X30); 
		my_lcd_wr_reg(0xED);  
		my_lcd_wr_reg(0x64); 
		my_lcd_wr_reg(0x03); 
		my_lcd_wr_reg(0X12); 
		my_lcd_wr_reg(0X81); 
		my_lcd_wr_reg(0xE8);  
		my_lcd_wr_reg(0x85); 
		my_lcd_wr_reg(0x10); 
		my_lcd_wr_reg(0x7A); 
		my_lcd_wr_reg(0xCB);  
		my_lcd_wr_reg(0x39); 
		my_lcd_wr_reg(0x2C); 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x34); 
		my_lcd_wr_reg(0x02); 
		my_lcd_wr_reg(0xF7);  
		my_lcd_wr_reg(0x20); 
		my_lcd_wr_reg(0xEA);  
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0xC0);    //Power control 
		my_lcd_wr_reg(0x1B);   //VRH[5:0] 
		my_lcd_wr_reg(0xC1);    //Power control 
		my_lcd_wr_reg(0x01);   //SAP[2:0];BT[3:0] 
		my_lcd_wr_reg(0xC5);    //VCM control 
		my_lcd_wr_reg(0x30); 	 //3F
		my_lcd_wr_reg(0x30); 	 //3C
		my_lcd_wr_reg(0xC7);    //VCM control2 
		my_lcd_wr_reg(0XB7); 
		my_lcd_wr_reg(0x36);    // Memory Access Control 
		my_lcd_wr_reg(0x48); 
		my_lcd_wr_reg(0x3A);   
		my_lcd_wr_reg(0x55); 
		my_lcd_wr_reg(0xB1);   
		my_lcd_wr_reg(0x00);   
		my_lcd_wr_reg(0x1A); 
		my_lcd_wr_reg(0xB6);    // Display Function Control 
		my_lcd_wr_reg(0x0A); 
		my_lcd_wr_reg(0xA2); 
		my_lcd_wr_reg(0xF2);    // 3Gamma Function Disable 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x26);    //Gamma curve selected 
		my_lcd_wr_reg(0x01); 
		my_lcd_wr_reg(0xE0);    //Set Gamma 
		my_lcd_wr_reg(0x0F); 
		my_lcd_wr_reg(0x2A); 
		my_lcd_wr_reg(0x28); 
		my_lcd_wr_reg(0x08); 
		my_lcd_wr_reg(0x0E); 
		my_lcd_wr_reg(0x08); 
		my_lcd_wr_reg(0x54); 
		my_lcd_wr_reg(0XA9); 
		my_lcd_wr_reg(0x43); 
		my_lcd_wr_reg(0x0A); 
		my_lcd_wr_reg(0x0F); 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x00); 		 
		my_lcd_wr_reg(0XE1);    //Set Gamma 
		my_lcd_wr_reg(0x00); 
		my_lcd_wr_reg(0x15); 
		my_lcd_wr_reg(0x17); 
		my_lcd_wr_reg(0x07); 
		my_lcd_wr_reg(0x11); 
		my_lcd_wr_reg(0x06); 
		my_lcd_wr_reg(0x2B); 
		my_lcd_wr_reg(0x56); 
		my_lcd_wr_reg(0x3C); 
		my_lcd_wr_reg(0x05); 
		my_lcd_wr_reg(0x10); 
		my_lcd_wr_reg(0x0F); 
		my_lcd_wr_reg(0x3F); 
		my_lcd_wr_reg(0x3F); 
		my_lcd_wr_reg(0x0F); 
		my_lcd_wr_reg(0x2B); 
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0x3f);
		my_lcd_wr_reg(0x2A); 
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xef);	 
		my_lcd_wr_reg(0x11); //Exit Sleep
		delay_ms(120);
		my_lcd_wr_reg(0x29); //display on	
	}else if(lcd_dev.id==0x6804) //6804初始化
	{
		my_lcd_wr_reg(0X11);
		delay_ms(20);
		my_lcd_wr_reg(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
		my_lcd_wr_reg(0X07); 
		my_lcd_wr_reg(0X42); 
		my_lcd_wr_reg(0X1D); 
		my_lcd_wr_reg(0XD1);//VCOMH VCOM_AC amplitude setting
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X1a);
		my_lcd_wr_reg(0X09); 
		my_lcd_wr_reg(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
		my_lcd_wr_reg(0X01);
		my_lcd_wr_reg(0X22);
		my_lcd_wr_reg(0XC0);//REV SM GS 
		my_lcd_wr_reg(0X10);
		my_lcd_wr_reg(0X3B);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X02);
		my_lcd_wr_reg(0X11);
		
		my_lcd_wr_reg(0XC5);// Frame rate setting = 72HZ  when setting 0x03
		my_lcd_wr_reg(0X03);
		
		my_lcd_wr_reg(0XC8);//Gamma setting
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X25);
		my_lcd_wr_reg(0X21);
		my_lcd_wr_reg(0X05);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X0a);
		my_lcd_wr_reg(0X65);
		my_lcd_wr_reg(0X25);
		my_lcd_wr_reg(0X77);
		my_lcd_wr_reg(0X50);
		my_lcd_wr_reg(0X0f);
		my_lcd_wr_reg(0X00);	  
						  
   		my_lcd_wr_reg(0XF8);
		my_lcd_wr_reg(0X01);	  

 		my_lcd_wr_reg(0XFE);
 		my_lcd_wr_reg(0X00);
 		my_lcd_wr_reg(0X02);
		
		my_lcd_wr_reg(0X20);//Exit invert mode

		my_lcd_wr_reg(0X36);
		my_lcd_wr_reg(0X08);//原来是a
		
		my_lcd_wr_reg(0X3A);
		my_lcd_wr_reg(0X55);//16位模式	  
		my_lcd_wr_reg(0X2B);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X01);
		my_lcd_wr_reg(0X3F);
		
		my_lcd_wr_reg(0X2A);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X00);
		my_lcd_wr_reg(0X01);
		my_lcd_wr_reg(0XDF);
		delay_ms(120);
		my_lcd_wr_reg(0X29); 	 
 	}else if(lcd_dev.id==0x5310)
	{  
		my_lcd_wr_reg(0xED);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0xFE);

		my_lcd_wr_reg(0xEE);
		my_lcd_wr_reg(0xDE);
		my_lcd_wr_reg(0x21);

		my_lcd_wr_reg(0xF1);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0xDF);
		my_lcd_wr_reg(0x10);

		//VCOMvoltage//
		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x8F);	  //5f

		my_lcd_wr_reg(0xC6);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE2);
		my_lcd_wr_reg(0xE2);
		my_lcd_wr_reg(0xE2);
		my_lcd_wr_reg(0xBF);
		my_lcd_wr_reg(0xAA);

		my_lcd_wr_reg(0xB0);
		my_lcd_wr_reg(0x0D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x0D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x11);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x19);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x21);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x5D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x5D);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB1);
		my_lcd_wr_reg(0x80);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x8B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x96);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x02);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x03);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB4);
		my_lcd_wr_reg(0x8B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x96);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA1);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB5);
		my_lcd_wr_reg(0x02);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x03);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x04);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB6);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3F);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x5E);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x64);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x8C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xAC);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDC);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x70);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x90);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xEB);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDC);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xB8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xBA);
		my_lcd_wr_reg(0x24);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC1);
		my_lcd_wr_reg(0x20);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x54);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xFF);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC2);
		my_lcd_wr_reg(0x0A);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x04);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC3);
		my_lcd_wr_reg(0x3C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3A);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x39);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x37);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x36);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x32);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2F);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x29);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x26);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x24);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x24);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x23);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x36);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x32);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2F);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x29);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x26);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x24);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x24);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x23);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x62);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x05);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x84);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF0);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x18);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA4);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x18);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x50);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x0C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x17);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x95);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE6);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC5);
		my_lcd_wr_reg(0x32);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x65);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x76);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC6);
		my_lcd_wr_reg(0x20);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x17);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xC9);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE0);
		my_lcd_wr_reg(0x16);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x1C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x21);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x36);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x46);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x52);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x64);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x7A);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x8B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB9);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xCA);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD9);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE0);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE1);
		my_lcd_wr_reg(0x16);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x1C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x22);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x36);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x45);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x52);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x64);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x7A);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x8B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB9);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xCA);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE0);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE2);
		my_lcd_wr_reg(0x05);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x0B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x1B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x34);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x4F);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x61);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x79);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x97);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA6);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD1);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD6);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDD);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE3);
		my_lcd_wr_reg(0x05);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x1C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x33);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x50);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x62);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x78);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x97);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA6);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC7);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD1);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD5);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDD);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE4);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x01);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x02);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x2A);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x4B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x5D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x74);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x84);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x93);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xBE);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xCD);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDD);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xE5);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x02);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x29);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x3C);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x4B);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x5D);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x74);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x84);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x93);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xA2);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xB3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xBE);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xC4);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xCD);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xD3);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xDC);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE6);
		my_lcd_wr_reg(0x11);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x34);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x56);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x76);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x77);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x66);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xBB);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x66);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x45);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x43);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE7);
		my_lcd_wr_reg(0x32);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x76);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x66);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x67);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x67);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x87);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xBB);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x77);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x56);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x23); 
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x33);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x45);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE8);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x87);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x77);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x66);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x88);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xAA);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xBB);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x99);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x66);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x44);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xE9);
		my_lcd_wr_reg(0xAA);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0xAA);

		my_lcd_wr_reg(0xCF);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xF0);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x50);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xF3);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0xF9);
		my_lcd_wr_reg(0x06);
		my_lcd_wr_reg(0x10);
		my_lcd_wr_reg(0x29);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0x3A);
		my_lcd_wr_reg(0x55);	//66

		my_lcd_wr_reg(0x11);
		delay_ms(100);
		my_lcd_wr_reg(0x29);
		my_lcd_wr_reg(0x35);
		my_lcd_wr_reg(0x00);

		my_lcd_wr_reg(0x51);
		my_lcd_wr_reg(0xFF);
		my_lcd_wr_reg(0x53);
		my_lcd_wr_reg(0x2C);
		my_lcd_wr_reg(0x55);
		my_lcd_wr_reg(0x82);
		my_lcd_wr_reg(0x2c); 
	}else if(lcd_dev.id==0x5510)
	{
		my_lcd_write_reg(0xF000,0x55);
		my_lcd_write_reg(0xF001,0xAA);
		my_lcd_write_reg(0xF002,0x52);
		my_lcd_write_reg(0xF003,0x08);
		my_lcd_write_reg(0xF004,0x01);
		//AVDD Set AVDD 5.2V
		my_lcd_write_reg(0xB000,0x0D);
		my_lcd_write_reg(0xB001,0x0D);
		my_lcd_write_reg(0xB002,0x0D);
		//AVDD ratio
		my_lcd_write_reg(0xB600,0x34);
		my_lcd_write_reg(0xB601,0x34);
		my_lcd_write_reg(0xB602,0x34);
		//AVEE -5.2V
		my_lcd_write_reg(0xB100,0x0D);
		my_lcd_write_reg(0xB101,0x0D);
		my_lcd_write_reg(0xB102,0x0D);
		//AVEE ratio
		my_lcd_write_reg(0xB700,0x34);
		my_lcd_write_reg(0xB701,0x34);
		my_lcd_write_reg(0xB702,0x34);
		//VCL -2.5V
		my_lcd_write_reg(0xB200,0x00);
		my_lcd_write_reg(0xB201,0x00);
		my_lcd_write_reg(0xB202,0x00);
		//VCL ratio
		my_lcd_write_reg(0xB800,0x24);
		my_lcd_write_reg(0xB801,0x24);
		my_lcd_write_reg(0xB802,0x24);
		//VGH 15V (Free pump)
		my_lcd_write_reg(0xBF00,0x01);
		my_lcd_write_reg(0xB300,0x0F);
		my_lcd_write_reg(0xB301,0x0F);
		my_lcd_write_reg(0xB302,0x0F);
		//VGH ratio
		my_lcd_write_reg(0xB900,0x34);
		my_lcd_write_reg(0xB901,0x34);
		my_lcd_write_reg(0xB902,0x34);
		//VGL_REG -10V
		my_lcd_write_reg(0xB500,0x08);
		my_lcd_write_reg(0xB501,0x08);
		my_lcd_write_reg(0xB502,0x08);
		my_lcd_write_reg(0xC200,0x03);
		//VGLX ratio
		my_lcd_write_reg(0xBA00,0x24);
		my_lcd_write_reg(0xBA01,0x24);
		my_lcd_write_reg(0xBA02,0x24);
		//VGMP/VGSP 4.5V/0V
		my_lcd_write_reg(0xBC00,0x00);
		my_lcd_write_reg(0xBC01,0x78);
		my_lcd_write_reg(0xBC02,0x00);
		//VGMN/VGSN -4.5V/0V
		my_lcd_write_reg(0xBD00,0x00);
		my_lcd_write_reg(0xBD01,0x78);
		my_lcd_write_reg(0xBD02,0x00);
		//VCOM
		my_lcd_write_reg(0xBE00,0x00);
		my_lcd_write_reg(0xBE01,0x64);
		//Gamma Setting
		my_lcd_write_reg(0xD100,0x00);
		my_lcd_write_reg(0xD101,0x33);
		my_lcd_write_reg(0xD102,0x00);
		my_lcd_write_reg(0xD103,0x34);
		my_lcd_write_reg(0xD104,0x00);
		my_lcd_write_reg(0xD105,0x3A);
		my_lcd_write_reg(0xD106,0x00);
		my_lcd_write_reg(0xD107,0x4A);
		my_lcd_write_reg(0xD108,0x00);
		my_lcd_write_reg(0xD109,0x5C);
		my_lcd_write_reg(0xD10A,0x00);
		my_lcd_write_reg(0xD10B,0x81);
		my_lcd_write_reg(0xD10C,0x00);
		my_lcd_write_reg(0xD10D,0xA6);
		my_lcd_write_reg(0xD10E,0x00);
		my_lcd_write_reg(0xD10F,0xE5);
		my_lcd_write_reg(0xD110,0x01);
		my_lcd_write_reg(0xD111,0x13);
		my_lcd_write_reg(0xD112,0x01);
		my_lcd_write_reg(0xD113,0x54);
		my_lcd_write_reg(0xD114,0x01);
		my_lcd_write_reg(0xD115,0x82);
		my_lcd_write_reg(0xD116,0x01);
		my_lcd_write_reg(0xD117,0xCA);
		my_lcd_write_reg(0xD118,0x02);
		my_lcd_write_reg(0xD119,0x00);
		my_lcd_write_reg(0xD11A,0x02);
		my_lcd_write_reg(0xD11B,0x01);
		my_lcd_write_reg(0xD11C,0x02);
		my_lcd_write_reg(0xD11D,0x34);
		my_lcd_write_reg(0xD11E,0x02);
		my_lcd_write_reg(0xD11F,0x67);
		my_lcd_write_reg(0xD120,0x02);
		my_lcd_write_reg(0xD121,0x84);
		my_lcd_write_reg(0xD122,0x02);
		my_lcd_write_reg(0xD123,0xA4);
		my_lcd_write_reg(0xD124,0x02);
		my_lcd_write_reg(0xD125,0xB7);
		my_lcd_write_reg(0xD126,0x02);
		my_lcd_write_reg(0xD127,0xCF);
		my_lcd_write_reg(0xD128,0x02);
		my_lcd_write_reg(0xD129,0xDE);
		my_lcd_write_reg(0xD12A,0x02);
		my_lcd_write_reg(0xD12B,0xF2);
		my_lcd_write_reg(0xD12C,0x02);
		my_lcd_write_reg(0xD12D,0xFE);
		my_lcd_write_reg(0xD12E,0x03);
		my_lcd_write_reg(0xD12F,0x10);
		my_lcd_write_reg(0xD130,0x03);
		my_lcd_write_reg(0xD131,0x33);
		my_lcd_write_reg(0xD132,0x03);
		my_lcd_write_reg(0xD133,0x6D);
		my_lcd_write_reg(0xD200,0x00);
		my_lcd_write_reg(0xD201,0x33);
		my_lcd_write_reg(0xD202,0x00);
		my_lcd_write_reg(0xD203,0x34);
		my_lcd_write_reg(0xD204,0x00);
		my_lcd_write_reg(0xD205,0x3A);
		my_lcd_write_reg(0xD206,0x00);
		my_lcd_write_reg(0xD207,0x4A);
		my_lcd_write_reg(0xD208,0x00);
		my_lcd_write_reg(0xD209,0x5C);
		my_lcd_write_reg(0xD20A,0x00);

		my_lcd_write_reg(0xD20B,0x81);
		my_lcd_write_reg(0xD20C,0x00);
		my_lcd_write_reg(0xD20D,0xA6);
		my_lcd_write_reg(0xD20E,0x00);
		my_lcd_write_reg(0xD20F,0xE5);
		my_lcd_write_reg(0xD210,0x01);
		my_lcd_write_reg(0xD211,0x13);
		my_lcd_write_reg(0xD212,0x01);
		my_lcd_write_reg(0xD213,0x54);
		my_lcd_write_reg(0xD214,0x01);
		my_lcd_write_reg(0xD215,0x82);
		my_lcd_write_reg(0xD216,0x01);
		my_lcd_write_reg(0xD217,0xCA);
		my_lcd_write_reg(0xD218,0x02);
		my_lcd_write_reg(0xD219,0x00);
		my_lcd_write_reg(0xD21A,0x02);
		my_lcd_write_reg(0xD21B,0x01);
		my_lcd_write_reg(0xD21C,0x02);
		my_lcd_write_reg(0xD21D,0x34);
		my_lcd_write_reg(0xD21E,0x02);
		my_lcd_write_reg(0xD21F,0x67);
		my_lcd_write_reg(0xD220,0x02);
		my_lcd_write_reg(0xD221,0x84);
		my_lcd_write_reg(0xD222,0x02);
		my_lcd_write_reg(0xD223,0xA4);
		my_lcd_write_reg(0xD224,0x02);
		my_lcd_write_reg(0xD225,0xB7);
		my_lcd_write_reg(0xD226,0x02);
		my_lcd_write_reg(0xD227,0xCF);
		my_lcd_write_reg(0xD228,0x02);
		my_lcd_write_reg(0xD229,0xDE);
		my_lcd_write_reg(0xD22A,0x02);
		my_lcd_write_reg(0xD22B,0xF2);
		my_lcd_write_reg(0xD22C,0x02);
		my_lcd_write_reg(0xD22D,0xFE);
		my_lcd_write_reg(0xD22E,0x03);
		my_lcd_write_reg(0xD22F,0x10);
		my_lcd_write_reg(0xD230,0x03);
		my_lcd_write_reg(0xD231,0x33);
		my_lcd_write_reg(0xD232,0x03);
		my_lcd_write_reg(0xD233,0x6D);
		my_lcd_write_reg(0xD300,0x00);
		my_lcd_write_reg(0xD301,0x33);
		my_lcd_write_reg(0xD302,0x00);
		my_lcd_write_reg(0xD303,0x34);
		my_lcd_write_reg(0xD304,0x00);
		my_lcd_write_reg(0xD305,0x3A);
		my_lcd_write_reg(0xD306,0x00);
		my_lcd_write_reg(0xD307,0x4A);
		my_lcd_write_reg(0xD308,0x00);
		my_lcd_write_reg(0xD309,0x5C);
		my_lcd_write_reg(0xD30A,0x00);

		my_lcd_write_reg(0xD30B,0x81);
		my_lcd_write_reg(0xD30C,0x00);
		my_lcd_write_reg(0xD30D,0xA6);
		my_lcd_write_reg(0xD30E,0x00);
		my_lcd_write_reg(0xD30F,0xE5);
		my_lcd_write_reg(0xD310,0x01);
		my_lcd_write_reg(0xD311,0x13);
		my_lcd_write_reg(0xD312,0x01);
		my_lcd_write_reg(0xD313,0x54);
		my_lcd_write_reg(0xD314,0x01);
		my_lcd_write_reg(0xD315,0x82);
		my_lcd_write_reg(0xD316,0x01);
		my_lcd_write_reg(0xD317,0xCA);
		my_lcd_write_reg(0xD318,0x02);
		my_lcd_write_reg(0xD319,0x00);
		my_lcd_write_reg(0xD31A,0x02);
		my_lcd_write_reg(0xD31B,0x01);
		my_lcd_write_reg(0xD31C,0x02);
		my_lcd_write_reg(0xD31D,0x34);
		my_lcd_write_reg(0xD31E,0x02);
		my_lcd_write_reg(0xD31F,0x67);
		my_lcd_write_reg(0xD320,0x02);
		my_lcd_write_reg(0xD321,0x84);
		my_lcd_write_reg(0xD322,0x02);
		my_lcd_write_reg(0xD323,0xA4);
		my_lcd_write_reg(0xD324,0x02);
		my_lcd_write_reg(0xD325,0xB7);
		my_lcd_write_reg(0xD326,0x02);
		my_lcd_write_reg(0xD327,0xCF);
		my_lcd_write_reg(0xD328,0x02);
		my_lcd_write_reg(0xD329,0xDE);
		my_lcd_write_reg(0xD32A,0x02);
		my_lcd_write_reg(0xD32B,0xF2);
		my_lcd_write_reg(0xD32C,0x02);
		my_lcd_write_reg(0xD32D,0xFE);
		my_lcd_write_reg(0xD32E,0x03);
		my_lcd_write_reg(0xD32F,0x10);
		my_lcd_write_reg(0xD330,0x03);
		my_lcd_write_reg(0xD331,0x33);
		my_lcd_write_reg(0xD332,0x03);
		my_lcd_write_reg(0xD333,0x6D);
		my_lcd_write_reg(0xD400,0x00);
		my_lcd_write_reg(0xD401,0x33);
		my_lcd_write_reg(0xD402,0x00);
		my_lcd_write_reg(0xD403,0x34);
		my_lcd_write_reg(0xD404,0x00);
		my_lcd_write_reg(0xD405,0x3A);
		my_lcd_write_reg(0xD406,0x00);
		my_lcd_write_reg(0xD407,0x4A);
		my_lcd_write_reg(0xD408,0x00);
		my_lcd_write_reg(0xD409,0x5C);
		my_lcd_write_reg(0xD40A,0x00);
		my_lcd_write_reg(0xD40B,0x81);

		my_lcd_write_reg(0xD40C,0x00);
		my_lcd_write_reg(0xD40D,0xA6);
		my_lcd_write_reg(0xD40E,0x00);
		my_lcd_write_reg(0xD40F,0xE5);
		my_lcd_write_reg(0xD410,0x01);
		my_lcd_write_reg(0xD411,0x13);
		my_lcd_write_reg(0xD412,0x01);
		my_lcd_write_reg(0xD413,0x54);
		my_lcd_write_reg(0xD414,0x01);
		my_lcd_write_reg(0xD415,0x82);
		my_lcd_write_reg(0xD416,0x01);
		my_lcd_write_reg(0xD417,0xCA);
		my_lcd_write_reg(0xD418,0x02);
		my_lcd_write_reg(0xD419,0x00);
		my_lcd_write_reg(0xD41A,0x02);
		my_lcd_write_reg(0xD41B,0x01);
		my_lcd_write_reg(0xD41C,0x02);
		my_lcd_write_reg(0xD41D,0x34);
		my_lcd_write_reg(0xD41E,0x02);
		my_lcd_write_reg(0xD41F,0x67);
		my_lcd_write_reg(0xD420,0x02);
		my_lcd_write_reg(0xD421,0x84);
		my_lcd_write_reg(0xD422,0x02);
		my_lcd_write_reg(0xD423,0xA4);
		my_lcd_write_reg(0xD424,0x02);
		my_lcd_write_reg(0xD425,0xB7);
		my_lcd_write_reg(0xD426,0x02);
		my_lcd_write_reg(0xD427,0xCF);
		my_lcd_write_reg(0xD428,0x02);
		my_lcd_write_reg(0xD429,0xDE);
		my_lcd_write_reg(0xD42A,0x02);
		my_lcd_write_reg(0xD42B,0xF2);
		my_lcd_write_reg(0xD42C,0x02);
		my_lcd_write_reg(0xD42D,0xFE);
		my_lcd_write_reg(0xD42E,0x03);
		my_lcd_write_reg(0xD42F,0x10);
		my_lcd_write_reg(0xD430,0x03);
		my_lcd_write_reg(0xD431,0x33);
		my_lcd_write_reg(0xD432,0x03);
		my_lcd_write_reg(0xD433,0x6D);
		my_lcd_write_reg(0xD500,0x00);
		my_lcd_write_reg(0xD501,0x33);
		my_lcd_write_reg(0xD502,0x00);
		my_lcd_write_reg(0xD503,0x34);
		my_lcd_write_reg(0xD504,0x00);
		my_lcd_write_reg(0xD505,0x3A);
		my_lcd_write_reg(0xD506,0x00);
		my_lcd_write_reg(0xD507,0x4A);
		my_lcd_write_reg(0xD508,0x00);
		my_lcd_write_reg(0xD509,0x5C);
		my_lcd_write_reg(0xD50A,0x00);
		my_lcd_write_reg(0xD50B,0x81);

		my_lcd_write_reg(0xD50C,0x00);
		my_lcd_write_reg(0xD50D,0xA6);
		my_lcd_write_reg(0xD50E,0x00);
		my_lcd_write_reg(0xD50F,0xE5);
		my_lcd_write_reg(0xD510,0x01);
		my_lcd_write_reg(0xD511,0x13);
		my_lcd_write_reg(0xD512,0x01);
		my_lcd_write_reg(0xD513,0x54);
		my_lcd_write_reg(0xD514,0x01);
		my_lcd_write_reg(0xD515,0x82);
		my_lcd_write_reg(0xD516,0x01);
		my_lcd_write_reg(0xD517,0xCA);
		my_lcd_write_reg(0xD518,0x02);
		my_lcd_write_reg(0xD519,0x00);
		my_lcd_write_reg(0xD51A,0x02);
		my_lcd_write_reg(0xD51B,0x01);
		my_lcd_write_reg(0xD51C,0x02);
		my_lcd_write_reg(0xD51D,0x34);
		my_lcd_write_reg(0xD51E,0x02);
		my_lcd_write_reg(0xD51F,0x67);
		my_lcd_write_reg(0xD520,0x02);
		my_lcd_write_reg(0xD521,0x84);
		my_lcd_write_reg(0xD522,0x02);
		my_lcd_write_reg(0xD523,0xA4);
		my_lcd_write_reg(0xD524,0x02);
		my_lcd_write_reg(0xD525,0xB7);
		my_lcd_write_reg(0xD526,0x02);
		my_lcd_write_reg(0xD527,0xCF);
		my_lcd_write_reg(0xD528,0x02);
		my_lcd_write_reg(0xD529,0xDE);
		my_lcd_write_reg(0xD52A,0x02);
		my_lcd_write_reg(0xD52B,0xF2);
		my_lcd_write_reg(0xD52C,0x02);
		my_lcd_write_reg(0xD52D,0xFE);
		my_lcd_write_reg(0xD52E,0x03);
		my_lcd_write_reg(0xD52F,0x10);
		my_lcd_write_reg(0xD530,0x03);
		my_lcd_write_reg(0xD531,0x33);
		my_lcd_write_reg(0xD532,0x03);
		my_lcd_write_reg(0xD533,0x6D);
		my_lcd_write_reg(0xD600,0x00);
		my_lcd_write_reg(0xD601,0x33);
		my_lcd_write_reg(0xD602,0x00);
		my_lcd_write_reg(0xD603,0x34);
		my_lcd_write_reg(0xD604,0x00);
		my_lcd_write_reg(0xD605,0x3A);
		my_lcd_write_reg(0xD606,0x00);
		my_lcd_write_reg(0xD607,0x4A);
		my_lcd_write_reg(0xD608,0x00);
		my_lcd_write_reg(0xD609,0x5C);
		my_lcd_write_reg(0xD60A,0x00);
		my_lcd_write_reg(0xD60B,0x81);

		my_lcd_write_reg(0xD60C,0x00);
		my_lcd_write_reg(0xD60D,0xA6);
		my_lcd_write_reg(0xD60E,0x00);
		my_lcd_write_reg(0xD60F,0xE5);
		my_lcd_write_reg(0xD610,0x01);
		my_lcd_write_reg(0xD611,0x13);
		my_lcd_write_reg(0xD612,0x01);
		my_lcd_write_reg(0xD613,0x54);
		my_lcd_write_reg(0xD614,0x01);
		my_lcd_write_reg(0xD615,0x82);
		my_lcd_write_reg(0xD616,0x01);
		my_lcd_write_reg(0xD617,0xCA);
		my_lcd_write_reg(0xD618,0x02);
		my_lcd_write_reg(0xD619,0x00);
		my_lcd_write_reg(0xD61A,0x02);
		my_lcd_write_reg(0xD61B,0x01);
		my_lcd_write_reg(0xD61C,0x02);
		my_lcd_write_reg(0xD61D,0x34);
		my_lcd_write_reg(0xD61E,0x02);
		my_lcd_write_reg(0xD61F,0x67);
		my_lcd_write_reg(0xD620,0x02);
		my_lcd_write_reg(0xD621,0x84);
		my_lcd_write_reg(0xD622,0x02);
		my_lcd_write_reg(0xD623,0xA4);
		my_lcd_write_reg(0xD624,0x02);
		my_lcd_write_reg(0xD625,0xB7);
		my_lcd_write_reg(0xD626,0x02);
		my_lcd_write_reg(0xD627,0xCF);
		my_lcd_write_reg(0xD628,0x02);
		my_lcd_write_reg(0xD629,0xDE);
		my_lcd_write_reg(0xD62A,0x02);
		my_lcd_write_reg(0xD62B,0xF2);
		my_lcd_write_reg(0xD62C,0x02);
		my_lcd_write_reg(0xD62D,0xFE);
		my_lcd_write_reg(0xD62E,0x03);
		my_lcd_write_reg(0xD62F,0x10);
		my_lcd_write_reg(0xD630,0x03);
		my_lcd_write_reg(0xD631,0x33);
		my_lcd_write_reg(0xD632,0x03);
		my_lcd_write_reg(0xD633,0x6D);
		//LV2 Page 0 enable
		my_lcd_write_reg(0xF000,0x55);
		my_lcd_write_reg(0xF001,0xAA);
		my_lcd_write_reg(0xF002,0x52);
		my_lcd_write_reg(0xF003,0x08);
		my_lcd_write_reg(0xF004,0x00);
		//Display control
		my_lcd_write_reg(0xB100, 0xCC);
		my_lcd_write_reg(0xB101, 0x00);
		//Source hold time
		my_lcd_write_reg(0xB600,0x05);
		//Gate EQ control
		my_lcd_write_reg(0xB700,0x70);
		my_lcd_write_reg(0xB701,0x70);
		//Source EQ control (Mode 2)
		my_lcd_write_reg(0xB800,0x01);
		my_lcd_write_reg(0xB801,0x03);
		my_lcd_write_reg(0xB802,0x03);
		my_lcd_write_reg(0xB803,0x03);
		//Inversion mode (2-dot)
		my_lcd_write_reg(0xBC00,0x02);
		my_lcd_write_reg(0xBC01,0x00);
		my_lcd_write_reg(0xBC02,0x00);
		//Timing control 4H w/ 4-delay
		my_lcd_write_reg(0xC900,0xD0);
		my_lcd_write_reg(0xC901,0x02);
		my_lcd_write_reg(0xC902,0x50);
		my_lcd_write_reg(0xC903,0x50);
		my_lcd_write_reg(0xC904,0x50);
		my_lcd_write_reg(0x3500,0x00);
		my_lcd_write_reg(0x3A00,0x55);  //16-bit/pixel
		my_lcd_wr_reg(0x1100);
		delay_us(120);
		my_lcd_wr_reg(0x2900);
	}else if(lcd_dev.id==0x9325)//9325
	{
		my_lcd_write_reg(0x00E5,0x78F0); 
		my_lcd_write_reg(0x0001,0x0100); 
		my_lcd_write_reg(0x0002,0x0700); 
		my_lcd_write_reg(0x0003,0x1030); 
		my_lcd_write_reg(0x0004,0x0000); 
		my_lcd_write_reg(0x0008,0x0202);  
		my_lcd_write_reg(0x0009,0x0000);
		my_lcd_write_reg(0x000A,0x0000); 
		my_lcd_write_reg(0x000C,0x0000); 
		my_lcd_write_reg(0x000D,0x0000);
		my_lcd_write_reg(0x000F,0x0000);
		//power on sequence VGHVGL
		my_lcd_write_reg(0x0010,0x0000);   
		my_lcd_write_reg(0x0011,0x0007);  
		my_lcd_write_reg(0x0012,0x0000);  
		my_lcd_write_reg(0x0013,0x0000); 
		my_lcd_write_reg(0x0007,0x0000); 
		//vgh 
		my_lcd_write_reg(0x0010,0x1690);   
		my_lcd_write_reg(0x0011,0x0227);
		//delayms(100);
		//vregiout 
		my_lcd_write_reg(0x0012,0x009D); //0x001b
		//delayms(100); 
		//vom amplitude
		my_lcd_write_reg(0x0013,0x1900);
		//delayms(100); 
		//vom H
		my_lcd_write_reg(0x0029,0x0025); 
		my_lcd_write_reg(0x002B,0x000D); 
		//gamma
		my_lcd_write_reg(0x0030,0x0007);
		my_lcd_write_reg(0x0031,0x0303);
		my_lcd_write_reg(0x0032,0x0003);// 0006
		my_lcd_write_reg(0x0035,0x0206);
		my_lcd_write_reg(0x0036,0x0008);
		my_lcd_write_reg(0x0037,0x0406); 
		my_lcd_write_reg(0x0038,0x0304);//0200
		my_lcd_write_reg(0x0039,0x0007); 
		my_lcd_write_reg(0x003C,0x0602);// 0504
		my_lcd_write_reg(0x003D,0x0008); 
		//ram
		my_lcd_write_reg(0x0050,0x0000); 
		my_lcd_write_reg(0x0051,0x00EF);
		my_lcd_write_reg(0x0052,0x0000); 
		my_lcd_write_reg(0x0053,0x013F);  
		my_lcd_write_reg(0x0060,0xA700); 
		my_lcd_write_reg(0x0061,0x0001); 
		my_lcd_write_reg(0x006A,0x0000); 
		//
		my_lcd_write_reg(0x0080,0x0000); 
		my_lcd_write_reg(0x0081,0x0000); 
		my_lcd_write_reg(0x0082,0x0000); 
		my_lcd_write_reg(0x0083,0x0000); 
		my_lcd_write_reg(0x0084,0x0000); 
		my_lcd_write_reg(0x0085,0x0000); 
		//
		my_lcd_write_reg(0x0090,0x0010); 
		my_lcd_write_reg(0x0092,0x0600); 
		
		my_lcd_write_reg(0x0007,0x0133);
		my_lcd_write_reg(0x00,0x0022);//
	}else if(lcd_dev.id==0x9328)//ILI9328   OK  
	{
  		my_lcd_write_reg(0x00EC,0x108F);// internal timeing      
 		my_lcd_write_reg(0x00EF,0x1234);// ADD        
		//my_lcd_write_reg(0x00e7,0x0010);      
        //my_lcd_write_reg(0x0000,0x0001);//开启内部时钟
        my_lcd_write_reg(0x0001,0x0100);     
        my_lcd_write_reg(0x0002,0x0700);//电源开启                    
		//my_lcd_write_reg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
        my_lcd_write_reg(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
        my_lcd_write_reg(0x0004,0x0000);                                   
        my_lcd_write_reg(0x0008,0x0207);	           
        my_lcd_write_reg(0x0009,0x0000);         
        my_lcd_write_reg(0x000a,0x0000);//display setting         
        my_lcd_write_reg(0x000c,0x0001);//display setting          
        my_lcd_write_reg(0x000d,0x0000);//0f3c          
        my_lcd_write_reg(0x000f,0x0000);
		//电源配置
        my_lcd_write_reg(0x0010,0x0000);   
        my_lcd_write_reg(0x0011,0x0007);
        my_lcd_write_reg(0x0012,0x0000);                                                                 
        my_lcd_write_reg(0x0013,0x0000);                                 
       	delay_ms(50); 
        my_lcd_write_reg(0x0010,0x1590);   
        my_lcd_write_reg(0x0011,0x0227);
        delay_ms(50); 
        my_lcd_write_reg(0x0012,0x009c);                  
        delay_ms(50); 
        my_lcd_write_reg(0x0013,0x1900);   
        my_lcd_write_reg(0x0029,0x0023);
        my_lcd_write_reg(0x002b,0x000e);
        delay_ms(50); 
        my_lcd_write_reg(0x0020,0x0000);                                                            
        my_lcd_write_reg(0x0021,0x013f);           
		delay_ms(50); 
		//伽马校正
        my_lcd_write_reg(0x0030,0x0007); 
        my_lcd_write_reg(0x0031,0x0707);   
        my_lcd_write_reg(0x0032,0x0006);
        my_lcd_write_reg(0x0035,0x0704);
        my_lcd_write_reg(0x0036,0x1f04); 
        my_lcd_write_reg(0x0037,0x0004);
        my_lcd_write_reg(0x0038,0x0000);        
        my_lcd_write_reg(0x0039,0x0706);     
        my_lcd_write_reg(0x003c,0x0701);
        my_lcd_write_reg(0x003d,0x000f);
        delay_ms(50); 
        my_lcd_write_reg(0x0050,0x0000); //水平GRAM起始位置 
        my_lcd_write_reg(0x0051,0x00ef); //水平GRAM终止位置                    
        my_lcd_write_reg(0x0052,0x0000); //垂直GRAM起始位置                    
        my_lcd_write_reg(0x0053,0x013f); //垂直GRAM终止位置  
 
         my_lcd_write_reg(0x0060,0xa700);        
        my_lcd_write_reg(0x0061,0x0001); 
        my_lcd_write_reg(0x006a,0x0000);
        my_lcd_write_reg(0x0080,0x0000);
        my_lcd_write_reg(0x0081,0x0000);
        my_lcd_write_reg(0x0082,0x0000);
        my_lcd_write_reg(0x0083,0x0000);
        my_lcd_write_reg(0x0084,0x0000);
        my_lcd_write_reg(0x0085,0x0000);
      
        my_lcd_write_reg(0x0090,0x0010);     
        my_lcd_write_reg(0x0092,0x0600);  
        //开启显示设置    
        my_lcd_write_reg(0x0007,0x0133); 
	}else if(lcd_dev.id==0x9320)//测试OK.
	{
		my_lcd_write_reg(0x00,0x0000);
		my_lcd_write_reg(0x01,0x0100);	//Driver Output Contral.
		my_lcd_write_reg(0x02,0x0700);	//LCD Driver Waveform Contral.
		my_lcd_write_reg(0x03,0x1030);//Entry Mode Set.
		//my_lcd_write_reg(0x03,0x1018);	//Entry Mode Set.
	
		my_lcd_write_reg(0x04,0x0000);	//Scalling Contral.
		my_lcd_write_reg(0x08,0x0202);	//Display Contral 2.(0x0207)
		my_lcd_write_reg(0x09,0x0000);	//Display Contral 3.(0x0000)
		my_lcd_write_reg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		my_lcd_write_reg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		my_lcd_write_reg(0x0d,0x0000);	//Frame Maker Position.
		my_lcd_write_reg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		delay_ms(50); 
		my_lcd_write_reg(0x07,0x0101);	//Display Contral.
		delay_ms(50); 								  
		my_lcd_write_reg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		my_lcd_write_reg(0x11,0x0007);								//Power Control 2.(0x0001)
		my_lcd_write_reg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		my_lcd_write_reg(0x13,0x0b00);								//Power Control 4.
		my_lcd_write_reg(0x29,0x0000);								//Power Control 7.
	
		my_lcd_write_reg(0x2b,(1<<14)|(1<<4));	    
		my_lcd_write_reg(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		my_lcd_write_reg(0x51,239);	//Set Y Star
		my_lcd_write_reg(0x52,0);	//Set Y End.t.
		my_lcd_write_reg(0x53,319);	//
	
		my_lcd_write_reg(0x60,0x2700);	//Driver Output Control.
		my_lcd_write_reg(0x61,0x0001);	//Driver Output Control.
		my_lcd_write_reg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		my_lcd_write_reg(0x80,0x0000);	//Display Position? Partial Display 1.
		my_lcd_write_reg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		my_lcd_write_reg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		my_lcd_write_reg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		my_lcd_write_reg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		my_lcd_write_reg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		my_lcd_write_reg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		my_lcd_write_reg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		my_lcd_write_reg(0x93,0x0001);	//Panel Interface Contral 3.
		my_lcd_write_reg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		my_lcd_write_reg(0x97,(0<<8));	//
		my_lcd_write_reg(0x98,0x0000);	//Frame Cycle Contral.	   
		my_lcd_write_reg(0x07,0x0173);	//(0x0173)
	}else if(lcd_dev.id==0X9331)//OK |/|/|			 
	{
		my_lcd_write_reg(0x00E7, 0x1014);
		my_lcd_write_reg(0x0001, 0x0100); // set SS and SM bit
		my_lcd_write_reg(0x0002, 0x0200); // set 1 line inversion
        my_lcd_write_reg(0x0003,(1<<12)|(3<<4)|(1<<3));//65K    
		//my_lcd_write_reg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
		my_lcd_write_reg(0x0008, 0x0202); // set the back porch and front porch
		my_lcd_write_reg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
		my_lcd_write_reg(0x000A, 0x0000); // FMARK function
		my_lcd_write_reg(0x000C, 0x0000); // RGB interface setting
		my_lcd_write_reg(0x000D, 0x0000); // Frame marker Position
		my_lcd_write_reg(0x000F, 0x0000); // RGB interface polarity
		//*************Power On sequence ****************//
		my_lcd_write_reg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
		my_lcd_write_reg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
		my_lcd_write_reg(0x0012, 0x0000); // VREG1OUT voltage
		my_lcd_write_reg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
		delay_ms(200); // Dis-charge capacitor power voltage
		my_lcd_write_reg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
		my_lcd_write_reg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		delay_ms(50); // Delay 50ms
		my_lcd_write_reg(0x0012, 0x000C); // Internal reference voltage= Vci;
		delay_ms(50); // Delay 50ms
		my_lcd_write_reg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
		my_lcd_write_reg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
		my_lcd_write_reg(0x002B, 0x000B); // Set Frame Rate
		delay_ms(50); // Delay 50ms
		my_lcd_write_reg(0x0020, 0x0000); // GRAM horizontal Address
		my_lcd_write_reg(0x0021, 0x013f); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//
		my_lcd_write_reg(0x0030, 0x0000);
		my_lcd_write_reg(0x0031, 0x0106);
		my_lcd_write_reg(0x0032, 0x0000);
		my_lcd_write_reg(0x0035, 0x0204);
		my_lcd_write_reg(0x0036, 0x160A);
		my_lcd_write_reg(0x0037, 0x0707);
		my_lcd_write_reg(0x0038, 0x0106);
		my_lcd_write_reg(0x0039, 0x0707);
		my_lcd_write_reg(0x003C, 0x0402);
		my_lcd_write_reg(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		my_lcd_write_reg(0x0050, 0x0000); // Horizontal GRAM Start Address
		my_lcd_write_reg(0x0051, 0x00EF); // Horizontal GRAM End Address
		my_lcd_write_reg(0x0052, 0x0000); // Vertical GRAM Start Address
		my_lcd_write_reg(0x0053, 0x013F); // Vertical GRAM Start Address
		my_lcd_write_reg(0x0060, 0x2700); // Gate Scan Line
		my_lcd_write_reg(0x0061, 0x0001); // NDL,VLE, REV 
		my_lcd_write_reg(0x006A, 0x0000); // set scrolling line
		//-------------- Partial Display Control ---------//
		my_lcd_write_reg(0x0080, 0x0000);
		my_lcd_write_reg(0x0081, 0x0000);
		my_lcd_write_reg(0x0082, 0x0000);
		my_lcd_write_reg(0x0083, 0x0000);
		my_lcd_write_reg(0x0084, 0x0000);
		my_lcd_write_reg(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		my_lcd_write_reg(0x0090, 0x0010);
		my_lcd_write_reg(0x0092, 0x0600);
		my_lcd_write_reg(0x0007, 0x0133); // 262K color and display ON
	}else if(lcd_dev.id==0x5408)
	{
		my_lcd_write_reg(0x01,0x0100);								  
		my_lcd_write_reg(0x02,0x0700);//LCD Driving Waveform Contral 
		my_lcd_write_reg(0x03,0x1030);//Entry Mode设置 	   
		//指针从左至右自上而下的自动增模式
		//Normal Mode(Window Mode disable)
		//RGB格式
		//16位数据2次传输的8总线设置
		my_lcd_write_reg(0x04,0x0000); //Scalling Control register     
		my_lcd_write_reg(0x08,0x0207); //Display Control 2 
		my_lcd_write_reg(0x09,0x0000); //Display Control 3	 
		my_lcd_write_reg(0x0A,0x0000); //Frame Cycle Control	 
		my_lcd_write_reg(0x0C,0x0000); //External Display Interface Control 1 
		my_lcd_write_reg(0x0D,0x0000); //Frame Maker Position		 
		my_lcd_write_reg(0x0F,0x0000); //External Display Interface Control 2 
 		delay_ms(20);
		//TFT 液晶彩色图像显示方法14
		my_lcd_write_reg(0x10,0x16B0); //0x14B0 //Power Control 1
		my_lcd_write_reg(0x11,0x0001); //0x0007 //Power Control 2
		my_lcd_write_reg(0x17,0x0001); //0x0000 //Power Control 3
		my_lcd_write_reg(0x12,0x0138); //0x013B //Power Control 4
		my_lcd_write_reg(0x13,0x0800); //0x0800 //Power Control 5
		my_lcd_write_reg(0x29,0x0009); //NVM read data 2
		my_lcd_write_reg(0x2a,0x0009); //NVM read data 3
		my_lcd_write_reg(0xa4,0x0000);	 
		my_lcd_write_reg(0x50,0x0000); //设置操作窗口的X轴开始列
		my_lcd_write_reg(0x51,0x00EF); //设置操作窗口的X轴结束列
		my_lcd_write_reg(0x52,0x0000); //设置操作窗口的Y轴开始行
		my_lcd_write_reg(0x53,0x013F); //设置操作窗口的Y轴结束行
		my_lcd_write_reg(0x60,0x2700); //Driver Output Control
		//设置屏幕的点数以及扫描的起始行
		my_lcd_write_reg(0x61,0x0001); //Driver Output Control
		my_lcd_write_reg(0x6A,0x0000); //Vertical Scroll Control
		my_lcd_write_reg(0x80,0x0000); //Display Position – Partial Display 1
		my_lcd_write_reg(0x81,0x0000); //RAM Address Start – Partial Display 1
		my_lcd_write_reg(0x82,0x0000); //RAM address End - Partial Display 1
		my_lcd_write_reg(0x83,0x0000); //Display Position – Partial Display 2
		my_lcd_write_reg(0x84,0x0000); //RAM Address Start – Partial Display 2
		my_lcd_write_reg(0x85,0x0000); //RAM address End – Partail Display2
		my_lcd_write_reg(0x90,0x0013); //Frame Cycle Control
		my_lcd_write_reg(0x92,0x0000);  //Panel Interface Control 2
		my_lcd_write_reg(0x93,0x0003); //Panel Interface control 3
		my_lcd_write_reg(0x95,0x0110);  //Frame Cycle Control
		my_lcd_write_reg(0x07,0x0173);		 
		delay_ms(50);
	}else if(lcd_dev.id==0x1505)//OK
	{
		// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        my_lcd_write_reg(0x0007,0x0000);
        delay_ms(50); 
        my_lcd_write_reg(0x0012,0x011C);//0x011A   why need to set several times?
        my_lcd_write_reg(0x00A4,0x0001);//NVM	 
        my_lcd_write_reg(0x0008,0x000F);
        my_lcd_write_reg(0x000A,0x0008);
        my_lcd_write_reg(0x000D,0x0008);	    
  		//伽马校正
        my_lcd_write_reg(0x0030,0x0707);
        my_lcd_write_reg(0x0031,0x0007); //0x0707
        my_lcd_write_reg(0x0032,0x0603); 
        my_lcd_write_reg(0x0033,0x0700); 
        my_lcd_write_reg(0x0034,0x0202); 
        my_lcd_write_reg(0x0035,0x0002); //?0x0606
        my_lcd_write_reg(0x0036,0x1F0F);
        my_lcd_write_reg(0x0037,0x0707); //0x0f0f  0x0105
        my_lcd_write_reg(0x0038,0x0000); 
        my_lcd_write_reg(0x0039,0x0000); 
        my_lcd_write_reg(0x003A,0x0707); 
        my_lcd_write_reg(0x003B,0x0000); //0x0303
        my_lcd_write_reg(0x003C,0x0007); //?0x0707
        my_lcd_write_reg(0x003D,0x0000); //0x1313//0x1f08
        delay_ms(50); 
        my_lcd_write_reg(0x0007,0x0001);
        my_lcd_write_reg(0x0017,0x0001);//开启电源
        delay_ms(50); 
  		//电源配置
        my_lcd_write_reg(0x0010,0x17A0); 
        my_lcd_write_reg(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        my_lcd_write_reg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        my_lcd_write_reg(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        my_lcd_write_reg(0x002A,0x0000);  
        my_lcd_write_reg(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        my_lcd_write_reg(0x0012,0x013E);// 0x013C  power supply on
        //Coordinates Control//
        my_lcd_write_reg(0x0050,0x0000);//0x0e00
        my_lcd_write_reg(0x0051,0x00EF); 
        my_lcd_write_reg(0x0052,0x0000); 
        my_lcd_write_reg(0x0053,0x013F); 
    	//Pannel Image Control//
        my_lcd_write_reg(0x0060,0x2700); 
        my_lcd_write_reg(0x0061,0x0001); 
        my_lcd_write_reg(0x006A,0x0000); 
        my_lcd_write_reg(0x0080,0x0000); 
    	//Partial Image Control//
        my_lcd_write_reg(0x0081,0x0000); 
        my_lcd_write_reg(0x0082,0x0000); 
        my_lcd_write_reg(0x0083,0x0000); 
        my_lcd_write_reg(0x0084,0x0000); 
        my_lcd_write_reg(0x0085,0x0000); 
  		//Panel Interface Control//
        my_lcd_write_reg(0x0090,0x0013);//0x0010 frenqucy
        my_lcd_write_reg(0x0092,0x0300); 
        my_lcd_write_reg(0x0093,0x0005); 
        my_lcd_write_reg(0x0095,0x0000); 
        my_lcd_write_reg(0x0097,0x0000); 
        my_lcd_write_reg(0x0098,0x0000); 
  
        my_lcd_write_reg(0x0001,0x0100); 
        my_lcd_write_reg(0x0002,0x0700); 
        my_lcd_write_reg(0x0003,0x1038);//扫描方向 上->下  左->右 
        my_lcd_write_reg(0x0004,0x0000); 
        my_lcd_write_reg(0x000C,0x0000); 
        my_lcd_write_reg(0x000F,0x0000); 
        my_lcd_write_reg(0x0020,0x0000); 
        my_lcd_write_reg(0x0021,0x0000); 
        my_lcd_write_reg(0x0007,0x0021); 
        delay_ms(20);
        my_lcd_write_reg(0x0007,0x0061); 
        delay_ms(20);
        my_lcd_write_reg(0x0007,0x0173); 
        delay_ms(20);
	}else if(lcd_dev.id==0xB505)
	{
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		
		my_lcd_write_reg(0x00a4,0x0001);
		delay_ms(20);		  
		my_lcd_write_reg(0x0060,0x2700);
		my_lcd_write_reg(0x0008,0x0202);
		
		my_lcd_write_reg(0x0030,0x0214);
		my_lcd_write_reg(0x0031,0x3715);
		my_lcd_write_reg(0x0032,0x0604);
		my_lcd_write_reg(0x0033,0x0e16);
		my_lcd_write_reg(0x0034,0x2211);
		my_lcd_write_reg(0x0035,0x1500);
		my_lcd_write_reg(0x0036,0x8507);
		my_lcd_write_reg(0x0037,0x1407);
		my_lcd_write_reg(0x0038,0x1403);
		my_lcd_write_reg(0x0039,0x0020);
		
		my_lcd_write_reg(0x0090,0x001a);
		my_lcd_write_reg(0x0010,0x0000);
		my_lcd_write_reg(0x0011,0x0007);
		my_lcd_write_reg(0x0012,0x0000);
		my_lcd_write_reg(0x0013,0x0000);
		delay_ms(20);
		
		my_lcd_write_reg(0x0010,0x0730);
		my_lcd_write_reg(0x0011,0x0137);
		delay_ms(20);
		
		my_lcd_write_reg(0x0012,0x01b8);
		delay_ms(20);
		
		my_lcd_write_reg(0x0013,0x0f00);
		my_lcd_write_reg(0x002a,0x0080);
		my_lcd_write_reg(0x0029,0x0048);
		delay_ms(20);
		
		my_lcd_write_reg(0x0001,0x0100);
		my_lcd_write_reg(0x0002,0x0700);
        my_lcd_write_reg(0x0003,0x1038);//扫描方向 上->下  左->右 
		my_lcd_write_reg(0x0008,0x0202);
		my_lcd_write_reg(0x000a,0x0000);
		my_lcd_write_reg(0x000c,0x0000);
		my_lcd_write_reg(0x000d,0x0000);
		my_lcd_write_reg(0x000e,0x0030);
		my_lcd_write_reg(0x0050,0x0000);
		my_lcd_write_reg(0x0051,0x00ef);
		my_lcd_write_reg(0x0052,0x0000);
		my_lcd_write_reg(0x0053,0x013f);
		my_lcd_write_reg(0x0060,0x2700);
		my_lcd_write_reg(0x0061,0x0001);
		my_lcd_write_reg(0x006a,0x0000);
		//my_lcd_write_reg(0x0080,0x0000);
		//my_lcd_write_reg(0x0081,0x0000);
		my_lcd_write_reg(0x0090,0X0011);
		my_lcd_write_reg(0x0092,0x0600);
		my_lcd_write_reg(0x0093,0x0402);
		my_lcd_write_reg(0x0094,0x0002);
		delay_ms(20);
		
		my_lcd_write_reg(0x0007,0x0001);
		delay_ms(20);
		my_lcd_write_reg(0x0007,0x0061);
		my_lcd_write_reg(0x0007,0x0173);
		
		my_lcd_write_reg(0x0020,0x0000);
		my_lcd_write_reg(0x0021,0x0000);	  
		my_lcd_write_reg(0x00,0x22);  
	}else if(lcd_dev.id==0xC505)
	{
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		delay_ms(20);		  
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
		my_lcd_write_reg(0x0000,0x0000);
 		my_lcd_write_reg(0x00a4,0x0001);
		delay_ms(20);		  
		my_lcd_write_reg(0x0060,0x2700);
		my_lcd_write_reg(0x0008,0x0806);
		
		my_lcd_write_reg(0x0030,0x0703);//gamma setting
		my_lcd_write_reg(0x0031,0x0001);
		my_lcd_write_reg(0x0032,0x0004);
		my_lcd_write_reg(0x0033,0x0102);
		my_lcd_write_reg(0x0034,0x0300);
		my_lcd_write_reg(0x0035,0x0103);
		my_lcd_write_reg(0x0036,0x001F);
		my_lcd_write_reg(0x0037,0x0703);
		my_lcd_write_reg(0x0038,0x0001);
		my_lcd_write_reg(0x0039,0x0004);
		
		my_lcd_write_reg(0x0090, 0x0015);	//80Hz
		my_lcd_write_reg(0x0010, 0X0410);	//BT,AP
		my_lcd_write_reg(0x0011,0x0247);	//DC1,DC0,VC
		my_lcd_write_reg(0x0012, 0x01BC);
		my_lcd_write_reg(0x0013, 0x0e00);
		delay_ms(120);
		my_lcd_write_reg(0x0001, 0x0100);
		my_lcd_write_reg(0x0002, 0x0200);
		my_lcd_write_reg(0x0003, 0x1030);
		
		my_lcd_write_reg(0x000A, 0x0008);
		my_lcd_write_reg(0x000C, 0x0000);
		
		my_lcd_write_reg(0x000E, 0x0020);
		my_lcd_write_reg(0x000F, 0x0000);
		my_lcd_write_reg(0x0020, 0x0000);	//H Start
		my_lcd_write_reg(0x0021, 0x0000);	//V Start
		my_lcd_write_reg(0x002A,0x003D);	//vcom2
		delay_ms(20);
		my_lcd_write_reg(0x0029, 0x002d);
		my_lcd_write_reg(0x0050, 0x0000);
		my_lcd_write_reg(0x0051, 0xD0EF);
		my_lcd_write_reg(0x0052, 0x0000);
		my_lcd_write_reg(0x0053, 0x013F);
		my_lcd_write_reg(0x0061, 0x0000);
		my_lcd_write_reg(0x006A, 0x0000);
		my_lcd_write_reg(0x0092,0x0300); 
 
 		my_lcd_write_reg(0x0093, 0x0005);
		my_lcd_write_reg(0x0007, 0x0100);
	}else if(lcd_dev.id==0x8989)//OK |/|/|
	{	   
		my_lcd_write_reg(0x0000,0x0001);//打开晶振
    	my_lcd_write_reg(0x0003,0xA8A4);//0xA8A4
    	my_lcd_write_reg(0x000C,0x0000);    
    	my_lcd_write_reg(0x000D,0x080C);   
    	my_lcd_write_reg(0x000E,0x2B00);    
    	my_lcd_write_reg(0x001E,0x00B0);    
    	my_lcd_write_reg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
    	my_lcd_write_reg(0x0002,0x0600);
    	my_lcd_write_reg(0x0010,0x0000);  
    	my_lcd_write_reg(0x0011,0x6078); //定义数据格式  16位色 		横屏 0x6058
    	my_lcd_write_reg(0x0005,0x0000);  
    	my_lcd_write_reg(0x0006,0x0000);  
    	my_lcd_write_reg(0x0016,0xEF1C);  
    	my_lcd_write_reg(0x0017,0x0003);  
    	my_lcd_write_reg(0x0007,0x0233); //0x0233       
    	my_lcd_write_reg(0x000B,0x0000);  
    	my_lcd_write_reg(0x000F,0x0000); //扫描开始地址
    	my_lcd_write_reg(0x0041,0x0000);  
    	my_lcd_write_reg(0x0042,0x0000);  
    	my_lcd_write_reg(0x0048,0x0000);  
    	my_lcd_write_reg(0x0049,0x013F);  
    	my_lcd_write_reg(0x004A,0x0000);  
    	my_lcd_write_reg(0x004B,0x0000);  
    	my_lcd_write_reg(0x0044,0xEF00);  
    	my_lcd_write_reg(0x0045,0x0000);  
    	my_lcd_write_reg(0x0046,0x013F);  
    	my_lcd_write_reg(0x0030,0x0707);  
    	my_lcd_write_reg(0x0031,0x0204);  
    	my_lcd_write_reg(0x0032,0x0204);  
    	my_lcd_write_reg(0x0033,0x0502);  
    	my_lcd_write_reg(0x0034,0x0507);  
    	my_lcd_write_reg(0x0035,0x0204);  
    	my_lcd_write_reg(0x0036,0x0204);  
    	my_lcd_write_reg(0x0037,0x0502);  
    	my_lcd_write_reg(0x003A,0x0302);  
    	my_lcd_write_reg(0x003B,0x0302);  
    	my_lcd_write_reg(0x0023,0x0000);  
    	my_lcd_write_reg(0x0024,0x0000);  
    	my_lcd_write_reg(0x0025,0x8000);  
    	my_lcd_write_reg(0x004f,0);        //行首址0
    	my_lcd_write_reg(0x004e,0);        //列首址0
	}else if(lcd_dev.id==0x4531)//OK |/|/|
	{
		my_lcd_write_reg(0X00,0X0001);   
		delay_ms(10);   
		my_lcd_write_reg(0X10,0X1628);   
		my_lcd_write_reg(0X12,0X000e);//0x0006    
		my_lcd_write_reg(0X13,0X0A39);   
		delay_ms(10);   
		my_lcd_write_reg(0X11,0X0040);   
		my_lcd_write_reg(0X15,0X0050);   
		delay_ms(10);   
		my_lcd_write_reg(0X12,0X001e);//16    
		delay_ms(10);   
		my_lcd_write_reg(0X10,0X1620);   
		my_lcd_write_reg(0X13,0X2A39);   
		delay_ms(10);   
		my_lcd_write_reg(0X01,0X0100);   
		my_lcd_write_reg(0X02,0X0300);   
		my_lcd_write_reg(0X03,0X1038);//改变方向的   
		my_lcd_write_reg(0X08,0X0202);   
		my_lcd_write_reg(0X0A,0X0008);   
		my_lcd_write_reg(0X30,0X0000);   
		my_lcd_write_reg(0X31,0X0402);   
		my_lcd_write_reg(0X32,0X0106);   
		my_lcd_write_reg(0X33,0X0503);   
		my_lcd_write_reg(0X34,0X0104);   
		my_lcd_write_reg(0X35,0X0301);   
		my_lcd_write_reg(0X36,0X0707);   
		my_lcd_write_reg(0X37,0X0305);   
		my_lcd_write_reg(0X38,0X0208);   
		my_lcd_write_reg(0X39,0X0F0B);   
		my_lcd_write_reg(0X41,0X0002);   
		my_lcd_write_reg(0X60,0X2700);   
		my_lcd_write_reg(0X61,0X0001);   
		my_lcd_write_reg(0X90,0X0210);   
		my_lcd_write_reg(0X92,0X010A);   
		my_lcd_write_reg(0X93,0X0004);   
		my_lcd_write_reg(0XA0,0X0100);   
		my_lcd_write_reg(0X07,0X0001);   
		my_lcd_write_reg(0X07,0X0021);   
		my_lcd_write_reg(0X07,0X0023);   
		my_lcd_write_reg(0X07,0X0033);   
		my_lcd_write_reg(0X07,0X0133);   
		my_lcd_write_reg(0XA0,0X0000); 
	}else if(lcd_dev.id==0x4535)
	{			      
		my_lcd_write_reg(0X15,0X0030);   
		my_lcd_write_reg(0X9A,0X0010);   
 		my_lcd_write_reg(0X11,0X0020);   
 		my_lcd_write_reg(0X10,0X3428);   
		my_lcd_write_reg(0X12,0X0002);//16    
 		my_lcd_write_reg(0X13,0X1038);   
		delay_ms(40);   
		my_lcd_write_reg(0X12,0X0012);//16    
		delay_ms(40);   
  		my_lcd_write_reg(0X10,0X3420);   
 		my_lcd_write_reg(0X13,0X3038);   
		delay_ms(70);   
		my_lcd_write_reg(0X30,0X0000);   
		my_lcd_write_reg(0X31,0X0402);   
		my_lcd_write_reg(0X32,0X0307);   
		my_lcd_write_reg(0X33,0X0304);   
		my_lcd_write_reg(0X34,0X0004);   
		my_lcd_write_reg(0X35,0X0401);   
		my_lcd_write_reg(0X36,0X0707);   
		my_lcd_write_reg(0X37,0X0305);   
		my_lcd_write_reg(0X38,0X0610);   
		my_lcd_write_reg(0X39,0X0610); 
		  
		my_lcd_write_reg(0X01,0X0100);   
		my_lcd_write_reg(0X02,0X0300);   
		my_lcd_write_reg(0X03,0X1030);//改变方向的   
		my_lcd_write_reg(0X08,0X0808);   
		my_lcd_write_reg(0X0A,0X0008);   
 		my_lcd_write_reg(0X60,0X2700);   
		my_lcd_write_reg(0X61,0X0001);   
		my_lcd_write_reg(0X90,0X013E);   
		my_lcd_write_reg(0X92,0X0100);   
		my_lcd_write_reg(0X93,0X0100);   
 		my_lcd_write_reg(0XA0,0X3000);   
 		my_lcd_write_reg(0XA3,0X0010);   
		my_lcd_write_reg(0X07,0X0001);   
		my_lcd_write_reg(0X07,0X0021);   
		my_lcd_write_reg(0X07,0X0023);   
		my_lcd_write_reg(0X07,0X0033);   
		my_lcd_write_reg(0X07,0X0133);   
	}else if(lcd_dev.id==0X1963)
	{
		my_lcd_wr_reg(0xE2);		//Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
		my_lcd_wr_data(0x1D);		//参数1 
		my_lcd_wr_data(0x02);		//参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
		my_lcd_wr_data(0x04);		//参数3 Validate M and N values   
		delay_us(100);
		my_lcd_wr_reg(0xE0);		// Start PLL command
		my_lcd_wr_data(0x01);		// enable PLL
		delay_ms(10);
		my_lcd_wr_reg(0xE0);		// Start PLL command again
		my_lcd_wr_data(0x03);		// now, use PLL output as system clock	
		delay_ms(12);  
		my_lcd_wr_reg(0x01);		//软复位
		delay_ms(10);
		
		my_lcd_wr_reg(0xE6);		//设置像素频率,33Mhz
		my_lcd_wr_data(0x2F);
		my_lcd_wr_data(0xFF);
		my_lcd_wr_data(0xFF);
		
		my_lcd_wr_reg(0xB0);		//设置LCD模式
		my_lcd_wr_data(0x20);		//24位模式
		my_lcd_wr_data(0x00);		//TFT 模式 
	
		my_lcd_wr_data((SSD_HOR_RESOLUTION-1)>>8);//设置LCD水平像素
		my_lcd_wr_data(SSD_HOR_RESOLUTION-1);		 
		my_lcd_wr_data((SSD_VER_RESOLUTION-1)>>8);//设置LCD垂直像素
		my_lcd_wr_data(SSD_VER_RESOLUTION-1);		 
		my_lcd_wr_data(0x00);		//RGB序列 
		
		my_lcd_wr_reg(0xB4);		//Set horizontal period
		my_lcd_wr_data((SSD_HT-1)>>8);
		my_lcd_wr_data(SSD_HT-1);
		my_lcd_wr_data(SSD_HPS>>8);
		my_lcd_wr_data(SSD_HPS);
		my_lcd_wr_data(SSD_HOR_PULSE_WIDTH-1);
		my_lcd_wr_data(0x00);
		my_lcd_wr_data(0x00);
		my_lcd_wr_data(0x00);
		my_lcd_wr_reg(0xB6);		//Set vertical period
		my_lcd_wr_data((SSD_VT-1)>>8);
		my_lcd_wr_data(SSD_VT-1);
		my_lcd_wr_data(SSD_VPS>>8);
		my_lcd_wr_data(SSD_VPS);
		my_lcd_wr_data(SSD_VER_FRONT_PORCH-1);
		my_lcd_wr_data(0x00);
		my_lcd_wr_data(0x00);
		
		my_lcd_wr_reg(0xF0);	//设置SSD1963与CPU接口为16bit  
		my_lcd_wr_data(0x03);	//16-bit(565 format) data for 16bpp 

		my_lcd_wr_reg(0x29);	//开启显示
		//设置PWM输出  背光通过占空比可调 
		my_lcd_wr_reg(0xD0);	//设置自动白平衡DBC
		my_lcd_wr_data(0x00);	//disable
	
		my_lcd_wr_reg(0xBE);	//配置PWM输出
		my_lcd_wr_data(0x05);	//1设置PWM频率
		my_lcd_wr_data(0xFE);	//2设置PWM占空比
		my_lcd_wr_data(0x01);	//3设置C
		my_lcd_wr_data(0x00);	//4设置D
		my_lcd_wr_data(0x00);	//5设置E 
		my_lcd_wr_data(0x00);	//6设置F 
		
		my_lcd_wr_reg(0xB8);	//设置GPIO配置
		my_lcd_wr_data(0x03);	//2个IO口设置成输出
		my_lcd_wr_data(0x01);	//GPIO使用正常的IO功能 
		my_lcd_wr_reg(0xBA);
		my_lcd_wr_data(0X01);	//GPIO[1:0]=01,控制LCD方向
		
		my_lcd_ssd_backlight_set(100);//背光设置为最亮
	}	
	my_lcd_display_dir(0);		 	//默认为竖屏
	//LCD_LED=1;					//点亮背光
	my_lcd_clear(WHITE);
}

