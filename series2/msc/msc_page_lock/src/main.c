/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates the ability to lock flash pages using
 * MSC page lock register
 * @version 0.0.1
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
#include "em_msc.h"

// page lock word to lock page127
#define PAGE_LOCK_WORD3         0x80000000
// Address in page 127
#define FLASH_ADDRESS           0x000FFFFC
// Data to write to flash
uint8_t data[4] = {0xAA, 0xBB, 0xCC, 0xDD};

/**************************************************************************//**
 * @brief  Flash erase/write
 *****************************************************************************/
void testFLASH(void)
{
  // Initialize MSC module
  MSC_Init();
  
  // Clear any previous data stored in the flash page
  MSC_ErasePage((uint32_t*)FLASH_ADDRESS);
  
  // Write to the last location in the flash page 127
  MSC_WriteWord((uint32_t*)FLASH_ADDRESS,&data, 4);
  
  // Deinitialize MSC module
  MSC_Deinit();
}

/**************************************************************************//**
 * @brief	Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Write to flash
  testFLASH();

  // Lock flash page-127
  MSC->PAGELOCK3 |= PAGE_LOCK_WORD3;

  // Increment value of data
  for(int i=0; i<4; i++){
	data[i] += 0x11;
  }

  // Write to flash
  testFLASH();

  // Infinite loop
  while(1);
}
