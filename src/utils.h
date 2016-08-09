#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__
#include <msp430.h>
extern void watchDog_close(void);
extern int gpio_init(void);
void delay(int i);
#endif