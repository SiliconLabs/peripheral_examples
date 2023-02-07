/***************************************************************************//**
 * @file main_vdac_timer_dma_waveform_xg25.c
 * @brief This project uses the VDAC and TIMER0 to output a 32 point sine wave
 * at a particular frequency (10 kHz by default). This project operates in EM1.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_emu.h"
#include "em_gpio.h"
#include "em_vdac.h"
#include "em_timer.h"
#include "em_ldma.h"

// 32 point sine table
#define SINE_TABLE_SIZE 32
static const uint16_t sineTable[SINE_TABLE_SIZE] = {
  2048 , 2447 , 2831 , 3185 , 3495 , 3750 , 3939 , 4056 ,
  4095 , 4056 , 3939 , 3750 , 3495 , 3185 , 2831 , 2447 ,
  2048 , 1648 , 1264 , 910  , 600  , 345  , 156  , 39   ,
  0    , 39   , 156  , 345  , 600  , 910  , 1264 , 1648 ,
};

// Note: change this to change which channel the VDAC outputs to. This value can
// be either a zero or one
#define CHANNEL_NUM 0

// Set the VDAC to max frequency of 1 MHz
#define CLK_VDAC_FREQ              1000000

// Note: change this to determine the frequency of the sine wave
#define WAVEFORM_FREQ              10000

// The timer needs to run at SINE_TABLE_SIZE times faster than the desired
// waveform frequency because it needs to output SINE_TABLE_SIZE points in that
// same amount of time
#define TIMER0_FREQ                (WAVEFORM_FREQ * SINE_TABLE_SIZE)

/*
 * The port and pin for the VDAC output is set in VDAC_OUTCTRL register. The
 * ABUSPORTSELCHx fields in this register are defined as follows:
 * 0  No GPIO selected for CHx ABUS output
 * 1  Port A selected
 * 2  Port B selected
 * 3  Port C selected
 * 4  Port D selected
 *
 * The VDAC port pin settings do not need to be set when the main output is
 * used. Refer to the device Reference Manual and Datasheet for more details. We
 * will be selecting the CH0 auxiliary output PA06 for this example.
 */

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable the GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Disable PA06 input
  GPIO_PinModeSet(gpioPortA, 6, gpioModeDisabled, 0);

  // Connect VDAC0 CH0 to an even pin on Port A via the ABUS
  GPIO->ABUSALLOC = GPIO_ABUSALLOC_AEVEN0_VDAC0CH0;
}

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *****************************************************************************/
void initVdac(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // The EM01GRPACLK is chosen as VDAC clock source since the VDAC will be
  // operating in EM1
  CMU_ClockSelectSet(cmuClock_VDAC0, cmuSelect_EM01GRPACLK);

  // Enable the VDAC clocks
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock.
  init.prescaler = VDAC_PrescaleCalc(VDAC0, (uint32_t)CLK_VDAC_FREQ);

  // Set reference to internal 1.25V low noise reference
  init.reference = vdacRef1V25;

  // Since the minimum load requirement for high capacitance mode is 25 nF, turn
  // this mode off
  initChannel.highCapLoadEnable = false;

  // Disable Main output
  initChannel.mainOutEnable = false;

  // Enable Auxiliary output
  initChannel.auxOutEnable = true;

  // Output to PA06
  initChannel.port = vdacChPortA;
  initChannel.pin = 6;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, CHANNEL_NUM);

  // Enable the VDAC
  VDAC_Enable(VDAC0, CHANNEL_NUM, true);
}

/**************************************************************************//**
 * @brief
 *    Timer initialization
 *****************************************************************************/
void initTimer(void)
{
  uint32_t timerFreq, topValue;

  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize TIMER0
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.dmaClrAct = true;
  init.enable = false;
  TIMER_Init(TIMER0, &init);

  // Set top (reload) value for the timer
  // Note: the timer runs off of the EM01GRPACLK clock
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (init.prescale + 1);
  topValue = (timerFreq / TIMER0_FREQ);

  // Set top value to overflow at the desired TIMER0_FREQ frequency
  TIMER_TopSet(TIMER0, topValue);

  // Enable TIMER0
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize the LDMA module
 *
 * @details
 *    Configure the channel descriptor to use the default memory to
 *    peripheral transfer descriptor. Modify it to not generate an interrupt
 *    upon transfer completion (we don't need it for this example).
 *    Also make the descriptor link to itself so that the descriptor runs
 *    continuously. Additionally, the transfer configuration selects the
 *    TIMER0 overflow signal as the trigger for the DMA transaction to occur.
 *
 * @note
 *    The descriptor object needs to at least have static scope persistence so
 *    that the reference to the object is valid beyond its first use in
 *    initialization. This is because this code loops back to the same
 *    descriptor after every dma transfer. If the reference isn't valid anymore,
 *    then all dma transfers after the first one will fail.
 ******************************************************************************/
void initLdma(void)
{
  // Descriptor loops through the sine table and outputs its values to the VDAC
  static LDMA_Descriptor_t loopDescriptor =
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&sineTable[0],   // Memory source address
                                     &VDAC0->CH0F,    // Peripheral destination address
                                     SINE_TABLE_SIZE, // Number of halfwords per transfer
                                     0);              // Link to same descriptor

  // Don't trigger interrupt when transfer is done
  loopDescriptor.xfer.doneIfs = 0;

  // Transfer halfwords (VDAC data register is 12 bits)
  loopDescriptor.xfer.size = ldmaCtrlSizeHalf;

  // Transfer configuration and trigger selection
  // Trigger on TIMER0 overflow and set loop count to size of the sine table
  // minus one
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER0_UFOF);

  // LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Start the transfer
  uint32_t channelNum = 0;
  LDMA_StartTransfer(channelNum, &transferConfig, &loopDescriptor);
}

/**************************************************************************//**
 * @brief
 *    Output a sine wave to DAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  // Enable DC-DC converter
  EMU_DCDCInit(&dcdcInit);

  // Initialize the GPIO, VDAC, LDMA and Timer
  initGpio();
  initVdac();
  initLdma();
  initTimer();

  while (1) {
    // Enter EM1 (won't exit)
    EMU_EnterEM1();
  }
}

