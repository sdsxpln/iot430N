#ifndef __PROTOCOL__HEADER
#define __PROTOCOL__HEADER
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#$[totoal_len][dest_addr_len][dest_addr][port][msg_len][msg][crc]@!

#define HEAD_F_SIZE 2
#define END_F_SIZE 2
#define TOTAL_F_SIZE 2
#define ADDR_LEN_F_SIZE 1
#define PORT_F_SIZE 1
#define MSG_LEN_F_SIZE 2
#define CRC_F_SIZE 2

#define  PORT_IR 1
#define  PORT_LORA 0
#define  PORT_CONFIG_UART 2
#define  PORT_MCU 3
#define  PORT_GW 4
#define  PORT_CLOUD 5
#define DONGLE_ADDR "FE-FE00-A1-160712-00001"

struct internel_cmd
{
  char header1;
  char header2;
  short total_len;
  char  dest_addr_len;
  char dest_addr[32];
  char port;
  short  msg_len;
  char *  msg;
  short crc;
  char end1;
  char end2;
};
#define MY_SN  "FF-FE00-AB-160620-12345"
#define SN_LEN (23)
#define HUB_ADDR_LEN 2
//FF-FE00-AB-160620-02345
#define LORA_433M_NODE 0xfe
#define IR_SWITCH 0xfe
#define VENDOR_CMIOT 0xfe

struct serial_num
{
  char protocol;//ff,��ʾ0xfe����433MЭ��
  short costumer_id;//fe00,��ʾ0x00fe;  
  char product_id;//ab����ʾ0xab
  //160620����������ƴ���ַ����ķ���
  char year;//16����ʾ16,2016��
  char month;//06,��ʾ6,6��
  char day;//20����ʾ20,20��
  //02345
  int addr;//02345,��ʾ2345����10���Ƶ�433M�豸��ַ��
};
int send_data_query(void);
int parse_data(unsigned char * buf,int len);
int process_lora_msg(unsigned char *buf,int len);
int process_ir_msg(unsigned char *buf,int len);
int process_mcu_msg(unsigned char *buf,int len);
int parse_sn(unsigned char *buf,int len);
int send_msg(unsigned char *msg,int msg_len,char port,unsigned char * sn,int len,char uart_channel);
unsigned short get_addr_from_sn(unsigned char * sn,int len);
#endif
