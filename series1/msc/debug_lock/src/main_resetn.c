/***************************************************************************//**
 * @file main_resetn.c
 * @brief This project demonstrates the device's ability to lock the debug
 * interface by clearing bits in the lock bits page. See the readme.txt for more
 * information.
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
