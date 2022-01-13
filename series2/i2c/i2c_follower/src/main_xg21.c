/***************************************************************************//**
 * @file main_xg21.c
 * @brief This project demonstrates the follower configuration of the EFx32xG21
 * I2C peripheral. Two EFx32xG2x devices are connected together, one running the
 * leader project, the other running the follower project. The leader reads the
 * follower's current buffer values, increments each value, and writes the new
 * values back to the follower device. The leader then reads back the follower
 * values again and verifies that the new values match what was previously
 * written. This program runs in a continuous loop, entering and exiting EM2 to
 * handle I2C transmissions. Follower toggles LED0 on during I2C transaction and
 * off when complete. Follower will set LED1 if an I2C transmission error is
 * encountered.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rtcc.h"
#include "em_core.h"
#include "bsp.h"

// Defines
#define I2C_ADDRESS                     0xE2
#define I2C_BUFFER_SIZE                 10

// Buffers
uint8_t i2c_Buffer[I2C_BUFFER_SIZE];
uint8_t i2c_BufferIndex;

// Transmission flags
volatile bool i2c_gotTargetAddress;
volatile bool i2c_rxInProgress;

/***************************************************************************//**
 * @brief GPIO initialization
 ******************************************************************************/
void initGPIO(void)
{
  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
}

/***************************************************************************//**
 * @brief RTCC initialization
 ******************************************************************************/
void initRTCC(void)
{
  // Select LFRCO as source for RTCC clock branch
  CMU_ClockSelectSet(cmuClock_RTCCCLK, cmuSelect_LFRCO);

  // Setup RTCC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;

  // Initialize RTCC
  RTCC_Reset();
  RTCC_Init(&rtccInit);
}

/***************************************************************************//**
 * @brief Setup I2C
 ******************************************************************************/
void initI2C(void)
{
  // Use default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

  // Configure to be addressable as follower
  i2cInit.master = false;

  // Enable GPIO pins PA5 (SDA) and PA6 (SCL)
  GPIO_PinModeSet(gpioPortA, 5, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(gpioPortA, 6, gpioModeWiredAndPullUpFilter, 1);

  // Route I2C pins to GPIO
  GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
                        | (gpioPortA << _GPIO_I2C_SDAROUTE_PORT_SHIFT
                        | (5 << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
                        | (gpioPortA << _GPIO_I2C_SCLROUTE_PORT_SHIFT
                        | (6 << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

  // Initialize the I2C
  I2C_Init(I2C0, &i2cInit);

  // Initialize the buffer index
  i2c_BufferIndex = 0;

  // Set up to enable follower mode
  I2C_SlaveAddressSet(I2C0, I2C_ADDRESS);
  I2C_SlaveAddressMaskSet(I2C0, 0xFE); // must match exact address

  // Configure interrupts
  I2C_IntClear(I2C0, _I2C_IF_MASK);
  I2C_IntEnable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_ACK | I2C_IEN_SSTOP | I2C_IEN_BUSERR | I2C_IEN_ARBLOST);
  NVIC_EnableIRQ(I2C0_IRQn);
}

/***************************************************************************//**
 * @brief I2C Interrupt Handler
 ******************************************************************************/
void I2C0_IRQHandler(void)
{
  uint32_t pending;
  uint32_t rxData;

  pending = I2C0->IF;

  // If some sort of fault, abort transfer.
  if (pending & (I2C_IF_BUSERR | I2C_IF_ARBLOST)) {
    i2c_rxInProgress = false;
    GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  } else {
    if (pending & I2C_IF_ADDR) {
      // Address Match, indicating that reception is started
      rxData = I2C0->RXDATA;
      I2C0->CMD = I2C_CMD_ACK;
      i2c_rxInProgress = true;

      if (rxData & 0x1) { // read bit set
        if (i2c_BufferIndex < I2C_BUFFER_SIZE) {
          // Transfer data
          I2C0->TXDATA     = i2c_Buffer[i2c_BufferIndex++];
        } else {
          // Invalid buffer index; transfer data as if follower non-responsive
          I2C0->TXDATA     = 0xFF;
        }
      } else {
        i2c_gotTargetAddress = false;
      }

      I2C_IntClear(I2C0, I2C_IF_ADDR | I2C_IF_RXDATAV);

      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    } else if (pending & I2C_IF_RXDATAV) {
      rxData = I2C0->RXDATA;

      if (!i2c_gotTargetAddress) {
        /******************************************************/
        /* Read target address from leader                    */
        /******************************************************/
        // Verify that target address is valid
        if (rxData < I2C_BUFFER_SIZE) {
          // Store target address
          i2c_BufferIndex = rxData;
          I2C0->CMD = I2C_CMD_ACK;
          i2c_gotTargetAddress = true;
        } else {
          I2C0->CMD = I2C_CMD_NACK;
        }
      } else {
        /******************************************************/
        /* Read new data and write to target address          */
        /******************************************************/
        // Verify that target address is valid
        if (i2c_BufferIndex < I2C_BUFFER_SIZE) {
          // Write new data to target address; auto increment target address
          i2c_Buffer[i2c_BufferIndex++] = rxData;
          I2C0->CMD = I2C_CMD_ACK;
        } else {
          I2C0->CMD = I2C_CMD_NACK;
        }
      }

      I2C_IntClear(I2C0, I2C_IF_RXDATAV);
    }

    if (pending & I2C_IF_ACK) {
      /******************************************************/
      /* Leader ACK'ed, so requesting more data             */
      /******************************************************/
      if (i2c_BufferIndex < I2C_BUFFER_SIZE) {
        // Transfer data
        I2C0->TXDATA = i2c_Buffer[i2c_BufferIndex++];
      } else {
        // Invalid buffer index; transfer data as if follower non-responsive
        I2C0->TXDATA = 0xFF;
      }

      I2C_IntClear(I2C0, I2C_IF_ACK);
    }

    if (pending & I2C_IF_SSTOP) {
      // End of transaction
      i2c_rxInProgress = false;

      I2C_IntClear(I2C0, I2C_IF_SSTOP);
    }
  }
}

/***************************************************************************//**
 * @brief Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize the RTCC
  initRTCC();

  // Initialize the I2C
  initGPIO();
  initI2C();

  while (1) {
    // Receiving I2C data; keep in EM1 during transmission
    while(i2c_rxInProgress) {
      EMU_EnterEM1();
    }

    // EM2 entry is a critical section and interrupts are disabled to prevent
    // race conditions
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();

    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Enter EM2; an I2C address match will wake up the device
    EMU_EnterEM2(true);

    CORE_EXIT_CRITICAL();
  }
}
