#include "usb.h"
#include <xc.h>
#include "usb_ch9.h"
#include "usb_hid.h"

/* Callbacks. These function names are set in usb_config.h. */
void app_set_configuration_callback(uint8_t configuration) {

}

uint16_t app_get_device_status_callback() {
    return 0x0000;
}

void app_endpoint_halt_callback(uint8_t endpoint, bool halted) {

}

int8_t app_set_interface_callback(uint8_t interface, uint8_t alt_setting) {
    return 0;
}

int8_t app_get_interface_callback(uint8_t interface) {
    return 0;
}

void app_out_transaction_callback(uint8_t endpoint) {

}

void app_in_transaction_complete_callback(uint8_t endpoint) {

}

int8_t app_unknown_setup_request_callback(const struct setup_packet *setup) {
    /* To use the HID device class, have a handler for unknown setup
     * requests and call process_hid_setup_request() (as shown here),
     * which will check if the setup request is HID-related, and will
     * call the HID application callbacks defined in usb_hid.h. For
     * composite devices containing other device classes, make sure
     * MULTI_CLASS_DEVICE is defined in usb_config.h and call all
     * appropriate device class setup request functions here.
     */
    return process_hid_setup_request(setup);
}

int16_t app_unknown_get_descriptor_callback(const struct setup_packet *pkt, const void **descriptor) {
    return -1;
}

void app_start_of_frame_callback(void) {

}

void app_usb_reset_callback(void) {

}

/* HID Callbacks. See usb_hid.h for documentation. */

static uint8_t report_buf[4];

static void get_report_callback(bool transfer_ok, void *context) {
    /* Nothing to do here really. It either succeeded or failed. If it
     * failed, the host will ask for it again. It's nice to be on the
     * device side in USB. */
}

int16_t app_get_report_callback(uint8_t interface, uint8_t report_type,
        uint8_t report_id, const void **report,
        usb_ep0_data_stage_callback *callback,
        void **context) {
    /* Set report, callback, and context; and the USB stack will send
     * the report, calling our callback (get_report_callback()) when
     * it has finished.
     */
    if (interface == 1) {
        *report = report_buf;
        *callback = get_report_callback;
        *context = NULL;
        return 3;
    }
}

int8_t app_set_report_callback(uint8_t interface, uint8_t report_type,
        uint8_t report_id) {
    /* To handle Set_Report, call usb_start_receive_ep0_data_stage()
     * here. See the documentation for HID_SET_REPORT_CALLBACK() in
     * usb_hid.h. For this device though, there are no output or
     * feature reports. */
    return -1;
}

uint8_t app_get_idle_callback(uint8_t interface, uint8_t report_id) {
    return 0;
}

int8_t app_set_idle_callback(uint8_t interface, uint8_t report_id,
        uint8_t idle_rate) {
    return -1;
}

int8_t app_get_protocol_callback(uint8_t interface) {
    return 1;
}

int8_t app_set_protocol_callback(uint8_t interface, uint8_t report_id) {
    return -1;
}