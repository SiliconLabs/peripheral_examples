/***************************************************************************//**
 * @file descriptors.c
 * @brief Global descriptors that describe the device to the host.
 * @version 0.0.1
 *******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "descriptors.h"

/****************************************************************************
 * Device Descriptor                                                        *
 ****************************************************************************/
SL_ALIGN(4)
const USB_DeviceDescriptor_TypeDef USBDESC_deviceDesc SL_ATTRIBUTE_ALIGN(4) = {
  .bLength            = USB_DEVICE_DESCSIZE,    // Descriptor size in bytes
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,  // Descriptor type
  .bcdUSB             = 0x0200,                 // USB version (0200 in BCD = USB 2.0)
  .bDeviceClass       = USB_CLASS_CDC,          // Class code
  .bDeviceSubClass    = 0,                      // Subclass code
  .bDeviceProtocol    = 0,                      // Protocol code
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE, // Max packet size for EP0
  .idVendor           = 0x10C4,                 // Vendor ID
  .idProduct          = 0x0008,                 // Product ID
  .bcdDevice          = 0x0000,                 // Release number in BCD
  .iManufacturer      = 1,                      // Index of string descriptor for manufacturer
  .iProduct           = 2,                      // Index of string descriptor for product
  .iSerialNumber      = 0,                      // Index of string descriptor for manufacturer
  .bNumConfigurations = 1                       // Number of supported configurations
};

/****************************************************************************
 * Configuration Descriptor Length Calculation                              *
 ****************************************************************************/
#define USB_CDC_UNION_FND_DSSCSIZE 5
#define CONFIG_DESC_TOTAL_LEN                  \
  (USB_CONFIG_DESCSIZE                         \
   + (USB_INTERFACE_DESCSIZE * NUM_INTERFACES) \
   + (USB_ENDPOINT_DESCSIZE  * NUM_EP_USED)    \
   + USB_CDC_HEADER_FND_DESCSIZE               \
   + USB_CDC_CALLMNG_FND_DESCSIZE              \
   + USB_CDC_ACM_FND_DESCSIZE                  \
   + USB_CDC_UNION_FND_DSSCSIZE)

/****************************************************************************
 * Configuration Descriptor and Subordinate Descriptors                     *
 *  - Interface Descriptors and Endpoint Descriptors                        *
 ****************************************************************************/
SL_ALIGN(4)
const uint8_t USBDESC_configDesc[] SL_ATTRIBUTE_ALIGN(4) = {

  // Configuration descriptor
  USB_CONFIG_DESCSIZE,          // bLength
  USB_CONFIG_DESCRIPTOR,        // bDescriptorType
  CONFIG_DESC_TOTAL_LEN,        // wTotalLength (LSB)
  CONFIG_DESC_TOTAL_LEN >> 8,   // wTotalLength (MSB)
  NUM_INTERFACES,               // bNumInterfaces
  1,                            // bConfigurationValue
  0,                            // iConfiguration
  CONFIG_DESC_BM_RESERVED_D7    // bmAttrib: Self powered
  | CONFIG_DESC_BM_SELFPOWERED,
  CONFIG_DESC_MAXPOWER_mA(100), // bMaxPower: 100 mA

  // CDC Communication CTRL Interface descriptor (Interface #0)
  USB_INTERFACE_DESCSIZE,       // bLength
  USB_INTERFACE_DESCRIPTOR,     // bDescriptorType
  CDC_CTRL_INTERFACE_NO,        // bInterfaceNumber
  0,                            // bAlternateSetting
  1,                            // bNumEndpoints
  USB_CLASS_CDC,                // bInterfaceClass
  USB_CLASS_CDC_ACM,            // bInterfaceSubClass
  0,                            // bInterfaceProtocol
  0,                            // iInterface

  // CDC Header Functional descriptor
  USB_CDC_HEADER_FND_DESCSIZE,  // bFunctionLength
  USB_CS_INTERFACE_DESCRIPTOR,  // bDescriptorType
  USB_CLASS_CDC_HFN,            // bDescriptorSubtype
  0x20,                         // bcdCDC spec.no LSB
  0x01,                         // bcdCDC spec.no MSB

  // CDC Call Management Functional descriptor
  USB_CDC_CALLMNG_FND_DESCSIZE, // bFunctionLength
  USB_CS_INTERFACE_DESCRIPTOR,  // bDescriptorType
  USB_CLASS_CDC_CMNGFN,         // bDescriptorSubtype
  0,                            // bmCapabilities
  CDC_DATA_INTERFACE_NO,        // bDataInterface

  // CDC Abstract Control Management Functional descriptor
  USB_CDC_ACM_FND_DESCSIZE,     // bFunctionLength
  USB_CS_INTERFACE_DESCRIPTOR,  // bDescriptorType
  USB_CLASS_CDC_ACMFN,          // bDescriptorSubtype
  0x02,                         // bmCapabilities
                                //   D7..D4: RESERVED (Reset to zero)
                                //   D3: 1 - Device supports the notification Network_Connection.
                                //   D2: 1 - Device supports the request Send_Break
                                //   D1: 1 - Device supports the request combination of
                                //           Set_Line_Coding, Set_Control_Line_State,
                                //           Get_Line_Coding, and the notification Serial_State.
                                //   D0: 1 - Device supports the request combination of Set_Comm_Feature,
                                //           Clear_Comm_Feature, and Get_Comm_Feature.

  // CDC Union Functional descriptor
  USB_CDC_UNION_FND_DSSCSIZE,  // bFunctionLength
  USB_CS_INTERFACE_DESCRIPTOR, // bDescriptorType
  USB_CLASS_CDC_UNIONFN,       // bDescriptorSubtype
  CDC_CTRL_INTERFACE_NO,       // bControlInterface
  CDC_DATA_INTERFACE_NO,       // bSubordinateInterface0

  // CDC Notification endpoint descriptor (IN) (INTERRUPT)
  USB_ENDPOINT_DESCSIZE,       // bLength
  USB_ENDPOINT_DESCRIPTOR,     // bDescriptorType
  CDC_EP_NOTIFY,               // bEndpointAddress (IN)
  USB_EPTYPE_INTR,             // bmAttributes
  USB_FS_INTR_EP_MAXSIZE,      // wMaxPacketSize (LSB)
  0,                           // wMaxPacketSize (MSB)
  0xFF,                        // bInterval

  // CDC Data Interface descriptor (Interface #1)
  USB_INTERFACE_DESCSIZE,      // bLength
  USB_INTERFACE_DESCRIPTOR,    // bDescriptorType
  CDC_DATA_INTERFACE_NO,       // bInterfaceNumber
  0,                           // bAlternateSetting
  2,                           // bNumEndpoints
  USB_CLASS_CDC_DATA,          // bInterfaceClass
  0,                           // bInterfaceSubClass
  0,                           // bInterfaceProtocol
  0,                           // iInterface

  // CDC Data interface endpoint descriptor (IN) (BULK)
  USB_ENDPOINT_DESCSIZE,       // bLength
  USB_ENDPOINT_DESCRIPTOR,     // bDescriptorType
  CDC_EP_DATA_IN,              // bEndpointAddress (IN)
  USB_EPTYPE_BULK,             // bmAttributes
  USB_FS_BULK_EP_MAXSIZE,      // wMaxPacketSize (LSB)
  0,                           // wMaxPacketSize (MSB)
  0,                           // bInterval

  // CDC Data interface endpoint descriptor (OUT) (BULK)
  USB_ENDPOINT_DESCSIZE,       // bLength
  USB_ENDPOINT_DESCRIPTOR,     // bDescriptorType
  CDC_EP_DATA_OUT,             // bEndpointAddress (OUT)
  USB_EPTYPE_BULK,             // bmAttributes
  USB_FS_BULK_EP_MAXSIZE,      // wMaxPacketSize (LSB)
  0,                           // wMaxPacketSize (MSB)
  0,                           // bInterval
};

/****************************************************************************
 * Optional String Descriptors                                              *
 ****************************************************************************/
STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09); // English
STATIC_CONST_STRING_DESC(iManufacturer,
                         'S', 'i', 'l', 'i', 'c', 'o', 'n', ' ', \
                         'L', 'a', 'b', 'o', 'r', 'a', 't', 'o', 'r', 'i', 'e', 's', ' ', \
                         'I', 'n', 'c', '.');
STATIC_CONST_STRING_DESC(iProduct,
                         'E', 'F', 'M', '3', '2', ' ', \
                         'U', 'S', 'B', ' ', \
                         'C', 'D', 'C', ' ', \
                         'D', 'e', 'v', 'i', 'c', 'e');
STATIC_CONST_STRING_DESC(iSerialNumber,
                         '0', '0', '0', '0', '1', '2', \
                         '3', '4', '5', '6', '7', '8');
const void* const USBDESC_strings[] = {
  &langID,
  &iManufacturer,
  &iProduct,
  &iSerialNumber
};

/****************************************************************************
 * Endpoint Buffer Size                                                     *
 ****************************************************************************/
// Each multiplier value specifies how much RAM to allocate for each endpoint's
// FIFO. 1 should be used for control/interrupt endpoints and 2 should be used
// for bulk endpoints. Each number represents X times the endpoint size (e.g.
// 2 means the RAM allocated will be equal to 2 times the endpoint size)
const uint8_t USBDESC_bufferingMultiplier[NUM_EP_USED + 1] = {
  1,        // Common Control endpoint
  1, 2, 2   // CDC interrupt and bulk endpoints
};

