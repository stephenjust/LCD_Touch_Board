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
        unsigned DEVICE_MODE :3;
        unsigned :1;
        unsigned char GEST_ID;
        union {
            struct {
                unsigned char TD_STATUS;
            };
            struct {
                unsigned TOUCH_POINTS :3;
            };
        };
        unsigned TOUCH1_EVENT :2;
        unsigned :2;
        unsigned TOUCH1_XH :4;
        unsigned char TOUCH1_XL;
        unsigned TOUCH1_YH :4;
        unsigned TOUCH1_ID :4;
        unsigned char TOUCH1_YL;
        unsigned :8;
        unsigned :8;
        unsigned TOUCH2_EVENT :2;
        unsigned :2;
        unsigned TOUCH2_XH :4;
        unsigned char TOUCH2_XL;
        unsigned TOUCH2_YH :4;
        unsigned TOUCH2_ID :4;
        unsigned char TOUCH2_YL;
        unsigned :8;
        unsigned :8;
        unsigned TOUCH3_EVENT :2;
        unsigned :2;
        unsigned TOUCH3_XH :4;
        unsigned char TOUCH3_XL;
        unsigned TOUCH3_YH :4;
        unsigned TOUCH3_ID :4;
        unsigned char TOUCH3_YL;
        unsigned :8;
        unsigned :8;
        unsigned TOUCH4_EVENT :2;
        unsigned :2;
        unsigned TOUCH4_XH :4;
        unsigned char TOUCH4_XL;
        unsigned TOUCH4_YH :4;
        unsigned TOUCH4_ID :4;
        unsigned char TOUCH4_YL;
        unsigned :8;
        unsigned :8;
        unsigned TOUCH5_EVENT :2;
        unsigned :2;
        unsigned TOUCH5_XH :4;
        unsigned char TOUCH5_XL;
        unsigned TOUCH5_YH :4;
        unsigned TOUCH5_ID :4;
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

