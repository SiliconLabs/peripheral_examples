/***************************************************************************//**
 * @file  cdc.h
 * @brief USB Communication Device Class (CDC) driver
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef CDC_H
#define CDC_H

#ifdef __cplusplus
extern "C" {
#endif

void cdcInit(void);
int  cdcSetupCmd(const USB_Setup_TypeDef *setup);
void cdcStateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

#ifdef __cplusplus
}
#endif

#endif // CDC_H
