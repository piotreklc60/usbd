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

#ifndef OS_H_
#define OS_H_

#include "std_libs.h"
#include "os_cfg.h"

/**
 * Constants
 */

/**
 * maximum number of threads in the system. IDLE thread number is 0.
 */
#ifndef OS_MAX_NUM_THREADS
#define OS_MAX_NUM_THREADS             1
#endif

/**
 * maximum number of interrupts in the system.
 */
#ifndef OS_MAX_NUM_INTERRUPTS
#define OS_MAX_NUM_INTERRUPTS          1
#endif

/**
 * maximum number of threads/interrupts in the system.
 */
#ifndef OS_MAX_NUM_CONTEXTS
#define OS_MAX_NUM_CONTEXTS            ((OS_MAX_NUM_INTERRUPTS > OS_MAX_NUM_THREADS) ? OS_MAX_NUM_INTERRUPTS : OS_MAX_NUM_THREADS)
#endif

#ifndef OS_DEFAULT_THREAD_STACK_SIZE
#define OS_DEFAULT_THREAD_STACK_SIZE   1024
#endif

/**
 * Types
 */

#ifndef OS_CONTEXT_TYPE_DT_EXTERNAL
typedef enum OS_Context_Type_Data_Tag
{
   OS_CONTEXT_TYPE_TASK                = 0,
   OS_CONTEXT_TYPE_INTERRUPT           = 1,
   OS_MAX_NUM_CONTEXT_TYPES            = 2
}OS_Context_Type_DT;
#endif

#ifndef OS_STACK_DEPTH_DT_EXTERNAL
typedef uint_fast16_t OS_Thread_Stack_Depth_DT;
#endif

#ifndef OS_STACK_DEPTH_DT_EXTERNAL
typedef uint_fast8_t OS_Thread_Priority_DT;
#endif

#ifndef OS_THREAD_HT_EXTERNAL
typedef void (*OS_Thread_HT) (void *parameters);
#endif

#if (OS_MAX_NUM_CONTEXTS < 256)
typedef uint_fast8_t OS_Context_Id;
#elif (OS_MAX_NUM_CONTEXTS < 65536)
typedef uint_fast16_t OS_Context_Id;
#else
typedef uint_fast32_t OS_Context_Id;
#endif

#ifndef OS_STACK_DEPTH_DT_EXTERNAL
typedef uint32_t OS_Time_DT;
#endif

/**
 * Functions
 */

/** @defgroup Init Operating System Abstraction Layer - Initialization
 *  @brief Initialization of OSA Engine, Threads creation / control and start.
 *  @{
 */

/**
 * @brief Initializes OSA engine
 */
void OS_Init (void);

/**
 * @brief Creates thread
 *
 * @return thread_id if creating thread succeeded; OS_MAX_NUM_CONTEXTS otherwise
 *
 * @param thread_func thread main function
 * @thread_params pointer to data which will be provided to thread function in m,oment of its start
 * @thread_name string with thread name
 * @stack_size size of stack created for this thread. if 0 then default value will be used
 * priority thread priority
 */
OS_Context_Id OS_Create_Thread(
   OS_Thread_HT thread_func,
   void *thread_params,
   const char *thread_name,
   OS_Thread_Stack_Depth_DT stack_size,
   OS_Thread_Priority_DT priority, ...);

void *OS_Get_Thread_Handler(OS_Context_Id thread_id);

/**
 * @brief Starts scheduler
 */
void OS_Start(void);

/**
 * @brief Gets type of curently active system context. It can be thread type, interrupt type or other (project specific).
 *
 * return type of curently active system context
 */
OS_Context_Type_DT OS_Get_Current_Context_Type(void);

/**
 * @brief Gets name of requested context type.
 *
 * @return pointer to requested context type name. It is always valid pointer - never will be NULL (can be for example "unknown").
 * @param context_type context type for which name is requested
 */
const char *OS_Get_Context_Type_Name(OS_Context_Type_DT context_type);

/**
 * @brief Gets unique ID of curently working context.
 * For interrupt mode it is interrupt ID, in thread mode it is thread_id + man number of interrupts.
 *
 * @return ID of currently working contextnumber.
 */
OS_Context_Id OS_Get_Current_Context_Unique_Id(void);

/**
 * @brief Gets ID of curently working context.
 * In thread mode it is thread number, in Inrettupt mode it is interrupt number. For other mode types it is project specific.
 *
 * @return ID of currently working contextnumber.
 */
OS_Context_Id OS_Get_Current_Context_Id(void);

/**
 * @brief Gets name of requested service in particular mode space.
 * In thread mode it is thread name, in Inrettupt mode it is interrupt number. For other mode types it is project specific.
 *
 * @return pointer to requested service name. It is always valid pointer - never will be NULL (can be for example "unknown").
 * @param context_type type of service for which name is requested
 * @param context_id number of service for which name is requested
 */
const char *OS_Get_Context_Name(OS_Context_Type_DT context_type, OS_Context_Id context_id);

/** @} */ // end of Init


/** @defgroup Time Operating System Abstraction Layer - Time management
 *  @brief Getting, calculating time, delays, etc.
 *  @{
 */

/**
 * @brief Gets current system time. It is time from startup, unit is 1[ms].
 */
OS_Time_DT OS_Get_Time(void);

/**
 * @brief function to postpone current task for time mentioned in miliseconds parameter.
 *
 * @param miliseconds amount of time for which task shall sleep.
 */
void OS_Sleep_Ms(uint32_t miliseconds);

/**
 * @brief function to postpone current task for time mentioned in seconds parameter.
 *
 * @param seconds amount of time for which task shall sleep.
 */
void OS_Sleep_S(uint32_t seconds);

/** @} */ // end of Time


/** @defgroup Invoke Operating System Abstraction Layer - Actions invoke mechanism
 *  @brief Invoke mechanism creation, initiialisation, use
 *  @{
 */

struct OS_Invoke_eXtendedTag;

typedef bool_t (*OS_Destination_Compare_HT) (struct OS_Invoke_eXtendedTag *invoke_params, OS_Context_Id current_context_unique_id);

typedef bool_t (*OS_Destination_Lock_HT) (struct OS_Invoke_eXtendedTag *invoke_params);

typedef void (*OS_Destination_Unlock_HT) (struct OS_Invoke_eXtendedTag *invoke_params);

typedef struct OS_Invoke_eXtendedTag
{
   /* this function is used when destination is an IRQ. It is used to disable destination IRQ. */
   OS_Destination_Lock_HT        lock;
   /* this function is used when destination is an IRQ. It is used to re-enable destination IRQ. after previously disabled by lock */
   OS_Destination_Unlock_HT      unlock;
   union
   {
      /* to use compare function destination_id_as_vendor_compare must be set to  true */
      OS_Destination_Compare_HT  is_context_different;
      OS_Context_Id              unique_id;
   }destination_id;
   SemaphoreHandle_t             task_lock;
   /*OS_Context_Type_DT*/uint8_t destination_type;
   bool_t                        destination_id_as_vendor_compare;
   bool_t                        is_invoke_needed;
   bool_t                        locked_from_irq;
}OS_Invoke_XT;

/**
 * @brief initializes Invoke parameters.
 *
 * @param invoke_params pointer to parameters to be initialized.
 */
void OS_Invoke_Init(OS_Invoke_XT *invoke_params);

/**
 * @brief sets vendor-specific function for comparing destination_id.
 *
 * @param invoke_params parameters of invoke destination
 * @param vendor_checker pointer to a function which will be used for comparing destination_id.
 */
void OS_Set_Destination_Id_Vendor_Checker(OS_Invoke_XT *invoke_params, OS_Destination_Compare_HT vendor_checker);

/**
 * @brief sets lock and unlock functions for IRQ destination.
 *
 * @param invoke_params parameters of invoke destination
 * @param lock pointer to a function which will be used to lock destination IRQ
 * @param unlock pointer to a function which will be used to unlock destination IRQ
 */
void OS_Set_Destination_Irq_Lock_Unlock(OS_Invoke_XT *invoke_params, OS_Destination_Lock_HT lock, OS_Destination_Unlock_HT unlock);

/**
 * @brief function which shall be called at least once from the context to which invoke operations are expected.
 *
 * @param invoke_params parameters of invoke destination
 */
void OS_Mark_Invoke_Params(OS_Invoke_XT *invoke_params);

/**
 * @brief This function only checks of current ocntext is different to the one pointed by invoke_params.
 *
 * @return true if contexts are different so invoke would be needed, false if both contexts are same.
 *
 * @param invoke_params parameters of invoke destination
 */
bool_t OS_Is_Invoke_Needed(OS_Invoke_XT *invoke_params);

/**
 * @brief function called during action invoking. It checks if invoke is needed,
 * if yes then secures conditions (for example block destination interrupt or waits for semaphore).
 *
 * @return true if OS_INVOKE/OS_TRY_INVOKE can call the action, false otherwise.
 *
 * @param invoke_params parameters of invoke destination
 * @param timeout time in miliseconds which defines how long function shall block source task. -1 fir indefinetly, 0 for no blocking.
 */
bool_t OS_Invoke_Sync_To_Destination(OS_Invoke_XT *invoke_params, OS_Time_DT timeout);

/**
 * If invoke was executed then at the end this function restores destination conditions (enable back the IRQ / semaphore).
 *
 * @return true when release succeeded false otherwise.
 *
 * @param invoke_params parameters of invoke destination
 */
bool_t OS_Invoke_Release_Destination(OS_Invoke_XT *invoke_params);

/**
 * @brief function to be called from destination task.
 * From the moment of this function call all incomming invokes from any context will be simply called.
 *
 * @param invoke_params parameters of invoke destination
 */
void OS_Destination_Thread_Open_Invoke_Window(OS_Invoke_XT *invoke_params);

/**
 * @brief function to be called from destination task.
 * From the moment of this function call all incomming invokes from any context cannot be called and must wait.
 *
 * @param invoke_params parameters of invoke destination
 */
void OS_Destination_Thread_Close_Invoke_Window(OS_Invoke_XT *invoke_params);

/**
 * @brief this macro is used to invoke the action.
 * By invoke it means action will be called in the way that the action is synchronized to the destination thread / interrupt.
 * By synchronized to the destination it means source action execution will not be interrupted by destination execution and vice versa.
 * Action execution will be done in the moment when destination context is not running or when destination calls OS_Destination_Process_Waiting_Invokes.
 * If there is a invoke action from exactly the same context as the destination then invoke is reduced to simple call.
 * Invoking implementation between 2 different contexts can vary depends to destination / source type:
 * 1. Interrupt/Task -> Interrupt: If destination Interrupt is not currently active then:
 *   a) if destination priority is lower than caller priority then invoke is reduced to simple call.
 *   b) if destination priority is higher than caller then destination IRQ is temporarily disabled, action is called and IRQ is back enabled.
 *  IF DESTINATION INTERRUPT IS CURRENTLY ACTIVE AND CALLER IRQ INTERRUPTED IT THEN INVOKE FAILS. WE ARE NOT ABLE TO SWITCH FROM ONE INTERRUPT TO ANOTHER.
 * 2. Interrupt -> Task: If destination task is currently to running then invoke is reduced to simple call. IF DESTINATION TASK IS RUNNING THEN INVOKE WILL FAIL.
 * 2. Task -> Task: Here synchronisation can be implemented using a mutex. If destinaiton is busy then caller will be suspended  by th OS until destination is free.
 *
 * @return true if invoke succeeded, false otherwise.
 *
 * @param _invoke_params pointer to OS_Invoke_XT structure with parameters of the destination.
 * @param _action the code to be invoked to the destination.
 * ATTENTION! ACTION MUST ALWAYS RETURN TRUE!!! OTHWERISE AFTER ITS EXECUTION DESTINATION IRQ/SEMAPHORE WILL NOT BE UNLOCKED!!!
 */
#define OS_INVOKE(_invoke_params, _action)                     (OS_Invoke_Sync_To_Destination(_invoke_params, portMAX_DELAY) && _action && OS_Invoke_Release_Destination(_invoke_params))

/**
 * @brief this macro is used to invoke the action.
 * This macro is very similar to OS_INVOKE with one exception - if source is a thread then it is blocked for _timeout miliseconds.
 * If invoke needs longer time to wait, it will fail.
 *
 * @return true if invoke succeeded, false otherwise.
 *
 * @param _invoke_params pointer to OS_Invoke_XT structure with parameters of the destination.
 * @param _action the code to be invoked to the destination.
 */
#define OS_INVOKE_TIMEOUT(_invoke_params, _action, _timeout)   (OS_Invoke_Sync_To_Destination(_invoke_params, _timeout) && _action && OS_Invoke_Release_Destination(_invoke_params))

/**
 * @brief this macro is used to invoke the action.
 * This macro is very similar to OS_INVOKE with one exception - if source is a thread then it is never blocked. If invoke needs waiting, it will fail.
 *
 * @return true if invoke succeeded, false otherwise.
 *
 * @param _invoke_params pointer to OS_Invoke_XT structure with parameters of the destination.
 * @param _action the code to be invoked to the destination.
 */
#define OS_TRY_INVOKE(_invoke_params, _action)                 (OS_Invoke_Sync_To_Destination(_invoke_params, 0) && _action && OS_Invoke_Release_Destination(_invoke_params))


/** @} */ // end of Time


#endif

