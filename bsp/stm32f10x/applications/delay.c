#include "delay.h"

void delay_us(u32 num)
{
    u32 i=0;

     while(num--){
      i=12; while(i--);
     }
}

void delay_ms(u16 num)
{
    u32 i;

     while(num--){
      i=12000; while(i--);
    }
}
