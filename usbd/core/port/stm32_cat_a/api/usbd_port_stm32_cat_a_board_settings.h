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

#ifndef USBD_PORT_STM32_CAT_A_BOARD_SETTINGS_H_
#define USBD_PORT_STM32_CAT_A_BOARD_SETTINGS_H_

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE
#define USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE  USBD_FEATURE_PRESENT
#endif

#ifndef USBD_PORT_STM32_CAT_A_USE_PULL_UP
#define USBD_PORT_STM32_CAT_A_USE_PULL_UP         USBD_FEATURE_PRESENT
#endif

#ifndef USBD_PORT_STM32_CAT_A_LP_IRQ_PRIORITY
#define USBD_PORT_STM32_CAT_A_LP_IRQ_PRIORITY      3
#endif

#ifndef USBD_PORT_STM32_CAT_A_HP_IRQ_PRIORITY
#define USBD_PORT_STM32_CAT_A_HP_IRQ_PRIORITY      3
#endif


#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE)
/**
 * @brief if USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE is defined as USBD_FEATURE_PRESENT
 * this function is called on USBD_DEV_Activate/USBD_DEV_Deactivate to configure/deconfigure interruption
 * from 5V on USB bus detection
 *
 * @param configure if USBD_TRUE then IRQ shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Configure_Vbus_Detection_Irq(USBD_Bool_DT configure);

/**
 * @brief if USBD_PORT_STM32_CAT_A_DETECT_VBUS_CHANGE is defined as USBD_FEATURE_PRESENT
 * this function must be called from outside of port.
 * It must be called on every change of VBUS (USB 5V) - every time 5V becomes present and disapears.
 *
 * @param vbus_5V_present USBD_TRUE if 5V supply from USB bus becomes present; USBD_FALSE when 5V disapears.
 */
extern void USBD_Port_STM32_CAT_A_Vbus_Detection(USBD_Bool_DT vbus_5V_present);
#endif

#if (USBD_FEATURE_PRESENT == USBD_PORT_STM32_CAT_A_USE_PULL_UP)
/**
 * @brief if USBD_PORT_STM32_CAT_A_USE_PULL_UP is defined as USBD_FEATURE_PRESENT
 * this function is called on USBD_DEV_Activate/USBD_DEV_Deactivate to configure/deconfigure pin
 * used for controlling 1.5kOhm resistor on line D+. There shall be used open-drain or other control type
 * which ensures that not configured pin will disable resistor.
 *
 * @param configure
 */
extern void USBD_Port_STM32_CAT_A_Configure_Pull_Up(USBD_Bool_DT configure);

/**
 * @brief if USBD_PORT_STM32_CAT_A_USE_PULL_UP is defined as USBD_FEATURE_PRESENT
 * this function must be defined outside port - in main program. It is responsible
 * for attaching and detaching 1.5kOhm resistor on D+ line. This function is called from port.
 *
 * @param attach_pull_up_resistor if USBD_TRUE then pull_up resistor must be attached, otherwise - detached.
 */
extern void USBD_Port_STM32_CAT_A_Pull_Up_Change(USBD_Bool_DT attach_pull_up_resistor);
#endif

/**
 * @brief this function is called on USBD_DEV_Activate/USBD_DEV_Deactivate to configure/deconfigure
 * USBCLKSource prescaller, depends to used CPU frequency. It can be DIV1 or DIV1.5
 */
extern void USBD_Port_STM32_CAT_A_Configure_USBCLKSource(void);

/**
 * @brief this function is called on USBD_DEV_Activate/USBD_DEV_Deactivate to configure/deconfigure
 * USBWakeUp, USB_LP and USB_HP interruptions. These interruptions are implemented in port
 * but must be configured externally, depends to priority levels defined in application.
 *
 * @param configure if USBD_TRUE then IRQs shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Configure_USB_Irqs(USBD_Bool_DT configure);

#if((USBD_MAX_NUM_ENDPOINTS > 1) && (USBD_PORT_STM32_CAT_A_LP_IRQ_PRIORITY > USBD_PORT_STM32_CAT_A_HP_IRQ_PRIORITY))
/**
 * @brief this function is called in LP IRQ to disable HP IRQ when its priority is higher
 *
 * @param configure if USBD_TRUE then IRQs shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Disable_USB_HP_Irq(void);

/**
 * @brief this function is called in LP IRQ to disable HP IRQ when its priority is higher
 *
 * @param configure if USBD_TRUE then IRQs shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Enable_USB_HP_Irq(void);
#endif

#if(USBD_PORT_STM32_CAT_A_LP_IRQ_PRIORITY < USBD_PORT_STM32_CAT_A_HP_IRQ_PRIORITY)
/**
 * @brief this function is called in HP IRQ to disable LP IRQ when its priority is higher
 *
 * @param configure if USBD_TRUE then IRQs shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Disable_USB_LP_Irq(void);

/**
 * @brief this function is called in HP IRQ to disable LP IRQ when its priority is higher
 *
 * @param configure if USBD_TRUE then IRQs shall be enabled, disabled otherwise
 */
extern void USBD_Port_STM32_CAT_A_Enable_USB_LP_Irq(void);
#endif

#if(USBD_FEATURE_PRESENT == USBD_SUSPEND_RESUME_SUPPORTED)
/**
 * @brief clears status of USBWakeupIrq status in EXTI controller
 */
extern void USBD_Port_STM32_CAT_A_Clear_Exti_Line_18_Irq_Status(void);
#endif

#endif
