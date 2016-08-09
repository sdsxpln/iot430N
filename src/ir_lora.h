#ifndef __IR_HEADER__
#define __IR_HEADER__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <protocol.h>
#define UART_CHANNEL_IR_1 1
#define UART_CHANNEL_LORA_2 2
#define UART_CHANNEL_CONFIG_3 3
//01
#define LORA_MODE_WAKEUP 1
//10
#define LORA_MODE_LOWPOWER 2
//00
#define LORA_MODE_NORMAL 0
//
#define LORA_MODE_SLEEP 3
extern unsigned short g_hub_addr;
extern unsigned short g_hub_channel;
extern unsigned char g_sn[SN_LEN];
extern unsigned short g_node_addr;

void lora_mode_config(char mode);
void lora_write(unsigned char *buf,int len);
void ir_read(unsigned char * buf,int len,int * read);
void lora_read(unsigned char *buf,int len,int * read);
void ir_write(unsigned char *buf,int len);
void  get_lora_param(unsigned char * rbuf);
void  set_lora_addr(unsigned short addr);
int read_hub_addr(unsigned short * addr);
int read_sn(unsigned char * buf);
int lora_is_ready(void);

void ir_power_on(void);
void ir_power_off(void);
void lora_power_on(void);
void lora_power_off(void);
#endif
