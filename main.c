/*
 * File:   main.c
 * Author: stephen
 *
 * Created on March 14, 2015, 10:39 PM
 */


#include <xc.h>
#include "backlight.h"

void main(void) {
    bl_init();
    bl_enable();
    while (1) {}
    return;
}
