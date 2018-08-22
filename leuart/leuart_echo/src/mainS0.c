/**************************************************************************//**
 * @main.c
 * @brief This project reads input on the LEUART RX port and echoes it back to
 * the TX port one line at a time.  It idles in EM2  See * readme.txt for
 * details.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2017 Silicon Labs, Inc. http://www.silabs.com</b>
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

#define BUFFER_SIZE 80
volatile uint32_t rx_data_ready = 0;
volatile char rx_buffer[BUFFER_SIZE];
volatile char tx_buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief LEUART interrupt service routine
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
  uint32_t flags;
  static uint32_t rx_count = 0;
  static uint32_t tx_count = 0;
  flags = LEUART_IntGet(LEUART0);
  LEUART_IntClear(LEUART0, flags);
  if (flags & LEUART_IF_RXDATAV)
  {
    /* Store incoming data into rx_buffer, set rx_data_ready when a full
     * line has been received
     */
    while (LEUART0->STATUS & LEUART_STATUS_RXDATAV)
    {
      rx_buffer[rx_count++] = LEUART0->RXDATA;
      if (rx_buffer[rx_count-1] == '\r' || rx_buffer[rx_count-1] == '\f')
      {
        rx_data_ready = 1;
        rx_buffer[rx_count-1] = '\0'; // Overwrite CR or LF character
        rx_count = 0;
        break;
      }
      if ( rx_count >= BUFFER_SIZE - 3 ) // Leave room for CR, LF, and NULL
      {
        rx_data_ready = 1;
        rx_buffer[rx_count] = '\0'; // Do not overwrite last character
        rx_count = 0;
        break;
      }
    }
  }

  if (flags & LEUART_IF_TXC)
  {
    if (tx_count < BUFFER_SIZE && tx_buffer[tx_count] != '\0')
    {
      LEUART0->TXDATA = tx_buffer[tx_count++]; // Transmit byte
    }
    else
    {
      tx_count = 0; // No more data to send
    }
  }

}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  char welcome_string[] = "Silicon Labs LEUART Code example!\r\f";
  int i;

  // Chip errata
  CHIP_Init();

  // Enable oscillator to GPIO and USART0 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LEUART0, true);
  // Enable CORE LE clock in order to access LE modules
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Select LFXO for LEUARTs (and wait for it to stabilize)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, true);

  // Do not prescale clock
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

  // set pin modes for UART TX and RX pins
  GPIO_PinModeSet(gpioPortD, 5, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortD, 4, gpioModePushPull, 1);

  // Initialize LEUART and route pins
  LEUART_Init(LEUART0, &init);
  LEUART0->ROUTE  = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;
  LEUART_IntEnable(LEUART0, LEUART_IF_RXDATAV);
  LEUART_IntEnable(LEUART0, LEUART_IF_TXC);
  NVIC_EnableIRQ(LEUART0_IRQn);

  // Print welcome message
  for (i = 0 ; welcome_string[i] != 0; i++)
  {
    tx_buffer[i] = welcome_string[i];
  }
  LEUART_IntSet(LEUART0,LEUART_IF_TXC);

  while (1)
  {
    /* When notified by the RX handler, copy data from the RX buffer to the
     * TX buffer, and start the TX handler */
    if (rx_data_ready)
    {
      LEUART_IntDisable(LEUART0, LEUART_IF_RXDATAV);
      LEUART_IntDisable(LEUART0, LEUART_IF_TXC);
      for (i = 0; rx_buffer[i] != 0 && i < BUFFER_SIZE-3; i++)
      {
        tx_buffer[i] = rx_buffer[i];
      }
      tx_buffer[i++] = '\r';
      tx_buffer[i++] = '\f';
      tx_buffer[i] = '\0';
      rx_data_ready = 0;
      LEUART_IntEnable(LEUART0, LEUART_IF_RXDATAV);
      LEUART_IntEnable(LEUART0, LEUART_IF_TXC);
      LEUART_IntSet(LEUART0, LEUART_IF_TXC);
    }
    EMU_EnterEM2(false);
  }
}

