/**************************************************************************//**
 * @main_pg12.c
 * @brief This project outputs 2 microamps to the pin specified in the
 * readme.txt file. This project operates in EM3.
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_idac.h"

/**************************************************************************//**
 * @brief 
 *    IDAC initialization
 *****************************************************************************/
void initIdac(void)
{
  // Enable IDAC clock
  CMU_ClockEnable(cmuClock_IDAC0, true);

  // Initialize IDAC
  IDAC_Init_TypeDef init = IDAC_INIT_DEFAULT;
  init.outMode = idacOutputAPORT1XCH24; // Choose output to be on PB8
  IDAC_Init(IDAC0, &init);

  // Choose the output current to be 2 microamps
  IDAC_RangeSet(IDAC0, idacCurrentRange1);
  IDAC_StepSet(IDAC0, 4);

  // Enable IDAC output mode and also enable the IDAC module itself
  IDAC_OutEnable(IDAC0, true);
  IDAC_Enable(IDAC0, true);
}

/**************************************************************************//**
 * @brief  
 *    Output 2 microamps on the pin specified in the readme.txt
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();
  
  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initIdac();

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

