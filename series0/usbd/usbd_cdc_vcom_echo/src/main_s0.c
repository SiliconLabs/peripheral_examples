/***************************************************************************//**
 * @file main_s0.c
 * @brief USB CDC example that implements a virtual COM port (VCOM) echo 
 * example. Data that is received from the USB host is processed by the USB
 * device and then sent back to the USB host. In this case, the USB host is the
 * computer and the USB device is the EFM32 board.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
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

