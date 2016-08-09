#ifndef __UART_HEADER__
#define __UART_HEADER__
extern unsigned  char buf[256];

extern void log_uart0_init(void);

void uart0_clear(void);
int uart0_read(unsigned char * buf,int len,int *read_bytes);
int uart0_write(unsigned char * buf,int len);

void ir_uart1_init(void);
void uart1_clear(void);
int uart1_read(unsigned char * buf,int len,int *read_bytes);
int uart1_write(unsigned char * buf,int len);

void lora_uart2_init(void);
void uart2_clear(void);
int uart2_read(unsigned char * buf,int len,int *read_bytes);
int uart2_write(unsigned char * buf,int len);

void config_uart3_init(void);
void uart3_clear(void);
int uart3_read(unsigned char * buf,int len,int *read_bytes);
int uart3_write(unsigned char * buf,int len);

void process_log_uart0(void);
void process_ir_uart1(void);
void process_lora_uart2(void);
void process_config_uart3(void);
#endif