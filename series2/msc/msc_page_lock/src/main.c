/**************************************************************************//**
 * @file
 * @brief Example illustrating how to lock flash pages and the where
 * to do so at different times after program execution begins depending
 * on application requirements.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
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
