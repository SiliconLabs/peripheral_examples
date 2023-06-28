/***************************************************************************//**
 * @file main.c
 * @brief Example illustrating how to lock flash pages and the where to do so at
 * different times after program execution begins depending on application
 * requirements.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_msc.h"

// Include the BSP header file here for board GPIO definitions
#include "bsp.h"

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  MSC_Status_TypeDef flashStatus;

  // Chip errata
  CHIP_Init();

  // Lock the user data page
  MSC->MISCLOCKWORD_SET = MSC_MISCLOCKWORD_UDLOCKBIT;

  /*
   * Drive the LED0 pin low so that the timing of the write to the
   * lock word is visible on a scope.  This GPIO pin has already been
   * initialized in the custom system file.
   */
  GPIO->P_CLR[BSP_GPIO_LED0_PORT].DOUT = 1 << BSP_GPIO_LED0_PIN;

  // Initialize the MSC to prepare for flash write/erase operations
  MSC_Init();

  // Attempt to erase the last page of main flash
  flashStatus = MSC_ErasePage((uint32_t*)(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE));

  /*
   * Halt on any flash operation status other than mscReturnLocked.
   * The expectation is that the erase should fail because the page
   * in question was locked in the custom system file.
   */
  switch (flashStatus)
  {
    case mscReturnOk:
    case mscReturnInvalidAddr:
    case mscReturnTimeOut:
    case mscReturnUnaligned:    __BKPT(0);
         break;
    case mscReturnLocked:
         break;
  }

  // Attempt to erase the user data page
  flashStatus = MSC_ErasePage((uint32_t*)USERDATA_BASE);

  /*
   * Halt on any flash operation status other than mscReturnLocked.
   * The expectation is that the erase should fail because the user
   * data page was locked by the write to MSC_MISCLOCKWORD above.
   */
  switch (flashStatus)
  {
    case mscReturnOk:
    case mscReturnInvalidAddr:
    case mscReturnTimeOut:
    case mscReturnUnaligned:    __BKPT(1);
         break;
    case mscReturnLocked:
         break;
  }

  // If both pages are locked, code will loop here
  while(1);
}
