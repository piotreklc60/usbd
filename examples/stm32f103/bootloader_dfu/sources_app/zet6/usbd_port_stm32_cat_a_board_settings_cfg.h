/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2025 Piotr Wojtowicz
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

#ifndef USBD_PORT_STM32_CAT_A_BOARD_SETTINGS_CFG_H_
#define USBD_PORT_STM32_CAT_A_BOARD_SETTINGS_CFG_H_

#include <stdint.h>

#include <string.h>
#include "usbd.h"


#define USBD_DP_PULL_UP_PORT                    GPIOC
#define USBD_DP_PULL_UP_PIN                     5
#define USBD_DP_PULL_UP_MODE                    GPIO_MODE_OUTPUT_PP
#define USBD_DP_PULL_UP_PULL                    GPIO_PULLUP
#define USBD_DP_PULL_UP_PORT_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define USBD_DP_PULL_UP_ACTIVE_STATE            0



#define USBD_VBUS_PORT                          GPIOC
#define USBD_VBUS_PIN                           4
#define USBD_VBUS_MODE                          GPIO_MODE_IT_RISING_FALLING
#define USBD_VBUS_PULL                          GPIO_PULLUP
#define USBD_VBUS_PORT_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define USBD_VBUS_ACTIVE_STATE                  1
// EXTI: VBUS
#define USBD_VBUS_EXTI_IRQChannel               EXTI4_IRQn
#define USBD_Port_STM32_CAT_A_Vbus_IrqHandler   EXTI4_IRQHandler

/* USB clock source */
#ifndef RCC_USBCLKSource_PLLCLK_1Div5
#define RCC_USBCLKSource_PLLCLK_1Div5   ((uint8_t)0x00)
#endif
#ifndef RCC_USBCLKSource_PLLCLK_Div1
#define RCC_USBCLKSource_PLLCLK_Div1    ((uint8_t)0x01)
#endif


#define USBD_RCC_USBCLKSource_PLLCLK_presc      RCC_USBCLKSource_PLLCLK_Div1
//#define USBD_RCC_USBCLKSource_PLLCLK_presc      RCC_USBCLKSource_PLLCLK_1Div5


#endif
