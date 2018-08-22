/**************************************************************************//**
 * @file
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * WTIMER module. PC10 (See readme for Exp. pin outs) is configured for output and
 * outputs a 1kHz signal. The LDMA continuously updates the CCVB register to
 * vary the duty cycle.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "em_gpio.h"
#include "em_timer.h"
#include "em_ldma.h"
#include "bsp.h"

// Output frequency in Hz
#define PWM_FREQ 1000

#define BUFFERSIZE 10

// Buffer of duty cycle values for DMA transfer to CCVB
// Buffer is populated after WTIMER is initialized and Top value is set
int buffer[BUFFERSIZE];

// Globally declared LDMA link descriptor
LDMA_Descriptor_t descLink;

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC10 (See readme for Exp. pin outs) as output
  GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief WTIMER initialization
 *****************************************************************************/
void initWTIMER(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef wtimerCCInit = TIMER_INITCC_DEFAULT;
  // use PWM mode, which sets output on overflow and clears on compare events
  wtimerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Set route to Location 15 and enable
  // WTIM0_CC0 #30 is PC10 (See readme for Exp. pin outs)
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set top value to overflow once per signal period
  TIMER_TopSet(WTIMER0, CMU_ClockFreqGet(cmuClock_WTIMER0) / PWM_FREQ);

  // Initialize and start wtimer with no prescaling
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = timerPrescale1;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Safely enable WTIMER0 CC0 interrupt flag
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(WTIMER0_IRQn);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief populate buffer of duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  // 100% duty cycle equals the set WTIMER0 top value
  int max = TIMER_TopGet(WTIMER0);

  for (int i = 0; i < BUFFERSIZE; i++)
  {
    // Buffer increments from 0 to 100% duty cycle
    buffer[i] =  (i * max) / (BUFFERSIZE - 1);
  }
}

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void initLDMA(void)
{
  // Default LDMA init
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Configure LDMA for transfer from memory to WTIMER0 CC0
  // LDMA will loop continuously
  LDMA_TransferCfg_t periTransferTx =
          LDMA_TRANSFER_CFG_PERIPHERAL_LOOP(ldmaPeripheralSignal_WTIMER0_CC0, 1);

  // Get default descriptor for looped transfer to CCVB
  LDMA_Descriptor_t xfer = LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&buffer, &WTIMER0->CC[0].CCVB, BUFFERSIZE, 0);

  // Store descriptor globally
  descLink = xfer;
  // Transfer one word per unit
  descLink.xfer.size = ldmaCtrlSizeWord;
  // No interrupt after transfer complete
  descLink.xfer.doneIfs = 0;
  // Do not ignore single requests
  // IE: When Timer CC0 event occurs, transfer 1 unit then wait for next compare
  descLink.xfer.ignoreSrec = 0;

  // Start transfer, LDMA will continuously update the CCVB on each compare event
  LDMA_StartTransfer(0, (void*)&periTransferTx, (void*)&descLink);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();
  initWTIMER();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initLDMA();

  while (1)
  {
    EMU_EnterEM1();
  }
}
