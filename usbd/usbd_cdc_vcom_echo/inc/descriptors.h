/***************************************************************************//**
 * @file descriptors.h
 * @brief Global descriptors that describe the device to the host. The actual
 * descriptors are defined in src/descriptors.c
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
#ifndef SILICON_LABS_DESCRIPTORS_H__
#define SILICON_LABS_DESCRIPTORS_H__

#include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const USB_DeviceDescriptor_TypeDef   USBDESC_deviceDesc;
extern const uint8_t                        USBDESC_configDesc[];
extern const void * const                   USBDESC_strings[4];
extern const uint8_t                        USBDESC_bufferingMultiplier[];

#ifdef __cplusplus
}
#endif

#endif /* SILICON_LABS_DESCRIPTORS_H__ */
