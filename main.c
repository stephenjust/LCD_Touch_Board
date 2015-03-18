/*
 * File:   main.c
 * Author: stephen
 *
 * Created on March 14, 2015, 10:39 PM
 */

#define _XTAL_FREQ 1000000

#include <xc.h>
#include "backlight.h"
#include "touchpanel.h"
#include "usb.h"

void interrupt_init(void);

int main()
{
    tp_init();
    interrupt_init();
    usb_init();
    bl_init();

    tp_enable();
    //bl_enable();
    while(1)
    {
    }
return 0;
}

void interrupt_init(void) {
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // Set up touch-panel interrupt
    TRISCbits.TRISC1 = 1; // Set pin C1 as input
    ANSELbits.ANS5 = 0; // Disable ADC on C1
    INTCON2bits.INTEDG1 = 0; // Falling edge triggered interrupt on C1
    INTCON3bits.INT1IE = 1; // Enable interrupt on pin C1
}

void interrupt high_priority isr() {
    // Check all USB interrupts
    if (UIR || PIR2bits.USBIF) {
        usb_service();
    }

    // Check touch panel interrupt
    if (INTCON3bits.INT1IF) {
        tp_service();
    }
}