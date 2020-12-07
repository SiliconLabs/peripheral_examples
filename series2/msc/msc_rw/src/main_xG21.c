/***************************************************************************//**
 * @file main_xG21.c
 * @brief This project demonstrates the use of the userdata page of the flash
 * memory. The value 32 is stored in the 4th byte of the page. A variable is
 * then set to the value stored at this location.
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

#include "sl_se_manager_util.h"

#define USERDATA ((uint32_t*)USERDATA_BASE)

uint32_t Cleared_value;
uint32_t Set_value;

sl_se_command_context_t cmd_ctx;

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Declare the value to be stored in flash
  uint32_t value = 32;

  // Clear the UserData page of any previous data stored
  sl_se_erase_user_data(&cmd_ctx);

  // Read the initial value in the cleared page
  Cleared_value = USERDATA[3];

  // Write the value into the 4th word of the Userdata portion of the flash
  sl_se_write_user_data(&cmd_ctx, 3*4, &value, 4);

  // Read the written data from the flash location it was stored in
  Set_value = USERDATA[3];

  // Infinite Loop
  while(1);
}
