/** All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F543xA Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1048576hz = 1048576/115200 = ~9.1 (009h|01h)
//   ACLK = REFO = ~32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//   See User Guide for baud rate divider table
//
//                 MSP430F5438A
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |     P5.6/UCA1TXD|------------>
//            |                 | 115200 - 8N1
//            |     P5.7/UCA1RXD|<------------
//
//   M. Morales
//   Texas Instruments Inc.
//   June 2009
//   Built with CCE Version: 3.2.2 and IAR Embedded Workbench Version: 4.11B
//******************************************************************************

#include <msp430.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <protocol.h>
#include <uart.h>
#include <ir_lora.h>

static int recv_buf_len1=0;
static char recv_buf1[256];

void ir_uart1_init(void)
{
  P5SEL = 0xc0;                             // P5.6,7 = USCI_A1 TXD/RXD
  
  UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 109;                              // 1MHz 9600 (see User's Guide)
  UCA1BR1 = 0;                              // 1MHz 9600
  UCA1MCTL |=UCBRS_2 + UCBRF_0;        // Modulation UCBRSx=1, UCBRFx=0
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt
};
 
int uart1_write(unsigned char * buf,int len)
{
  int i=0;
  if(buf)
  {
    while(len--)
    {
      while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
      UCA1TXBUF = buf[i];
      i++;    
    }
  }
  return i;
}
void uart1_clear(void)
{
    memset(recv_buf1,0,recv_buf_len1);
    recv_buf_len1=0;
}

int uart1_read(unsigned char * buf,int len,int *read_bytes)
{
    int read_len=0;
    _DINT();
    if(recv_buf_len1==0)
    {
      *read_bytes=0;
      goto out;
    }
    read_len=(recv_buf_len1>=len)?len:recv_buf_len1;
    memcpy(buf,recv_buf1,read_len);
    *read_bytes=read_len;
    uart1_clear();
out:
  _EINT();
  return 0;
}
// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    recv_buf1[recv_buf_len1++]=UCA1RXBUF;
    if(recv_buf_len1==256)
    {
      recv_buf_len1=0;
    }
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}
static unsigned char buf[256];
/*IR串口，收到红外的数据就只需要透传出去即可*/
void process_ir_uart1(void)
{
#  if 1
    int len=0;
    uart1_read(buf,256,&len);
    if(len)
    {
      led0_flash_data();
      send_msg(buf,len,PORT_IR,g_sn,SN_LEN,UART_CHANNEL_LORA_2);//一般是学习数据，或者命令操作的返回码
      memset(buf,0,256);   
    }
#   endif 
    
#    if 0
    int len=0,total_len=0,tmp_len=0;
    if(recv_buf_len1>0)
    {
      while(len<4)
      {
        uart1_read(buf,256,&len);
        delay(400);
      }
      //printf("len=%d,buf[0]=%c\r\n",len,buf[0]);
      if(len)
      {  
        tmp_len=len;
        if((buf[0]=='#')&&(buf[1]=='$'))
        {
        //check len
          total_len=buf[2]+(buf[3]<<8);
          if(total_len>len)
          {
            delay(500);
            len=0;
            while(recv_buf_len1<(total_len-tmp_len))
            {     
              ;
            } 
            uart1_read(buf+tmp_len,256-tmp_len,&len);
            tmp_len=tmp_len+len;
          } 
           send_msg(buf,total_len,PORT_IR,g_sn,SN_LEN,UART_CHANNEL_LORA_2);//一般是学习数据，或者命令操作的返回码
           memset(buf,0,256);
           led0_flash_data();
        }
      }
    }
#    endif
}