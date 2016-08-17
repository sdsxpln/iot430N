#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void SetVcoreUp (unsigned int level);

void set_xt2_to_source()
{
  SetVcoreUp(PMMCOREV_1); 
  //i2c_delay_ms(100);
  SetVcoreUp(PMMCOREV_2); 
   SetVcoreUp(PMMCOREV_3); 
 //  XT2_Start(XT2DRIVE_3);
   //Init_FLL_Settle(25000, 25);
   //SFRIE1 |= OFIE;
  //  while(1);
    
  //i2c_delay_ms(100);
  //SetVcoreUp(PMMCOREV_3);                     // Set VCore to 1.8MHz for 20MHz
  //i2c_delay_ms(100);
  P11DIR = BIT1+BIT2;                       // P11.1-2 to output direction

  P11SEL |= BIT1+BIT2;                      // P11.1-2 to output SMCLK,MCLK
  
  P7DIR = BIT0+BIT1;                       // P11.1-2 to output directio
  P7SEL |= BIT0+BIT1; 
  
  UCSCTL6 |= XCAP_3;  
  UCSCTL6 &= ~XT1OFF; 
  
  unsigned int i;
  P5SEL |= BIT2 + BIT3;                     //P5.2和P5.3选择为晶振XT2输入 

 // UCSCTL6 |= XT2DRIVE_3;
  UCSCTL6 &= ~(XT2OFF);
  //UCSCTL6 |= XT2DRIVE_3;
  
  UCSCTL3 |= SELREF_2; 
  UCSCTL4 |= SELA_2;  
  
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
    SFRIFG1 &= ~OFIFG;                          // 清除振荡器失效标志
  // for (i = 0xfF; i > 0; i--);                 // 延时，等待XT2起振
  } while (SFRIFG1 & OFIFG);                    // 判断XT2是否起振
  UCSCTL3 |= SELREF_0; 
  UCSCTL4 |= SELA_0;  
   UCSCTL4 |= SELS_5 + SELM_5;                   //选择MCLK、SMCLK为XT2,
  //UCSCTL5 |=DIVM_5+DIVS_5;
  return;
}

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;     
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level; 
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG)) 
      while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
    SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
    // Lock PMM registers for write access
    PMMCTL0_H = 0x00;  
}