/******************************************************************************
	            MSP430F5438A��СϵͳV3.0

���ܣ�����͸��
���ڲ����ʣ�115200

Ӳ��ԭ��ͼ��
            TXD --> P3.4
            RXD --> P3.5

//   MSP430F543xA Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK

//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.

//   USCI_A0 RX interrupt triggers TX Echo.

//   Baud rate divider with 1048576hz = 1048576/115200 = ~9.1 (009h|01h)

//   ACLK = REFO = ~32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz

//   See User Guide for baud rate divider table

//                 MSP430F5438A

//             -----------------

//         /|\|                 |

//          | |                 |

//          --|RST              |

//            |                 |

//            |     P3.4/UCA0TXD|------------>

//            |                 | 115200 - 8N1

//            |     P3.5/UCA0RXD|<------------

//   Built with IAR Embedded Workbench Version: 5.3

Time��2014��11��9
by��������ӹ�����
�Ա���mcu-dream.taobao.com
******************************************************************************/
#include <msp430x54x.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include  <uart.h>
void watchDog_close(void);

void clock_init(void)
{
}

void board_init(void)
{
  //set_xt2_to_source();
  watchDog_close();
  ir_power_on();
  lora_power_on();
  log_uart0_init();
  ir_uart1_init();
  lora_uart2_init();
  config_uart3_init();
  key_init();
  led_init();
  timer1_init();
}
extern int get_search_cmd;
void main(void)
{
  board_init();
  printf("\r\nxDevice Running\r\n");
  init_lora_device();
  flash_led0_slow(4);//show we begin to run
  while(1)
  {
   // i2c_test();
    process_key();//ok
   // process_log_uart0();//ok,�����ڴ�ӡ��һ��û���������
    process_ir_uart1();//ok,͸����lora�Ϳ�����
    delay(200);
    process_lora_uart2();//����lora�����ݣ�һ����search����������ƣ�û��MCU���Ƶ�
    delay(200);
    process_config_uart3();//���ڶ�д���кź�hub��ַ��������Ϣ��MCU PORT
    delay(200);  
  }
}

