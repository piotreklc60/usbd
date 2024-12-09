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

#include <stdint.h>
#include <string.h>

#include "os.h"
#include "stm32f10x_lib.h"
#include "gpio.h"
#include "stm32_usart_init.h"
#include "iocmd.h"
#include "cpu_mem_defs.h"


#include "usbd.h"
#include "main_usbd_init.h"

#include "iocmd_exe_usart.h"
#include "iocmd_exe_vcom.h"
#include "cmd_line.h"


int critical_global_cntr = 0;


void system_initialize(void);
void led_init(void);

const IOCMD_Print_Exe_Params_XT *logs_exe = NULL;
void main_set_logs_exe(const IOCMD_Print_Exe_Params_XT *exe)
{
   logs_exe = exe;
}


void Vcom_0_On_Write(Buff_Ring_XT *buf, Buff_Ring_Extensions_XT *extension, Buff_Size_DT size, Buff_Bool_DT rewind_occured)
{
   uint8_t data;
   if(0 != Buff_Ring_Read(buf, &data, sizeof(data), BUFF_TRUE))
   {
      extension->on_write = BUFF_MAKE_INVALID_HANDLER(Buff_Ring_Extension_On_Write);
      USART_SendData(USART1, (u16)data);
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
   }
}

// -----------------------------------------------------------------------------
//  USART1 IRQ
// -----------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
   Buff_Ring_XT *buf;
   uint8_t data;

   if(0 != (USART1->SR & USART_FLAG_RXNE))
   {
      data = (uint8_t)USART_ReceiveData(USART1);

//      Cmd_Parse_Bytes(usart_get_exe(), &data, 1);

      /* transmit data received from UART to VCOM only if DTR line is set (VCOM is open) */
      if(CDC_VCOM_Get_Dtr(VCOM_UART))
      {
         Buff_Ring_Write(CDC_Vcom_Get_In_Buf(VCOM_UART), &data, sizeof(data), BUFF_FALSE, BUFF_TRUE);
      }

 //     Cmd_Parse_Byte(usart_get_exe(), data);
   }
   else if(USART_GetITStatus(USART1, USART_IT_TXE))
   {
      buf = CDC_Vcom_Get_Out_Buf(VCOM_UART);

      if(BUFF_CHECK_PTR(Buff_Ring_XT, buf))
      {
         ENTER_CRITICAL();
         if(0 == Buff_Ring_Read(buf, &data, sizeof(data), BUFF_FALSE))
         {
            buf->extension->on_write = Vcom_0_On_Write;

            EXIT_CRITICAL();

            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
         }
         else
         {
            EXIT_CRITICAL();

            USART_SendData(USART1, (u16)data);
         }
         USART_ClearITPendingBit(USART1, USART_IT_TXE);
      }
   }
}

// -----------------------------------------------------------------------------
//  TASK_LED
// -----------------------------------------------------------------------------
void TASK_LED(void *pvParameters)
{
   IOCMD_NOTICE(MAIN_TASK_LED, "led thread entered!");

   while(1)
   {
      IOCMD_NOTICE(MAIN_TASK_LED, "led ON!");
      LED_ON();
      OS_Sleep_Ms(500);
      IOCMD_NOTICE(MAIN_TASK_LED, "led FF!");
      LED_OFF();
      OS_Sleep_Ms(500);
   }
} /* TASK_LED */

// -----------------------------------------------------------------------------
//  Task_Logger_Commander
// -----------------------------------------------------------------------------
void Task_Logger_Commander(void *pvParameters)
{
   const IOCMD_Print_Exe_Params_XT *exe = vcom_get_exe();
   Buff_Ring_XT *out_buf;
   Buff_Size_DT size;
   uint8_t data[64];

   OS_Sleep_Ms(1);

   out_buf = CDC_Vcom_Get_Out_Buf(VCOM_CMD);

   while(1)
   {
      if(CDC_VCOM_Get_Dtr(VCOM_CMD))
      {
         proc_logs(logs_exe);
      }
      if(!Buff_Ring_Is_Empty(out_buf, BUFF_TRUE))
      {
         size = Buff_Ring_Read(out_buf, data, sizeof(data), BUFF_TRUE);
         Cmd_Parse_Bytes(exe, data, (size_t)size);
      }
      OS_Sleep_Ms(1);
   }
} /* Task_Logger_Commander */

// -----------------------------------------------------------------------------
//  main
// -----------------------------------------------------------------------------
int main(void)
{
   Buff_Ring_XT *out_buf;
   const IOCMD_Print_Exe_Params_XT *exe;
   // initialize clocks, interrupts and other things
   system_initialize();

   IOCMD_Logs_Init();

   led_init();
   LED_OFF();

   configure_usart(IOCMD_EXE_USART_NUM, 115200, USART_StopBits_1, USART_Parity_No, 1, 0);

   main_usbd_init();

   out_buf = CDC_Vcom_Get_Out_Buf(VCOM_UART);
   if(BUFF_CHECK_PTR(Buff_Ring_Extensions_XT, out_buf->extension))
   {
      out_buf->extension->on_write = Vcom_0_On_Write;
   }

   exe = vcom_get_exe();

   IOCMD_Install_Standard_Output(exe);
   logs_exe = exe;

   OS_Init();

   IOCMD_NOTICE(MAIN_TASK_USART, "Init USBD");

   // configure HCLK clock as SysTick clock source
   SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );


   // create FreeRTOS tasks
   if(OS_MAX_NUM_CONTEXTS == OS_Create_Thread(
      TASK_LED,
      NULL,
      "led1",
      configMINIMAL_STACK_SIZE * 4,
      tskIDLE_PRIORITY + 2))
   {
      // application should never get here, unless there is a memory allocation problem
      IOCMD_Printf("Task %s creation failed!\n\r", "led1");
   }

   // create FreeRTOS tasks
   if(OS_MAX_NUM_CONTEXTS == OS_Create_Thread(
      Task_Logger_Commander,
      NULL,
      "cmd",
      configMINIMAL_STACK_SIZE * 4,
      tskIDLE_PRIORITY + 2))
   {
      // application should never get here, unless there is a memory allocation problem
      IOCMD_Printf("Task %s creation failed!\n\r", "vcom_1");
   }

   // start the sheduler
   OS_Start();


   while(1)
   {
      ;
   }

} /* main */



void led_init(void) {
   GPIO_InitTypeDef gpio_led;

   gpio_led.GPIO_Pin = LED1_PIN;
   gpio_led.GPIO_Speed = GPIO_Speed_50MHz;
   gpio_led.GPIO_Mode = LED1_PIN_MODE;
   GPIO_Init(LED1_PORT, &gpio_led);
} /* led_init */



void system_initialize(void) {
   ErrorStatus HSEStartUpStatus;
   uint32_t i;

   /* RCC system reset(for debug purpose) */
   RCC_DeInit();

   /* Enable HSE */
   RCC_HSEConfig(RCC_HSE_ON);

   //default HSEStartUp_TimeOut is too short for out 4MHz oscillator to start (~80ms)
   for (i = 0; i < 0xFFFF; i++){
      asm("nop");
   }

   /* Wait till HSE is ready */
   HSEStartUpStatus = RCC_WaitForHSEStartUp();

   if(HSEStartUpStatus == SUCCESS)   {
      /* Enable Prefetch Buffer */
      FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

      /* Flash 2 wait state */
      FLASH_SetLatency(FLASH_Latency_2);

      /* HCLK = SYSCLK */
      RCC_HCLKConfig(RCC_SYSCLK_Div1);

      /* PCLK2 = HCLK */
      RCC_PCLK2Config(RCC_HCLK_Div4);

      /* PCLK1 = HCLK/2 */
      RCC_PCLK1Config(RCC_HCLK_Div2);   // SPI2: max div = 4 /4MHz HSE

      /* PLLCLK = 8MHz * 6 = 48 MHz */
      RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_12);

      /* Enable PLL */
      RCC_PLLCmd(ENABLE);

      /* Wait till PLL is ready */
      while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
      }

      /* Select PLL as system clock source */
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

      /* Wait till PLL is used as system clock source */
      while(RCC_GetSYSCLKSource() != 0x08) {
      }
   }

#ifdef  VECT_TAB_RAM
   /* Set the Vector Table base location at 0x20000000 */
   NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
   /* Set the Vector Table base location at 0x08000000 */
   //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
   NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

#endif

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
} /* system_initialize */

/*! \file main.c
   \brief Main application's entry point

*/

