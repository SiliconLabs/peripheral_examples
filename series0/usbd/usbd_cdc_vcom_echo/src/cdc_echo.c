/***************************************************************************//**
 * @file cdc.c
 * @brief USB Communication Device Class (CDC) driver for a VCOM echo
 * application.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_usb.h"
#include "cdc_echo.h"

// The serial port LINE CODING data structure, used to carry information
// about serial port baudrate, parity, etc. between host and device.
SL_PACK_START(1)
typedef struct {
  uint32_t dwDTERate;   // Baudrate
  uint8_t  bCharFormat; // Stop bits: 0 = one stop bit, 1 = 1.5 stop bits, 2 = two stop bits
  uint8_t  bParityType; // Parity: 0 = none, 1 = odd, 2 = even, 3 = mark, 4 = space
  uint8_t  bDataBits;   // Data bits: 5, 6, 7, 8, or 16
  uint8_t  dummy;       // To ensure size is a multiple of 4 bytes
} SL_ATTRIBUTE_PACKED cdcLineCoding_TypeDef;
SL_PACK_END()

// The LineCoding variable must be 4-byte aligned
SL_ALIGN(4)
SL_PACK_START(1)
static cdcLineCoding_TypeDef SL_ATTRIBUTE_ALIGN(4) cdcLineCoding = {
  115200, // Baud rate
  0,      // One stop bit
  0,      // No parity bits
  8,      // 8 data bits
  0       // Dummy value to ensure size is a multiple of 4 bytes
};
SL_PACK_END()

// Note: change this to change the receive buffer size
// By default, the receive buffer size is same size as the max size of a full speed bulk endpoint
#define CDC_USB_RX_BUF_SIZE  (USB_FS_BULK_EP_MAXSIZE)

// Create a 4-byte aligned uint8_t array for the USB receive buffer
STATIC_UBUF(usbRxBuffer, CDC_USB_RX_BUF_SIZE);

// Global for letting us know if USB data transmission is currently in progress or not
static bool usbTxActive;

// Function prototypes for receiving/transmitting data over USB
static int usbDataReceived(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
static int usbDataTransmitted(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);

/**************************************************************************//**
* @brief
*    Callback that gets called when the data stage of a CDC_SET_LINECODING
*    setup command has completed
*
* @param[in] status
*    Transfer status code.
*
* @param[in] xferred
*    Number of bytes transferred.
*
* @param[in] remaining
*    Number of bytes not transferred.
*
* @return
*    USB_STATUS_OK if data accepted.
*    USB_STATUS_REQ_ERR if data calls for modes we can not support.
*****************************************************************************/
static int lineCodingReceived(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
 (void) remaining;

 uint32_t frame = 0;

 // We have received new serial port communication settings from USB host
 if ((status == USB_STATUS_OK) && (xferred == 7)) {

   // Check bDataBits, valid values are: 5, 6, 7, 8 or 16 bits
   if (cdcLineCoding.bDataBits == 5) {
     frame |= USART_FRAME_DATABITS_FIVE;
   } else if (cdcLineCoding.bDataBits == 6) {
     frame |= USART_FRAME_DATABITS_SIX;
   } else if (cdcLineCoding.bDataBits == 7) {
     frame |= USART_FRAME_DATABITS_SEVEN;
   } else if (cdcLineCoding.bDataBits == 8) {
     frame |= USART_FRAME_DATABITS_EIGHT;
   } else if (cdcLineCoding.bDataBits == 16) {
     frame |= USART_FRAME_DATABITS_SIXTEEN;
   } else {
     return USB_STATUS_REQ_ERR;
   }

   // Check bParityType, valid values are: 0=None 1=Odd 2=Even 3=Mark 4=Space
   if (cdcLineCoding.bParityType == 0) {
     frame |= USART_FRAME_PARITY_NONE;
   } else if (cdcLineCoding.bParityType == 1) {
     frame |= USART_FRAME_PARITY_ODD;
   } else if (cdcLineCoding.bParityType == 2) {
     frame |= USART_FRAME_PARITY_EVEN;
   } else if (cdcLineCoding.bParityType == 3) {
     return USB_STATUS_REQ_ERR;
   } else if (cdcLineCoding.bParityType == 4) {
     return USB_STATUS_REQ_ERR;
   } else {
     return USB_STATUS_REQ_ERR;
   }

   // Check bCharFormat, valid values are: 0=1 1=1.5 2=2 stop bits
   if (cdcLineCoding.bCharFormat == 0) {
     frame |= USART_FRAME_STOPBITS_ONE;
   } else if (cdcLineCoding.bCharFormat == 1) {
     frame |= USART_FRAME_STOPBITS_ONEANDAHALF;
   } else if (cdcLineCoding.bCharFormat == 2) {
     frame |= USART_FRAME_STOPBITS_TWO;
   } else {
     return USB_STATUS_REQ_ERR;
   }

   return USB_STATUS_OK;
 }
 return USB_STATUS_REQ_ERR;
}

/**************************************************************************//**
 * @brief
 *    Callback that gets called whenever a USB setup command is received from
 *    the host.
 *
 * @param[in] setup
 *    Pointer to a USB setup packet
 *
 * @return
 *    USB_STATUS_OK --> if command was accepted
 *    USB_STATUS_REQ_UNHANDLED --> when command is unknown, the USB device
 *                                 stack will handle the request.
 *****************************************************************************/
int cdcSetupCmd(const USB_Setup_TypeDef *setup)
{
  int retVal = USB_STATUS_REQ_UNHANDLED;

  if ((setup->Type == USB_SETUP_TYPE_CLASS) && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {

    // Determine the type of setup request
    switch (setup->bRequest) {

      // USB host is trying to get the line coding settings from the device
      case USB_CDC_GETLINECODING:
        if ((setup->wValue == 0)
              && (setup->wIndex == CDC_CTRL_INTERFACE_NO) // Interface number
              && (setup->wLength == 7)                    // Length of cdcLineCoding
              && (setup->Direction == USB_SETUP_DIR_IN))  // Transfer direction (from host perspective)
        {
          USBD_Write(0, (void*) &cdcLineCoding, 7, NULL); // Send current settings to the host
          retVal = USB_STATUS_OK;
        }
        break;

      // USB host is trying to set the device's line coding settings
      case USB_CDC_SETLINECODING:
        if ((setup->wValue == 0)
              && (setup->wIndex == CDC_CTRL_INTERFACE_NO) // Interface number
              && (setup->wLength == 7)                    // Length of cdcLineCoding
              && (setup->Direction == USB_SETUP_DIR_OUT)) // Transfer direction (from host perspective)
        {
          USBD_Read(0, (void*) &cdcLineCoding, 7, lineCodingReceived); // Get new settings from the host
          retVal = USB_STATUS_OK;
        }
        break;

      // RS-232 signal used to tell the DCE device the DTE device is now present
      case USB_CDC_SETCTRLLINESTATE:
        if ((setup->wIndex == CDC_CTRL_INTERFACE_NO) // Interface number
              && (setup->wLength == 0))              // No data
        {
          retVal = USB_STATUS_OK; // Do nothing (non-compliant behaviour)
        }
        break;
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief
 *    Callback that gets called each time the USB device state is changed.
 *    Also starts CDC operation once the device has been configured by the
 *    USB host.
 *
 * @details
 *    This example doesn't do anything with the state transition from the
 *    suspended state to the configured state. The code checks for this
 *    transition only so that the user could add functionality if they so
 *    desired. The same goes for the the state transition from configured to
 *    anything but the suspended state. The same also goes for any state
 *    transition to the suspended state.
 *
 * @note
 *    Refer to section 4 of the AN0065 USB Device application note for the
 *    USB stack's state machine
 *
 * @param[in] oldState
 *    The old USB device state
 *
 * @param[in] newState
 *    The new (current) USB device state
 *****************************************************************************/
void cdcStateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState)
{
  // If the USB device was configured
  if (newState == USBD_STATE_CONFIGURED) {

    // If we transitioned from the suspended state to the configured state due to bus activity
    if (oldState == USBD_STATE_SUSPENDED) {} // Currently does nothing

    // Initially, we are waiting to receive data from the USB host over USB
    usbTxActive = false;

    // Setup a new USB receive transfer on the USB host's OUT endpoint
    USBD_Read(CDC_EP_DATA_OUT, (void*) usbRxBuffer,
              CDC_USB_RX_BUF_SIZE, usbDataReceived);
  }
  // Else if we have been de-configured
  else if ((oldState == USBD_STATE_CONFIGURED) && (newState != USBD_STATE_SUSPENDED)) {
    // Currently nothing is done here
  }
  // Else if we have been suspended
  else if (newState == USBD_STATE_SUSPENDED) {
    // Currently nothing is done here
  }
}

/**************************************************************************//**
 * @brief
 *    Callback function that gets called whenever data is received from the
 *    host over USB
 *
 * @param[in] status
 *    Transfer status code
 *
 * @param[in] xferred
 *    Number of bytes transferred
 *
 * @param[in] remaining
 *    Number of bytes not transferred
 *
 * @return
 *    USB_STATUS_OK
 *****************************************************************************/
static int usbDataReceived(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
  (void) remaining; // Unused parameter

  // If the status is OK and we actually received data
  if ((status == USB_STATUS_OK) && (xferred > 0)) {

    // If the TX line is not busy, we can send a new USB packet
    if (!usbTxActive) {

      // Setup a USB transmit transfer
      usbTxActive = true;
      USBD_Write(CDC_EP_DATA_IN, (void*) usbRxBuffer,
          xferred, usbDataTransmitted);

      // Setup a new USB receive transfer
      USBD_Read(CDC_EP_DATA_OUT, (void*) usbRxBuffer,
          CDC_USB_RX_BUF_SIZE, usbDataReceived);
    }
  }
  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *    Callback function that gets called whenever a packet with data has
 *    been transmitted over USB to the USB host
 *
 * @param[in] status
 *    Transfer status code
 *
 * @param[in] xferred
 *    Number of bytes transferred
 *
 * @param[in] remaining
 *    Number of bytes not transferred
 *
 * @return
 *    USB_STATUS_OK.
 *****************************************************************************/
static int usbDataTransmitted(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
{
  (void) xferred;   // Unused parameter
  (void) remaining; // Unused parameter

  // Mark that the USB transmit transaction has been completed
  if (status == USB_STATUS_OK) {
    usbTxActive = false;
  }

  return USB_STATUS_OK;
}

