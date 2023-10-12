/***************************************************************************//**
 * @file main_xg24.c
 *
 * @brief This example demonstrates I2C leader operation with DMA.
 *        Having the LDMA handle the movement of data between the I2C
 * receive and transmit FIFOs allows the CPU to perform other tasks
 * while the relatively slow transfers proceed.
 *
 * As with the example project that uses polled operation, two
 * EFM32/EFR32 devices are connected together with one running the
 * follower example project and this code driving the leader device.
 *
 * The leader example starts up and enters a while loop waiting for an
 * interrupt that is requested when button 0 is pressed, at which time
 * a simple test sequence is run.
 *
 * The test routine first reads the follower device's current buffer of
 * uint8_t data and increments each value by 1.  The updated values are
 * written back to the follower and read again to verify that the
 * updated data was correctly received.
 *
 * Upon success, LED0 is toggled and the device re-enters the while
 * loop, waiting for button 0 to run the test again.  If there is an
 * I2C error or if the verification of the received data fails, LED1 is
 * turned on and the code halts at a breakpoint so that the received
 * data and I2C status and interrupt flag registers can be examined.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_ldma.h"

#include "bsp.h"

// I2C pins (SCL = PC5/EXP15; SDA = PC7/EXP16)
#define I2C_SCL_PORT            gpioPortC
#define I2C_SCL_PIN             5
#define I2C_SDA_PORT            gpioPortC
#define I2C_SDA_PIN             7
#define I2C_DOMAIN_POWER_PORT   gpioPortD
#define I2C_DOMAIN_POWER_PIN    3

// Address of the I2C follower device (left-shifted to bits [7:1])
#define I2C_FOLLOWER_ADDRESS    0xE2

// Read/write bit/mask for the I2C follower device address
#define I2C_RNOTW_BIT           0x01
#define I2C_RNOTW_MASK          0xFE

// Buffer configuration
#define I2C_BUFFER_SIZE         10

uint8_t i2c_rxBuffer[I2C_BUFFER_SIZE];
uint8_t i2c_txBuffer[I2C_BUFFER_SIZE];

// LDMA configuration and descriptors
#define I2C_DMA_RXCHAN          0
#define I2C_DMA_TXCHAN          1

LDMA_Descriptor_t i2c_rxDesc[5];
LDMA_Descriptor_t i2c_txDesc[1];

// Transmission status
volatile bool i2c_startTx;
volatile I2C_TransferReturn_TypeDef i2c_xferStatus;

/***************************************************************************//**
 * @brief GPIO initialization
 ******************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED0 and LED1 as outputs
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  // Configure PB0 as input and interrupt
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT,
                      BSP_GPIO_PB0_PIN,
                      BSP_GPIO_PB0_PIN,
                      false,
                      true,
                      true);

  // Enable NVIC GPIO interrupt
#if (BSP_GPIO_PB0_PIN & 1)
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
#else
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
#endif
}

/***************************************************************************//**
 * @brief Setup I2C
 ******************************************************************************/
void initI2C(void)
{
  CMU_ClockEnable(cmuClock_I2C0, true);

  // Use default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

  /*
   * Power up the Si7021 RHT sensor domain on the mainboard.  This
   * also powers up the sensor's local SCL and SDA pull-ups, which
   * eliminates the need to jumper resistors to the EXP header pins.
   */
  GPIO_PinModeSet(I2C_DOMAIN_POWER_PORT, I2C_DOMAIN_POWER_PIN, gpioModePushPull, 1);

  // Configure SCL and SDA for open-drain operation
  GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_PIN, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_PIN, gpioModeWiredAndPullUpFilter, 1);

  // Route I2C pins to GPIO
  GPIO->I2CROUTE[0].SCLROUTE = (I2C_SCL_PORT << _GPIO_I2C_SCLROUTE_PORT_SHIFT
                             | (I2C_SCL_PIN << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].SDAROUTE = (I2C_SDA_PORT << _GPIO_I2C_SDAROUTE_PORT_SHIFT
                             | (I2C_SDA_PIN  << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SCLPEN | GPIO_I2C_ROUTEEN_SDAPEN;

  // Initialize the I2C
  I2C_Init(I2C0, &i2cInit);

  // Set the status flags and index
  i2c_startTx = false;
}

/***************************************************************************//**
 * @brief Read I2C_BUFFER_SIZE bytes starting at address 0 from the follower
 ******************************************************************************/
void I2C_LeaderRead()
{
  // Enable automatic ACK for all bytes received except the last one
  I2C0->CTRL_SET = I2C_CTRL_AUTOACK;

  /*
   * This example sends the follower address and the 1-byte read
   * address (0x0) simply by loading them into the TX FIFO.  Because
   * only two bytes are transmitted before the repeated start, there
   * is no need to use DMA.  The START condition is set later.
   */
  I2C0->TXDATA = (I2C_FOLLOWER_ADDRESS & I2C_RNOTW_MASK);
  I2C0->TXDATA = 0;

  // Setup the receive DMA to trigger on data valid
  LDMA_TransferCfg_t i2c_rxReq =
    (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_I2C0_RXDATAV);

  /*
   * The first RX DMA descriptor receives all but the last byte of data
   * and then links to the next descriptor.
   */
  i2c_rxDesc[0] =
      (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE((uint32_t)(&I2C0->RXDATA),
                                                         (uint32_t)(i2c_rxBuffer),
                                                         (I2C_BUFFER_SIZE - 1),
                                                         1);

  /*
   * The next descriptor disables automatic acknowledge because the
   * last byte has to be NACKed to end the read.  Do this by having the
   * LDMA clear the AUTOACK bit and link to the next descriptor.
   */
  i2c_rxDesc[1] =
      (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_CTRL_AUTOACK,
                                                       (uint32_t)(&I2C0->CTRL_CLR),
                                                       1);

  /*
   * The next descriptor enables the MSTOP interrupt so that when the
   * last byte is received, the transfer can be marked as complete.
   * Do this by having the DMA set the MSTOP bit in the interrupt
   * enable register and link to the next descriptor.
   */
  i2c_rxDesc[2] =
      (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(I2C_IEN_MSTOP,
                                                       (uint32_t)(&I2C0->IEN_SET),
                                                       1);

  /*
   * This descriptor receives the last byte of data by itself so that
   * it can be NACKed and then links to the next descriptor.
   */
  i2c_rxDesc[3] =
      (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE((uint32_t)(&I2C0->RXDATA),
                                                         (uint32_t)(i2c_rxBuffer + I2C_BUFFER_SIZE - 1),
                                                         1,
                                                         1);

  /*
   * The final descriptor NACKs the last byte and issues a STOP to
   * properly end the transfer.  Do this by having the DMA set the
   * NACK and STOP bits in the command register.
   */
  i2c_rxDesc[4] =
      (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE((I2C_CMD_NACK | I2C_CMD_STOP),
                                                      (uint32_t)(&I2C0->CMD));

  /*
   * Mark transfer in progress, start the RX DMA channel, and send the
   * START condition to transmit the data already in the FIFO.
   */
  i2c_xferStatus = i2cTransferInProgress;
  LDMA_StartTransfer(I2C_DMA_RXCHAN, &i2c_rxReq, &i2c_rxDesc[0]);
  I2C0->CMD = I2C_CMD_START;

  /*
   * Disable the LDMA RX channel interrupt.  It is not needed because
   * the relevant I2C interrupts are used on end of transfer or error
   * conditions.
   *
   * Receiving data takes several steps because of the need to NACK
   * the last byte received, send stop, and request an interrupt when
   * everything is done.  This is done with linked descriptors, which
   * is something the Gecko SDK DMADRV component does not have a means
   * of handling short of simply creating a linked descriptors as was
   * done here.
   *
   * DMADRV has no means of handling read errors, so it's necessary to
   * have the I2C error interrupts enabled anyway.
   */
  LDMA_IntDisable((1 << I2C_DMA_RXCHAN));

  /*
   * Enable the TXC interrupt in order to issue the repeated start
   * followed by the device bus address to begin reading data.  Error
   * conditions interrupts get enabled here, too.
   */
  NVIC_ClearPendingIRQ(I2C0_IRQn);
  NVIC_EnableIRQ(I2C0_IRQn);
  I2C_IntClear(I2C0, _I2C_IF_MASK);
  I2C_IntEnable(I2C0, (I2C_IEN_TXC     |
                       I2C_IEN_ARBLOST |
                       I2C_IEN_BUSERR  |
                       I2C_IEN_NACK));
}

/***************************************************************************//**
 * @brief Write I2C_BUFFER_SIZE bytes starting at address 0 to the follower
 ******************************************************************************/
void I2C_LeaderWrite()
{
  // Setup the transmit DMA to trigger on available buffer space
  LDMA_TransferCfg_t i2c_txReq =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_I2C0_TXBL);

  // This descriptor transmits the data in i2c_txBuffer[]
  i2c_txDesc[0] =
    (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((uint32_t)(i2c_txBuffer),
                                                       (uint32_t)(&I2C0->TXDATA),
                                                       I2C_BUFFER_SIZE);

  /*
   * As in the read function, some simplification of the code is
   * possible by writing the follower address and the starting address
   * of the write directly to the TX FIFO instead of having the DMA
   * send it.
   */
  i2c_xferStatus = i2cTransferInProgress;
  I2C0->TXDATA = (I2C_FOLLOWER_ADDRESS & I2C_RNOTW_MASK);
  I2C0->TXDATA = 0;

  // Start the TX DMA channel but don't send START yet
  LDMA_StartTransfer(I2C_DMA_TXCHAN, &i2c_txReq, &i2c_txDesc[0]);

  /*
   * Disable the LDMA TX channel interrupt.  Because DMADRV owns the
   * LDMA_IRQHandler(), it's not possible to put custom handling for
   * the I2C in there.
   *
   * Instead, the MSTOP interrupt denotes successful transfer
   * completion.  Because the follower is receiving data, it ought to
   * ACK each byte as received unless their is an error, so these
   * interrupts are also enabled here.
   */
  LDMA_IntDisable(1 << I2C_DMA_TXCHAN);

  NVIC_ClearPendingIRQ(I2C0_IRQn);
  NVIC_EnableIRQ(I2C0_IRQn);
  I2C_IntClear(I2C0, _I2C_IF_MASK);
  I2C_IntEnable(I2C0, (I2C_IEN_MSTOP   |
                       I2C_IEN_ARBLOST |
                       I2C_IEN_BUSERR  |
                       I2C_IEN_NACK));

  // Everything is ready; send the START condition
  I2C0->CMD = I2C_CMD_START;

  // Automatically send STOP when there is no data left to transmit
  I2C0->CTRL_SET = I2C_CTRL_AUTOSE;
}

/*****************************************************************************
 * @brief  I2C event and error interrupt handling
 *
 * When the leader has successfully sent a STOP condition, the MSTOP
 * interrupt is triggered and the transfer is marked as complete.
 *
 * The ARBLOST, BUSERR, and NACK errors are also handled here.  In any
 * of these cases, the current transfer is aborted and the last transfer
 * status is set to an error condition.  NACK is only enabled during
 * transmit activity except for the repeat start condition that begins
 * clocking read data out of the device.
 *****************************************************************************/
void I2C0_IRQHandler(void)
{
  uint32_t flags = I2C_IntGetEnabled(I2C0);

  if (flags & I2C_IF_TXC)
  {
    // Don't need TXC or NACK anymore so disable them
    I2C_IntDisable(I2C0, (I2C_IEN_TXC | I2C_IEN_NACK));
    I2C_IntClear(I2C0, I2C_IF_TXC);

    // Send the device bus address to start the read
    I2C0->TXDATA = (I2C_FOLLOWER_ADDRESS | I2C_RNOTW_BIT);

    // Send repeat start for read operation
    I2C0->CMD = I2C_CMD_START;
  }

  // MSTOP set on success; transfer is complete
  else if (flags & I2C_IF_MSTOP)
  {
    // Disable all interrupts
    I2C_IntDisable(I2C0, _I2C_IEN_MASK);
    I2C_IntClear(I2C0, _I2C_IF_MASK);

    // Reset control register
    I2C0->CTRL = _I2C_CTRL_RESETVALUE;

    // Transfer is complete on MSTOP interrupt
    i2c_xferStatus = i2cTransferDone;
  }

  // Need to abort on any of the following error conditions
  else {
    if (flags & I2C_IF_ARBLOST) {
      i2c_xferStatus = i2cTransferArbLost;
    } else {
      if (flags & I2C_IF_BUSERR) {
        i2c_xferStatus = i2cTransferBusErr;
      } else {
        if (flags & I2C_IF_NACK) {
          i2c_xferStatus = i2cTransferNack;
        }
      }
    }

    // Abort on error
    I2C0->CMD = I2C_CMD_ABORT;
  }
}

/***************************************************************************//**
 * @brief Wait in EM1 during transfer; check status when done
 *
 * @note In a real application, other code could execute while the LDMA
 *       manages the I2C activity.  Transfer completion would be
 * processed in the I2C interrupt handler and signaled through the use
 * of a state variable (as is done here) or, better still, with a
 * user designated callback function or RTOS semaphore.
 ******************************************************************************/
void dmaWaitAndCheck(void)
{
  // Wait in EM1 while the transfer is in progress
  while (i2c_xferStatus == i2cTransferInProgress) {
    EMU_EnterEM1();
  }

  // Did the read complete successfully; if not, breakpoint here
  if (i2c_xferStatus != i2cTransferDone) {
    __BKPT(0);
  }
}

/***************************************************************************//**
 * @brief I2C read/increment/write/verify test sequence
 ******************************************************************************/
bool testI2C(void)
{
  int i;
  bool I2CWriteVerify;

  // Initial read of bytes from follower
  I2C_LeaderRead();

  // Wait in EM1 while the read progresses
  dmaWaitAndCheck();

  // Increment received values for write-back to the follower
  for (i = 0; i < I2C_BUFFER_SIZE; i++) {
    i2c_txBuffer[i] = i2c_rxBuffer[i] + 1;
  }

  // Block write new values to follower
  I2C_LeaderWrite();

  // Wait in EM1 while the write progresses
  dmaWaitAndCheck();

  // Block read from follower
  I2C_LeaderRead();

  // Wait in EM1 while the read progresses
  dmaWaitAndCheck();

  // Verify I2C transmission
  I2CWriteVerify = true;

  for (i = 0; i < I2C_BUFFER_SIZE; i++) {
    if (i2c_txBuffer[i] != i2c_rxBuffer[i]) {
      I2CWriteVerify = false;
      break;
    }
  }

  return I2CWriteVerify;
}

/**************************************************************************//**
 * @brief LDMA interrupt handler
 *
 * @note Even though no LDMA interrupts are used to manage the I2C
 *       transfers, the LDMA interrupt is enabled when LDNA_Init() is
 * called and the error request is always enabled.  This is helpful for
 * debugging LDMA transfers that might attempt to access unimplemented
 * memory (e.g. an uninitialized pointer).  While the CPU would hard
 * fault in such a case, the LDMA cannot cause faults, so the error
 * interrupt is requested instead.
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t flags = LDMA_IntGetEnabled();

  if (flags & LDMA_IF_ERROR) {
    // Breakpoint on error
    __BKPT(1);
  } else {
    // A channel completion interrupt shouldn't happen in this example
    __BKPT(2);
  }
}
/**************************************************************************//**
 * @brief GPIO interrupt handler
 *****************************************************************************/
#if (BSP_GPIO_PB0_PIN & 1)
void GPIO_ODD_IRQHandler(void)
#else
void GPIO_EVEN_IRQHandler(void)
#endif
{
  // Clear pending
  uint32_t flags = GPIO_IntGet();
  GPIO_IntClear(flags);

  // Button 0 pressed; start the read/write/verify test
  i2c_startTx = true;
}

/***************************************************************************//**
 * @brief Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize peripherals
  initGPIO();
  initI2C();

  LDMA_Init_t initLdma = LDMA_INIT_DEFAULT;
  LDMA_Init(&initLdma);

  while (1) {
    if (i2c_startTx)
    {
      // Run test and check for success
      if (!testI2C())
      {
        // Indicate error with LED1 and breakpoint on failure
        GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
        __BKPT(3);
      }
      else
      {
        // Toggle LED0 on each pass
        GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

        // Transmission complete; set to run test again
        i2c_startTx = false;
      }
    }
  }
}
