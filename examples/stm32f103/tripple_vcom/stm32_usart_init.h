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

#ifndef STM32_USART_INIT_H_
#define STM32_USART_INIT_H_

#include <stdint.h>

#include <string.h>
#include "iocmd.h"


#define USART1_TX_PORT           GPIOA
#define USART1_TX_PIN            GPIO_Pin_9

#define USART1_RX_PORT           GPIOA
#define USART1_RX_PIN            GPIO_Pin_10



#define USART2_TX_PORT           GPIOA
#define USART2_TX_PIN            GPIO_Pin_2

#define USART2_RX_PORT           GPIOA
#define USART2_RX_PIN            GPIO_Pin_3



#define USART3_TX_PORT           GPIOB
#define USART3_TX_PIN            GPIO_Pin_10

#define USART3_RX_PORT           GPIOB
#define USART3_RX_PIN            GPIO_Pin_11



void configure_usart(uint8_t usart_num, uint32_t baudrate, uint16_t stop_bits, uint16_t patity, uint8_t enable_rx_irq, uint8_t enable_tx_irq);

#endif /* STM32_USART_INIT_H_ */
