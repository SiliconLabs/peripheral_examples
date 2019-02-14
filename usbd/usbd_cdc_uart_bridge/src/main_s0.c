/***************************************************************************//**
 * @file main_series0.c
 * @brief USB CDC example that implements a USART to USB bridge. Input that is
 * received on the USB device's USART RX pin gets processed and then sent over
 * USB to the USB host. Input that is received from the USB host is then
 * processed and sent to the USB device's USART TX pin. In this case, the USB
 * host is the computer and the USB device is the EFM32 board.
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

// Generic includes
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_emu.h"

// USB specific includes
#include "em_usb.h"
#include "cdc.h"
#include "descriptors.h"

/***************************************************************************//**
 * @brief
 *    Entrypoint for the C program
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize the communication class device
  // (Setup the DMA and USART pins)
  CDC_Init();

  // Set the callback functions (see Drivers/cdc.c)
  const USBD_Callbacks_TypeDef callbacks = {
    .usbReset        = NULL,
    .usbStateChange  = CDC_StateChangeEvent, // Called when the device changes state
    .setupCmd        = CDC_SetupCmd,         // Called on each setup request from the host
    .isSelfPowered   = NULL,
    .sofInt          = NULL
  };

  // Set the initialization struct descriptors (see src/descriptors.c)
  const USBD_Init_TypeDef usbInitStruct = {
    .deviceDescriptor    = &USBDESC_deviceDesc,
    .configDescriptor    = USBDESC_configDesc,
    .stringDescriptors   = USBDESC_strings,
    .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
    .callbacks           = &callbacks,
    .bufferingMultiplier = USBDESC_bufferingMultiplier,
    .reserved            = 0
  };

  // Initialize and start USB device stack
  USBD_Init(&usbInitStruct);

  /* When using a debugger it is practical to uncomment the following three
     lines to force the host to re-enumerate the device. */
  // USBD_Disconnect();
  // USBTIMER_DelayMs( 1000 );
  // USBD_Connect();

  // Enter EM1 to save energy
  while (1) {
    EMU_EnterEM1();
  }
}

