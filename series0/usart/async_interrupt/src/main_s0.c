/**************************************************************************//**
 * @main_s0.c
 * @brief This project reads input on the UART RX port and echoes it back to
 * the TX port one line at a time.  It does not use the VCOM or stdio.  See
 * readme.txt for details.
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
#include "em_gpio.h"
#include "em_usart.h"
#include "em_chip.h"

#define BUFFER_SIZE 80
volatile uint32_t rx_data_ready = 0;
volatile char rx_buffer[BUFFER_SIZE];
volatile char tx_buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief USART1 RX interrupt service routine
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
  static uint32_t i = 0;
  uint32_t flags;
  flags = USART_IntGet(USART1);
  USART_IntClear(USART1, flags);

  /* Store incoming data into rx_buffer, set rx_data_ready when a full
  * line has been received
  */
  rx_buffer[i++] = USART_Rx(USART1);

  if (rx_buffer[i - 1] == '\r' || rx_buffer[i - 1] == '\f')
  {
    rx_data_ready = 1;
    rx_buffer[i - 1] = '\0'; // Overwrite CR or LF character
    i = 0;
  }

  if ( i >= BUFFER_SIZE - 2 )
  {
    rx_data_ready = 1;
    rx_buffer[i] = '\0'; // Do not overwrite last character
    i = 0;
  }
}

/**************************************************************************//**
 * @brief USART1 TX interrupt service routine
 *****************************************************************************/
void USART1_TX_IRQHandler(void)
{
  static uint32_t i = 0;
  uint32_t flags;
  flags = USART_IntGet(USART1);
  USART_IntClear(USART1, flags);

  if (flags & USART_IF_TXC)
  {
    if (i < BUFFER_SIZE && tx_buffer[i] != '\0')
    {
    	USART_Tx(USART1, tx_buffer[i++]); // Transmit byte
    }
    else
    {
      i = 0; // No more data to send
    }
  }
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  char welcome_string[] = "Silicon Labs UART Code example!\r\f";
  uint32_t i;

  // Chip errata
  CHIP_Init();

  // Enable oscillator to GPIO and USART1 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // set pin modes for USART TX and RX pins
  GPIO_PinModeSet(gpioPortC, 1, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);

  // Initialize USART asynchronous mode and route pins
  USART_InitAsync(USART1, &init);
  USART1->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN;

  //Initializae USART Interrupts
  USART_IntEnable(USART1, USART_IEN_RXDATAV);
  USART_IntEnable(USART1, USART_IEN_TXC);

  //Enabling USART Interrupts
  NVIC_EnableIRQ(USART1_RX_IRQn);
  NVIC_EnableIRQ(USART1_TX_IRQn);

  // Print welcome message
  for (i = 0 ; welcome_string[i] != 0; i++)
  {
    tx_buffer[i] = welcome_string[i];
  }
  USART_IntSet(USART1, USART_IFS_TXC);

  while (1)
  {
    /* When notified by the RX handler, copy data from the RX buffer to the
     * TX buffer, and start the TX handler */
    if (rx_data_ready)
    {
      USART_IntDisable(USART1, USART_IEN_RXDATAV);
      USART_IntDisable(USART1, USART_IEN_TXC);

      for (i = 0; rx_buffer[i] != 0 && i < BUFFER_SIZE-3; i++)
      {
        tx_buffer[i] = rx_buffer[i];
      }

      tx_buffer[i++] = '\r';
      tx_buffer[i++] = '\f';
      tx_buffer[i] = '\0';
      rx_data_ready = 0;

      USART_IntEnable(USART1, USART_IEN_RXDATAV);
      USART_IntEnable(USART1, USART_IEN_TXC);
      USART_IntSet(USART1, USART_IFS_TXC);
    }
  }
}

