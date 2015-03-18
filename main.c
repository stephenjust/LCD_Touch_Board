/*
 * File:   main.c
 * Author: stephen
 *
 * Created on March 14, 2015, 10:39 PM
 */

#define _XTAL_FREQ 1000000

#include <xc.h>
#include "backlight.h"
#include "usb.h"

void interrupt_init(void);

int main()
{
    interrupt_init();
    usb_init();
    bl_init();
    //bl_enable();
    while(1)
    {
    }
return 0;
}

void interrupt_init(void) {
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void interrupt high_priority isr() {
    usb_service();
}