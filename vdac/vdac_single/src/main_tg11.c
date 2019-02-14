/**************************************************************************//**
 * @file main_tg11.c
 * @brief This project uses the VDAC in sample/off mode to output 0.5V to a pin
 * in EM3. See readme.txt for details.
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

// Note: change this to change which channel the VDAC outputs to. This value can
// be either a zero or one
#define CHANNEL_NUM 0

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *
 * @details
 *    In order to operate in EM2/3 mode, the clock must be set to asynchronous
 *    mode. The prescaler is set because the maximum frequency for the VDAC
 *    clock is 1 MHz. Of course, if you'd like to lower the clock (increase the
 *    prescaler) that is fine too. This function also sets the alternate VDAC
 *    output pin because the main output pin was not brought out to an expansion
 *    header pin or a breakout pad.
 *****************************************************************************/
void initVdac(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Enable the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // VDAC clock source for asynchronous mode is 12 MHz internal VDAC oscillator
  init.asyncClockMode = true;

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock.
  init.prescaler = VDAC_PrescaleCalc(1000000, false, 0);

  // Set reference to internal 1.25V low noise reference
  init.reference = vdacRef1V25Ln;
  
  // Set the refresh period to every 8 clock cycles
  init.refresh = vdacRefresh8;

  // Set the output mode to sample/off
  initChannel.sampleOffMode = true;
  
  // Set the trigger mode to SWREFRESH so that the internal timer or a software
  // trigger will trigger the VDAC conversion
  initChannel.trigMode = vdacTrigModeSwRefresh;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, CHANNEL_NUM);

  // Enable alternate output pin
  VDAC0->OPA[0].OUT |= VDAC_OPA_OUT_ALTOUTEN | VDAC_OPA_OUT_ALTOUTPADEN_OUT0;
  
  // Disable main output pin because we're not using it
  VDAC0->OPA[0].OUT &= ~(VDAC_OPA_OUT_MAINOUTEN);

  // Enable the VDAC
  VDAC_Enable(VDAC0, CHANNEL_NUM, true);
}

/**************************************************************************//**
 * @brief
 *    Calculate the digital value that maps to the desired output voltage
 *
 * @note
 *    The vRef parameter must match the reference voltage selected during
 *    initialization
 *
 * @param [in] vOut
 *    Desired output voltage
 *
 * @param [in] vRef
 *    Reference voltage used by the VDAC
 *
 * @return
 *    The digital value that maps to the desired output voltage
 *****************************************************************************/
uint32_t getVdacValue(float vOut, float vRef)
{
  return (uint32_t)((vOut * 4095) / vRef);
}

/**************************************************************************//**
 * @brief
 *    Output 0.5 volts to VDAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();
  
  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initVdac();

  // Calculate the 12-bit output value for 0.5 V
  uint32_t vdacValue = getVdacValue(0.5, 1.25);

  // Write the output value to VDAC DATA register
  VDAC_ChannelOutputSet(VDAC0, CHANNEL_NUM, vdacValue);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 while the VDAC is doing conversions
  }
}

