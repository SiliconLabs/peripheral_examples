#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_emu.h"
#include "bsp.h"
#include "stdio.h"

#if defined(BSP_STK_BRD2204A) || defined(BSP_STK_BRD2102A) /* GG11 or TG11 STK square wave on PD1 */
#define SQUARE_WAVE_PORT gpioPortD
#define SQUARE_WAVE_PIN  1
#define SQUARE_WAVE_LOC TIMER_ROUTELOC0_CC0LOC_LOC2
#else /* All other STKs, square wave on PA0 */
#define SQUARE_WAVE_PORT gpioPortA
#define SQUARE_WAVE_PIN  0
#define SQUARE_WAVE_LOC TIMER_ROUTELOC0_CC0LOC_LOC0
#endif

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons and PA0 output.
 *****************************************************************************/
static void gpioSetup(void)
{
  // Enable GPIO clock.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure PB1 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // Set PA0 to push-pull output
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(SQUARE_WAVE_PORT, SQUARE_WAVE_PIN, gpioModePushPull, 1);
}

GPIO_DriveStrength_TypeDef driveStrength = gpioDriveStrengthStrongAlternateStrong;
uint32_t slewRate = 6;

/**************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons).
 *        PB0 Cycles through slew rate settings.
 *        PB1 Cycles through drive strength settings.
 *****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  // Get and clear all pending GPIO interrupts.
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  // Act on interrupts.
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    // BTN0: Slew Rate
    slewRate += 1;
    if (slewRate > 7)
    {
      slewRate = 0;
    }
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    // BTN1: cycle through drive strength settings.
    if (driveStrength == gpioDriveStrengthStrongAlternateStrong)
    {
      driveStrength = gpioDriveStrengthWeakAlternateWeak;
    }
    else
    {
      driveStrength = gpioDriveStrengthStrongAlternateStrong;
    }
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

#define OUTPUT_FREQ_Hz (1000000UL)
/**************************************************************************//**
 * @brief Timer setup to output a square wave on PA0
 *****************************************************************************/
void timerSetup(void)
{
  // Set Timer0 to toggle PA0 at 1 MHz
  CMU_ClockEnable(cmuClock_TIMER0, true);
  uint32_t hfperFreq = CMU_ClockFreqGet(cmuClock_HFPER);
  TIMER_TopBufSet(TIMER0, (hfperFreq / (2 * OUTPUT_FREQ_Hz)));
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER0, &init);

  TIMER_InitCC_TypeDef initCh = TIMER_INITCC_DEFAULT;
  initCh.cofoa = timerOutputActionToggle;
  initCh.mode = timerCCModeCompare;
  TIMER_InitCC(TIMER0, 0, &initCh);

  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER0->ROUTELOC0 |= SQUARE_WAVE_LOC;
}

/**************************************************************************//**
 * @brief  Main function
 *
 * This example demonstrates the slew rate settings for the GPIO. A 1 MHz
 * square wave is generated on a pin. The slew rate setting is changed by
 * pressing BTN0. The drive strength setting is changed by pressing BTN1. The
 * effects of these settings on the pin's slew rate can be observed by placing
 * a 50 pF capacitor on the output pin and observing the change in rise and
 * fall times of the 1 MHz square wave.
 *
 * Output pin is PD1 on SLSTK3701 (EFM32GG11 STK) and SLSTK3301 (EFM32TG11 STK)
 * and PA0 on other kits.
 *
 * To test:
 * 1. Place a 50 pF capacitor between the square wave output and GND.
 * 2. Upload and run the example.
 * 3. While observing the rise and fall times of the waveform on PA0/PD1:
 *    a. Press BTN0 to change the slew rate setting
 *    b. Press BTN1 to change the drive strength setting
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize Push Buttons and PA0 */
  gpioSetup();

  /* Initialize PA0 square-wave output */
  timerSetup();

  /* Infinite loop */
  while (1)
  {
    /* Update PA0 drive strength and slew rate */
    GPIO_DriveStrengthSet(SQUARE_WAVE_PORT, driveStrength);
    GPIO_SlewrateSet(SQUARE_WAVE_PORT, slewRate, slewRate);

    /* Enter EM1 until a push-button interrupt triggers a wake-up */
    EMU_EnterEM1();
  }
}
