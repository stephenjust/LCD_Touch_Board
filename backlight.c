#include <xc.h>
#include "backlight.h"

static unsigned char backlight_level = 0xFF;

void bl_init(void) {
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;
    ANSELbits.ANS7 = 0;
    CCP1CON |= 0x0C;
    PSTRCONbits.STRA = 0;
    bl_disable();
}

void bl_enable(void) {
    LATCbits.LATC5 = 1;
}

void bl_disable(void) {
    LATCbits.LATC5 = 0;
}

void bl_level(unsigned char level) {
    backlight_level = level;
}

void bl_increase_level(void) {
    backlight_level++;
    CCPR1L = backlight_level;
}

void bl_decrease_level(void) {
    backlight_level--;
    CCPR1L = backlight_level;
}
