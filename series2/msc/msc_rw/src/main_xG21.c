/**************************************************************************//**
 * @file
 * @brief This project demonstrates the use of the userdata page of the
 * flash memory. The value 32 is stored in the 4th byte of the page. A
 * variable is then set to the value stored at this location
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
 
#include "em_device.h"
#include "em_chip.h"
#include "em_se.h"

#define USERDATA ((uint8_t*)USERDATA_BASE)

uint32_t Cleared_value;
uint32_t Set_value;

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
  SE_eraseUserData();

  // Read the initial value in the cleared page
  Cleared_value = USERDATA[4];

  // Write the value into the first location of the UserData portion of the flash
  SE_writeUserData(4, &value, 4);

  // Read the written data from the flash location it was stored in
  Set_value = USERDATA[4];

  // Infinite Loop
  while(1);
}
