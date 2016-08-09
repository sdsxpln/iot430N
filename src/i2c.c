#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char uint8_t;
#include "power_clk.h"
void ucb0_i2c_init(void)  
{  
      P3SEL &= ~BIT2;                        // P3.2@UCB0SCL  
      P3DIR |= BIT2;  
      P3OUT |= BIT2;  
      // 输出9个时钟以恢复I2C总线状态  
      for( uint8_t i= 0; i <9 ; i++)  
      {  
        P3OUT |= BIT2;  
        __delay_cycles(8000);  
        P3OUT &= ~BIT2;  
        __delay_cycles(8000);  
      }  
       
      P3SEL |= (BIT1 + BIT2);                // P3.1@UCB0SDAP3.2@UCB0SCL  
       
      UCB0CTL1 |= UCSWRST;  
      UCB0CTL0 = UCMST+ UCMODE_3 + UCSYNC;  // I2C主机模式  
      UCB0CTL1 |= UCSSEL_2;                  // 选择SMCLK  
      UCB0BR0 = 10;  //100l
      UCB0BR1 = 0;  
      UCB0CTL0 &= ~UCSLA10;                  // 7位地址模式  
      UCB0I2CSA = 0x50;           // AT24C02 EEPROM地址  
      UCB0CTL1 &= ~UCSWRST;  
}  

void ucb0_i2c_deinit(void)  
{  
      P3SEL &= ~(BIT1 + BIT2);                // P3.1@UCB0SDAP3.2@UCB0SCL  
      //some thing more
}  

uint8_t at24c02_readbyte( uint8_t word_addr, uint8_t *pword_value)  
{  
  UCB0CTL1 |= UCTR;                // 写模式  
  UCB0CTL1 |= UCTXSTT;             // 发送启动位和写控制字节  
   
  UCB0TXBUF = word_addr;            //发送字节地址，必须要先填充TXBUF  
  // 等待UCTXIFG=1与UCTXSTT=0 同时变化等待一个标志位即可  
  while(!(UCB0IFG& UCTXIFG))  
  {  
    if( UCB0IFG& UCNACKIFG )      // 若无应答 UCNACKIFG=1  
    {  
      return 1;  
    }  
  }                         
   
  UCB0CTL1 &= ~UCTR;                //读模式  
  UCB0CTL1 |= UCTXSTT;             // 发送启动位和读控制字节  
   
  while(UCB0CTL1& UCTXSTT);       // 等待UCTXSTT=0  
  // 若无应答 UCNACKIFG = 1  
  UCB0CTL1 |= UCTXSTP;             // 先发送停止位  
   
  while(!(UCB0IFG& UCRXIFG));     // 读取字节内容  
  *pword_value = UCB0RXBUF;        // 读取BUF寄存器在发送停止位之后  
   
  while( UCB0CTL1& UCTXSTP );  
   
  return 0;  
} ;


uint8_t at24c02_writebyte( uint8_t word_addr, uint8_t word_value )  
{  
      while( UCB0CTL1& UCTXSTP );  
      UCB0CTL1 |= UCTR;                // 写模式  
      UCB0CTL1 |= UCTXSTT;             // 发送启动位  
       
      UCB0TXBUF = word_addr;           // 发送字节地址  
      // 等待UCTXIFG=1与UCTXSTT=0 同时变化等待一个标志位即可  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // 若无应答 UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
      
      UCB0TXBUF = word_value;          // 发送字节内容  
      while(!(UCB0IFG& UCTXIFG));     // 等待UCTXIFG=1  
       
      UCB0CTL1 |= UCTXSTP;  
      while(UCB0CTL1& UCTXSTP);       // 等待发送完成        
      return 0;  
}  


uint8_t __at24c02_readMultBytes(uint8_t word_addr, uint8_t *pword_buf, uint8_t len )  
    {  
      while( UCB0CTL1& UCTXSTP );  
      UCB0CTL1 |= UCTR;                // 写模式  
      UCB0CTL1 |= UCTXSTT;             // 发送启动位和写控制字节  
       
      UCB0TXBUF = word_addr;           // 发送字节地址  
      // 等待UCTXIFG=1与UCTXSTT=0 同时变化等待一个标志位即可  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // 若无应答 UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
       
      UCB0CTL1 &= ~UCTR;               // 读模式  
      UCB0CTL1 |= UCTXSTT;             // 发送启动位和读控制字节  
       
      while(UCB0CTL1& UCTXSTT);       // 等待UCTXSTT=0  
      // 若无应答 UCNACKIFG = 1  
       
      for( uint8_t i= 0; i< len -1 ; i++)  
      {  
        while(!(UCB0IFG& UCRXIFG));   // 读取字节内容，不包括最后一个字节内容  
        *pword_buf++= UCB0RXBUF;  
      }  
       
      UCB0CTL1 |= UCTXSTP;             // 在接收最后一个字节之前发送停止位  
       
      while(!(UCB0IFG& UCRXIFG));     // 读取最后一个字节内容  
      *pword_buf = UCB0RXBUF;  
       
      while( UCB0CTL1& UCTXSTP );  
     
      return 0;  
}  

uint8_t at24c02_readMultBytes(uint8_t word_addr, uint8_t *pword_buf, uint8_t len )  
{    
  int cnt=len;
  uint8_t addr=word_addr,* src_addr=pword_buf;
  while(1)
  {
    if(cnt<=8)
    {
      __at24c02_readMultBytes(addr,src_addr,cnt); delay_ms(10);
      break;
    }
    else
    {
       __at24c02_readMultBytes(addr,src_addr,8); delay_ms(10);
       addr=addr+8;
       cnt=cnt-8;
       src_addr=src_addr+8;
    }
  }
     
}  


uint8_t __at24c02_writeMultBytes( uint8_t word_addr, uint8_t *pword_buf, uint8_t len)  
{  
      while( UCB0CTL1& UCTXSTP );  
      UCB0CTL1 |= UCTR;                // 写模式  
      UCB0CTL1 |= UCTXSTT;             // 发送启动位  
       
      UCB0TXBUF = word_addr;           // 发送字节地址  
      // 等待UCTXIFG=1与UCTXSTT=0 同时变化等待一个标志位即可  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // 若无应答 UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
       
      for( uint8_t i= 0; i < len; i++)  
      {  
        UCB0TXBUF = *pword_buf++;      // 发送寄存器内容  
        while(!(UCB0IFG& UCTXIFG));   // 等待UCTXIFG=1     
      }  
       
      UCB0CTL1 |= UCTXSTP;  
      while(UCB0CTL1& UCTXSTP);       // 等待发送完成  
       
      return 0;  
}  

uint8_t at24c02_writeMultBytes( uint8_t word_addr, uint8_t *pword_buf, uint8_t len)  
{ 
  int cnt=len;
  uint8_t addr=word_addr,* src_addr=pword_buf;
  while(1)
  {
    if(cnt<=8)
    {
      __at24c02_writeMultBytes(addr,src_addr,cnt); delay_ms(10);
      break;
    }
    else
    {
       __at24c02_writeMultBytes(addr,src_addr,8);
       addr=addr+8;
       cnt=cnt-8;
       src_addr=src_addr+8;
       delay_ms(10);
    }
  }
}  

# if 1 
void i2c_test(void)
{ 
  char data[32],data_r[32];
    int i=0;
     char d=0;
     for(i=0;i<32;i++)
     {
       data[i]=i;
       data_r[i]=0xff;
     }
     ucb0_i2c_init();
     delay_ms(100);
      at24c02_writeMultBytes(0,data,32);
     printf("waiting\r\n"); printf("waiting\r\n"); delay_ms(100);
     at24c02_readMultBytes(0,data_r,32);
#     if 1
     for(i=0;i<32;i++)
     {
    //   at24c02_writebyte(i,data[i]);
    //   printf("waiting\r\n"); printf("waiting\r\n");
       //   at24c02_readbyte(i,&d);
          printf("test data_r[%d]=%d*********\r\n",i,data_r[i]);
     }
# endif    
     
     
}
#endif

//http://blog.csdn.net/girlkoo/article/details/9047191
//http://blog.csdn.net/xukai871105/article/details/10286599
//http://blog.csdn.net/xukai871105/article/details/10286599
