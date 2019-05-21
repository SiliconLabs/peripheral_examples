/**************************************************************************//**
 * @file main_gg11.c
 * @brief This project demonstrates how to get raw microphone data using i2s 
 * and transferring it over USART via LDMA transfers. See readme for details.
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

#include <stdlib.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_ldma.h"
#include "em_usart.h"
#include "em_device.h"
#include "em_emu.h"
#include "bsp.h"

// Sample frequency in Hz
// 8kHz * 512 / 120 = 34133 Hz
#define SAMPLE_FREQUENCY 34133

// Defined pin locations for microphone i2S interfacing
#define MIC_ENABLE_PORT gpioPortD
#define MIC_ENABLE_PIN  0
#define I2S_PORT        gpioPortI
#define I2S_TX_PIN      12
#define I2S_RX_PIN      13
#define I2S_CLK_PIN     14
#define I2S_CS_PIN      15

/// Globally declared LDMA link descriptors
LDMA_Descriptor_t leftDesc;
LDMA_Descriptor_t rightDesc;

// Buffers to hold microphone input
#define BUFFER_SIZE 10
static volatile uint32_t leftBuffer[BUFFER_SIZE];
int leftBufferIndex = 0;

/// Single byte used to dispose of right microphone data
uint8_t rightData;

/**************************************************************************//**
 * @brief Configure and start stereo microphone on USART3
 * @Note: we used USART3 to avoid bus or pin conflicts
 *****************************************************************************/
void MICMODE_InitMIC(uint32_t sampleFrequency)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable clock for USART3
  CMU_ClockEnable(cmuClock_USART3, true);

  // Enable GPIO clock and I2S pins
  GPIO_PinModeSet(I2S_PORT, I2S_RX_PIN, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(I2S_PORT, I2S_TX_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(I2S_PORT, I2S_CLK_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(I2S_PORT, I2S_CS_PIN, gpioModePushPull, 1);

  // Initialize USART3 to receive data from microphones synchronously
  USART_InitI2s_TypeDef def = USART_INITI2S_DEFAULT;
  def.sync.databits = usartDatabits8;
  def.format = usartI2sFormatW32D32;
  def.sync.enable = usartDisable;
  def.sync.autoTx = true;
  def.justify = usartI2sJustifyLeft;

  // Separate DMA requests for left and right channel data
  def.dmaSplit = true;

  // Set baud rate to achieve desired sample frequency
  def.sync.baudrate = sampleFrequency * 64;

  USART_InitI2s(USART3, &def);

  // Enable route to GPIO pins for I2S transfer on route #5
  USART3->ROUTEPEN =  USART_ROUTEPEN_TXPEN
                      | USART_ROUTEPEN_RXPEN
                      | USART_ROUTEPEN_CSPEN
                      | USART_ROUTEPEN_CLKPEN;
  
  USART3->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC5
                      | USART_ROUTELOC0_RXLOC_LOC5
                      | USART_ROUTELOC0_CSLOC_LOC5
                      | USART_ROUTELOC0_CLKLOC_LOC5;

  // Enable USART3
  USART_Enable(USART3, usartEnable);

  // Initialize and set mic_enable pin (PD0)
  GPIO_PinModeSet(MIC_ENABLE_PORT, MIC_ENABLE_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief Configure and start DMA
 * @detail Starting DMA transfers that transfer the data from the different
 * mic channels to either buffers or dummy variables. We are only using the
 * the left channel's data to show what a buffer of mic data looks like so
 * the DMA transfer for the right channel only puts the right mic data 
 * into a dummy variable
 *****************************************************************************/
void MICMODE_InitLDMA()
{
  // Default LDMA init
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Configure LDMA for transfer from USART to memory (left channel)
  // LDMA will loop continuously
  LDMA_TransferCfg_t leftCfg =
  LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAV);

  //Globally store and configure link descriptors for left microphone transfer
  LDMA_Descriptor_t leftXfer = LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA, (uint8_t *)leftBuffer, 4 * BUFFER_SIZE, 0);
  leftDesc = leftXfer;
  // trigger interrupt on left microphone transfer complete (buffer full)
  leftDesc.xfer.doneIfs = 1;
  leftDesc.xfer.ignoreSrec = 0;

  // Configure LDMA for transfer from USART to memory (right channel)
  // LDMA will loop continuously and discard right channel data
  LDMA_TransferCfg_t rightCfg =
  LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAVRIGHT);

  // Set up right microphone descriptor
  LDMA_Descriptor_t rightXfer =
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA, &rightData, 1, 0);
  rightDesc = rightXfer;
  rightDesc.xfer.size = ldmaCtrlSizeByte;

  // Don't trigger interrupts on right microphone transfers
  rightDesc.xfer.doneIfs = 0;
  rightDesc.xfer.ignoreSrec = 0;

  // Ensure destination address does not increment
  rightDesc.xfer.dstInc = 0;

  // Start left and right transfers
  LDMA_StartTransfer(0, (void *)&leftCfg, (void *)&leftDesc);
  LDMA_StartTransfer(1, (void *)&rightCfg, (void *)&rightDesc);
}

/***************************************************************************//**
 * @brief LDMA IRQ handler.
 * @detail Handler that triggers on each complete LDMA transfer and sets the
 * corresponding flag
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  // Clear all interrupt flags
  LDMA->IFC |= 0xFFFFFFFF;

  leftBufferIndex = 0;
}

/***************************************************************************//**
 * @brief Initializing CMU
 * @detail Setting the HF clock's frequency to 72MHz
 ******************************************************************************/
void initCMU()
{
  // Set HF clock to 72MHz
  CMU_HFRCOBandSet(cmuHFRCOFreq_72M0Hz);
}

/***************************************************************************//**
 * @brief Main
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Configuring clocks in the Clock Management Unit (CMU)
  initCMU();

  MICMODE_InitMIC(SAMPLE_FREQUENCY);
  MICMODE_InitLDMA();

  while (1){}
}
