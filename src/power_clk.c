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
  //i2c_delay_ms(100);
  //SetVcoreUp(PMMCOREV_3);                     // Set VCore to 1.8MHz for 20MHz
  //i2c_delay_ms(100);
  unsigned int i;
  P5SEL |= BIT2 + BIT3;                     //P5.2��P5.3ѡ��Ϊ����XT2���� 
  UCSCTL6 &= ~XT2OFF;
  UCSCTL3 |= SELREF_2; 
  UCSCTL4 |= SELA_2;  
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
    SFRIFG1 &= ~OFIFG;                          // �������ʧЧ��־
    for (i = 0xfFF; i > 0; i--);                 // ��ʱ���ȴ�XT2����
  } while (SFRIFG1 & OFIFG);                    // �ж�XT2�Ƿ�����
  UCSCTL6 &= ~XT2DRIVE0;
  UCSCTL4 |= SELS_5 + SELM_5;                   //ѡ��MCLK��SMCLKΪXT2,
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