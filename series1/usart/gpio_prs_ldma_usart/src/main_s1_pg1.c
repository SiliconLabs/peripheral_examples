/**************************************************************************//**
 * @main_s1_PG1.c
 * @brief This example demonstrates using LDMA with PRS where USART packet
 * transfers are triggered every push of Push Button 0..
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs, Inc. http://www.silabs.com</b>
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

#define TX_LDMA_CHANNEL 1
#define TXBUFFER_SIZE   	 7

volatile char TxBuffer[20] = "Hello!\n";

// Descriptor and config for the LDMA operation for sending data
static LDMA_TransferCfg_t ldmaTXConfig;
static LDMA_Descriptor_t descriptors[4];

/**************************************************************************//**
 * @brief Initialize descriptors array
 *****************************************************************************/
void initDescriptors(void) {

	// Memory to peripheral transfer, Source: TxBuffer, Destination: USART0->TXDATA, Bytes to receive: 1
	descriptors[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(TxBuffer, &(USART0->TXDATA), 1, 1);
	descriptors[0].xfer.doneIfs = 0;

	// Setting USART0 TXC flag to trigger LDMA transfers using PRS signals
	descriptors[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(((PRS_CH_CTRL_SOURCESEL_USART0 & _PRS_CH_CTRL_SOURCESEL_MASK) |
																		(PRS_CH_CTRL_SIGSEL_USART0TXC & _PRS_CH_CTRL_SIGSEL_MASK) |
																		(uint32_t)prsEdgePos << _PRS_CH_CTRL_EDSEL_SHIFT),
																		&PRS->CH[0].CTRL,
																		1);

	// Memory to peripheral transfer, Source: TxBuffer, Destination: USART0->TXDATA, Bytes to receive: TXBUFFER_SIZE - 1
	descriptors[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&TxBuffer[1], &(USART0->TXDATA), TXBUFFER_SIZE-1, 0);
	descriptors[2].xfer.decLoopCnt = 1;

	// Setting GPIO pin 8 to trigger LDMA transfers using PRS signals again
	descriptors[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(((PRS_CH_CTRL_SOURCESEL_GPIOL & _PRS_CH_CTRL_SOURCESEL_MASK) |
																		(PRS_CH_CTRL_SIGSEL_GPIOPIN6 & _PRS_CH_CTRL_SIGSEL_MASK) |
																		(uint32_t)prsEdgePos << _PRS_CH_CTRL_EDSEL_SHIFT),
																		&PRS->CH[0].CTRL,
																		1);
}

/**************************************************************************//**
 * @brief Initialize USART0
 *****************************************************************************/
void initUSART0 (void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART0, true);

	// Configure GPIO mode
	// set pin modes for USART TX and RX pins
	GPIO_PinModeSet(gpioPortA, 1, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1);


	// Start with default config
	USART_InitAsync_TypeDef config = USART_INITASYNC_DEFAULT;

	// Init USART
	USART_InitAsync(USART0, &config);

	// Set USART pin locations
	USART0->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0;
	// Enable USART pins
	USART0->ROUTEPEN |= USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;


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
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL,
					  PRS_CH_CTRL_SIGSEL_GPIOPIN6, prsEdgePos);

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
	LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &descriptors[0]);
	LDMA_IntClear(1 << TX_LDMA_CHANNEL);
	USART_IntClear(USART0, USART_IntGet(USART0));
}

/**************************************************************************//**
 * @Initialize LDMA Descriptors
 *****************************************************************************/
void initLDMA(void)
{
	CMU_ClockEnable(cmuClock_LDMA, true);
	LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
	LDMA_Init(&ldmaInit); // Initializing default LDMA settings

	// One byte will transfer everytime the USART TXBL flag is high
	ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PRS_REQ0);

	// Starting both ldma transfers are different channels
	LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &descriptors[0]);
}

/**************************************************************************//**
 * @brief Transmitting TxBuffer
 *****************************************************************************/
void transmitTxBuffer(void)
{
  for(int i = 0; i < TXBUFFER_SIZE; i++)
	  USART_Tx(USART0, TxBuffer[i]);
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

	// Initialize descriptors
	initDescriptors();

	// Setup LDMA channels for transfer across SPI
	initLDMA();

	transmitTxBuffer();

	while(1);
}
