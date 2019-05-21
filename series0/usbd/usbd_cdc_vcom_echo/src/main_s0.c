/***************************************************************************//**
 * @file main_s0.c
 * @brief USB CDC example that implements a virtual COM port (VCOM) echo
 * example. Data that is received from the USB host is processed by the USB
 * device and then sent back to the USB host. In this case, the USB host is the
 * computer and the USB device is the EFM32 board.
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
#include "em_emu.h"
#include "em_chip.h"

// USB specific includes
#include "em_usb.h"
#include "cdc_echo.h"
#include "descriptors.h"

extern void cdcStateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

/***************************************************************************//**
 * @brief
 *    Main
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Set the callback functions (see src/cdc.c)
  USBD_Callbacks_TypeDef callbacks = {
    .usbReset        = NULL,
    .usbStateChange  = cdcStateChangeEvent, // Called when the device changes state
    .setupCmd        = cdcSetupCmd,         // Called on each setup request from the host
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

  // Enter EM1 to save energy
  while (1) {
    EMU_EnterEM1();
  }
}

