/***************************************************************************//**
 * @file main_gpio_prs_s2.c
 * @brief GPIO Peripheral Reflex System (PRS) setup
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
#include "em_iadc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_chip.h"
#include "bsp.h"
#include "em_timer.h"
#include "retargetserial.h"
#include <stdio.h>

#define ADC_PORT        gpioPortC 
#define ADC_PIN         3

#define PRS_CHANNEL     2

#define clksrcadc 4000000 // The HFRCODPLL (CLK_CMU_ADC) is running at 19 MHz.
                          // The HSCLKRATE is set to 3 (DIV4).
                          // CLK_SRC_ADC is 4 MHz

volatile IADC_Result_t adcResult;

/**************************************************************************//**
 * @brief  Delay function 
 * Sit in a tight wait loop to create "milliseconds" of delay
 *****************************************************************************/
void Delay(uint16_t milliseconds)
{
  uint16_t countMs=0;

  /* Enable & Clear overflow interrupt to know get 1ms indication */
  TIMER_IntEnable(TIMER0, TIMER_IEN_OF);
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  
  /* Enable timer */
  TIMER_Enable(TIMER0, true);
  
  /* Count every ms while waiting for "milliseconds" ms to elapse */
  while(countMs < milliseconds)
  {
    if(TIMER_IF_OF == (TIMER_IntGet(TIMER0) & _TIMER_IF_OF_MASK))
    {
      countMs++;
      TIMER_IntClear(TIMER0, TIMER_IF_OF);
    }  
  }
  
  /* Disable timer */
  TIMER_Enable(TIMER0, false);
}
  
/**************************************************************************//**
 * @brief Setup GPIO for button, ADC and PRS
 *****************************************************************************/
static void gpioSetup(void)
{
  /* Configure GPIO ad PRS Clock */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  /* --- BUTTON SETUP ---- */
  /* Configure Button PB0 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, false, true, true);

  /* --- ADC - GPIO SETUP ---- */
  /* Configure analog input - Set port pin to disabled */
  GPIO_PinModeSet(ADC_PORT, ADC_PIN, gpioModeDisabled, 1);

  /* Set xBUSALLOC to grant access to ADC0 -- no emlib function to do this */
  /* Chosen ADC port pin is on port c, so CDBUS needs to be set up to route to ADC0 */
  GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0; 

  /* Set up PRS GPIO and ADC as producer and consumer respectively */
  PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO, BSP_GPIO_PB0_PIN);
  PRS_ConnectConsumer(PRS_CHANNEL, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
}

/**************************************************************************//**
 * @brief Setup ADC measurement on ADC_PORT and ADC_PIN
 *****************************************************************************/
static void adcSetup(void)
{
  /* Configure IADC Clock */
  CMU_ClockEnable(cmuClock_IADC0, true);

  /* Configure IADC clock source for use while in EM2 */
  /* Note that HFRCOEM23 is the lowest frequency source available for the IADC */
#ifdef BSP_WSTK_BRD4181A
//  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_HFRCOEM23);
#else
    CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);
#endif

  IADC_Init_t init = IADC_INIT_DEFAULT;
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, clksrcadc, 0);

  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;/* Use unbuffered AVDD as reference */
  initAllConfigs.configs[0].adcClkPrescale = 9;/* Divides CLK_SRC_ADC by 10 */
  
  /* Initialize the IADC */
  IADC_init(IADC0, &init, &initAllConfigs);
  
  /* Assign pins to positive and negative inputs in differential mode */
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;
  initSingleInput.posInput = iadcPosInputPortCPin3;    // PC03 - EXP_HEADER10
 
  /* Initialize the Single conversion inputs */
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  initSingle.triggerSelect = iadcTriggerSelPrs0PosEdge;
  initSingle.triggerAction = iadcTriggerActionOnce;
  initSingle.start = true;
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  /* Enable ADC Interrupt when Single Conversion Complete */
  IADC_enableInt(IADC0, IADC_IEN_SINGLEDONE);
  
  /* Enable ADC interrupt vector in NVIC*/
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief Timer Initialization
 * Interrupt Service Routine for ADC
 *****************************************************************************/
void timerSetup(void)
{ 
  /* Configure Timer Clock */
  CMU_ClockEnable(cmuClock_TIMER0, true);

  /* Set Timer0 to toggle PA0 at 1 MHz - Select clock source at fixed 20MHz FSRCO */
  CMU_ClockSelectSet(cmuClock_TIMER0, cmuSelect_FSRCO); 
  
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.prescale = (TIMER_Prescale_TypeDef) _TIMER_CFG_PRESC_DIV2;
  TIMER_Init(TIMER0, &init);

  /* Timer runs at 10MHz, 1 tick = 0.1 uS. How many ticks = 1ms ? */
  TIMER_TopSet(TIMER0, 0x270F); 
}

/**************************************************************************//**
 * @brief ADC0_IRQHandler
 * Interrupt Service Routine for ADC
 *****************************************************************************/
void IADC_IRQHandler(void)
{ 
  /* Clear ADC0 interrupt flag */
  IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);
  
  /* Read conversion result to clear Single Data Valid flag */
  adcResult = IADC_readSingleResult(IADC0);
}

/**************************************************************************//**
 * @brief  Main function
 * Main is called from __iar_program_start, see assembly startup file
 *****************************************************************************/
int main(void)
{  
  CHIP_Init();
  RETARGET_SerialInit();

  gpioSetup();
  adcSetup();
  timerSetup();
  
  while(1)
  {
    /* Enter EM1 */ 
    EMU_EnterEM1();
    printf("woke up");

    /* After ADC Single conversion wake up -> Delay 1000mS */  
    Delay(1000);
    printf("ADC result = %u  - going back to sleep\r\n", (unsigned int) adcResult.data);
  }
}
