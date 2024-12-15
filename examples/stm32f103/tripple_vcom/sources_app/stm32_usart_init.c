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
#include "stm32f10x_conf.h"

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_nvic.h"
#include "iocmd.h"
#include "memconfig.h"

void configure_usart(uint8_t usart_num, uint32_t baudrate, uint16_t stop_bits, uint16_t patity, uint8_t enable_rx_irq, uint8_t enable_tx_irq)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
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
         NVIC_IRQChannel = USART1_IRQChannel;
//         NVIC_IRQChannel = USART1_IRQn;

         // reset peripheral and enable its clock
         RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
         RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      }
      else if(usart_num == 2)
      {
         USARTx = USART2;
         GPIO_Rx_Port = USART2_RX_PORT;
         GPIO_Tx_Port = USART2_TX_PORT;
         GPIO_Rx_Pin  = USART2_RX_PIN;
         GPIO_Tx_Pin  = USART2_TX_PIN;
         NVIC_IRQChannel = USART2_IRQChannel;
//         NVIC_IRQChannel = USART2_IRQn;

         // reset peripheral and enable its clock
         RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);
         RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      }
      else if(usart_num == 3)
      {
         USARTx = USART3;
         GPIO_Rx_Port = USART3_RX_PORT;
         GPIO_Tx_Port = USART3_TX_PORT;
         GPIO_Rx_Pin  = USART3_RX_PIN;
         GPIO_Tx_Pin  = USART3_TX_PIN;
         NVIC_IRQChannel = USART3_IRQChannel;
//         NVIC_IRQChannel = USART3_IRQn;

         // reset peripheral and enable its clock
         RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
         RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      }
      else
      {
         return;
      }
   }


   // configure GPIO
   {
      GPIO_InitStructure.GPIO_Pin   = GPIO_Tx_Pin;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
      GPIO_Init(GPIO_Tx_Port, &GPIO_InitStructure);

      GPIO_InitStructure.GPIO_Pin = GPIO_Rx_Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
      GPIO_Init(GPIO_Rx_Port, &GPIO_InitStructure);
   }


   // configure USART peripheral
   // and enable it
   {
      USART_InitStructure.USART_BaudRate   = baudrate;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits   = stop_bits;
      USART_InitStructure.USART_Parity     = patity ;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

      // Initialize registers
      USART_Init(USARTx, &USART_InitStructure);
      // Enable the USART
      USART_Cmd(USARTx, ENABLE);
   }


   // Enable the USART RX Interrupt
   if(enable_rx_irq || enable_tx_irq)
   {
      NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);

      // Configure the transmit and receive ISR
      if(enable_tx_irq)
      {
         USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
      }
      if(enable_rx_irq)
      {
         USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
      }
   }
}
