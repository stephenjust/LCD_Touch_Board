#include <xc.h>
#include "i2c.h"
#include "touchpanel.h"
#include "usb/usb.h"

#define I2C_SLAVE 0x38
#define T_COUNT_INDEX 31

static unsigned char hid_report_in[HID_INT_IN_EP_SIZE] DEVICE_HID_DIGITIZER_IN_BUFFER_ADDRESS;
static touch_data tp_data;
static volatile int tp_wait = 0;

void tp_service(void) {
    if (!INTCON3bits.INT1F) return;
    INTCON3bits.INT1E = 0;
    INTCON3bits.INT1F = 0;

    tp_wait = 1;

    INTCON3bits.INT1E = 1;
}

int tp_waiting(void) {
    return tp_wait;
}

void tp_clear_waiting(void) {
    tp_wait = 0;
}

void tp_init(void) {
    // Set up INT signal
    TRISCbits.TRISC1 = 1; // Set pin C1 as input
    ANSELbits.ANS5 = 0; // Disable ADC on C1
    INTCON2bits.INTEDG1 = 0; // Falling edge triggered interrupt on C1
    INTCON3bits.INT1IE = 0; // Disable interrupt on pin C1 (until we enable tp)

    // Set up WAKE signal
    ANSELbits.ANS4 = 0; // Disable ADC on C0
    LATCbits.LATC0 = 0;
    TRISCbits.TRISC0 = 0;
}

void tp_enable(void) {
    LATCbits.LATC0 = 1;
    INTCON3bits.INT1IE = 1;
}

void tp_disable(void) {
    INTCON3bits.INT1IE = 0;
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

/**
 * Populate USB buffer with touch pad multitouch data
 *
 * This should only be called by whatever method is handling USB delegation
 */
void tp_send(void) {
    // Report ID for multi-touch contact information reports (based on report descriptor)
    hid_report_in[0] = 0x01; //Report ID in byte[0]

    hid_report_in[T_COUNT_INDEX] = tp_data.data.TD_STATUS & 0b00000111; // Number of valid contacts

    // Touch point 1
    if (hid_report_in[T_COUNT_INDEX] >= 1 && (tp_data.data.TOUCH1_XH >> 6) != 1)
        hid_report_in[1] = 1;
    else
        hid_report_in[1] = 0;
    //First contact info in bytes 1-6
    hid_report_in[2] = (tp_data.data.TOUCH1_YH >> 4) & 0x00001111; //Contact ID
    hid_report_in[3] = tp_data.data.TOUCH1_XL; //X-coord LSB
    hid_report_in[4] = tp_data.data.TOUCH1_XH & 0x00001111; //X-coord MSB
    hid_report_in[5] = tp_data.data.TOUCH1_YL; //Y-coord LSB
    hid_report_in[6] = tp_data.data.TOUCH1_YH & 0x00001111; //Y-coord MSB

    // Touch point 2
    if (hid_report_in[T_COUNT_INDEX] >= 2 && (tp_data.data.TOUCH2_XH >> 6) != 1)
        hid_report_in[7] = 1;
    else
        hid_report_in[7] = 0;
    hid_report_in[8] = (tp_data.data.TOUCH2_YH >> 4) & 0x00001111; //Contact ID
    hid_report_in[9] = tp_data.data.TOUCH2_XL; //X-coord LSB
    hid_report_in[10] = tp_data.data.TOUCH2_XH & 0x00001111; //X-coord MSB
    hid_report_in[11] = tp_data.data.TOUCH2_YL; //Y-coord LSB
    hid_report_in[12] = tp_data.data.TOUCH2_YH & 0x00001111; //Y-coord MSB

    // Touch point 3
    if (hid_report_in[T_COUNT_INDEX] >= 3 && (tp_data.data.TOUCH3_XH >> 6) != 1)
        hid_report_in[13] = 1;
    else
        hid_report_in[13] = 0;
    hid_report_in[14] = (tp_data.data.TOUCH3_YH >> 4) & 0x00001111; //Contact ID
    hid_report_in[15] = tp_data.data.TOUCH3_XL; //X-coord LSB
    hid_report_in[16] = tp_data.data.TOUCH3_XH & 0x00001111; //X-coord MSB
    hid_report_in[17] = tp_data.data.TOUCH3_YL; //Y-coord LSB
    hid_report_in[18] = tp_data.data.TOUCH3_YH & 0x00001111; //Y-coord MSB

    // Touch point 4
    if (hid_report_in[T_COUNT_INDEX] >= 4 && (tp_data.data.TOUCH4_XH >> 6) != 1)
        hid_report_in[19] = 1;
    else
        hid_report_in[19] = 0;
    hid_report_in[20] = (tp_data.data.TOUCH4_YH >> 4) & 0x00001111; //Contact ID
    hid_report_in[21] = tp_data.data.TOUCH4_XL; //X-coord LSB
    hid_report_in[22] = tp_data.data.TOUCH4_XH & 0x00001111; //X-coord MSB
    hid_report_in[23] = tp_data.data.TOUCH4_YL; //Y-coord LSB
    hid_report_in[24] = tp_data.data.TOUCH4_YH & 0x00001111; //Y-coord MSB

    // Touch point 5
    if (hid_report_in[T_COUNT_INDEX] >= 5 && (tp_data.data.TOUCH5_XH >> 6) != 1)
        hid_report_in[25] = 1;
    else
        hid_report_in[25] = 0;
    hid_report_in[26] = (tp_data.data.TOUCH5_YH >> 4) & 0x00001111; //Contact ID
    hid_report_in[27] = tp_data.data.TOUCH5_XL; //X-coord LSB
    hid_report_in[28] = tp_data.data.TOUCH5_XH & 0x00001111; //X-coord MSB
    hid_report_in[29] = tp_data.data.TOUCH5_YL; //Y-coord LSB
    hid_report_in[30] = tp_data.data.TOUCH5_YH & 0x00001111; //Y-coord MSB
}