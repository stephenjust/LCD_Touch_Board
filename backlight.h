/* 
 * File:   backlight.h
 * Author: stephen
 *
 * Created on March 14, 2015, 10:45 PM
 */

#ifndef BACKLIGHT_H
#define	BACKLIGHT_H

#ifdef	__cplusplus
extern "C" {
#endif

void bl_init(void);
void bl_enable(void);
void bl_disable(void);
void bl_level(unsigned char level);
void bl_increase_level(void);
void bl_decrease_level(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BACKLIGHT_H */

