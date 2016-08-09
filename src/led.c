#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include "power_clk.h"
#include <string.h>
void led0_on(void);
void led1_on(void);
void led_init(void)
{
  P6DIR |= BIT4;  //P8.0����Ϊ�����P8���0λ����1�������1������λ���䣩
  P6OUT &=~ BIT4;  //P8.1����Ϊ�����P8���1λ����1�������1������λ���䣩
  led0_on();
}
void led0_on(void)
{
       P6OUT |= BIT4; 
}

void led0_off(void)
{
      P6OUT &= ~BIT4;  //P8.0 = 0��LED��
}

void led1_on(void)
{
     P8OUT &= ~BIT1;  //P8.1 = 0��LED�� 
}

void led1_off(void)
{
      P8OUT |= BIT1;   //P8.1 = 1��LED��
}

void led0_flash_data(void)
{
# if 0
  led0_on();
  delay_ms(50);
  led0_off();
  delay_ms(50);
  led0_on();
#  endif
  flash_led0_fast(4);
}

int gpio_init(void)
{
  P1DIR |= 0x01;                            // Set P1.0 to output direction
  while (1)                                 // Test P1.4
  {
    if (0x010 & P1IN)
      P1OUT |= 0x01;                        // if P1.4 set, set P1.0
    else
      P1OUT &= ~0x01;                       // else reset
  }
}
