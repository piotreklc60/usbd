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

#include "stm32_usart_init.h"
#include "stm32f1xx_hal.h"
#include "iocmd.h"
#include "memconfig.h"

void configure_usart(uint8_t usart_num, uint32_t baudrate, uint16_t stop_bits, uint16_t patity, uint8_t enable_rx_irq, uint8_t enable_tx_irq)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   UART_HandleTypeDef USART_InitStructure;
   USART_TypeDef* USARTx;
   GPIO_TypeDef* GPIO_Rx_Port;
   GPIO_TypeDef* GPIO_Tx_Port;
   uint16_t GPIO_Rx_Pin;
   uint16_t GPIO_Tx_Pin;
   uint8_t NVIC_IRQChannel;


   // identify USART number, reset the peripheral and enable its clock
   {
      if(usart_num == 1)
      {
         USARTx = USART1;
         GPIO_Rx_Port = USART1_RX_PORT;
         GPIO_Tx_Port = USART1_TX_PORT;
         GPIO_Rx_Pin  = USART1_RX_PIN;
         GPIO_Tx_Pin  = USART1_TX_PIN;
         NVIC_IRQChannel = USART1_IRQn;

         // reset peripheral and enable its clock
         __HAL_RCC_USART1_FORCE_RESET();
         __HAL_RCC_USART1_CLK_ENABLE();
         __HAL_RCC_USART1_RELEASE_RESET();
      }
      else if(usart_num == 2)
      {
         USARTx = USART2;
         GPIO_Rx_Port = USART2_RX_PORT;
         GPIO_Tx_Port = USART2_TX_PORT;
         GPIO_Rx_Pin  = USART2_RX_PIN;
         GPIO_Tx_Pin  = USART2_TX_PIN;
         NVIC_IRQChannel = USART2_IRQn;

         // reset peripheral and enable its clock
         __HAL_RCC_USART2_FORCE_RESET();
         __HAL_RCC_USART2_CLK_ENABLE();
         __HAL_RCC_USART2_RELEASE_RESET();
      }
      else if(usart_num == 3)
      {
         USARTx = USART3;
         GPIO_Rx_Port = USART3_RX_PORT;
         GPIO_Tx_Port = USART3_TX_PORT;
         GPIO_Rx_Pin  = USART3_RX_PIN;
         GPIO_Tx_Pin  = USART3_TX_PIN;
         NVIC_IRQChannel = USART3_IRQn;

         // reset peripheral and enable its clock
         __HAL_RCC_USART3_FORCE_RESET();
         __HAL_RCC_USART3_CLK_ENABLE();
         __HAL_RCC_USART3_RELEASE_RESET();
      }
      else
      {
         return;
      }
   }


   // configure GPIO
   {
      GPIO_InitStructure.Pin   = GPIO_Tx_Pin;
      GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
      GPIO_InitStructure.Pull  = GPIO_NOPULL;
      HAL_GPIO_Init(GPIO_Tx_Port, &GPIO_InitStructure);

      GPIO_InitStructure.Pin   = GPIO_Rx_Pin;
      GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
      GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
      HAL_GPIO_Init(GPIO_Rx_Port, &GPIO_InitStructure);
   }


   // configure USART peripheral
   // and enable it
   {
      USART_InitStructure.Instance = USARTx;
      USART_InitStructure.Init.BaudRate = baudrate;
      USART_InitStructure.Init.WordLength = UART_WORDLENGTH_8B;
      USART_InitStructure.Init.StopBits = stop_bits;
      USART_InitStructure.Init.Parity = patity;
      USART_InitStructure.Init.Mode = UART_MODE_TX_RX;
      USART_InitStructure.Init.HwFlowCtl = UART_HWCONTROL_NONE;
      USART_InitStructure.Init.OverSampling = UART_OVERSAMPLING_16;
      if (HAL_UART_Init(&USART_InitStructure) != HAL_OK)
      {
         /* error */
      }
   }


   // Enable the USART RX Interrupt
   if(enable_rx_irq || enable_tx_irq)
   {
      HAL_NVIC_SetPriority(NVIC_IRQChannel, 2, 0);
      HAL_NVIC_EnableIRQ(NVIC_IRQChannel);

      // Configure the transmit and receive ISR
      if(enable_tx_irq)
      {
         USARTx->CR1 |= UART_FLAG_TXE;
      }
      if(enable_rx_irq)
      {
         USARTx->CR1 |= UART_FLAG_RXNE;
      }
   }
}
