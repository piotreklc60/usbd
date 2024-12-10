/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2024 Piotr Wojtowicz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * -----------------------------------------------------------------------------------------------------------------------------
 */

#ifndef USBD_PORT_STM32F103_BOARD_SETTINGS_CFG_H_
#define USBD_PORT_STM32F103_BOARD_SETTINGS_CFG_H_

#include <stdint.h>

#include <string.h>
#include "usbd.h"


#define USBD_PORT_STM32F103_DETECT_VBUS_CHANGE  USBD_FEATURE_PRESENT
#define USBD_PORT_STM32F103_USE_PULL_UP         USBD_FEATURE_PRESENT


#define USBD_DP_PULL_UP_PORT                    GPIOC
#define USBD_DP_PULL_UP_PIN                     GPIO_Pin_5
#define USBD_DP_PULL_UP_MODE                    GPIO_Mode_Out_PP
#define USBD_DP_PULL_UP_ACTIVE_STATE            0



#define USBD_VBUS_PORT                          GPIOC
#define USBD_VBUS_PIN                           GPIO_Pin_4
#define USBD_VBUS_MODE                          GPIO_Mode_IN_FLOATING
#define USBD_VBUS_ACTIVE_STATE                  1
// EXTI: VBUS
#define USBD_VBUS_GPIO_PortSource               GPIO_PortSourceGPIOC
#define USBD_VBUS_GPIO_PinSource                GPIO_PinSource4
#define USBD_VBUS_EXTI_Line                     EXTI_Line4
#define USBD_VBUS_EXTI_IRQChannel               EXTI4_IRQChannel
#define USBD_Port_STM32F103_Vbus_IrqHandler     EXTI4_IRQHandler

/* USB clock source */
#ifndef RCC_USBCLKSource_PLLCLK_1Div5
#define RCC_USBCLKSource_PLLCLK_1Div5   ((u8)0x00)
#endif
#ifndef RCC_USBCLKSource_PLLCLK_Div1
#define RCC_USBCLKSource_PLLCLK_Div1    ((u8)0x01)
#endif


#define USBD_RCC_USBCLKSource_PLLCLK_presc      RCC_USBCLKSource_PLLCLK_Div1
//#define USBD_RCC_USBCLKSource_PLLCLK_presc      RCC_USBCLKSource_PLLCLK_1Div5


#endif
