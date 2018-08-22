/**************************************************************************//**
 * @file main_tg11.c
 * @brief This project uses the DAC and the internal sine wave generator to
 * produce a 16-point sine wave at a frequency of f_sinewave Hz centered at the
 * reference voltage divided by 2. This project operates in EM1. By default,
 * this program outputs a sinewave at 31,250 Hz.
 * f_sinewave = f_HFPERCLK / (32 * (PRESCALE + 1))
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
#include "em_emu.h"
#include "em_vdac.h"
#include "em_prs.h"

// Note: change this to change the frequency of the sinewave
// Note: PRESCALE must be greater than or equal to 18 because the DAC_CLK can only be programmed to a maximum of 1 MHz.
// Note: PRESCALE must also be less than or equal to 127 because the PRESCALE is a 7 bit unsigned integer
// f_DAC_CLK = f_IN_CLK / (PRESCALE + 1), where f_IN_CLK in this example is the HFPERCLK (19 MHz)
// f_sinewave = f_HFPERCLK / (32 * (PRESCALE + 1))
#define PRESCALE 18

/**************************************************************************//**
 * @brief
 *    PRS initialization
 *
 * @details
 *    PRS channel inputting to DAC channel 0 controls the sinewave enable.
 *    PRS channel inputting to DAC channel 1 controls the output enable.
 *    This example maps PRS channel 0 to DAC channel 0 and PRS channel 1 to DAC
 *    channel 1. Setting PRS channel 0 and 1 high both enables the sinewave and
 *    enables DAC output.
 *
 * @note
 *    The channels chosen when setting their levels must match the channels that
 *    were mapped to the DAC channels chosen in initDac().
 *****************************************************************************/
void initPrs(void)
{
  // Enable the PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Set the level for PRS channel 0 and 1 to be high
  PRS_LevelSet(PRS_SWLEVEL_CH0LEVEL | PRS_SWLEVEL_CH1LEVEL,
               _PRS_SWLEVEL_CH0LEVEL_MASK | _PRS_SWLEVEL_CH1LEVEL_MASK);
}

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *
 * @details
 *    This function uses the alternate VDAC output pin because the main output 
 *    pin was not brought out to an expansion header pin or a breakout pad.
 *****************************************************************************/
void initVdac(void)
{
  // Enable the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Initialize the VDAC
  VDAC_Init_TypeDef init = VDAC_INIT_DEFAULT;
  init.asyncClockMode    = false;         // Clock source for synchronous mode is HFPERCLK
  init.prescaler         = PRESCALE;      // Set the prescaler
  init.reference         = vdacRef1V25Ln; // Set reference to internal 1.25V low noise
  init.sineEnable        = true;          // Enable sine wave generation mode
  init.outEnablePRS      = true;          // Enable PRS control of output driver
  VDAC_Init(VDAC0, &init);

  // Initialize VDAC channel 0 and enable it
  VDAC_InitChannel_TypeDef initChannel0 = VDAC_INITCHANNEL_DEFAULT;
  initChannel0.prsSel   = vdacPrsSelCh0;   // Select PRS channel 0 for DAC channel 0
  initChannel0.prsAsync = false;           // Use synchronous mode
  initChannel0.trigMode = vdacTrigModePrs; // Select trigger as PRS
  VDAC_InitChannel(VDAC0, &initChannel0, 0);
  
  // Initialize VDAC channel 1 and enable it
  VDAC_InitChannel_TypeDef initChannel1 = VDAC_INITCHANNEL_DEFAULT;
  initChannel1.prsSel   = vdacPrsSelCh1;   // Select PRS channel 1 for DAC channel 1
  initChannel1.prsAsync = false;           // Use synchronous mode
  initChannel1.trigMode = vdacTrigModePrs; // Select trigger as PRS
  VDAC_InitChannel(VDAC0, &initChannel1, 1);
  
  // Set the settle time to zero for maximum update rate (mask it out)
  VDAC0->OPA[0].TIMER &= ~(_VDAC_OPA_TIMER_SETTLETIME_MASK);
  
  // Enable alternate output pin and disable main output pin because we're not using it
  VDAC0->OPA[0].OUT |= VDAC_OPA_OUT_ALTOUTEN | VDAC_OPA_OUT_ALTOUTPADEN_OUT0;
  VDAC0->OPA[0].OUT &= ~(VDAC_OPA_OUT_MAINOUTEN);
  
  // Enable VDAC channels 0 and 1
  VDAC_Enable(VDAC0, 0, true);
  VDAC_Enable(VDAC0, 1, true);
}

/**************************************************************************//**
 * @brief
 *    Output a sine wave to DAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initPrs();
  initVdac();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

