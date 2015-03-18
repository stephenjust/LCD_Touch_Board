#include <xc.h>
#include "i2c.h"
#include "touchpanel.h"
#include "usb.h"

#define I2C_SLAVE 0x38
#define T_COUNT_INDEX 31

static touch_data tp_data;

void tp_service(void) {
    INTCON3bits.INT1E = 0;
    INTCON3bits.INT1F = 0;

    tp_read();
    tp_send();

    INTCON3bits.INT1E = 1;
}

void tp_init(void) {
    LATCbits.LATC0 = 0;
    TRISCbits.TRISC0 = 0;
}

void tp_enable(void) {
    LATCbits.LATC0 = 1;
}

void tp_disable(void) {
    LATCbits.LATC0 = 0;
}

/**
 * Return the number of touch points
 * @return
 */
unsigned char tp_points(void) {
    unsigned char num_points;

    // Read number of points
    i2c_Start();
    i2c_Address(I2C_SLAVE, I2C_WRITE);
    i2c_Write(0x02);
    i2c_Restart();
    i2c_Address(I2C_SLAVE, I2C_READ);
    num_points = i2c_Read(0);
    i2c_Stop();

    num_points = num_points & 0b00000111;

    return num_points;
}

void tp_read(void) {
    unsigned char i;

    // Read one byte
    i2c_Start();      			// send Start
    i2c_Address(I2C_SLAVE, I2C_WRITE);	// Send slave address with write operation
    i2c_Write(0x00);			// Set register to start reading
    i2c_Restart();			// Restart
    i2c_Address(I2C_SLAVE, I2C_READ);	// Send slave address with read operation

    for (i = 0; i < 0x1E; i++) {
        tp_data.raw[i] = i2c_Read(1);
    }
    tp_data.raw[0x1E] = i2c_Read(0);

   i2c_Stop();				// send Stop
}

void tp_send(void) {
    if (usb_is_configured() &&
            !usb_in_endpoint_halted(1) &&
            !usb_in_endpoint_busy(1)) {

        unsigned char *buf = usb_get_in_buffer(1);

        // Report ID for multi-touch contact information reports (based on report descriptor)
        buf[0] = 0x01; //Report ID in byte[0]

        buf[T_COUNT_INDEX] = tp_data.data.TD_STATUS & 0b00000111; // Number of valid contacts

        // Touch point 1
        if (buf[T_COUNT_INDEX] >= 1 && (tp_data.data.TOUCH1_XH >> 6) != 1)
            buf[1] = 1;
        else
            buf[1] = 0;
        //First contact info in bytes 1-6
        buf[2] = (tp_data.data.TOUCH1_YH >> 4) & 0x00001111; //Contact ID
        buf[3] = tp_data.data.TOUCH1_XL; //X-coord LSB
        buf[4] = tp_data.data.TOUCH1_XH & 0x00001111; //X-coord MSB
        buf[5] = tp_data.data.TOUCH1_YL; //Y-coord LSB
        buf[6] = tp_data.data.TOUCH1_YH & 0x00001111; //Y-coord MSB

        // Touch point 2
        if (buf[T_COUNT_INDEX] >= 2 && (tp_data.data.TOUCH2_XH >> 6) != 1)
            buf[7] = 1;
        else
            buf[7] = 0;
        buf[8] = (tp_data.data.TOUCH2_YH >> 4) & 0x00001111; //Contact ID
        buf[9] = tp_data.data.TOUCH2_XL; //X-coord LSB
        buf[10] = tp_data.data.TOUCH2_XH & 0x00001111; //X-coord MSB
        buf[11] = tp_data.data.TOUCH2_YL; //Y-coord LSB
        buf[12] = tp_data.data.TOUCH2_YH & 0x00001111; //Y-coord MSB

        // Touch point 3
        if (buf[T_COUNT_INDEX] >= 3 && (tp_data.data.TOUCH3_XH >> 6) != 1)
            buf[13] = 1;
        else
            buf[13] = 0;
        buf[14] = (tp_data.data.TOUCH3_YH >> 4) & 0x00001111; //Contact ID
        buf[15] = tp_data.data.TOUCH3_XL; //X-coord LSB
        buf[16] = tp_data.data.TOUCH3_XH & 0x00001111; //X-coord MSB
        buf[17] = tp_data.data.TOUCH3_YL; //Y-coord LSB
        buf[18] = tp_data.data.TOUCH3_YH & 0x00001111; //Y-coord MSB

        // Touch point 4
        if (buf[T_COUNT_INDEX] >= 4 && (tp_data.data.TOUCH4_XH >> 6) != 1)
            buf[19] = 1;
        else
            buf[19] = 0;
        buf[20] = (tp_data.data.TOUCH4_YH >> 4) & 0x00001111; //Contact ID
        buf[21] = tp_data.data.TOUCH4_XL; //X-coord LSB
        buf[22] = tp_data.data.TOUCH4_XH & 0x00001111; //X-coord MSB
        buf[23] = tp_data.data.TOUCH4_YL; //Y-coord LSB
        buf[24] = tp_data.data.TOUCH4_YH & 0x00001111; //Y-coord MSB

        // Touch point 5
        if (buf[T_COUNT_INDEX] >= 5 && (tp_data.data.TOUCH5_XH >> 6) != 1)
            buf[25] = 1;
        else
            buf[25] = 0;
        buf[26] = (tp_data.data.TOUCH5_YH >> 4) & 0x00001111; //Contact ID
        buf[27] = tp_data.data.TOUCH5_XL; //X-coord LSB
        buf[28] = tp_data.data.TOUCH5_XH & 0x00001111; //X-coord MSB
        buf[29] = tp_data.data.TOUCH5_YL; //Y-coord LSB
        buf[30] = tp_data.data.TOUCH5_YH & 0x00001111; //Y-coord MSB

        usb_send_in_buffer(1, 32);
    }
}