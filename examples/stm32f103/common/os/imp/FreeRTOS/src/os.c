/** ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
 *
 * Copyright (c) 2018 Piotr Wojtowicz
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

#include "os.h"
#include "FreeRTOS.h"
#include "task.h"


#define OS_Num_Elems(tab)     (sizeof(tab) / sizeof(tab[0]))

static TaskHandle_t os_thread_handler[OS_MAX_NUM_THREADS];
static const char *os_thread_name[OS_MAX_NUM_THREADS];
static const char* const os_context_type_names[OS_MAX_NUM_CONTEXT_TYPES] = {"Task", "Irq "};
static const char* const os_irq_name[] =
{
   "---",
   "Reset",          // The reset handler
   "NMI",
   "HardFault",
   "MemManagFault",
   "BusFault",
   "UsageFault",
   "---", "---", "---", "---",         /* Reserved */
   "SVC",
   "DebugMonitor",
   "---",                 /* Reserved */
   "PendSV",
   "SysTick",
   "WWDG",
   "PVD",
   "TAMPER",
   "RTC",
   "FLASH",
   "RCC",
   "EXTI0",
   "EXTI1",
   "EXTI2",
   "EXTI3",
   "EXTI4",
   "DMA1",
   "DMA2",
   "DMA3",
   "DMA4",
   "DMA5",
   "DMA6",
   "DMA7",
   "ADC",
   "USB_HP_CAN1_TX",
   "USB_LP_CAN1_RX0",
   "CAN_RX1",
   "CAN_SCE",
   "EXTI9_5",
   "TIM1_BRK",
   "TIM1_UP",
   "TIM1_TRG_COM",
   "TIM1_CC",
   "TIM2",
   "TIM3",
   "TIM4",
   "I2C1_EV",
   "I2C1_ER",
   "I2C2_EV",
   "I2C2_ER",
   "SPI1",
   "SPI2",
   "USART1",
   "USART2",
   "USART3",
   "EXTI15_10",
   "RTCAlarm",
   "USBWakeUp"
};
static OS_Context_Id os_num_installed_threads;
static uint_fast16_t os_critical_global_cntr_isr;

TaskHandle_t xTaskGetCurrentTaskHandle( void );

void OS_Init (void)
{
   os_thread_name[0] = "IDLE";
   os_num_installed_threads    = 1;
   os_critical_global_cntr_isr = 0;
} /* OS_Init */

OS_Context_Id OS_Create_Thread(
   OS_Thread_HT thread_func,
   void *thread_params,
   const char *thread_name,
   OS_Thread_Stack_Depth_DT stack_size,
   OS_Thread_Priority_DT priority, ...)
{
   TaskHandle_t handle;
   OS_Context_Id result = OS_MAX_NUM_CONTEXTS;

   if((NULL != thread_func) && (os_num_installed_threads < OS_MAX_NUM_THREADS))
   {

      if (pdPASS == xTaskCreate( thread_func, thread_name, stack_size, thread_params, priority, &handle ))
      {
         vTaskSetTaskNumber(handle, os_num_installed_threads);
         if(NULL != thread_name)
         {
            os_thread_name[os_num_installed_threads] = thread_name;
         }
         else
         {
            os_thread_name[os_num_installed_threads] = "---";
         }
         result = os_num_installed_threads;

         os_num_installed_threads++;

         os_thread_handler[result] = handle;
      }
   }

   return result;
} /* OS_Create_Thread */

void *OS_Get_Thread_Handler(OS_Context_Id thread_id)
{
   TaskHandle_t result = NULL;

   if(thread_id < os_num_installed_threads)
   {
      result = os_thread_handler[thread_id];
   }

   return result;
} /* OS_Get_Thread_Handler */

void OS_Start(void)
{
   vTaskStartScheduler();
} /* OS_Start */

OS_Context_Type_DT OS_Get_Current_Context_Type(void)
{
   uint32_t current_interrupt;
   OS_Context_Type_DT result = OS_CONTEXT_TYPE_TASK;

   /* Obtain the number of the currently executing interrupt. */
   __asm volatile( "mrs %0, ipsr" : "=r"( current_interrupt ) );

   if(0 != current_interrupt)
   {
      result = OS_CONTEXT_TYPE_INTERRUPT;
   }

   return result;
} /* OS_Get_Current_Context_Type */

const char *OS_Get_Context_Type_Name(OS_Context_Type_DT context_type)
{
   const char *result = "---";

   if(context_type < OS_MAX_NUM_CONTEXT_TYPES)
   {
      result = os_context_type_names[context_type];
   }

   return result;
} /* OS_Get_Context_Type_Name */

OS_Context_Id OS_Get_Current_Context_Unique_Id(void)
{
   uint32_t current_interrupt;
   OS_Context_Id result = OS_MAX_NUM_CONTEXTS;

   /* Obtain the number of the currently executing interrupt. */
   __asm volatile( "mrs %0, ipsr" : "=r"( current_interrupt ) );

   if(0 == current_interrupt)
   {
      result = uxTaskGetTaskNumber(xTaskGetCurrentTaskHandle()) + OS_MAX_NUM_INTERRUPTS;
   }
   else
   {
      result = (OS_Context_Id)current_interrupt;
   }

   return result;
} /* OS_Get_Current_Context_Unique_Id */

OS_Context_Id OS_Get_Current_Context_Id(void)
{
   uint32_t current_interrupt;
   OS_Context_Id result = OS_MAX_NUM_CONTEXTS;

   /* Obtain the number of the currently executing interrupt. */
   __asm volatile( "mrs %0, ipsr" : "=r"( current_interrupt ) );

   if(0 == current_interrupt)
   {
      result = uxTaskGetTaskNumber(xTaskGetCurrentTaskHandle());
   }
   else
   {
      result = (OS_Context_Id)current_interrupt;
   }

   return result;
} /* OS_Get_Current_Context_Id */

const char *OS_Get_Context_Name(OS_Context_Type_DT context_type, OS_Context_Id context_id)
{
   const char *result = "---";

   if((OS_CONTEXT_TYPE_TASK == context_type) && (context_id < os_num_installed_threads))
   {
      if(NULL != os_thread_name[context_id])
      {
         result = os_thread_name[context_id];
      }
   }
   else if((OS_CONTEXT_TYPE_INTERRUPT == context_type) && (context_id < OS_Num_Elems(os_irq_name)))
   {
      if(NULL != os_irq_name[context_id])
      {
         result = os_irq_name[context_id];
      }
   }

   return result;
} /* OS_Get_Context_Name */

void OS_Enter_Critical_Section(void)
{
   __asm volatile( "cpsid i" ::: "memory" );
   __asm volatile( "dsb" );
   __asm volatile( "isb" );
   ++os_critical_global_cntr_isr;
}

void OS_Exit_Critical_Section(void)
{
   if(os_critical_global_cntr_isr > 0)
   {
      --os_critical_global_cntr_isr;
   }
   if(0 == os_critical_global_cntr_isr)
   {
      __asm volatile( "cpsie i" ::: "memory" );
      __asm volatile( "dsb" );
      __asm volatile( "isb" );
   }
}


OS_Time_DT OS_Get_Time(void)
{
   OS_Time_DT result = 0;
   uint32_t current_interrupt;

   /* Obtain the number of the currently executing interrupt. */
   __asm volatile( "mrs %0, ipsr" : "=r"( current_interrupt ) );

   if(0 == current_interrupt)
   {
      result = xTaskGetTickCount();
   }
   else
   {
      result = xTaskGetTickCount(); // actually, here should be xTaskGetTickCountFromISR but it is crashing..
   }

   return result;
} /* OS_Get_Time */

void OS_Sleep_Ms(uint32_t miliseconds)
{
   vTaskDelay(miliseconds * configTICK_RATE_HZ / 1000);
} /* OS_Sleep_Ms */

void OS_Sleep_S(uint32_t seconds)
{
   vTaskDelay(seconds * configTICK_RATE_HZ);
} /* OS_Sleep_S */


void OS_Invoke_Init(OS_Invoke_XT *invoke_params)
{
   if(NULL != invoke_params)
   {
      memset(invoke_params, 0, sizeof(*invoke_params));
   }
} /* OS_Invoke_Init */

void OS_Set_Destination_Id_Vendor_Checker(OS_Invoke_XT *invoke_params, OS_Destination_Compare_HT vendor_checker)
{
   if(NULL != invoke_params)
   {
      invoke_params->destination_id.is_context_different = vendor_checker;
      invoke_params->destination_id_as_vendor_compare    = true;
   }
} /* OS_Set_Destination_Id_Vendor_Checker */

void OS_Set_Destination_Irq_Lock_Unlock(OS_Invoke_XT *invoke_params, OS_Destination_Lock_HT lock, OS_Destination_Unlock_HT unlock)
{
   if(NULL != invoke_params)
   {
      invoke_params->lock   = lock;
      invoke_params->unlock = unlock;
   }
} /* OS_Set_Destination_Irq_Lock_Unlock */

void OS_Mark_Invoke_Params(OS_Invoke_XT *invoke_params)
{
   if(NULL != invoke_params)
   {
      if(!invoke_params->destination_id_as_vendor_compare)
      {
         invoke_params->destination_id.unique_id = OS_Get_Current_Context_Unique_Id();
      }
      invoke_params->destination_type = OS_Get_Current_Context_Type();
      if((NULL == invoke_params->task_lock) && (OS_CONTEXT_TYPE_TASK == invoke_params->destination_type))
      {
         invoke_params->task_lock = xSemaphoreCreateMutex();
         if(NULL != invoke_params->task_lock)
         {
            (void)xSemaphoreTake(invoke_params->task_lock, 0);
         }
      }
   }
} /* OS_Mark_Invoke_Params */

bool_t OS_Is_Invoke_Needed(OS_Invoke_XT *invoke_params)
{
   OS_Context_Id current_context_unique_id;
   bool_t result = false;

   if(NULL != invoke_params)
   {
      current_context_unique_id = OS_Get_Current_Context_Unique_Id();

      if(!invoke_params->destination_id_as_vendor_compare)
      {
         result = (current_context_unique_id != invoke_params->destination_id.unique_id);
      }
      else
      {
         result = invoke_params->destination_id.is_context_different(invoke_params, current_context_unique_id);
      }
   }
   invoke_params->is_invoke_needed = result;

   return result;
} /* OS_Is_Invoke_Needed */

bool_t OS_Invoke_Sync_To_Destination(OS_Invoke_XT *invoke_params, OS_Time_DT timeout)
{
   OS_Context_Type_DT current_context_type = OS_Get_Current_Context_Type();
   bool_t result = false;

   if(NULL != invoke_params)
   {
      if(!OS_Is_Invoke_Needed(invoke_params))
      {
         result = true;
      }
      /**
       * we are going to invoke to IRQ context.
       * If we are here then it means destination IRQ is higher priority and not active at this moment
       * or it is lower priority and not active or currently interrupted by us (if we are IRQ with higher priority).
       * Call "lock" function. If destination IRQ is currently not active then it should disable itself and return true.
       * If destination IRQ is currently active then "lock" function shall do nothing and return false.
       */
      else if(OS_CONTEXT_TYPE_INTERRUPT == invoke_params->destination_type)
      {
         result = (!invoke_params->locked_from_irq) && invoke_params->lock(invoke_params);
         if(result)
         {
            invoke_params->locked_from_irq = true;
         }
      }
      /**
       * we are going to invoke to TASK context from IRQ.
       */
      else if(NULL != invoke_params->task_lock)
      {
         if(OS_CONTEXT_TYPE_INTERRUPT == current_context_type)
         {
            /* for binary semaphore/mutex uxSemaphoreGetCount returns 1 when it is available */
            result = (!invoke_params->locked_from_irq) && (0 != uxSemaphoreGetCount(invoke_params->task_lock));

            if(result)
            {
               invoke_params->locked_from_irq = true;
            }
         }
         /**
          * we are going to invoke to TASK context from another task.
          */
         else
         {
            result = xSemaphoreTake(invoke_params->task_lock, timeout);
         }
      }
   }

   return result;
} /* OS_Invoke_Sync_To_Destination */

bool_t OS_Invoke_Release_Destination(OS_Invoke_XT *invoke_params)
{
   bool_t result = false;

   if(NULL != invoke_params)
   {
      result = true;
      if(invoke_params->is_invoke_needed)
      {
         if(OS_CONTEXT_TYPE_INTERRUPT == invoke_params->destination_type)
         {
            invoke_params->unlock(invoke_params);
            invoke_params->locked_from_irq = false;
         }
         else if(OS_CONTEXT_TYPE_INTERRUPT == OS_Get_Current_Context_Type())
         {
            invoke_params->locked_from_irq = false;
         }
         else if(NULL != invoke_params->task_lock)
         {
            xSemaphoreGive(invoke_params->task_lock);
         }
      }
   }

   return result;
} /* OS_Invoke_Release_Destination */

void OS_Destination_Thread_Open_Invoke_Window(OS_Invoke_XT *invoke_params)
{
   if((NULL != invoke_params) && (NULL != invoke_params->task_lock))
   {
      xSemaphoreGive(invoke_params->task_lock);
   }
} /* OS_Destination_Thread_Open_Invoke_Window */

void OS_Destination_Thread_Close_Invoke_Window(OS_Invoke_XT *invoke_params)
{
   if((NULL != invoke_params) && (NULL != invoke_params->task_lock))
   {
      xSemaphoreTake(invoke_params->task_lock, portMAX_DELAY);
   }
} /* OS_Destination_Thread_Close_Invoke_Window */


