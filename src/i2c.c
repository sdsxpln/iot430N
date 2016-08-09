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
      // ���9��ʱ���Իָ�I2C����״̬  
      for( uint8_t i= 0; i <9 ; i++)  
      {  
        P3OUT |= BIT2;  
        __delay_cycles(8000);  
        P3OUT &= ~BIT2;  
        __delay_cycles(8000);  
      }  
       
      P3SEL |= (BIT1 + BIT2);                // P3.1@UCB0SDAP3.2@UCB0SCL  
       
      UCB0CTL1 |= UCSWRST;  
      UCB0CTL0 = UCMST+ UCMODE_3 + UCSYNC;  // I2C����ģʽ  
      UCB0CTL1 |= UCSSEL_2;                  // ѡ��SMCLK  
      UCB0BR0 = 10;  //100l
      UCB0BR1 = 0;  
      UCB0CTL0 &= ~UCSLA10;                  // 7λ��ַģʽ  
      UCB0I2CSA = 0x50;           // AT24C02 EEPROM��ַ  
      UCB0CTL1 &= ~UCSWRST;  
}  

void ucb0_i2c_deinit(void)  
{  
      P3SEL &= ~(BIT1 + BIT2);                // P3.1@UCB0SDAP3.2@UCB0SCL  
      //some thing more
}  

uint8_t at24c02_readbyte( uint8_t word_addr, uint8_t *pword_value)  
{  
  UCB0CTL1 |= UCTR;                // дģʽ  
  UCB0CTL1 |= UCTXSTT;             // ��������λ��д�����ֽ�  
   
  UCB0TXBUF = word_addr;            //�����ֽڵ�ַ������Ҫ�����TXBUF  
  // �ȴ�UCTXIFG=1��UCTXSTT=0 ͬʱ�仯�ȴ�һ����־λ����  
  while(!(UCB0IFG& UCTXIFG))  
  {  
    if( UCB0IFG& UCNACKIFG )      // ����Ӧ�� UCNACKIFG=1  
    {  
      return 1;  
    }  
  }                         
   
  UCB0CTL1 &= ~UCTR;                //��ģʽ  
  UCB0CTL1 |= UCTXSTT;             // ��������λ�Ͷ������ֽ�  
   
  while(UCB0CTL1& UCTXSTT);       // �ȴ�UCTXSTT=0  
  // ����Ӧ�� UCNACKIFG = 1  
  UCB0CTL1 |= UCTXSTP;             // �ȷ���ֹͣλ  
   
  while(!(UCB0IFG& UCRXIFG));     // ��ȡ�ֽ�����  
  *pword_value = UCB0RXBUF;        // ��ȡBUF�Ĵ����ڷ���ֹͣλ֮��  
   
  while( UCB0CTL1& UCTXSTP );  
   
  return 0;  
} ;


uint8_t at24c02_writebyte( uint8_t word_addr, uint8_t word_value )  
{  
      while( UCB0CTL1& UCTXSTP );  
      UCB0CTL1 |= UCTR;                // дģʽ  
      UCB0CTL1 |= UCTXSTT;             // ��������λ  
       
      UCB0TXBUF = word_addr;           // �����ֽڵ�ַ  
      // �ȴ�UCTXIFG=1��UCTXSTT=0 ͬʱ�仯�ȴ�һ����־λ����  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // ����Ӧ�� UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
      
      UCB0TXBUF = word_value;          // �����ֽ�����  
      while(!(UCB0IFG& UCTXIFG));     // �ȴ�UCTXIFG=1  
       
      UCB0CTL1 |= UCTXSTP;  
      while(UCB0CTL1& UCTXSTP);       // �ȴ��������        
      return 0;  
}  


uint8_t __at24c02_readMultBytes(uint8_t word_addr, uint8_t *pword_buf, uint8_t len )  
    {  
      while( UCB0CTL1& UCTXSTP );  
      UCB0CTL1 |= UCTR;                // дģʽ  
      UCB0CTL1 |= UCTXSTT;             // ��������λ��д�����ֽ�  
       
      UCB0TXBUF = word_addr;           // �����ֽڵ�ַ  
      // �ȴ�UCTXIFG=1��UCTXSTT=0 ͬʱ�仯�ȴ�һ����־λ����  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // ����Ӧ�� UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
       
      UCB0CTL1 &= ~UCTR;               // ��ģʽ  
      UCB0CTL1 |= UCTXSTT;             // ��������λ�Ͷ������ֽ�  
       
      while(UCB0CTL1& UCTXSTT);       // �ȴ�UCTXSTT=0  
      // ����Ӧ�� UCNACKIFG = 1  
       
      for( uint8_t i= 0; i< len -1 ; i++)  
      {  
        while(!(UCB0IFG& UCRXIFG));   // ��ȡ�ֽ����ݣ����������һ���ֽ�����  
        *pword_buf++= UCB0RXBUF;  
      }  
       
      UCB0CTL1 |= UCTXSTP;             // �ڽ������һ���ֽ�֮ǰ����ֹͣλ  
       
      while(!(UCB0IFG& UCRXIFG));     // ��ȡ���һ���ֽ�����  
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
      UCB0CTL1 |= UCTR;                // дģʽ  
      UCB0CTL1 |= UCTXSTT;             // ��������λ  
       
      UCB0TXBUF = word_addr;           // �����ֽڵ�ַ  
      // �ȴ�UCTXIFG=1��UCTXSTT=0 ͬʱ�仯�ȴ�һ����־λ����  
      while(!(UCB0IFG& UCTXIFG))  
      {  
        if( UCB0IFG& UCNACKIFG )      // ����Ӧ�� UCNACKIFG=1  
        {  
          return 1;  
        }  
      }    
       
      for( uint8_t i= 0; i < len; i++)  
      {  
        UCB0TXBUF = *pword_buf++;      // ���ͼĴ�������  
        while(!(UCB0IFG& UCTXIFG));   // �ȴ�UCTXIFG=1     
      }  
       
      UCB0CTL1 |= UCTXSTP;  
      while(UCB0CTL1& UCTXSTP);       // �ȴ��������  
       
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
