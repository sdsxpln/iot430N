#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void (*led_handle)(void);
void timer1_int_on();
void timer1_int_off();
static int flash_led_cnt=0;
void timer1_init()
{
//    TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
    timer1_int_off();
    TA1CCR0 = 500000;
    TA1CTL = TASSEL_1 + MC_2 + TACLR;         // SMCLK, contmode, clear TAR    
    led_handle=0;
    flash_led_cnt=0;
}
void timer1_int_on()
{
  TA1CCTL0 = CCIE;  
}

void timer1_int_off()
{
  TA1CCTL0 &= ~CCIE;  
  flash_led_cnt=0;
}

// Timer A0 interrupt service routine

void update_led_handle(void (*func)(void) )
{
  timer1_int_off();
  led_handle=func;
  timer1_int_on();
}
#pragma vector=TIMER1_A0_VECTOR

__interrupt void TIMER1_A0_ISR(void)

{
  if(led_handle)
  {
    flash_led_cnt--;
    led_handle();
    if(flash_led_cnt==0)
    {
      timer1_int_off();
      led0_on();
    }
  }
}
void __flash_led0_fast(void);
void flash_led0_fast(int cnt)
{
  __disable_interrupt();
  update_led_handle(__flash_led0_fast);
  flash_led_cnt=cnt;
  __enable_interrupt();
}
void __flash_led0_slow(void);
void flash_led0_slow(int cnt)
{
  __disable_interrupt();
  update_led_handle(__flash_led0_slow);
  flash_led_cnt=cnt;
   __enable_interrupt();
}

void flash_led0_off(void)
{
  timer1_int_off();
  led0_on(); 
}

void __flash_led0_fast(void)
{
  static int led_status=0;
  if(led_status==0)
  {
    led0_on();
    led_status=1;
  }
  else
  {
    led_status=0;
    led0_off();
  }
 // P1OUT ^= 0x01;// Toggle P1.0
 
  TA1CCR0 += 5000;    // Add Offset to CCR0
}

void __flash_led0_slow(void)
{
  static int led_status=0;
  if(led_status==0)
  {
    led0_on();
    led_status=1;
  }
  else
  {
    led_status=0;
    led0_off();
  }
 // P1OUT ^= 0x01;// Toggle P1.0
 
  TA1CCR0 += 20000;    // Add Offset to CCR0
}