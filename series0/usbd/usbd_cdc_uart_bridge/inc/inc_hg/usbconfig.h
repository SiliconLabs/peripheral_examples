/***************************************************************************//**
 * @file usbconfig.h
 * @brief List of #defines that configure the USB stack.
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
 
#ifndef __USBCONFIG_H
#define __USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Compile stack for device mode
// Needed for emusb/em_usbdxxx.c files to be defined
#define USB_DEVICE

// Choose the clock source for low power mode (this can be either the LFXO or LFRCO)
#define USB_USBC_32kHz_CLK   USB_USBC_32kHz_CLK_LFXO

// If the ONSUSPEND option is set, the USB controller will automatically enter low power mode
// (clocked by 32 kHz clock) whenever the USB enters suspend mode. If ONVBUSOFF is set, the
// USB controller will automatically enter low power mode whenever power is lost
// on VBUS. This requires that the USB regulator is used and that VREGI is connected to VBUS.
// Needed for emusb/em_usbd.c and emusb/em_usbdint.c
#define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)

// Specify the total number of endpoints used (in addition to EP0)
// See src/descriptors.c for the endpoint definitions
// Needed for certain emusb/em_usbdxxx.c files
#define NUM_EP_USED      3

// Specify the number of application timers needed
// This must at least be 1 for the UartRxTimeout() functionality provided in the Drivers/cdc.c code
// Needed for emusb/em_usbtimer.c
#define NUM_APP_TIMERS   1

// Specify which timer to use for the CDC UartRxTimeout() timer
// This #define chooses Timer0 by default
// Needed for Drivers/cdc.c
#define CDC_TIMER_ID     0

// Define the interface numbers
// Needed for Drivers/cdc.c
#define CDC_CTRL_INTERFACE_NO   0
#define CDC_DATA_INTERFACE_NO   1

// Define the total number of interfaces
// Needed for src/descriptors.c
#define NUM_INTERFACES   2

// Define USB endpoint addresses for the interfaces
// Needed for src/descriptors.c and Drivers/cdc.c
#define CDC_EP_DATA_OUT  0x01  // Endpoint for CDC data transmission (host sends to device)
#define CDC_EP_DATA_IN   0x81  // Endpoint for CDC data reception (host receives from device)
#define CDC_EP_NOTIFY    0x82  // Notification endpoint (not used)

// DMA configuration options
// Needed for Drivers/cdc.c
#define CDC_UART_TX_DMA_CHANNEL     0
#define CDC_UART_RX_DMA_CHANNEL     1
#define CDC_TX_DMA_SIGNAL           DMAREQ_USART1_TXBL
#define CDC_RX_DMA_SIGNAL           DMAREQ_USART1_RXDATAV

// USART configuration options
// Needed for Drivers/cdc.c
#define CDC_UART                    USART1
#define CDC_UART_CLOCK              cmuClock_USART1
#define CDC_UART_ROUTE              (USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0)
#define CDC_UART_TX_PORT            gpioPortC
#define CDC_UART_TX_PIN             0
#define CDC_UART_RX_PORT            gpioPortC
#define CDC_UART_RX_PIN             1

// This define is used in Drivers/cdc.c, but it is left as an empty define since
// we are using the STK (starter kit) instead of the DK (development kit)
#define CDC_ENABLE_DK_UART_SWITCH()

#ifdef __cplusplus
}
#endif

#endif // __USBCONFIG_H

