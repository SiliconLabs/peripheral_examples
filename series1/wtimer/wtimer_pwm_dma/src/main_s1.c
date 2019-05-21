/**************************************************************************//**
 * @main_series1.c
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * TIMER module. The GPIO pin specified in the readme.txt is configured to output
 * a 1kHz signal. The DMA continuously updates the CCVB register to vary the
 * duty cycle.
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
#include "em_gpio.h"
#include "em_timer.h"
#include "em_ldma.h"

// Note: change this to set the desired Output frequency in Hz
#define PWM_FREQ 1000

// Buffer size
#define BUFFER_SIZE 10

// Note: change this to change the duty cycles used in this example
static const uint16_t dutyCyclePercentages[BUFFER_SIZE] =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

// Buffer of duty cycle values for DMA transfer to CCVB
// Buffer is populated after TIMER is initialized and Top value is set
static uint16_t buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC10 as output
  GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for output compare
  // Use PWM mode, which sets output on overflow and clears on compare events
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(WTIMER0, 0, &timerCCInit);

  // Set route to Location 30 and enable
  // WTIM0_CC0 #30 is PC10
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set top value to overflow at the desired PWM_FREQ frequency
  TIMER_TopSet(WTIMER0, CMU_ClockFreqGet(cmuClock_WTIMER0) / PWM_FREQ);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(WTIMER0, &timerInit);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief
 *    Populate buffer with timer duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (uint16_t) (TIMER_TopGet(WTIMER0) * dutyCyclePercentages[i] / 100);
  }
}

/**************************************************************************//**
* @brief
*    Initialize the LDMA module
*
* @details
*    Configure the channel descriptor to use the default peripheral to
*    memory transfer descriptor. Modify it to not generate an interrupt
*    upon transfer completion (we don't need it). Additionally, the transfer
*    configuration selects the WTIMER0_CC0 signal as the trigger for the LDMA
*    transaction to occur.
*
* @note
*    The descriptor object needs to at least have static scope persistence so
*    that the reference to the object is valid beyond its first use in
*    initialization
*****************************************************************************/
void initLdma(void)
{
  // Channel descriptor configuration
  static LDMA_Descriptor_t descriptor =
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&buffer,            // Memory source address
                                    &WTIMER0->CC[0].CCVB, // Peripheral destination address
                                    BUFFER_SIZE,         // Number of bytes per transfer
                                    0);                  // Link to same descriptor
  descriptor.xfer.size     = ldmaCtrlSizeHalf; // Unit transfer size
  descriptor.xfer.doneIfs  = 0;                // Don't trigger interrupt when done

  // Transfer configuration and trigger selection
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_WTIMER0_CC0);

  // LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Start the transfer
  uint32_t channelNum = 0;
  LDMA_StartTransfer(channelNum, &transferConfig, &descriptor);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initTimer();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initLdma();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

