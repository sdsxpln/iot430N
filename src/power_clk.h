#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CPU_CLOCK 1000000
        #if CPU_CLOCK == 1000000
            #define delay_us(us)    __delay_cycles(CPU_CLOCK*(us)/1000000)
            #define delay_ms(ms)    __delay_cycles(CPU_CLOCK*(ms)/1000)
        #else
            #pragma error "CPU_CLOCK is defined implicitly!"
        #endif
