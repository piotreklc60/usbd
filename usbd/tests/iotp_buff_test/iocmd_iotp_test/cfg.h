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

#ifndef CFG_H_
#define CFG_H_

#include "std_libs.h"

#define TEST_NUM_INTERFACES         3

#define TEST_NUM_CONFIGS            4

#define TEST_EP0_MPS                8

#define TEST_DEBUG_STATE    false

#define TEST_MAX_DATA_SIZE                      65536

#define TEST_NUM_INCREMENTAL_SIZE_TESTS         8192

#define TEST_NUM_RANDOM_DATA_SIZE_TESTS         8192

#define TEST_MAX_NUM_CONNECTED_TRANSACTIONS     16

#define TEST_RING_BUF_MEMORY_SIZE               TEST_MAX_DATA_SIZE

#define NUM_ELEMS(array)        (sizeof(array) / sizeof(array[0]))

#define TEST_LIMITED_NUM_LOOPS      8193



#define MAIN_TEST_IOTP_COLLECTION \
X_ADD_IOTP(0,  USB_EP_DIRECTION_IN,  &tp_ep0in )\
X_ADD_IOTP(0,  USB_EP_DIRECTION_OUT, &tp_ep0out)\
X_ADD_IOTP(1,  USB_EP_DIRECTION_IN,  &tp_ep1in )\
X_ADD_IOTP(2,  USB_EP_DIRECTION_OUT, &tp_ep2out)\
X_ADD_IOTP(3,  USB_EP_DIRECTION_IN,  &tp_ep3in )\
X_ADD_IOTP(3,  USB_EP_DIRECTION_OUT, &tp_ep3out)\
X_ADD_IOTP(4,  USB_EP_DIRECTION_IN,  &tp_ep4in )\
X_ADD_IOTP(5,  USB_EP_DIRECTION_OUT, &tp_ep5out)\
X_ADD_IOTP(6,  USB_EP_DIRECTION_IN,  &tp_ep6in )\
X_ADD_IOTP(6,  USB_EP_DIRECTION_OUT, &tp_ep6out)\
X_ADD_IOTP(7,  USB_EP_DIRECTION_IN,  &tp_ep7in )\
X_ADD_IOTP(8,  USB_EP_DIRECTION_OUT, &tp_ep8out)\
X_ADD_IOTP(9,  USB_EP_DIRECTION_IN,  &tp_ep9in )\
X_ADD_IOTP(9,  USB_EP_DIRECTION_OUT, &tp_ep9out)\
X_ADD_IOTP(10, USB_EP_DIRECTION_IN,  &tp_ep10in)\
X_ADD_IOTP(11, USB_EP_DIRECTION_OUT, &tp_ep11out)



#ifdef X_ADD_IOTP
#undef X_ADD_IOTP
#endif
#define X_ADD_IOTP(a,b,c)   +1


typedef enum
{
    TEST_MAX_NUM_IOTPS = 0 MAIN_TEST_IOTP_COLLECTION
}Test_Max_Num_IOTPs_T;

typedef uint64_t ptrdiff_t;


// #define USE_IO_PRINTF
// #define USE_IOTP_PRINTF
// #define USE_PORT_PRINTF
// #define USE_TEST_PRINTF

#ifdef USE_IO_PRINTF
#define io_printf       printf
#else
#define io_printf       dummy_printf
#endif

#ifdef USE_IOTP_PRINTF
#define iotp_printf     printf
#else
#define iotp_printf     dummy_printf
#endif

#ifdef USE_PORT_PRINTF
#define port_printf     printf
#else
#define port_printf     dummy_printf
#endif

#ifdef USE_TEST_PRINTF
#define test_printf     printf
#else
#define test_printf     dummy_printf
#endif


void dummy_printf(const char *arg, ...);


// #define USBD_ENTER_FUNC()   printf("-> %s\n\r", __FUNCTION__)
// #define USBD_EXIT_FUNC()    printf("<-\n\r")


#define USBD_DECLARE_INVOKE_PARAMS(_invoke_params)      USBD_Bool_DT _invoke_params;

#define USBD_INIT_INVOKE_PARAMS(_invoke_params)         _invoke_params = USBD_TRUE

#define USBD_MARK_INVOKE_DESTINATION(_invoke_params) \
_invoke_params = USBD_FALSE; \
USBD_WARN(MAIN_APP_TEST, "mark invoke params!")

#define USBD_IS_INVOKE_NEEDED(_invoke_params)           _invoke_params

#define USBD_UNMARK_INVOKE_DESTINATION(_invoke_params) \
_invoke_params = USBD_TRUE; \
USBD_WARN(MAIN_APP_TEST, "unmark invoke params!")


#endif
