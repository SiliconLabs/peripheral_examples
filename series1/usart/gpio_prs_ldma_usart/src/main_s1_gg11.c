/***************************************************************************//**
 * @file main_s1_gg11.c
 * @brief This example demonstrates using LDMA with PRS where USART packet
 * transfers are triggered every push of Push Button 0.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
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
	descriptors[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(((PRS_CH_CTRL_SOURCESEL_GPIOH & _PRS_CH_CTRL_SOURCESEL_MASK) |
																		(PRS_CH_CTRL_SIGSEL_GPIOPIN8 & _PRS_CH_CTRL_SIGSEL_MASK) |
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
	GPIO_PinModeSet(gpioPortC, 1, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);


	// Start with default config
	USART_InitAsync_TypeDef config = USART_INITASYNC_DEFAULT;

	// Init USART
	USART_InitAsync(USART0, &config);

	// Set USART pin locations
	USART0->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC5 | USART_ROUTELOC0_TXLOC_LOC5;
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
