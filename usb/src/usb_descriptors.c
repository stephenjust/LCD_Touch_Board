/*
 * USB Descriptors file
 *
 * This file may be used by anyone for any purpose and may be used as a
 * starting point making your own application using M-Stack.
 *
 * It is worth noting that M-Stack itself is not under the same license as
 * this file.
 *
 * M-Stack is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  For details, see sections 7, 8, and 9
 * of the Apache License, version 2.0 which apply to this file.  If you have
 * purchased a commercial license for this software from Signal 11 Software,
 * your commerical license superceeds the information in this header.
 *
 * Alan Ott
 * Signal 11 Software
 */

#include "usb_config.h"
#include "usb.h"
#include "usb_ch9.h"
#include "usb_hid.h"

#ifdef __C18
#define ROMPTR rom
#else
#define ROMPTR
#endif

/* Configuration Packet
 *
 * This packet contains a configuration descriptor, one or more interface
 * descriptors, class descriptors(optional), and endpoint descriptors for a
 * single configuration of the device.  This struct is specific to the
 * device, so the application will need to add any interfaces, classes and
 * endpoints it intends to use.  It is sent to the host in response to a
 * GET_DESCRIPTOR[CONFIGURATION] request.
 *
 * While Most devices will only have one configuration, a device can have as
 * many configurations as it needs.  To have more than one, simply make as
 * many of these structs as are required, one for each configuration.
 *
 * An instance of each configuration packet must be put in the
 * usb_application_config_descs[] array below (which is #defined in
 * usb_config.h) so that the USB stack can find it.
 *
 * See Chapter 9 of the USB specification from usb.org for details.
 *
 * It's worth noting that adding endpoints here does not automatically
 * enable them in the USB stack.  To use an endpoint, it must be declared
 * here and also in usb_config.h.
 *
 * The configuration packet below is for the mouse demo application.
 * Yours will of course vary.
 */
struct configuration_1_packet {
	struct configuration_descriptor  config;
	/* First Interface */
	struct interface_descriptor      interface;
	struct hid_descriptor            hid;
	struct endpoint_descriptor       ep;
	struct endpoint_descriptor       ep1_out;

	/* Second Interface */
	struct interface_descriptor      interface2;
	struct hid_descriptor            hid2;
	struct endpoint_descriptor       ep2;
	struct endpoint_descriptor       ep2_out;
};


/* Device Descriptor
 *
 * Each device has a single device descriptor describing the device.  The
 * format is described in Chapter 9 of the USB specification from usb.org.
 * USB_DEVICE_DESCRIPTOR needs to be defined to the name of this object in
 * usb_config.h.  For more information, see USB_DEVICE_DESCRIPTOR in usb.h.
 */
const ROMPTR struct device_descriptor this_device_descriptor =
{
	sizeof(struct device_descriptor), // bLength
	DESC_DEVICE, // bDescriptorType
	0x0200, // 0x0200 = USB 2.0, 0x0110 = USB 1.1
	0x00, // Device class
	0x00, // Device Subclass
	0x00, // Protocol.
	EP_0_LEN, // bMaxPacketSize0
	0xA0A0, // Vendor
	0x0003, // Product
	0x0001, // device release (1.0)
	1, // Manufacturer
	2, // Product
	0, // Serial
	NUMBER_OF_CONFIGURATIONS // NumConfigurations
};

//HID Report Descriptor for a 5-contact multi-touch digitizer.
static const ROMPTR uint8_t multitouch_report_descriptor[] = {
    //Byte[0] = Report ID == 0x01 == When sending packets of contact information to the host, use the literal value Report ID = 0x01 for this example.

    //First contact point info in bytes 1-6.
    //Byte[1] = Bits7-3: pad bits (unused), Bit1:In Range, Bit0:Tip Switch
    //Byte[2] = Contact identifier number (see note below)
    //Byte[3] = X-coordinate LSB
    //Byte[4] = X-coordinate MSB
    //Byte[5] = Y-coordinate LSB
    //Byte[6] = Y-coordinate MSB
    //...
    //Byte[n]= 8-bit number indicating how many of the above contact points are valid.

    0x05, 0x0D,	          // USAGE_PAGE (Digitizers)
    0x09, 0x04,	          // USAGE (Touch Screen)
    0xA1, 0x01,           // COLLECTION (Application)
    0x85, 0x01, 	  //   REPORT_ID (Touch)              	//When the firmware wants to send a HID report (containing contact information), the Report ID byte should match this number (0x01 in this case).
    0x09, 0x22,  //10     //   USAGE (Finger)
    0xA1, 0x02,           //     COLLECTION (Logical)
    0x09, 0x42,           //       USAGE (Tip Switch)
    0x15, 0x00,           //       LOGICAL_MINIMUM (0)
    0x25, 0x01,           //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,  //20     //       REPORT_SIZE (1)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs) 		//Makes one, 1-bit field for tip switch
    0x95, 0x07,  //30     //       REPORT_COUNT (7)
    0x81, 0x03,           //       INPUT (Cnst,Ary,Abs)			//Makes seven, 1-bit fields, which are pad bits (no valid data)
    0x75, 0x08,           //       REPORT_SIZE (8)
    0x09, 0x51,           //       USAGE (Contact Identifier)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,  //40     //       INPUT (Data,Var,Abs) 		//Makes one, 8-bit field for the contact identifier number.  Firmware arbitrarily assigns a contact ID, which stays the same until the contact becomes invalid (finger removed from screen).
    0x05, 0x01,           //       USAGE_PAGE (Generic Desk..
    0x26, 0x20, 0x03,     //       LOGICAL_MAXIMUM (800)        // Touch panel width
    0x75, 0x10,           //       REPORT_SIZE (16)
    0x55, 0x0E,           //       UNIT_EXPONENT (-2)           //10^(-2)
    0x65, 0x33,           //       UNIT (Inches, English Linear)
    0x09, 0x30,           //       USAGE (X)
    0x35, 0x00,           //       PHYSICAL_MINIMUM (0)
    0x46, 0xAD, 0x01,     //       PHYSICAL_MAXIMUM (0x1AD = 429)     //429 * 10^(-2) = 4.29 inches X-dimension
    0x81, 0x02,  //60     //       INPUT (Data,Var,Abs)           //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800 (4800 is the LOGICAL_MAXIMUM, which would correspond to far edge of the screen, which is 1600x10mil distance from X origin [left of screen])
    0x26, 0xE0, 0x01,     //       LOGICAL_MAXIMUM (480)        // Touch panel height
    0x46, 0x03, 0x01,     //       PHYSICAL_MAXIMUM (0x103 = 259)    //259 * 10^(-2) = 2.59 inches Y-dimension
    0x09, 0x31,           //       USAGE (Y)
    0x81, 0x02,  //70     //       INPUT (Data,Var,Abs)			//Makes one, 16-bit field for Y coordinate info.  Valid values from: 0-3000 (3000 is the LOGICAL_MAIXMUM, which would correspond to the far bottom of the screen, which is 1000x10mil distance from the Y origin [top of screen])
    0xC0,                 //    END_COLLECTION
    0xA1, 0x02,           //    COLLECTION (Logical)
    0x05, 0x0D,	          //       USAGE_PAGE (Digitizers)
    0x09, 0x42,           //       USAGE (Tip Switch)
    0x15, 0x00,           //       LOGICAL_MINIMUM (0)
    0x25, 0x01,           //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,           //       REPORT_SIZE (1)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs) 		//Makes one, 1-bit field for Tip Switch
    0x95, 0x07,           //       REPORT_COUNT (7)
    0x81, 0x03,           //       INPUT (Cnst,Ary,Abs)			//Makes seven, 1-bit fields that are pad bits (no valid data)
    0x75, 0x08,           //       REPORT_SIZE (8)
    0x09, 0x51,           //       USAGE (Contact Identifier)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 8-bit field for the contact identifier number.
    0x05, 0x01,           //       USAGE_PAGE (Generic Desk..
    0x26, 0x20, 0x03,     //       LOGICAL_MAXIMUM (800)        // Screen width
    0x75, 0x10,  //110    //       REPORT_SIZE (16)
    0x55, 0x0E,           //       UNIT_EXPONENT (-2)           //10^(-2)
    0x65, 0x33,           //       UNIT (Inches, English Linear)
    0x09, 0x30,           //       USAGE (X)
    0x35, 0x00,           //       PHYSICAL_MINIMUM (0)
    0x46, 0xAD, 0x01,     //       PHYSICAL_MAXIMUM (0x1AD = 429)     //429 * 10^(-2) = 4.29 inches X-dimension
    0x81, 0x02,           //       INPUT (Data,Var,Abs)         //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800
    0x26, 0xE0, 0x01,     //       LOGICAL_MAXIMUM (480)        // Screen height
    0x46, 0x03, 0x01,     //       PHYSICAL_MAXIMUM (0x103 = 259)    //259 * 10^(-2) = 2.59 inches Y-dimension
    0x09, 0x31,           //       USAGE (Y)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)         //Makes one, 16-bit field for X coordinate info, valid values: 0-3071
    0xC0,                 //    END_COLLECTION
    0xA1, 0x02,           //    COLLECTION (Logical)
    0x05, 0x0D,	          //       USAGE_PAGE (Digitizers)
    0x09, 0x42,  //140    //       USAGE (Tip Switch)
    0x15, 0x00,           //       LOGICAL_MINIMUM (0)
    0x25, 0x01,           //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,           //       REPORT_SIZE (1)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,  //150    //       INPUT (Data,Var,Abs) 		//Makes one, 1-bit field for Tip Switch
    0x95, 0x07,           //       REPORT_COUNT (7)
    0x81, 0x03,           //       INPUT (Cnst,Ary,Abs)			//Makes seven, 1-bit fields that are pad bits (no valid data)
    0x75, 0x08,  //160    //       REPORT_SIZE (8)
    0x09, 0x51,           //       USAGE (Contact Identifier)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 8-bit field for the contact identifier number.
    0x05, 0x01,           //       USAGE_PAGE (Generic Desk..
    0x26, 0x20, 0x03,     //       LOGICAL_MAXIMUM (800)        // Screen width
    0x75, 0x10,           //       REPORT_SIZE (16)
    0x55, 0x0E,           //       UNIT_EXPONENT (-2)       //10^(-2)
    0x65, 0x33,           //       UNIT (Inches, English Linear)  //But exponent -2, so Physical Maximum is in 10?s of mils.
    0x09, 0x30,           //       USAGE (X)
    0x35, 0x00,           //       PHYSICAL_MINIMUM (0)
    0x46, 0xAD, 0x01,     //       PHYSICAL_MAXIMUM (0x1AD = 429)     //429 * 10^(-2) = 4.29 inches X-dimension
    0x81, 0x02,           //       INPUT (Data,Var,Abs)           //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800
    0x26, 0xE0, 0x01,     //       LOGICAL_MAXIMUM (480)        // Screen height
    0x46, 0x03, 0x01,     //       PHYSICAL_MAXIMUM (0x103 = 259)    //259 * 10^(-2) = 2.59 inches Y-dimension
    0x09, 0x31,           //       USAGE (Y)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 16-bit field for X coordinate info, valid values: 0-3071
    0xC0,                 //    END_COLLECTION
    0xA1, 0x02,           //    COLLECTION (Logical)
    0x05, 0x0D,	 //201    //       USAGE_PAGE (Digitizers)
    0x09, 0x42,           //       USAGE (Tip Switch)
    0x15, 0x00,           //       LOGICAL_MINIMUM (0)
    0x25, 0x01,           //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,           //       REPORT_SIZE (1)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs) 		//Makes one, 1-bit field for Tip Switch
    0x95, 0x07,           //       REPORT_COUNT (7)
    0x81, 0x03,           //       INPUT (Cnst,Ary,Abs)			//Makes seven, 1-bit fields that are pad bits (no valid data)
    0x75, 0x08,           //       REPORT_SIZE (8)
    0x09, 0x51,           //       USAGE (Contact Identifier)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 8-bit field for the contact identifier number.
    0x05, 0x01,  //231    //       USAGE_PAGE (Generic Desk..
    0x26, 0x20, 0x03,     //       LOGICAL_MAXIMUM (800)
    0x75, 0x10,           //       REPORT_SIZE (16)
    0x55, 0x0E,           //       UNIT_EXPONENT (-2)       //10^(-2)
    0x65, 0x33,           //       UNIT (Inches, English Linear)  //But exponent -2, so Physical Maximum is in 10?s of mils.
    0x09, 0x30,           //       USAGE (X)
    0x35, 0x00,           //       PHYSICAL_MINIMUM (0)
    0x46, 0xAD, 0x01,     //       PHYSICAL_MAXIMUM (0x1AD = 429)     //429 * 10^(-2) = 4.29 inches X-dimension
    0x81, 0x02,           //       INPUT (Data,Var,Abs)           //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800
    0x26, 0xE0, 0x01,     //       LOGICAL_MAXIMUM (480)             //16:10 aspect ratio (X:Y)
    0x46, 0x03, 0x01,     //       PHYSICAL_MAXIMUM (0x103 = 259)    //259 * 10^(-2) = 2.59 inches Y-dimension
    0x09, 0x31,           //       USAGE (Y)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 16-bit field for X coordinate info, valid values: 0-3071
    0xC0,        //260    //    END_COLLECTION
    0xA1, 0x02,           //    COLLECTION (Logical)
    0x05, 0x0D,	          //       USAGE_PAGE (Digitizers)
    0x09, 0x42,           //       USAGE (Tip Switch)
    0x15, 0x00,           //       LOGICAL_MINIMUM (0)
    0x25, 0x01,           //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,           //       REPORT_SIZE (1)
    0x95, 0x01,           //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs) 		//Makes one, 1-bit field for Tip Switch
    0x95, 0x07,           //       REPORT_COUNT (7)
    0x81, 0x03,           //       INPUT (Cnst,Ary,Abs)			//Makes seven, 1-bit fields that are pad bits (no valid data)
    0x75, 0x08,           //       REPORT_SIZE (8)
    0x09, 0x51,           //       USAGE (Contact Identifier)
    0x95, 0x01,  //290    //       REPORT_COUNT (1)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 8-bit field for the contact identifier number.
    0x05, 0x01,           //       USAGE_PAGE (Generic Desk..
    0x26, 0x20, 0x03,     //       LOGICAL_MAXIMUM (800)
    0x75, 0x10,           //       REPORT_SIZE (16)
    0x55, 0x0E,           //       UNIT_EXPONENT (-2)       //10^(-2)
    0x65, 0x33,           //       UNIT (Inches, English Linear)  //But exponent -2, so Physical Maximum is in 10?s of mils.
    0x09, 0x30,           //       USAGE (X)
    0x35, 0x00,           //       PHYSICAL_MINIMUM (0)
    0x46, 0xAD, 0x01,     //       PHYSICAL_MAXIMUM (0x1AD = 429)     //429 * 10^(-2) = 4.29 inches X-dimension
    0x81, 0x02,           //       INPUT (Data,Var,Abs)           //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800
    0x26, 0xE0, 0x01,     //       LOGICAL_MAXIMUM (480)             //16:10 aspect ratio (X:Y)
    0x46, 0x03, 0x01,     //       PHYSICAL_MAXIMUM (0x103 = 259)    //259 * 10^(-2) = 2.59 inches Y-dimension
    0x09, 0x31,  //320    //       USAGE (Y)
    0x81, 0x02,           //       INPUT (Data,Var,Abs)			//Makes one, 16-bit field for X coordinate info, valid values: 0-3071
    0xC0,                 //   END_COLLECTION
    0x05, 0x0D,	          //   USAGE_PAGE (Digitizers)
    0x09, 0x54,	          //   USAGE (Actual count)
    0x95, 0x01,           //   REPORT_COUNT (1)
    0x75, 0x08,           //   REPORT_SIZE (8)
    0x25, 0x05,           //   LOGICAL_MAXIMUM (5)          //Maximum number of valid contact points simutaneously supported
    0x81, 0x02,           //   INPUT (Data,Var,Abs)         //Makes one, 8-bit field for the actual number of valid contacts reported
    0x85, 0x02,           //   REPORT_ID (Feature)          //When the firmware wants to send a feature report, the Report ID byte should match this number (0x02 in this case).
    0x09, 0x55,           //   USAGE(Maximum Count)
    0xB1, 0x02,           //   FEATURE (Data,Var,Abs)
    0xC0         //342    // END_COLLECTION
};// end of HID report descriptor

/* HID Report descriptor for the second interface. See the HID specification
 * for more deatils. This is the mouse example from the "HID Descriptor Tool"
 * which can be downloaded from USB.org. It is modified to only have 3 axes
 * just to be different from the one above. */
static const ROMPTR uint8_t mouse_report_descriptor2[] = {
   0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

/* Configuration Packet Instance
 *
 * This is an instance of the configuration_packet struct containing all the
 * data describing a single configuration of this device.  It is wise to use
 * as much C here as possible, such as sizeof() operators, and #defines from
 * usb_config.h.  When stuff is wrong here, it can be difficult to track
 * down exactly why, so it's good to get the compiler to do as much of it
 * for you as it can.
 */
static const ROMPTR struct configuration_1_packet configuration_1 =
{
	{
	// Members from struct configuration_descriptor
	sizeof(struct configuration_descriptor),
	DESC_CONFIGURATION,
	sizeof(configuration_1), // wTotalLength (length of the whole packet)
	1, // bNumInterfaces
	1, // bConfigurationValue
	2, // iConfiguration (index of string descriptor)
	0b10000000,
	100/2,   // 100/2 indicates 100mA
	},

	/* First Interface */
	{
	// Members from struct interface_descriptor
	sizeof(struct interface_descriptor), // bLength;
	DESC_INTERFACE,
	0x1, // InterfaceNumber
	0x0, // AlternateSetting
	0x2, // bNumEndpoints (num besides endpoint 0)
	HID_INTERFACE_CLASS, // bInterfaceClass 3=HID, 0xFF=VendorDefined
	0x00, // bInterfaceSubclass (0=NoBootInterface for HID)
	0x00, // bInterfaceProtocol
	0x03, // iInterface (index of string describing interface)
	},

	{
	// Members from struct hid_descriptor
	sizeof(struct hid_descriptor),
	DESC_HID,
	0x0101, // bcdHID
	0x0, // bCountryCode
	1,   // bNumDescriptors
	DESC_REPORT, // bDescriptorType2
	sizeof(multitouch_report_descriptor), // wDescriptorLength
	},

	{
	// Members of the Endpoint Descriptor (EP1 IN)
	sizeof(struct endpoint_descriptor),
	DESC_ENDPOINT,
	0x01 | 0x80, // endpoint #1 0x80=IN
	EP_INTERRUPT, // bmAttributes
	EP_1_IN_LEN, // wMaxPacketSize
	1, // bInterval in ms.
	},

	{
	// Members of the Endpoint Descriptor (EP1 OUT)
	sizeof(struct endpoint_descriptor),
	DESC_ENDPOINT,
	0x01 /*| 0x00*/, // endpoint #1 0x00=OUT
	EP_INTERRUPT, // bmAttributes
	EP_1_OUT_LEN, // wMaxPacketSize
	1, // bInterval in ms.
	},
};

/* String Descriptors
 *
 * String descriptors are optional. If strings are used, string #0 is
 * required, and must contain the language ID of the other strings.  See
 * Chapter 9 of the USB specification from usb.org for more info.
 *
 * Strings are UTF-16 Unicode, and are not NULL-terminated, hence the
 * unusual syntax.
 */

/* String index 0, only has one character in it, which is to be set to the
   language ID of the language which the other strings are in. */
static const ROMPTR struct {uint8_t bLength;uint8_t bDescriptorType; uint16_t lang; } str00 = {
	sizeof(str00),
	DESC_STRING,
	0x0409 // US English
};

static const ROMPTR struct {uint8_t bLength;uint8_t bDescriptorType; uint16_t chars[12]; } vendor_string = {
	sizeof(vendor_string),
	DESC_STRING,
	{'S','t','e','p','h','e','n',' ','J','u','s','t'}
};

static const ROMPTR struct {uint8_t bLength;uint8_t bDescriptorType; uint16_t chars[20]; } product_string = {
	sizeof(product_string),
	DESC_STRING,
	{'U','S','B',' ','M','u','l','t','i','t','o','u','c','h',' ','P','a','n','e','l'}
};

// FIXME: Change these!
static const ROMPTR struct {uint8_t bLength;uint8_t bDescriptorType; uint16_t chars[23]; } interface_string1 = {
	sizeof(interface_string1),
	DESC_STRING,
	{'I','n','t','e','r','f','a','c','e',' ','1',':',' ','H','o','r','i','z','o','n','t','a','l'}
};

static const ROMPTR struct {uint8_t bLength;uint8_t bDescriptorType; uint16_t chars[21]; } interface_string2 = {
	sizeof(interface_string2),
	DESC_STRING,
	{'I','n','t','e','r','f','a','c','e',' ','2',':',' ','V','e','r','t','i','c','a','l'}
};

/* Get String function
 *
 * This function is called by the USB stack to get a pointer to a string
 * descriptor.  If using strings, USB_STRING_DESCRIPTOR_FUNC must be defined
 * to the name of this function in usb_config.h.  See
 * USB_STRING_DESCRIPTOR_FUNC in usb.h for information about this function.
 * This is a function, and not simply a list or map, because it is useful,
 * and advisable, to have a serial number string which may be read from
 * EEPROM or somewhere that's not part of static program memory.
 */
int16_t usb_application_get_string(uint8_t string_number, const void **ptr)
{
	if (string_number == 0) {
		*ptr = &str00;
		return sizeof(str00);
	}
	else if (string_number == 1) {
		*ptr = &vendor_string;
		return sizeof(vendor_string);
	}
	else if (string_number == 2) {
		*ptr = &product_string;
		return sizeof(product_string);
	}
	else if (string_number == 3) {
		*ptr = &interface_string1;
		return sizeof(interface_string1);
	}
	else if (string_number == 4) {
		*ptr = &interface_string2;
		return sizeof(interface_string2);
	}
	else if (string_number == 5) {
		/* This is where you might have code to do something like read
		   a serial number out of EEPROM and return it. */
		return -1;
	}

	return -1;
}

/* Configuration Descriptor List
 *
 * This is the list of pointters to the device's configuration descriptors.
 * The USB stack will read this array looking for descriptors which are
 * requsted from the host.  USB_CONFIG_DESCRIPTOR_MAP must be defined to the
 * name of this array in usb_config.h.  See USB_CONFIG_DESCRIPTOR_MAP in
 * usb.h for information about this array.  The order of the descriptors is
 * not important, as the USB stack reads bConfigurationValue for each
 * descriptor to know its index.  Make sure NUMBER_OF_CONFIGURATIONS in
 * usb_config.h matches the number of descriptors in this array.
 */
const struct configuration_descriptor *usb_application_config_descs[] =
{
	(struct configuration_descriptor*) &configuration_1,
};
STATIC_SIZE_CHECK_EQUAL(USB_ARRAYLEN(USB_CONFIG_DESCRIPTOR_MAP), NUMBER_OF_CONFIGURATIONS);
STATIC_SIZE_CHECK_EQUAL(sizeof(USB_DEVICE_DESCRIPTOR), 18);

/* HID Descriptor Function */
int16_t usb_application_get_hid_descriptor(uint8_t interface, const void **ptr)
{
	/* Only one interface in this demo. The two-step assignment avoids an
	 * incorrect error in XC8 on PIC16. */
	if (interface == 1) {
		const void *p = &configuration_1.hid;
		*ptr = p;
		return sizeof(configuration_1.hid);
	}
	else if (interface == 2) {
		const void *p = &configuration_1.hid2;
		*ptr = p;
		return sizeof(configuration_1.hid2);
	}

	return -1;
}

/** HID Report Descriptor Function */
int16_t usb_application_get_hid_report_descriptor(uint8_t interface, const void **ptr)
{
	if (interface == 1) {
		*ptr = multitouch_report_descriptor;
		return sizeof(multitouch_report_descriptor);
	}
	else if (interface == 2) {
		*ptr = mouse_report_descriptor2;
		return sizeof(mouse_report_descriptor2);
	}

	return -1;
}
