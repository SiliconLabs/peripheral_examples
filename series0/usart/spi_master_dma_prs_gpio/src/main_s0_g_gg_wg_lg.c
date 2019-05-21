/**************************************************************************//**
 * @main_series0_G_GG_WG_LG.c
 * @brief Demonstrates USART1 as SPI master.
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
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "em_prs.h"
#include "bsp.h"

#define TX_BUFFER_SIZE   10
#define RX_BUFFER_SIZE   TX_BUFFER_SIZE

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
uint8_t RxBuffer[RX_BUFFER_SIZE];
DMA_CB_TypeDef dmacb;

uint32_t numTransfers = 0;
volatile uint32_t clearFlag;
/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  clearFlag = TIMER0->CC[0].CCV;  // Reading this clears the interrupt flag

  numTransfers++;
  uint8_t *newStart = &TxBuffer[numTransfers];
  if(numTransfers < 10)
  {
    DMA_ActivateBasic(channel, primary, false, (void*) &USART1->TXDATA, (void*) newStart, 0);
  }
}

/**************************************************************************//**
 * @brief Setup push button BTN1
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure push button BTN1 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Configure interrupt on push button PB1 for rising edge but not enabled - PRS sensing instead
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, false, false);

  // Clear pending gpio interrupts
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  GPIO_IntClear(0x00FF);
}

/**************************************************************************//**
 * @brief Initialize USART1
 *****************************************************************************/
void initUSART1 (void)
{
  CMU_ClockEnable(cmuClock_USART1, true);

  // Configure GPIO mode
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); // US1_CLK is push pull
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 1); // US1_CS is push pull
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1); // US1_TX (MOSI) is push pull
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);    // US1_RX (MISO) is input

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master       = true;            // master mode
  config.baudrate     = 1000000;         // CLK freq is 1 MHz
  config.autoCsEnable = true;            // CS pin controlled by hardware, not firmware
  config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf         = true;            // send MSB first
  config.enable       = usartDisable;    // Make sure to keep USART disabled until it's all set up  

  // Init USART with the settings in config
  USART_InitSync(USART1, &config);

  // Set and enable USART pin locations
  USART1->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC1;

  // Enable USART1
  USART_Enable(USART1, usartEnable);
}

/**************************************************************************//**
 * @brief Setup push button BTN1 as PRS source for TIMER 0 CC0.
 *****************************************************************************/
void initPrs()
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Select PB0 as PRS input signal
  if (BSP_GPIO_PB0_PIN > 7) {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH,
        (uint32_t)(BSP_GPIO_PB0_PIN - 8), prsEdgePos);
  } else {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL,
        BSP_GPIO_PB0_PIN, prsEdgePos);
  }
}

/**************************************************************************//**
 * @brief Initialize TIMER0 for compare capture mode, each GPIO press will trigger
 * a timer capture which will generate the signal our DMA Tx Transfers trigger
 * off of
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture settings
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Set Interrupt flag on every edge
  timerCCInit.eventCtrl = timerEventFalling;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;

  timerCCInit.prsSel = timerPRSSELCh0;
  timerCCInit.prsInput = true;

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Initialize timer with defined prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1;

  // Initializing TIMER0
  TIMER_Init(TIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module to send a transfer every time a timer capture
 *    complete signal is generated. A timer capture is triggered whenever PB0
 *    is pressed. Another transfer will be setup and started in the transfer
 *    complete handler until the full TxBuffer is sent
 * @note
 *    The callback object needs to at least have static scope persistence so
 *    that the reference to the object is valid beyond its first use in
 *    initialization. This is because the handler needs access to the callback
 *    function. If reference isn't valid anymore, then all dma transfers after
 *    the first one will fail.
 *****************************************************************************/
void initTransferDma(void)
{
    // Setup callback function
    dmacb.cbFunc  = dmaCallback;
    dmacb.userPtr = NULL;

    // Channel configuration for TX transmission
    DMA_CfgChannel_TypeDef channelConfigTX;
    channelConfigTX.highPri   = false;             // Set high priority for the channel
    channelConfigTX.enableInt = true;              // Interrupt used to reset the transfer
    channelConfigTX.select    = DMAREQ_TIMER0_CC0; // Select DMA trigger
    channelConfigTX.cb        = &dmacb;
    
    uint32_t channelNumTX     = 0;
    DMA_CfgChannel(channelNumTX, &channelConfigTX);

    // Channel descriptor configuration for TX transmission
    static DMA_CfgDescr_TypeDef descriptorConfigTX;
    descriptorConfigTX.dstInc  = dmaDataIncNone;  // Destination doesn't move
    descriptorConfigTX.srcInc  = dmaDataInc1;     // Source moves one entry at a time
    descriptorConfigTX.size    = dmaDataSize1;    // Transfer 8 bits each time
    descriptorConfigTX.arbRate = dmaArbitrate1;   // Arbitrate after every DMA transfer
    descriptorConfigTX.hprot   = 0;               // Access level/protection not an issue
    
    bool isPrimaryDescriptor = true;
    DMA_CfgDescr(channelNumTX, isPrimaryDescriptor, &descriptorConfigTX);

    bool isUseBurst = false;
    DMA_ActivateBasic(channelNumTX,
                      isPrimaryDescriptor,
                      isUseBurst,
                      (void *) &USART1->TXDATA,  // Destination address to transfer to, change back to: (void *) &USART1->TXDATA
                      (void *) TxBuffer,         // Source address to transfer from
                      0);                        // Number of DMA transfers minus 1
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module to receive a transfer whenever the RXDATAV
 *    signal is generated. This signal is generated whenever valid data comes
 *    in from the slave. The transfer will be complete once RxBuffer is full
 *****************************************************************************/
void initReceiveDma(void)
{
    // Channel configuration for RX transmission
    DMA_CfgChannel_TypeDef channelConfigRX;
    channelConfigRX.highPri   = false;          // Set high priority for the channel
    channelConfigRX.enableInt = false;          // Interrupt used to reset the transfer
    channelConfigRX.select    = DMAREQ_USART1_RXDATAV;    // Select DMA trigger
    
    uint32_t channelNumRX     = 1;
    DMA_CfgChannel(channelNumRX, &channelConfigRX);

    // Channel descriptor configuration for RX transmission
    static DMA_CfgDescr_TypeDef descriptorConfigRX;
    descriptorConfigRX.dstInc  = dmaDataInc1;     // Destination moves one spot in the buffer every transmit
    descriptorConfigRX.srcInc  = dmaDataIncNone;  // Source doesn't move
    descriptorConfigRX.size    = dmaDataSize1;    // Transfer 8 bits each time
    descriptorConfigRX.arbRate = dmaArbitrate1;   // Arbitrate after every DMA transfer
    descriptorConfigRX.hprot   = 0;               // Access level/protection not an issue
    
    bool isPrimaryDescriptor = true;
    DMA_CfgDescr(channelNumRX, isPrimaryDescriptor, &descriptorConfigRX);

    // Activate basic DMA cycle (used for memory-peripheral transfers)
    bool isUseBurst = false;
    DMA_ActivateBasic(channelNumRX,
                      isPrimaryDescriptor,
                      isUseBurst,
                      (void *) RxBuffer,         // Destination address to transfer to
                      (void *) &USART1->RXDATA,  // Source address to transfer from
                      RX_BUFFER_SIZE - 1);       // Number of DMA transfers minus 1
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  // Initialize chip
  CHIP_Init();

  // Init GPIO
  initGpio();

  // Initialize USART1 as SPI slave
  initUSART1();

  // Init Prs to hook GPIO to trigger timer captures
  initPrs();

  // Init Timer in timer capture mode
  initTIMER();

  // Initializing the DMA
  CMU_ClockEnable(cmuClock_DMA, true);
  DMA_Init_TypeDef init;
  
  init.hprot = 0;                      // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Setup LDMA channels for transfer across SPI
  initTransferDma();
  initReceiveDma();

  // Place breakpoint here and observe RxBuffer
  // RxBuffer should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9
  while(1);
}

