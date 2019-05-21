/**************************************************************************//**
 * @file main_pg12.c
 * @brief This project shows how to configure the LEUART for half-duplex
 * communication. See the readme.txt for details.
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

// Note: Enable (uncomment) one of these defines to choose whether the board
// should start off transmitting or receiving
#define INITIAL_TRANSMITTER
// #define INITIAL_RECEIVER

#define RX_BUFFER_SIZE 80  // Software receive buffer size

static uint32_t isDataReceived = 0;   // Flag indicating whether LEUART0's receiver has data
static char volatile rxBuffer[RX_BUFFER_SIZE]; // Software receive buffer
static char* txMsg;                   // Pointer to a message to transmit

/**************************************************************************//**
 * @brief
 *    Disable the transmitter and enable the receiver for the specified LEUART
 *
 * @param [in] leuart
 *    Pointer to LEUART peripheral register block
 *****************************************************************************/
void switchToRxMode(LEUART_TypeDef *leuart)
{
  leuart->CMD = (LEUART_CMD_TXDIS | LEUART_CMD_RXEN);
}

/**************************************************************************//**
 * @brief
 *    Disable the receiver and enable the transmitter for the specified LEUART
 *
 * @param [in] leuart
 *    Pointer to LEUART peripheral register block
 *****************************************************************************/
void switchToTxMode(LEUART_TypeDef *leuart)
{
  leuart->CMD = (LEUART_CMD_RXDIS | LEUART_CMD_TXEN);
}

/**************************************************************************//**
 * @brief
 *    Get the message to send
 *
 * @param [in] leuart
 *    Pointer to LEUART peripheral register block
 *****************************************************************************/
char* getTxSuccessMsg()
{
  #if defined INITIAL_TRANSMITTER
    static char txSuccessMsg[] = "Initial Transmitter: Receive success and transmitting now\n";
  #elif defined INITIAL_RECEIVER
    static char txSuccessMsg[] = "Initial Receiver: Receive success and transmitting now\n";
  #endif
    return txSuccessMsg;
}

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
 *
 * @notes
 *    The LEUART transmitter and receivers need to be disabled first because
 *    the LEUART_CTRL_AUTOTRI mode tristates the TX pin while it is inactive.
 *    Therefore, enabling interrupts has the possibility of immediately
 *    triggering an interrupt if the floating value is seen as active.
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
  init.enable = leuartDisable;
  LEUART_Init(LEUART0, &init);

  // Enable LEUART0 RX/TX pins on PD[11:10] (see readme.txt for details)
  LEUART0->ROUTEPEN  = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
  LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;

  // Loopback and auto tristate TX when idle
  LEUART0->CTRL |= (LEUART_CTRL_LOOPBK | LEUART_CTRL_AUTOTRI);

  // Enable LEUART0 RX/TX interrupts
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
  NVIC_EnableIRQ(LEUART0_IRQn);

  // Set the transmission mode
  #if defined INITIAL_TRANSMITTER
    switchToTxMode(LEUART0);
  #elif defined INITIAL_RECEIVER
    switchToRxMode(LEUART0);
  #endif
}

/**************************************************************************//**
 * @brief
 *    LEUART0 interrupt service routine
 *
 * @details
 *    Keep receiving data while there is still data left in the hardware RX buffer.
 *    Store incoming data into rxBuffer and set isDataReceived when a linefeed
 *    '\n' is sent or if there is no more room in the buffer. Whenever LEUART0
 *    finishes transmitting, it will switch to receive mode and vice-versa.
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

  // RX portion of the interrupt handler.
  if (flags & LEUART_IF_RXDATAV) {
    while (LEUART0->STATUS & LEUART_STATUS_RXDATAV) { // While there is still incoming data
      char data = LEUART_Rx(LEUART0);
      if ((rxIndex < RX_BUFFER_SIZE - 2) && (data != '\n')) { // Save two spots for '\n' and '\0'
        rxBuffer[rxIndex++] = data;
      } else {
        rxBuffer[rxIndex++] = '\n';
        rxBuffer[rxIndex] = '\0';
        isDataReceived = 1;
        rxIndex = 0;
        switchToTxMode(LEUART0); // Done receiving so start transmitting
        break;
      }
    }
  }

  // TX portion of the interrupt handler.
  if (flags & LEUART_IF_TXC) {
    if ((txIndex < RX_BUFFER_SIZE) && (txMsg[txIndex] != '\0')) {
      LEUART_Tx(LEUART0, txMsg[txIndex++]); // Send the data
    } else {
      txIndex = 0;
      switchToRxMode(LEUART0); // Done transmitting so start receiving
    }
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    Initialize the GPIO and LEUART modules. After the RX handler tells us that
 *    we have data, send a message back saying that the receive was a success.
 *    Finally, send the data to the other board and go into EM2 until an RX or
 *    TX interrupt wakes up the CPU.
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

  // Start transmitting the welcome message
  #ifdef INITIAL_TRANSMITTER
    txMsg = "LEUART half duplex code example\n";
    LEUART_IntSet(LEUART0, LEUART_IFS_TXC); // We manually set the TXC flag to start the transfer
  #endif

  while (1)
  {
    // If notified by the LEUART0/1 RX handler, send a success message
    if (isDataReceived) {
      txMsg = getTxSuccessMsg();
      isDataReceived = 0;
      LEUART_IntSet(LEUART0, LEUART_IFS_TXC);
    }

    // Wait for incoming data in EM2 to save energy
    EMU_EnterEM2(false);
  }
}

