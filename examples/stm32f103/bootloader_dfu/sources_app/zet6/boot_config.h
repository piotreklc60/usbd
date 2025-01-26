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

#ifndef BOOT_CONFIG_H_
#define BOOT_CONFIG_H_

#include "memconfig.h"

/****************** USER APPLICATION ADDRESS SETTINGS *****************
 * Here we can change location of the user application.
 * By default USER APPLICATION is followed by the bootloader,
 * so this address is bootloader address + 10kB
 * (0x08000000 - bootloader address, 0x00002800 - 10kB relocation).
 */
#define DEF_APP_ADDRESS                APP_START



/********************* SUPPORTED FEATURES SETTINGS ********************
 * Here we can change which features of the bootloader
 * we want to compile. If we want to turn ON some of them,
 * we have to define support for it as 1, otherwise 0
 */

#define BOOT_CALL_BY_RESET_SUPPORT     1
#define BOOT_LED_NOTOFICATION_SUPPORT  1

/***************************** LED SETTINGS ***************************
 * Here we can change LED pin location and output mode
 */

#define LED1_PORT                      GPIOA
#define LED1_PIN                       8
#define LED1_PIN_MODE                  GPIO_MODE_OUTPUT_PP
#define LED1_PIN_PULL                  GPIO_PULLUP
#define LED1_ACTIVE_STATE              0

/**************************** FORCE SETTINGS **************************
 * Here we can change location and mode of the pin used to force
 * reprogramming flash memory by the bootloader
 */

#define FORCE_PORT                     GPIOA
#define FORCE_PIN                      0
#define FORCE_PIN_MODE                 GPIO_MODE_INPUT
#define FORCE_PIN_PULL                 GPIO_PULLDOWN
#define FORCE_ACTIVE_STATE             1


/********************** USBD PULL_UP PIN SETTINGS *********************
 * Here we can change location and mode of the pin used to control
 * connectivity of the PULL-UP resistor 1k5 Ohm between
 * USB 5V Power line and D+ line
 */

/* defined in usbd_port_stm32_cat_a_board_settings_cfg.h */



#if(LED1_ACTIVE_STATE == 0)
#define LED_OFF()       LED1_PORT->BSRR = 1 << LED1_PIN
#define LED_ON()        LED1_PORT->BRR  = 1 << LED1_PIN
#else
#define LED_OFF()       LED1_PORT->BRR  = 1 << LED1_PIN
#define LED_ON()        LED1_PORT->BSRR = 1 << LED1_PIN
#endif



#if(0 != FORCE_ACTIVE_STATE)
#define IS_FORCE_ACTIVE()     (0 != (FORCE_PORT->IDR & (1 << FORCE_PIN)))
#else
#define IS_FORCE_ACTIVE()     (0 == (FORCE_PORT->IDR & (1 << FORCE_PIN)))
#endif



/***************************** STACK SIZE *****************************
 * Bootloader Stack Size
 */
#define STACK_SIZE                     1024


#endif /*BOOT_CONFIG_H_*/
