/***************************************************************************//**
 * @file em_prs.c
 * @brief Peripheral Reflex System (PRS) Peripheral API
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_prs.h"
#if defined(PRS_COUNT) && (PRS_COUNT > 0)

#include "em_assert.h"

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PRS
 * @brief Peripheral Reflex System (PRS) Peripheral API
 * @details
 *  This module contains functions to control the PRS peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs. The PRS allows configurable, fast, and autonomous
 *  communication between peripherals on the MCU or SoC.
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Set a source and signal for a channel.
 *
 * @param[in] ch
 *   A channel to define the signal and source for.
 *
 * @param[in] source
 *   A source to select for the channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   A signal (for selected @p source) to use. Use one of PRS_CH_CTRL_SIGSEL_x
 *   defines.
 *
 * @param[in] edge
 *   An edge (for selected source/signal) to generate the pulse for.
 ******************************************************************************/
void PRS_SourceSignalSet(unsigned int ch,
                         uint32_t source,
                         uint32_t signal,
                         PRS_Edge_TypeDef edge)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

  PRS->CH[ch].CTRL = (source & _PRS_CH_CTRL_SOURCESEL_MASK)
                     | (signal & _PRS_CH_CTRL_SIGSEL_MASK)
                     | (uint32_t)edge;
}

#if defined(PRS_CH_CTRL_ASYNC)
/***************************************************************************//**
 * @brief
 *   Set the source and asynchronous signal for a channel.
 *
 * @details
 *   Asynchronous reflexes are not clocked on HFPERCLK and can be used even in
 *   EM2/EM3.
 *   There is a limitation to reflexes operating in asynchronous mode in
 *   that they can only be used by a subset of the reflex consumers. See
 *   the PRS chapter in the reference manual for the complete list of
 *   supported asynchronous signals and consumers.
 *
 * @note
 *   This function is not supported on EFM32GxxxFyyy parts.
 *   In asynchronous mode, the edge detector only works in EM0 and should
 *   not be used. The EDSEL parameter in PRS_CHx_CTRL register is set to 0 (OFF)
 *   by default.
 *
 * @param[in] ch
 *   A channel to define the source and asynchronous signal for.
 *
 * @param[in] source
 *   A source to select for the channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   An asynchronous signal (for selected @p source) to use. Use one of the
 *   PRS_CH_CTRL_SIGSEL_x defines that support asynchronous operation.
 ******************************************************************************/
void PRS_SourceAsyncSignalSet(unsigned int ch,
                              uint32_t source,
                              uint32_t signal)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

  PRS->CH[ch].CTRL = PRS_CH_CTRL_ASYNC
                     | (source & _PRS_CH_CTRL_SOURCESEL_MASK)
                     | (signal & _PRS_CH_CTRL_SIGSEL_MASK)
                     | PRS_CH_CTRL_EDSEL_OFF;
}
#endif

#if defined(_PRS_ROUTELOC0_MASK) || (_PRS_ROUTE_MASK)
/***************************************************************************//**
 * @brief
 *   Send the output of a PRS channel to a GPIO pin.
 *
 * @details
 *   This function is used to send the output of a PRS channel to a GPIO pin.
 *   Note that there are certain restrictions to where a PRS channel can be
 *   routed. Consult the datasheet of the device to see if a channel can be
 *   routed to the requested GPIO pin.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] loc
 *   PRS routing location.
 ******************************************************************************/
void PRS_GpioOutputLocation(unsigned int ch,
                            unsigned int location)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

#if defined(_PRS_ROUTE_MASK)
  PRS->ROUTE |= (location << 8)
                | (1 << ch);
#else
  uint32_t shift = (ch % 4) * 8;
  uint32_t mask = location << shift;
  uint32_t locationGroup = ch / 4;
  /* Since all ROUTELOCx registers are in consecutive memory locations, we can treat them
   * as an array starting at ROUTELOC0 and use locationGroup to index into this array */
  volatile uint32_t * routeloc = &PRS->ROUTELOC0;
  routeloc[locationGroup] |= mask;
  PRS->ROUTEPEN |= 1 << ch;
#endif
}
#endif

/** @} (end addtogroup PRS) */
/** @} (end addtogroup emlib) */
#endif /* defined(PRS_COUNT) && (PRS_COUNT > 0) */
