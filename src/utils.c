#include <msp430.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void watchDog_close(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    return;
}

void delay(int i)
{
  int x, y;
  
  for(x=i; x>0; x--)
    for(y=100; y>0; y--)
      _NOP();
}


void dump_data(unsigned char *buf,int len)
{
  int i=0;
  for(i=0;i<len;i++)
  {
      printf("0x%02x ",buf[i]);
        
      if ((i + 1) % 8 == 0)
            
        printf("\r\n");
  }
  printf("\r\n");
}
void  device_sleep(void)
{
  printf("%s %d to do.........\r\n",__func__,__LINE__);
}

unsigned short CRC16(unsigned char *Pushdata,unsigned short length) 
{    
        
  unsigned short Reg_CRC=0xffff;
  unsigned char Temp_reg=0x00;
  unsigned short i,j;
  for( i = 0; i<length; i ++) 
  {    
            
    Reg_CRC^= *Pushdata++;
    for (j = 0; j<8; j++)
    {    
                
      if (Reg_CRC & 0x0001) 
        Reg_CRC=Reg_CRC>>1^0xA001;
      else  
        Reg_CRC >>=1;
    }      
       
  }     
 // printf("crc16=0x%x %d\r\n",(Reg_CRC),(Reg_CRC));
  return (Reg_CRC);    
   
}  