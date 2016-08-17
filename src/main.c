/******************************************************************************
	            MSP430F5438A最小系统V3.0

功能：串口透传
串口波特率：115200

硬件原理图：
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

Time：2014、11、9
by：梦想电子工作室
淘宝：mcu-dream.taobao.com
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
   // process_log_uart0();//ok,处理串口打印，一般没有这个需求
    process_ir_uart1();//ok,透传给lora就可以了
    delay(200);
    process_lora_uart2();//接收lora的数据，一般是search包，红外控制，没有MCU控制的
    delay(200);
    process_config_uart3();//用于读写序列号和hub地址等配置信息，MCU PORT
    delay(200);  
  }
}

