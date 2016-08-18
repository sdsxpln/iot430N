#include <msp430x54x.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <protocol.h>
#include <power_clk.h>
//ok
extern unsigned char g_hub_sn[SN_LEN];
extern int get_search_cmd;
void process_key(void)
{
    int debounce=10;
    if(!(P9IN&BIT6))
    {
      while(debounce--)
      {
        if((P9IN&BIT6))
        {
          printf("no thing\r\n");
          break;
        }
        delay(20);
      }
      if(debounce<=0)
      {
        printf("key pressed\r\n"); 
        flash_led0_fast(10);
        if((get_search_cmd==1)||(strlen(g_hub_sn)==SN_LEN))
        {           
            if(get_search_cmd==1)
            {
              update_hub_sn(); 
              get_search_cmd=0;
            }
            send_join();    
        }
      }
    }
}
//ok
void key_init(void)
{
    P1DIR&=~BIT4;//p5.5设置为输入 AUX
    P1REN |=  BIT4;                //上拉下拉电阻使能
    P1OUT |=  BIT4;                //P5.5 为上拉
    delay(1000);
}

#if 0
//only port1 and port2 可以中断配置
void key_init(void)
{
    P5DIR&=~BIT5;//p5.5设置为输入 AUX
    P5REN |=  BIT5;                //上拉下拉电阻使能
    P5OUT |=  BIT5;                //P5.5 为上拉
 
    P5IE |= BIT5;  // P5.5 interrupt enabled
 
    P5IES |= BIT5; // P5.5 Hi/Lo edge
 
    P5IFG &= ~BIT5;// P5.5 IFG cleared
    delay(1000);
}



static char toggle=1;
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
{
  
    int debounce=10;
    if(P5IN&BIT5)
    {
      while(debounce--)
      {
        if(P5IN&BIT5)
        {
          
        }
        delay(500);
      }
      if(toggle==1)
      {
             led1_off();
             led2_off();
      }
      else
      {
             led1_on();
             led2_on();
      }
      toggle=!toggle;
    }   
    P5IFG &= ~BIT5; 
}
#endif