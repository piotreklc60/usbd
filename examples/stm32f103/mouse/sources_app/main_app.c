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

#include "std_libs.h"
#include "main.h"

#include "os.h"
#include "gpio.h"
#include "iocmd.h"


#include "usbd.h"
#include "main_usbd_init.h"

#include "iocmd_exe_vcom.h"
#include "cmd_line.h"



#ifdef IOCMD_USE_LOG
static uint8_t main_working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];
#endif



void SystemClock_Config(void);



const IOCMD_Print_Exe_Params_XT *logs_exe = NULL;

// -----------------------------------------------------------------------------
//  TASK_LED
// -----------------------------------------------------------------------------
void Task_Led(void * argument)
{
   USBD_UNUSED_PARAM(argument);

   USBD_NOTICE(MAIN_TASK_LED, "led thread entered!");

   while(1)
   {
      USBD_NOTICE(MAIN_TASK_LED, "led ON!");
      LED_ON();
      OS_Sleep_Ms(500);
      USBD_NOTICE(MAIN_TASK_LED, "led FF!");
      LED_OFF();
      OS_Sleep_Ms(500);
   }
} /* Task_Led */

// -----------------------------------------------------------------------------
//  Task_Logger_Commander
// -----------------------------------------------------------------------------
void Task_Logger_Commander(void *pvParameters)
{
   const IOCMD_Print_Exe_Params_XT *exe = vcom_get_exe();
   Buff_Ring_XT *out_buf;
   Buff_Size_DT size;
   uint8_t data[64];

   USBD_UNUSED_PARAM(pvParameters);

   OS_Sleep_Ms(1);

   out_buf = CDC_Vcom_Get_Out_Buf(VCOM_CMD);

   while(1)
   {
#ifdef IOCMD_USE_LOG
      if(CDC_VCOM_Get_Dtr(VCOM_CMD) && ((VCOM_CMD)->data.comm_physical_params.baudrate > 9600))
      {
         IOCMD_Proc_Buffered_Logs(false, logs_exe, main_working_buf, sizeof(main_working_buf));
      }
#endif
      if(!Buff_Ring_Is_Empty(out_buf, BUFF_TRUE))
      {
         size = Buff_Ring_Read(out_buf, data, sizeof(data), BUFF_TRUE);
         Mouse_Parse_Cmd_Bytes(data, (size_t)size);
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
   const IOCMD_Print_Exe_Params_XT *exe;

   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

   /* Configure the system clock */
   SystemClock_Config();

#ifdef IOCMD_USE_LOG
   IOCMD_Logs_Init();
#endif

   /* Initialize all configured peripherals */
   MX_GPIO_Init();

   main_usbd_init();

   exe = vcom_get_exe();

   IOCMD_Install_Standard_Output(exe);
   logs_exe = exe;

   OS_Init();

   // create FreeRTOS tasks
   if(OS_MAX_NUM_CONTEXTS == OS_Create_Thread(
      Task_Led,
      NULL,
      "led1",
      configMINIMAL_STACK_SIZE * 2,
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

   /* We should never get here as control is now taken by the scheduler */

   /* Infinite loop */
   while (1)
   {
   }
} /* main */

/*! \file main.c
   \brief Main application's entry point

*/

