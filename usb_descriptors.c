/********************************************************************
 FileName:     	usb_descriptors.c
 Dependencies:	See INCLUDES section
 Processor:		PIC18, PIC24, PIC32 USB Microcontrollers
 Hardware:		This demo is natively intended to be used on Microchip USB demo
 				boards supported by the MCHPFSUSB stack.  See release notes for
 				support matrix.  This demo can be modified for use on other hardware
 				platforms.
 Complier:  	Microchip C18 (for PIC18) or C30 (for PIC24) or C32 (for PIC32)
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the ï¿½Companyï¿½) for its PICï¿½ Microcontroller is intended and
 supplied to you, the Companyï¿½s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN ï¿½AS ISï¿½ CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

*********************************************************************
-usb_descriptors.c-
-------------------------------------------------------------------
Filling in the descriptor values in the usb_descriptors.c file:
-------------------------------------------------------------------

[Device Descriptors]
The device descriptor is defined as a USB_DEVICE_DESCRIPTOR type.
This type is defined in usb_ch9.h  Each entry into this structure
needs to be the correct length for the data type of the entry.

[Configuration Descriptors]
The configuration descriptor was changed in v2.x from a structure
to a uint8_t array.  Given that the configuration is now a byte array
each byte of multi-byte fields must be listed individually.  This
means that for fields like the total size of the configuration where
the field is a 16-bit value "64,0," is the correct entry for a
configuration that is only 64 bytes long and not "64," which is one
too few bytes.

The configuration attribute must always have the _DEFAULT
definition at the minimum. Additional options can be ORed
to the _DEFAULT attribute. Available options are _SELF and _RWU.
These definitions are defined in the usb_device.h file. The
_SELF tells the USB host that this device is self-powered. The
_RWU tells the USB host that this device supports Remote Wakeup.

[Endpoint Descriptors]
Like the configuration descriptor, the endpoint descriptors were
changed in v2.x of the stack from a structure to a uint8_t array.  As
endpoint descriptors also has a field that are multi-byte entities,
please be sure to specify both bytes of the field.  For example, for
the endpoint size an endpoint that is 64 bytes needs to have the size
defined as "64,0," instead of "64,"

Take the following example:
    // Endpoint Descriptor //
    0x07,                       //the size of this descriptor //
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP02_IN,                   //EndpointAddress
    _INT,                       //Attributes
    0x08,0x00,                  //size (note: 2 bytes)
    0x02,                       //Interval

The first two parameters are self-explanatory. They specify the
length of this endpoint descriptor (7) and the descriptor type.
The next parameter identifies the endpoint, the definitions are
defined in usb_device.h and has the following naming
convention:
_EP<##>_<dir>
where ## is the endpoint number and dir is the direction of
transfer. The dir has the value of either 'OUT' or 'IN'.
The next parameter identifies the type of the endpoint. Available
options are _BULK, _INT, _ISO, and _CTRL. The _CTRL is not
typically used because the default control transfer endpoint is
not defined in the USB descriptors. When _ISO option is used,
addition options can be ORed to _ISO. Example:
_ISO|_AD|_FE
This describes the endpoint as an isochronous pipe with adaptive
and feedback attributes. See usb_device.h and the USB
specification for details. The next parameter defines the size of
the endpoint. The last parameter in the polling interval.

-------------------------------------------------------------------
Adding a USB String
-------------------------------------------------------------------
A string descriptor array should have the following format:

rom struct{byte bLength;byte bDscType;word string[size];}sdxxx={
sizeof(sdxxx),DSC_STR,<text>};

The above structure provides a means for the C compiler to
calculate the length of string descriptor sdxxx, where xxx is the
index number. The first two bytes of the descriptor are descriptor
length and type. The rest <text> are string texts which must be
in the unicode format. The unicode format is achieved by declaring
each character as a word type. The whole text string is declared
as a word array with the number of characters equals to <size>.
<size> has to be manually counted and entered into the array
declaration. Let's study this through an example:
if the string is "USB" , then the string descriptor should be:
(Using index 02)
rom struct{byte bLength;byte bDscType;word string[3];}sd002={
sizeof(sd002),DSC_STR,'U','S','B'};

A USB project may have multiple strings and the firmware supports
the management of multiple strings through a look-up table.
The look-up table is defined as:
rom const unsigned char *rom USB_SD_Ptr[]={&sd000,&sd001,&sd002};

The above declaration has 3 strings, sd000, sd001, and sd002.
Strings can be removed or added. sd000 is a specialized string
descriptor. It defines the language code, usually this is
US English (0x0409). The index of the string must match the index
position of the USB_SD_Ptr array, &sd000 must be in position
USB_SD_Ptr[0], &sd001 must be in position USB_SD_Ptr[1] and so on.
The look-up table USB_SD_Ptr is used by the get string handler
function.

-------------------------------------------------------------------

The look-up table scheme also applies to the configuration
descriptor. A USB device may have multiple configuration
descriptors, i.e. CFG01, CFG02, etc. To add a configuration
descriptor, user must implement a structure similar to CFG01.
The next step is to add the configuration descriptor name, i.e.
cfg01, cfg02,.., to the look-up table USB_CD_Ptr. USB_CD_Ptr[0]
is a dummy place holder since configuration 0 is the un-configured
state according to the definition in the USB specification.

********************************************************************************

 Change History:
  Rev   Description
  ----  -----------------------------------------
  2.7   Updated descriptors to fix some WHQL certification related
        issues
  2.7b  Minor change to enable and demonstrate USB remote wakeup.

********************************************************************/

/*********************************************************************
 * Descriptor specific type definitions are defined in:
 * usb_device.h
 *
 * Configuration options are defined in:
 * usb_config.h
 ********************************************************************/


/** INCLUDES *******************************************************/
#include <usb/usb.h>
#include <usb/usb_device_hid.h>

/** CONSTANTS ******************************************************/
#if defined(COMPILER_MPLAB_C18)
#pragma romdata
#endif

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR device_dsc=
{
    0x12,    // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    MY_VID,                 // Vendor ID
    MY_PID,                 // Product ID
    0x0003,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration 1 Descriptor */
const uint8_t configDescriptor1[]={
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,  // CONFIGURATION descriptor type
    DESC_CONFIG_WORD(0x0022),   // Total length of data for this cfg
    1,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    0,                      // Configuration string index
    _DEFAULT | _SELF | _RWU,// Attributes, see usb_device.h
    150,                     // Max power consumption (2X mA)

    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,      // INTERFACE descriptor type
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    1,                      // Number of endpoints in this intf
    HID_INTF,               // Class code
    0,					    // Subclass code
    0,    // Protocol code
    0,                      // Interface string index

    /* HID Class-Specific Descriptor */
    0x09,//sizeof(USB_HID_DSC)+3,    // Size of this descriptor in bytes
    DSC_HID,                // HID descriptor type
    DESC_CONFIG_WORD(0x0112),        // HID Spec Release Number in BCD format (1.12)
    0x00,                   // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,         // Number of class descriptors, see usbcfg.h
    DSC_RPT,                // Report descriptor type
    DESC_CONFIG_WORD(HID_RPT01_SIZE),   //sizeof(hid_rpt01),  // Size of the report descriptor

    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    HID_EP | _EP_IN,            //EndpointAddress
    _INTERRUPT,                 //Attributes
    DESC_CONFIG_WORD(64),       //size
    0x04                        //Interval in ms.  4ms = up to 250Hz update rate.
};


//Language code string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[1];}sd000={
sizeof(sd000),USB_DESCRIPTOR_STRING,{0x0409}};

//Manufacturer string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[12];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{'S','t','e','p','h','e','n',' ','J','u','s','t'}};

//Product string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[15];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{'H','I','D',' ','M','u','l','t','i','-','T','o','u','c','h'}};


//Class specific descriptor - HID report descriptor:

//In order to understand the HID report descriptor, it is helpful to become familiar
//with the HID specifications (HID1_11.pdf was used to develop this example), and the
//"HUT" (HID Usage Tables) specifications (Hut1_12.pdf was used during development of this example).
//These documents can be found at the USB implementers forum website: www.usb.org

//Additionally, Microsoft provides a few reference documents pertaining to the development of
//HID touch/multi-touch devices for Windows Touch enabled operating systems.
//The below example HID report descriptor is almost identical to one of Microsoft's example
//descriptors.  Microsoft provides some example descriptors and background information in
//the following documents:

//"Digitizer Drivers for Windows Touch and Pen-Based Computers" (publicly available download: DigitizerDrvs_touch.docx)
//"Human Interface Device (HID) Extensions and Windows/Device Protocol for Multi-Touch Digitizers" (not
	//currently a public document[but might be in the future??], contact Microsoft if a
	//public version is unavailable.  However, this document isn't critical to developing
	//a touch/multi-touch device.  Most of the relevant content is already in the "Digitizer
	//Drivers for Windows Touch and Pen-Based Computers" document, and in Microsoft's developer
	//blogs relating to Windows 7 Touch)


//At a minimum a multi-touch capable finger input digitizer
//must support the following report information (one set of data for each simultaneous contact supported):
//"Tip Switch" (ex: a soft button, indicating that the tip of the human's finger is touching the screen)
//"In-range" indicator: For example, if the touch screen can detect when the user's finger is
//		hovering over an area, but not in direct contact (ex: it uses optical detection
//		methods), it may set the in-range indicator bit, but not necessarily simultanously
//		set the tip switch.  If a hard contact is detected, both the tip switch and in
//		range bits should be set.
//X coordinate of contact point (see note)
//Y coordinate of contact point (see note)
//Other input usages are optional.

//NOTE (X/Y Coordinates): The origin is the upper left corner of the screen.  For the X coordinates,
//the leftmost part of the screen has X coordinate = 0.  The rightmost edge of the screen has
//X coordinate = 4800 (for this example HID report descriptor).  The topmost part of the screen
//has Y coordinate = 0.  The bottom most part of the screen has Y coordinate = 3000 for this
//example HID report descriptor.

const struct{uint8_t report[HID_RPT01_SIZE];}hid_rpt01={
    {
/* Data format:
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * | Tip |xxxxx| Contact Identifier                |
 * | X                                             |
 * |                                               |
 * | Y                                             |
 * |                                               |
 * ... 5 times
 * | Contact Count                                 |
 */
    0x05, 0x0d,                    // USAGE_PAGE (Digitizers)
    0x09, 0x04,                    // USAGE (Touch Screen)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x09, 0x22,                    //   USAGE (Finger)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x25, 0x3f,                    //     LOGICAL_MAXIMUM (63)
    0x09, 0x51,                    //     USAGE (Contact Identifier)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xa4,                          //     PUSH
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x26, 0x20, 0x03,              //     LOGICAL_MAXIMUM (800)
    0x46, 0xad, 0x01,              //     PHYSICAL_MAXIMUM (429)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x65, 0x33,                    //     UNIT (Eng Lin:0x33)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x26, 0xe0, 0x01,              //     LOGICAL_MAXIMUM (480)
    0x46, 0x03, 0x01,              //     PHYSICAL_MAXIMUM (259)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xb4,                          //     POP
    0xc0,                          //   END_COLLECTION
    0x05, 0x0d,                    //   USAGE_PAGE (Digitizers)
    0x09, 0x22,                    //   USAGE (Finger)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x25, 0x3f,                    //     LOGICAL_MAXIMUM (63)
    0x09, 0x51,                    //     USAGE (Contact Identifier)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xa4,                          //     PUSH
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x26, 0x20, 0x03,              //     LOGICAL_MAXIMUM (800)
    0x46, 0xad, 0x01,              //     PHYSICAL_MAXIMUM (429)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x65, 0x33,                    //     UNIT (Eng Lin:0x33)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x26, 0xe0, 0x01,              //     LOGICAL_MAXIMUM (480)
    0x46, 0x03, 0x01,              //     PHYSICAL_MAXIMUM (259)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xb4,                          //     POP
    0xc0,                          //   END_COLLECTION
    0x05, 0x0d,                    //   USAGE_PAGE (Digitizers)
    0x09, 0x22,                    //   USAGE (Finger)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x25, 0x3f,                    //     LOGICAL_MAXIMUM (63)
    0x09, 0x51,                    //     USAGE (Contact Identifier)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xa4,                          //     PUSH
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x26, 0x20, 0x03,              //     LOGICAL_MAXIMUM (800)
    0x46, 0xad, 0x01,              //     PHYSICAL_MAXIMUM (429)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x65, 0x33,                    //     UNIT (Eng Lin:0x33)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x26, 0xe0, 0x01,              //     LOGICAL_MAXIMUM (480)
    0x46, 0x03, 0x01,              //     PHYSICAL_MAXIMUM (259)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xb4,                          //     POP
    0xc0,                          //   END_COLLECTION
    0x05, 0x0d,                    //   USAGE_PAGE (Digitizers)
    0x09, 0x22,                    //   USAGE (Finger)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x25, 0x3f,                    //     LOGICAL_MAXIMUM (63)
    0x09, 0x51,                    //     USAGE (Contact Identifier)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xa4,                          //     PUSH
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x26, 0x20, 0x03,              //     LOGICAL_MAXIMUM (800)
    0x46, 0xad, 0x01,              //     PHYSICAL_MAXIMUM (429)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x65, 0x33,                    //     UNIT (Eng Lin:0x33)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x26, 0xe0, 0x01,              //     LOGICAL_MAXIMUM (480)
    0x46, 0x03, 0x01,              //     PHYSICAL_MAXIMUM (259)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xb4,                          //     POP
    0xc0,                          //   END_COLLECTION
    0x05, 0x0d,                    //   USAGE_PAGE (Digitizers)
    0x09, 0x22,                    //   USAGE (Finger)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x25, 0x3f,                    //     LOGICAL_MAXIMUM (63)
    0x09, 0x51,                    //     USAGE (Contact Identifier)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xa4,                          //     PUSH
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x26, 0x20, 0x03,              //     LOGICAL_MAXIMUM (800)
    0x46, 0xad, 0x01,              //     PHYSICAL_MAXIMUM (429)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x65, 0x33,                    //     UNIT (Eng Lin:0x33)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x26, 0xe0, 0x01,              //     LOGICAL_MAXIMUM (480)
    0x46, 0x03, 0x01,              //     PHYSICAL_MAXIMUM (259)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xb4,                          //     POP
    0xc0,                          //   END_COLLECTION
    0x05, 0x0d,                    //   USAGE_PAGE (Digitizers)
    0x09, 0x54,                    //   USAGE (Contact Count)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x25, 0x05,                    //   LOGICAL_MAXIMUM (5)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x09, 0x55,                    //   USAGE (Contact Count Maximum)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xc0                           // END_COLLECTION
    }
};// end of HID report descriptor


//Array of configuration descriptors
const uint8_t *const USB_CD_Ptr[]=
{
    (const uint8_t *const)&configDescriptor1
};

//Array of string descriptors
const uint8_t *const USB_SD_Ptr[]=
{
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

/** EOF usb_descriptors.c ***************************************************/

