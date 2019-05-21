/**************************************************************************//**
 * @file main_tg11.c
 * @brief This project uses the VDAC and TIMER0 to output a 32 point sine wave
 * at a particular frequency (10 kHz by default). This project operates in
 * EM1.
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

// Note: change this to determine the frequency of the sine wave
#define WAVEFORM_FREQ 10000

// The timer needs to run at SINE_TABLE_SIZE times faster than the desired
// waveform frequency because it needs to output SINE_TABLE_SIZE points in that
// same amount of time
#define TIMER0_FREQ (WAVEFORM_FREQ * SINE_TABLE_SIZE)

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *****************************************************************************/
void initVdac(void)
{
  // Enable the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Initialize the VDAC
  VDAC_Init_TypeDef init = VDAC_INIT_DEFAULT;
  init.prescaler = VDAC_PrescaleCalc(1000000, false, 0); // Use a 1 MHz VDAC
  init.reference = vdacRef1V25Ln; // Set reference to internal 1.25V low noise
  VDAC_Init(VDAC0, &init);

  // Initialize VDAC channel 0
  VDAC_InitChannel_TypeDef initChannel0 = VDAC_INITCHANNEL_DEFAULT;
  VDAC_InitChannel(VDAC0, &initChannel0, 0);

  // Set the settle time to zero for maximum update rate (mask it out)
  VDAC0->OPA[0].TIMER &= ~(_VDAC_OPA_TIMER_SETTLETIME_MASK);

  // Enable alternate output pin and disable main output pin because we're not using it
  VDAC0->OPA[0].OUT |= VDAC_OPA_OUT_ALTOUTEN | VDAC_OPA_OUT_ALTOUTPADEN_OUT0;
  VDAC0->OPA[0].OUT &= ~(VDAC_OPA_OUT_MAINOUTEN);

  // Enable VDAC channel 0
  VDAC_Enable(VDAC0, 0, true);
}

/**************************************************************************//**
 * @brief
 *    Timer initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize TIMER0
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.enable = false;
  TIMER_Init(TIMER0, &init);

  // Set top (reload) value for the timer
  // Note: the timer runs off of the HFPER clock
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER) / TIMER0_FREQ;
  TIMER_TopBufSet(TIMER0, topValue);

  // Automatically clear the DMA request
  TIMER0->CTRL |= TIMER_CTRL_DMACLRACT;

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
                                     &VDAC0->CH0DATA, // Peripheral destination address
                                     SINE_TABLE_SIZE, // Number of halfwords per transfer
                                     0);              // Link to same descriptor
  loopDescriptor.xfer.doneIfs = 0;             // Don't trigger interrupt when transfer is done
  loopDescriptor.xfer.size = ldmaCtrlSizeHalf; // Transfer halfwords (VDAC data register is 12 bits)

  // Transfer configuration and trigger selection
  // Trigger on TIMER0 overflow and set loop count to size of the sine table minus one
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
 *    Ouput a sine wave to DAC channel 0
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
  initLdma();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
