/**************************************************************************//**
 * @file
 * @brief This project demonstrates the use of the RTCC by taking user input
 * from the VCOM and setting an alarm to trigger at the specified date and
 * time.
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
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_gpio.h"
#include "retargetserial.h"
#include "retargetserialconfig.h"

uint32_t rtccFlag;
int start = -1;
int strtol(char*,char **,int);

/**************************************************************************//**
 * @brief RTCC interrupt service routine
 *****************************************************************************/
void RTCC_IRQHandler(void)
{
  // Read the interrupt source
  rtccFlag = RTCC_IntGet();

  // Toggle LED to turn it on
  GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN); // LEDS active low
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Turn on the clock for the GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable LED1
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
  GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN); // LEDs active low
}

/**************************************************************************//**
 * @brief Functions to take input from VCOM
 *****************************************************************************/
char* setCurrentTime(char hour[])
{
  // Ask the user for input
  printf("\nPlease enter the current time in the format hhmmss:\n ");

  // Receive the input from the VCOM and store as a string
  for(int i = 0; i < 6;i++)
  {
    while(start == -1)
    {
      scanf("%d", &start);
      if(start != -1)
      {
        start = start + '0';
        hour[i+2] = start;
      }
    }
    start = -1;
  }
  return hour;
}

char* setCurrentDate(char date[])
{
  // Ask the user for input
  printf("Please enter the current date in the format yymmdd:\n ");

  // Receive the input from the VCOM and store as a string
  for(int i = 0; i < 6;i++)
  {
    while(start == -1)
    {
      scanf("%d", &start);
      if(start != -1)
      {
        start = start + '0';
        date[i+2] = start;
      }
    }
    start = -1;
  }
  return date;
}

char* setAlarmTime(char alarmh[])
{
  // Ask the user for input
  printf("\nPlease enter the time for the alarm in the format hhmmss:\n ");

  // Receive the input from the VCOM and store as a string
  for(int i = 0; i < 6;i++)
  {
    while(start == -1)
    {
      scanf("%d", &start);
      if(start != -1)
      {
        start = start + '0';
        alarmh[i+2] = start;
      }
    }
    start = -1;
  }
  return alarmh;
}

char* setAlarmDate(char alarmd[])
{
  // Ask the user for input
  printf("\nPlease enter the date for the alarm in the format yymmdd:\n ");

  // Receive the input from the VCOM and store as a string
  for(int i = 0; i <6;i++)
  {
    while(start == -1)
    {
      scanf("%d", &start);
      if(start != -1)
      {
        start = start + '0';
        alarmd[i+2] = start;
      }
    }
    start = -1;
  }
  return alarmd;
}

/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void rtccSetup(int start_time, int start_date, int alarmh_start, int alarmd_start)
{
  // Configure the RTCC settings
  RTCC_Init_TypeDef rtcc = RTCC_INIT_DEFAULT;
  rtcc.enable   = false;
  rtcc.presc = rtccCntPresc_32768;
  rtcc.cntMode = rtccCntModeCalendar;
  rtcc.cntWrapOnCCV1 = true;

  // Configure the compare settings
  RTCC_CCChConf_TypeDef compare = RTCC_CH_INIT_COMPARE_DEFAULT;

  // Turn on the clock for the RTCC
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Initialise RTCC with pre-defined settings
  RTCC_Init(&rtcc);

  // Set current date and time
  RTCC_DateSet(start_date);
  RTCC_TimeSet(start_time);

  // Initialise RTCC compare with a date, the date when interrupt will occur
  RTCC_ChannelInit(1, &compare);
  RTCC_ChannelDateSet(1, alarmd_start);
  RTCC_ChannelTimeSet(1,alarmh_start);

  // Set channel 1 to cause an interrupt
  RTCC_IntEnable(RTCC_IEN_CC1);
  NVIC_ClearPendingIRQ(RTCC_IRQn);
  NVIC_EnableIRQ(RTCC_IRQn);

  // Start counter after all initialisations are complete
  RTCC_Enable(true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Initialise USART, enable board controller VCOM function, and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
  RETARGET_ReadChar();

  // Initialise the variables needed to find the VCOM input
  char hour_time[] = "00hhmmss";
  char date[] = "20yymmdd";
  char alarmh[] = "00hhmmss";
  char alarmd[] = "20yymmdd";

  // Print the initial statement to the VCOM
  printf("This example works to set the time and then a future wake up time\n");

  // Store the strings with the timing information
  char *dates = setCurrentDate(date);
  char *hour = setCurrentTime(hour_time);
  char *alarm_date = setAlarmDate(alarmd);
  char *alarm_time = setAlarmTime(alarmh);

  printf("\nThe alarm has been set\n");

  // Convert the strings received into hex for passing to the RTCC
  int start_time = (int)strtol(hour,NULL,16);
  int start_date = (int)strtol(dates,NULL,16);
  int alarmh_start = (int)strtol(alarm_time,NULL,16);
  int alarmd_start = (int)strtol(alarm_date,NULL,16);

  // Initialisations
  initGPIO();

  rtccSetup(start_time, start_date,alarmh_start,alarmd_start);

  // Infinite loop
  while(1)
  {
    EMU_EnterEM1();
  }
}
