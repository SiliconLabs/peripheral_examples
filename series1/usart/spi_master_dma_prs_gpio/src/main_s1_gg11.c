/**************************************************************************//**
 * @main_series1_GG11.c
 * @brief Demonstrates USART0 as SPI master.
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
#include "em_ldma.h"
#include "em_prs.h"
#include "bsp.h"

#define RX_LDMA_CHANNEL 0
#define TX_LDMA_CHANNEL 1 

#define TX_BUFFER_SIZE   10
#define RX_BUFFER_SIZE   TX_BUFFER_SIZE

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
uint8_t RxBuffer[RX_BUFFER_SIZE] = {0};

// Descriptor and config for the LDMA operation for sending data
static LDMA_Descriptor_t ldmaTXDescriptor;
static LDMA_TransferCfg_t ldmaTXConfig;

// Descriptor and config for the LDMA operation for receiving data
static LDMA_Descriptor_t ldmaRXDescriptor;
static LDMA_TransferCfg_t ldmaRXConfig;

/**************************************************************************//**
 * @brief Initialize USART0
 *****************************************************************************/
void initUSART0 (void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART0, true);

  // Configure GPIO mode
  GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 0); // US0_CLK is push pull
  GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 1); // US0_CS is push pull
  GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1); // US0_TX (MOSI) is push pull
  GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 1);    // US0_RX (MISO) is input

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master       = true;            // master mode
  config.baudrate     = 1000000;         // CLK freq is 1 MHz
  config.autoCsEnable = true;            // CS pin controlled by hardware, not firmware
  config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf         = true;            // send MSB first
  config.enable       = usartDisable;    // Make sure to keep USART disabled until it's all set up  

  // Init USART with the settings in config
  USART_InitSync(USART0, &config);

  // Set USART pin locations
  USART0->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC0) | // US0_CLK       on location 0 = PE12 per datasheet section 6.4 = EXP Header pin 8
                      (USART_ROUTELOC0_CSLOC_LOC0)  | // US0_CS        on location 0 = PE13 per datasheet section 6.4 = EXP Header pin 10
                      (USART_ROUTELOC0_TXLOC_LOC0)  | // US0_TX (MOSI) on location 0 = PE10 per datasheet section 6.4 = EXP Header pin 4
                      (USART_ROUTELOC0_RXLOC_LOC0);   // US0_RX (MISO) on location 0 = PE11 per datasheet section 6.4 = EXP Header pin 6

  // Enable USART pins
  USART0->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

  // Enable push button 0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Enable push button 0 to generate interrupt signals so it can supply a signal to our prs
  // but we turn off all of it's normal interrupt modes to avoid unwanted interrupt triggers
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 0, 0, false);

  // Clear pending gpio interrupts
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  GPIO_IntClear(0x00FF);

  // Using GPIO pin 8 to trigger LDMA transfers using PRS signals
  CMU_ClockEnable(cmuClock_PRS, true);
  PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN8, prsEdgePos);

  // Configuring the PRS to send the PRS signal to the LDMA Channel 0
  PRS->DMAREQ0 = PRS_DMAREQ0_PRSSEL_PRSCH0;

  // Enable USART0
  USART_Enable(USART0, usartEnable);
}

/**************************************************************************//**
 * @IRQ Handler to reset transfers
 *****************************************************************************/
void LDMA_IRQHandler()
{
  uint32_t flags = LDMA_IntGet();

  if(flags & (1 << RX_LDMA_CHANNEL))
  {
    LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);
    LDMA_IntClear(1 << RX_LDMA_CHANNEL);
  }
  else
  {
    LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);
    LDMA_IntClear(1 << TX_LDMA_CHANNEL);
  }
}

/**************************************************************************//**
 * @Initialize LDMA Descriptors
 *****************************************************************************/
void initLDMA(void)
{
  CMU_ClockEnable(cmuClock_LDMA, true);
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
  LDMA_Init(&ldmaInit); // Initializing default LDMA settings

  // Peripheral to memory transfer, Source: USART0->RXDATA, Destination: RxBuffer, Bytes to receive: 10
  ldmaRXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(USART0->RXDATA), RxBuffer, RX_BUFFER_SIZE);
  // One byte will transfer everytime the USART RXDATAV flag is high
  ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART0_RXDATAV);

  // Memory to peripheral transfer, Source: TxBuffer, Destination: USART0->TXDATA, Bytes to receive: 10
  ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(TxBuffer, &(USART0->TXDATA), TX_BUFFER_SIZE);
  // One byte will transfer everytime the USART TXBL flag is high
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PRS_REQ0);

  //Starting both ldma transfers are different channels
  LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);
  LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  // Initialize chip
  CHIP_Init();

  // Initialize USART0 as SPI slave
  initUSART0();

  // Setup LDMA channels for transfer across SPI
  initLDMA();

  // Place breakpoint here and observe RxBuffer
  // RxBuffer should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9
  while(1);
}

