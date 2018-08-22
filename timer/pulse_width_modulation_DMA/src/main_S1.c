/**************************************************************************//**
 * @file
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * TIMER module. PC10 (See readme for Exp. pin outs) is configured for output and
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
// Buffer is populated after TIMER is initialized and Top value is set
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
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // use PWM mode, which sets output on overflow and clears on compare events
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set route to Location 15 and enable
  // TIM0_CC0 #15 is PC10 (See readme for Exp. pin outs)
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC15;
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set top value to overflow once per signal period
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_TIMER0) / PWM_FREQ);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1;
  TIMER_Init(TIMER0, &timerInit);

  // Safely enable TIMER0 CC0 interrupt flag
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(TIMER0, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief populate buffer of duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  // 100% duty cycle equals the set TIMER0 top value
  int max = TIMER_TopGet(TIMER0);

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

  // Configure LDMA for transfer from memory to TIMER0 CC0
  // LDMA will loop continuously
  LDMA_TransferCfg_t periTransferTx =
          LDMA_TRANSFER_CFG_PERIPHERAL_LOOP(ldmaPeripheralSignal_TIMER0_CC0, 1);

  // Get default descriptor for looped transfer to CCVB
  LDMA_Descriptor_t xfer = LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&buffer, &TIMER0->CC[0].CCVB, BUFFERSIZE, 0);

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
  initTIMER();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initLDMA();

  while (1)
  {
    EMU_EnterEM1();
  }
}
