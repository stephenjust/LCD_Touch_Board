#include <xc.h>
#include "i2c.h"
#include "touchpanel.h"
#include "usb/usb.h"
#include "usb/usb_device_hid.h"

#define I2C_SLAVE 0x38

static unsigned char hid_report_in[HID_INT_IN_EP_SIZE] DEVICE_HID_DIGITIZER_IN_BUFFER_ADDRESS;
static touch_data tp_data;

extern USB_HANDLE lastTransmission;

void tp_service(void) {
    if (!INTCON3bits.INT1IF) return;
    INTCON3bits.INT1IE = 0;

    tp_read();
    tp_send();

    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IE = 1;
}

void tp_init(void) {
    // Set up INT signal
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1; // Enable interrupts
    INTCONbits.GIEL = 1; // Enable low-priority interrupts
    TRISCbits.TRISC1 = 1; // Set pin C1 as input
    ANSELbits.ANS5 = 0; // Disable ADC on C1
    INTCON2bits.INTEDG1 = 0; // Falling edge triggered interrupt on C1
    INTCON3bits.INT1IE = 0; // Disable interrupt on pin C1 (until we enable tp)
    INTCON3bits.INT1IF = 0; // Clear interrupt flag
    INTCON3bits.INT1IP = 0; // Set interrupt as low priority

    // Set up WAKE signal
    ANSELbits.ANS4 = 0; // Disable ADC on C0
    LATCbits.LATC0 = 0;
    TRISCbits.TRISC0 = 0;

    i2c_Init();
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
    i2c_Start(); // send Start
    i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address with write operation
    i2c_Write(0x00); // Set register to start reading
    i2c_Restart(); // Restart
    i2c_Address(I2C_SLAVE, I2C_READ); // Send slave address with read operation

    for (i = 0; i < 0x1E; i++) {
        tp_data.raw[i] = i2c_Read(1);
    }
    tp_data.raw[0x1E] = i2c_Read(0);

    i2c_Stop(); // send Stop
}

/**
 * Populate USB buffer with touch pad multitouch data
 *
 * This should only be called by whatever method is handling USB delegation
 */
void tp_send(void) {
    while (USBHandleBusy(lastTransmission)) {}

    // Report ID for multi-touch contact information reports (based on report descriptor)
    hid_report_in[0] = 0x01; //Report ID in byte[0]


    // Touch point 1
    hid_report_in[1] = ((tp_data.data.TOUCH_POINTS >= 1) ? 3 : 0)
            | tp_data.data.TOUCH1_ID << 2;
    //First contact info in bytes 1-6
    hid_report_in[2] = tp_data.data.TOUCH1_XL; //X-coord LSB
    hid_report_in[3] = tp_data.data.TOUCH1_XH; //X-coord MSB
    hid_report_in[4] = tp_data.data.TOUCH1_YL; //Y-coord LSB
    hid_report_in[5] = tp_data.data.TOUCH1_YH; //Y-coord MSB

    // Touch point 2
    hid_report_in[6] = ((tp_data.data.TOUCH_POINTS >= 2) ? 3 : 0)
            | tp_data.data.TOUCH2_ID << 2;
    hid_report_in[7] = tp_data.data.TOUCH2_XL; //X-coord LSB
    hid_report_in[8] = tp_data.data.TOUCH2_XH; //X-coord MSB
    hid_report_in[9] = tp_data.data.TOUCH2_YL; //Y-coord LSB
    hid_report_in[10] = tp_data.data.TOUCH2_YH; //Y-coord MSB

    // Touch point 3
    hid_report_in[11] = ((tp_data.data.TOUCH_POINTS >= 3) ? 3 : 0)
            | tp_data.data.TOUCH3_ID << 2;
    hid_report_in[12] = tp_data.data.TOUCH3_XL; //X-coord LSB
    hid_report_in[13] = tp_data.data.TOUCH3_XH; //X-coord MSB
    hid_report_in[14] = tp_data.data.TOUCH3_YL; //Y-coord LSB
    hid_report_in[15] = tp_data.data.TOUCH3_YH; //Y-coord MSB

    // Touch point 4
    hid_report_in[16] = ((tp_data.data.TOUCH_POINTS >= 4) ? 3 : 0)
            | tp_data.data.TOUCH4_ID << 2;
    hid_report_in[17] = tp_data.data.TOUCH4_XL; //X-coord LSB
    hid_report_in[18] = tp_data.data.TOUCH4_XH; //X-coord MSB
    hid_report_in[19] = tp_data.data.TOUCH4_YL; //Y-coord LSB
    hid_report_in[20] = tp_data.data.TOUCH4_YH; //Y-coord MSB

    // Touch point 5
    hid_report_in[21] = ((tp_data.data.TOUCH_POINTS >= 5) ? 3 : 0)
            | tp_data.data.TOUCH5_ID << 2;
    hid_report_in[22] = tp_data.data.TOUCH5_XL; //X-coord LSB
    hid_report_in[23] = tp_data.data.TOUCH5_XH; //X-coord MSB
    hid_report_in[24] = tp_data.data.TOUCH5_YL; //Y-coord LSB
    hid_report_in[25] = tp_data.data.TOUCH5_YH; //Y-coord MSB

    hid_report_in[26] = tp_data.data.TOUCH_POINTS; // Number of valid contacts

    lastTransmission = HIDTxPacket(HID_EP, (uint8_t*) hid_report_in, 27);
}