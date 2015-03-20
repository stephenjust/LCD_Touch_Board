/*
 * File:   main.c
 * Author: stephen
 *
 * Created on March 14, 2015, 10:39 PM
 */

#include <xc.h>
#include "backlight.h"
#include "touchpanel.h"
#include "usb/usb.h"
#include "usb/usb_device_hid.h"

#include <app_device_hid_digitizer_multi.h>
#define _XTAL_FREQ 48000000

void interrupt_init(void);

int main()
{
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    USBDeviceInit();
    USBDeviceAttach();
    
    bl_init();
    tp_init();

    //tp_enable();

    //bl_enable();
    while(1)
    {
        SYSTEM_Tasks();

        /* If the USB device isn't configured yet, we can't really do anything
         * else since we don't have a host to talk to.  So jump back to the
         * top of the while loop. */
        if( USBGetDeviceState() < CONFIGURED_STATE )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        /* If we are currently suspended, then we need to see if we need to
         * issue a remote wakeup.  In either case, we shouldn't process any
         * keyboard commands since we aren't currently communicating to the host
         * thus just continue back to the start of the while loop. */
        if( USBIsDeviceSuspended()== true )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        //Application specific tasks
        APP_DeviceHIDDigitizerTasks();
    }//end while

    return 0;
}

bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch( (int) event )
    {
        case EVENT_TRANSFER:
            break;

        case EVENT_SOF:
            APP_DeviceHIDDigitizerSOFHandler();
            break;

        case EVENT_SUSPEND:
            break;

        case EVENT_RESUME:
            break;

        case EVENT_CONFIGURED:
            /* When the device is configured, we can (re)initialize the
             * demo code. */
            APP_DeviceHIDDigitizerInitialize();
            break;

        case EVENT_SET_DESCRIPTOR:
            break;

        case EVENT_EP0_REQUEST:
            /* We have received a non-standard USB request.  The HID driver
             * needs to check to see if the request was for it. */
            USBCheckHIDRequest();
            break;

        case EVENT_BUS_ERROR:
            break;

        case EVENT_TRANSFER_TERMINATED:
            break;

        default:
            break;
    }
    return true;
}

void interrupt high_priority isr() {
    LATCbits.LATC3 = 1;
    // Check all USB interrupts
    USBDeviceTasks();

    // Check touch panel interrupt
    if (INTCON3bits.INT1IF) {
        tp_service();
    }
    LATCbits.LATC3 = 0;
}
