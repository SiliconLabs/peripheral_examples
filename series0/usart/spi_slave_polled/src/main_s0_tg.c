/**************************************************************************//**
 * @main_series1.c
 * @brief Demonstrates USART1 as SPI slave.
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
#include "em_gpio.h"
#include "em_usart.h"

#define RX_BUFFER_SIZE   10
#define TX_BUFFER_SIZE   RX_BUFFER_SIZE

uint8_t RxBuffer[RX_BUFFER_SIZE];
uint8_t RxBufferIndex = 0;

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9};
uint8_t TxBufferIndex = 1;

/**************************************************************************//**
 * @brief Initialize USART1
 *****************************************************************************/
void initUSART1 (void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // Configure GPIO mode
  GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 1);    // US0_CLK is input
  GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 1);     // US0_CS is input
  GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 1);    // US0_TX (MOSI) is input
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 1); // US0_RX (MISO) is push pull

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master    = false;
  config.clockMode = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf      = true;            // send MSB first
  config.enable    = usartDisable;    // making sure to keep USART disabled until we've set everything up
  USART_InitSync(USART1, &config);

  // Set USART pin locations
  USART1->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC1;

  // Pre-loading our TXDATA register so our slave's echo can be in synch with the master
  USART1->TXDATA = TxBuffer[0];

  // Enable USART1
  USART_Enable(USART1, usartEnable);
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  // Initialize chip
  CHIP_Init();

  // Initialize USART1 as SPI slave
  initUSART1();

  while(1)
  {
    for(int i = 0; i < RX_BUFFER_SIZE; i++)
    {
      RxBuffer[RxBufferIndex++] = USART_Rx(USART1);  // Polls the USART1 status registers until it see's the flag for a received message
      USART_Tx(USART1, TxBuffer[TxBufferIndex++]);   // Waits until our transfer buffer is empty then loads in our TXDATA register
    }

    if(TxBufferIndex == TX_BUFFER_SIZE)
    {
      TxBufferIndex = 0;
    }

    if(RxBufferIndex == RX_BUFFER_SIZE)
    {
      RxBufferIndex = 0;
    }

    // Put a break point here to view the full RxBuffer,
    // The RxBuffer should be: 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
  }
}
