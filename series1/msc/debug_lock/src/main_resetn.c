/**************************************************************************//**
 * @file
 * @brief This project demonstrates the device's ability to lock the debug
 * interface by clearing bits in the lock bits page. See the readme.txt for
 * more information.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_msc.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void) 
{
  // Chip errata
  CHIP_Init();

  // The DLW is the 127th word of the Lockexbits Page
  uint32_t *DLW_ADDR = (((uint32_t *)LOCKBITS_BASE) + 127);

  // If the debug interface is unlocked, lock it and reset
  if ((*DLW_ADDR & 0xF) == 0xF)
  {
    // Clear the DLW lock bits to lock the device
    uint32_t lockClear = 0xFFFFFFF0;
    MSC_Init();
    MSC_WriteWord(DLW_ADDR, &lockClear, 4);
    MSC_Deinit();
	
    // The debug lock will still not take effect until a pin or power on reset is issued
  }

  while (1);
}
