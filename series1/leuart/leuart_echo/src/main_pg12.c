/**************************************************************************//**
 * @main_pg12.c
 * @brief This project reads input on the LEUART RX port and echoes it back to
 * the TX port one line at a time. It idles in EM2. See readme.txt for
 * details.
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
#include "em_gpio.h"
#include "em_leuart.h"
#include "em_chip.h"

#define RX_BUFFER_SIZE 80             // Software receive buffer size

static uint32_t rxDataReady = 0;      // Flag indicating receiver does not have data
static volatile char rxBuffer[RX_BUFFER_SIZE]; // Software receive buffer
static char txBuffer[RX_BUFFER_SIZE]; // Software transmit buffer

/**************************************************************************//**
 * @brief
 *    Initialize the GPIO pins for the LEUART module
 *****************************************************************************/
void initGpio(void)
{
  // GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LEUART0 TX and RX pins
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);    // RX
}

/**************************************************************************//**
 * @brief
 *    Initialize the LEUART module
 *****************************************************************************/
void initLeuart(void)
{
  // Enable LE (low energy) clocks
  CMU_ClockEnable(cmuClock_HFLE, true); // Necessary for accessing LE modules
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO); // Set a reference clock

  // Enable clocks for LEUART0
  CMU_ClockEnable(cmuClock_LEUART0, true);
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1); // Don't prescale LEUART clock

  // Initialize the LEUART0 module
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  LEUART_Init(LEUART0, &init);

  // Enable LEUART0 RX/TX pins on PD[11:10] (see readme.txt for details)
  LEUART0->ROUTEPEN  = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
  LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;

  // Enable LEUART0 RX/TX interrupts
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
  NVIC_EnableIRQ(LEUART0_IRQn);
}

/**************************************************************************//**
 * @brief
 *    LEUART0 interrupt service routine
 *
 * @details
 *    Keep receiving data while there is still data left in the hardware RX buffer.
 *    Store incoming data into rxBuffer and set rxDataReady when a linefeed '\n' is
 *    sent or if there is no more room in the buffer.
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
  // Note: These are static because the handler will exit/enter
  //       multiple times to fully transmit a message.
  static uint32_t rxIndex = 0;
  static uint32_t txIndex = 0;

  // Acknowledge the interrupt
  uint32_t flags = LEUART_IntGet(LEUART0);
  LEUART_IntClear(LEUART0, flags);

  // RX portion of the interrupt handler
  if (flags & LEUART_IF_RXDATAV) {
    while (LEUART0->STATUS & LEUART_STATUS_RXDATAV) { // While there is still incoming data
      char data = LEUART_Rx(LEUART0);
      if ((rxIndex < RX_BUFFER_SIZE - 2) && (data != '\n')) { // Save two spots for '\n' and '\0'
        rxBuffer[rxIndex++] = data;
      } else { // Done receiving
        rxBuffer[rxIndex++] = '\n';
        rxBuffer[rxIndex] = '\0';
        rxDataReady = 1;
        rxIndex = 0;
        break;
      }
    }
  }

  // TX portion of the interrupt handler
  if (flags & LEUART_IF_TXC) {
    if ((txIndex < RX_BUFFER_SIZE) && (txBuffer[txIndex] != '\0')) {
      LEUART_Tx(LEUART0, txBuffer[txIndex++]); // Send the data
    } else { // Done transmitting
      txIndex = 0;
    }
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    Initialize the GPIO and LEUART modules. Send a welcome message over the TX
 *    pin. After the RX handler tells us that we have data, start
 *    processing the received data. Start by disabling RX and TX interrupts so
 *    that the data we received in rxBuffer does not get overwritten by one of
 *    the handlers before we read it. Copy the data in rxBuffer into the
 *    txBuffer and then trigger a TX interrupt to echo it out over the TX pin.
 *    If there is no data availabe, go into EM2 until an RX or TX interrupt
 *    wakes up the CPU.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  initLeuart();

  // Print the welcome message
  char welcomeString[] = "LEUART echo code example\r\n";
  for (uint32_t i = 0 ; welcomeString[i] != 0; i++) {
    txBuffer[i] = welcomeString[i];
  }
  LEUART_IntSet(LEUART0, LEUART_IFS_TXC);

  uint32_t i;
  while (1) {

    // When notified by the RX handler, start processing the received data
    if (rxDataReady) {
      LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Disable interrupts
      for (i = 0; rxBuffer[i] != 0; i++) {
        txBuffer[i] = rxBuffer[i]; // Copy rxBuffer into txBuffer
      }
      txBuffer[i] = '\0';
      rxDataReady = 0; // Indicate that we need new data
      LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Re-enable interrupts
      LEUART_IntSet(LEUART0, LEUART_IFS_TXC);
    }

    // Wait for incoming data in EM2 to save energy
    EMU_EnterEM2(false);
  }
}

