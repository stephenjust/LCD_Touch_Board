/* 
 * File:   touchpanel.h
 * Author: stephen
 *
 * Created on March 17, 2015, 11:02 PM
 */

#ifndef TOUCHPANEL_H
#define	TOUCHPANEL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef union {
    unsigned char raw[31];
    struct {
        unsigned :4;
        unsigned char DEVICE_MODE :3;
        unsigned :1;
        unsigned char GEST_ID;
        union {
            struct {
                unsigned char TD_STATUS;
            };
            struct {
                unsigned char TOUCH_POINTS :3;
            };
        };
        unsigned char TOUCH1_XH;
        unsigned char TOUCH1_XL;
        unsigned char TOUCH1_YH;
        unsigned char TOUCH1_YL;
        unsigned :8;
        unsigned :8;
        unsigned char TOUCH2_XH;
        unsigned char TOUCH2_XL;
        unsigned char TOUCH2_YH;
        unsigned char TOUCH2_YL;
        unsigned :8;
        unsigned :8;
        unsigned char TOUCH3_XH;
        unsigned char TOUCH3_XL;
        unsigned char TOUCH3_YH;
        unsigned char TOUCH3_YL;
        unsigned :8;
        unsigned :8;
        unsigned char TOUCH4_XH;
        unsigned char TOUCH4_XL;
        unsigned char TOUCH4_YH;
        unsigned char TOUCH4_YL;
        unsigned :8;
        unsigned :8;
        unsigned char TOUCH5_XH;
        unsigned char TOUCH5_XL;
        unsigned char TOUCH5_YH;
        unsigned char TOUCH5_YL;
    } data;
} touch_data;

void tp_service(void);
void tp_init(void);
void tp_enable(void);
void tp_disable(void);
void tp_read(void);
void tp_send(void);


#ifdef	__cplusplus
}
#endif

#endif	/* TOUCHPANEL_H */

