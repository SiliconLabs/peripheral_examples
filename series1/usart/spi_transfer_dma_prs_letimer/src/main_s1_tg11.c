/**************************************************************************//**
 * @main_series1_tg11.c
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
#include "em_letimer.h"
#include "em_prs.h"

#define PRS_TX_CHANNEL    0
#define PRS_RX_CHANNEL    1

#define TX_BUFFER_SIZE   10
#define RX_BUFFER_SIZE   TX_BUFFER_SIZE

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
int numTransfers = 0;  //Number of transfers completed so far

uint8_t RxBuffer[RX_BUFFER_SIZE] = {0};

// Descriptor and config for the LDMA operation for sending data
static LDMA_Descriptor_t ldmaTXDescriptor;
static LDMA_TransferCfg_t ldmaTXConfig;

// Descriptor and config for the LDMA operation for receiving data
static LDMA_Descriptor_t ldmaRXDescriptor;
static LDMA_TransferCfg_t ldmaRXConfig;

#define letimerClkFreq  19000000
// Desired letimer interrupt frequency (in Hz)
#define letimerDesired  100000

#define letimerCompare  letimerClkFreq / letimerDesired

/**************************************************************************//**
 * @brief LETIMER initialization
 *
 * Note: The Letimer is being initialized to only generate a number of
 * pulses equal to the size of our TxBuffer
 *****************************************************************************/
void initLETIMER(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable clock to the LE modules interface
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Select LFXO for the LETIMER
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Reload COMP0 on underflow, idle output, and run in repeat mode
  letimerInit.comp0Top  = true;
  letimerInit.ufoa0     = letimerUFOAPulse;
  letimerInit.repMode   = letimerRepeatDouble;

  // Need REP0 != 0 to pulse on underflow
  LETIMER_RepeatSet(LETIMER0, 0, TX_BUFFER_SIZE);

  // Compare on wake-up interval count
  LETIMER_CompareSet(LETIMER0, 0, letimerCompare);

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);

  // Use LETIMER0 as sync PRS to trigger DMA transfers to the slave
  CMU_ClockEnable(cmuClock_PRS, true);
  PRS_SourceSignalSet(PRS_TX_CHANNEL,
                      PRS_CH_CTRL_SOURCESEL_LETIMER0,
                      PRS_CH_CTRL_SIGSEL_LETIMER0CH0,
                      prsEdgePos);

  PRS->DMAREQ0 = PRS_DMAREQ0_PRSSEL_PRSCH0;
}

/**************************************************************************//**
 * @brief Initialize USART0
 *****************************************************************************/
void initUSART0 (void)
{
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART0, true);

    // Configure GPIO mode
    GPIO_PinModeSet(gpioPortA, 12, gpioModePushPull, 0); // US0_CLK is push pull
    GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);  // US0_CS is push pull
    GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 1); // US0_TX (MOSI) is push pull
    GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 1);    // US0_RX (MISO) is input

    // Start with default config, then modify as necessary
    USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
    config.master       = true;            // master mode
    config.baudrate     = 1000000;         // CLK freq is 1 MHz
    config.autoCsEnable = true;            // CS pin controlled by hardware, not firmware
    config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
    config.msbf         = true;            // send MSB first
    config.enable    = usartDisable;    // Make sure to keep USART disabled until it's all set up

    // Init USART with the settings in config
    USART_InitSync(USART0, &config);

    // Set USART pin locations
    USART0->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC5) | // US0_CLK       on location 5 = PA12 per datasheet section 6.4 = EXP Header pin 8
                        (USART_ROUTELOC0_CSLOC_LOC2)  | // US0_CS        on location 2 = PC8 per datasheet section 6.4 = EXP Header pin 10
                        (USART_ROUTELOC0_TXLOC_LOC2)  | // US0_TX (MOSI) on location 2 = PC11 per datasheet section 6.4 = EXP Header pin 4
                        (USART_ROUTELOC0_RXLOC_LOC2);   // US0_RX (MISO) on location 2 = PC10 per datasheet section 6.4 = EXP Header pin 6

    // Enable USART pins
    USART0->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

    // Enable USART0
    USART_Enable(USART0, usartEnable);
}

/**************************************************************************//**
 * @Initialize LDMA Descriptors
 *
 * Note: The transfer descriptor will be triggered by periodic pulses from the
 * letimer through PRS. While the Rx descriptor waits for packets from the slave
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
  ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(TxBuffer, &(USART0->TXDATA), 1);
  // One byte will transfer everytime the USART TXBL flag is high
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PRS_REQ0);

  //Starting both ldma transfers are different channels
  LDMA_StartTransfer(PRS_TX_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);
  LDMA_StartTransfer(PRS_RX_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);
}

/**************************************************************************//**
 * @brief LDMA IRQHandler that triggers the next transfer
 *
 * Note: Since we want to increment our source pointer every transfer, but
 * each new transfer starts back over at the beginning of TxBuffer we have
 * to make a new pointer to the element we want to send each transfer
 * and feed that into our new LDMA descriptor every loop
 *****************************************************************************/
void LDMA_IRQHandler()
{
  uint32_t flags = LDMA_IntGet();

  if(flags & (1 << PRS_TX_CHANNEL))
  {
    LDMA_IntClear(1 << PRS_TX_CHANNEL);
    numTransfers++;
    if(numTransfers == TX_BUFFER_SIZE)
    {
      numTransfers = 0;
    }

    uint8_t *newPointer = &(TxBuffer[numTransfers]);

    ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(newPointer, &(USART0->TXDATA), 1);
    LDMA_StartTransfer(PRS_TX_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);
  }
  else
  {
    LDMA_StartTransfer(PRS_RX_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);
  }
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  // Initialize chip
  CHIP_Init();

  // Initialize LETIMER0 to generate periodic prs interrupts
  initLETIMER();

  // Initialize USART0 as SPI slave
  initUSART0();

  // Setup LDMA channels for transfer across SPI
  initLDMA();

  // Place breakpoint here and observe RxBuffer after transfers have completed
  // RxBuffer should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9
  while(1);
}
