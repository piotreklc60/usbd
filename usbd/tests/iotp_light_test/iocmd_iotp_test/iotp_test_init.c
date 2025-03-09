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

#include "iotp_test_init.h"

#ifndef USBD_H_
#include "usbd.h"
#endif

#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_io_internal.h"
#endif

#include "cfg.h"
#include "port_test_control.h"
#include "main_get_exe.h"

typedef struct
{
    USBDC_Params_XT                         usbdc;
    USBD_IOTP_Params_XT               tp;
    USB_EP_Direction_ET                    dir;
}test_params_T;

/*
static USBDC_Params_XT test_usbdc;
static USBD_IOTP_Params_XT test_tp;
static USB_EP_Direction_ET test_dir;
// static uint8_t test_ep_num;
*/

static test_params_T test_params[2 * (USBD_MAX_NUM_ENDPOINTS + 1)];
static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
    IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
    printf("file: %s\nline: %d\n", __FILE__, __LINE__);\
    report_error()

static void report_error(void)
{
    char ec;

    printf("some error occured!\n\rpress any key to continue... ");
    ec = getc(stdin);
}


static void prepare_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
} /* prepare_test */

static void perform_test(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];

    USBD_IOTP_Init(usbd, &(test->usbdc), ep_num, test->dir, &(test->tp));
} /* perform_test */

static void check_result(USBD_Params_XT *usbd, uint8_t ep_index, uint8_t ep_num)
{
    test_params_T *test = &test_params[ep_index];

    if(!USBD_COMPARE_PTRS(void, USBD_IOTP_GET_USBD_FROM_TP(&(test->tp)), void, usbd))
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"usbd\" pointer inside of TP");
        REPORT_ERROR();
    }
    if(!USBD_COMPARE_PTRS(void, USBD_IOTP_Get_USBD(&(test->tp)), void, usbd))
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"usbd\" pointer inside of TP / read by USBD_IOTP_Get_USBD");
        REPORT_ERROR();
    }
    if(!USBD_COMPARE_PTRS(void, USBD_IOTP_GET_USBDC_FROM_TP(&(test->tp)), void, &(test->usbdc)))
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"(test->usbdc)\" pointer inside of TP");
        REPORT_ERROR();
    }
    if(!USBD_COMPARE_PTRS(void, USBD_IOTP_Get_USBDC(&(test->tp)), void, &(test->usbdc)))
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"(test->usbdc)\" pointer inside of TP / read by USBD_IOTP_Get_USBDC");
        REPORT_ERROR();
    }
    if(USBD_IOTP_GET_EP_NUM_FROM_TP(&(test->tp)) != ep_num)
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"ep_num\" inside of TP");
        REPORT_ERROR();
    }
    if(USBD_IOTP_Get_EP_Num(&(test->tp)) != ep_num)
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"ep_num\" inside of TP / read by USBD_IOTP_Get_EP_Num");
        REPORT_ERROR();
    }
    if(USBD_IOTP_GET_EP_DIR_FROM_TP(&(test->tp)) != test->dir)
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"dir\" inside of TP");
        REPORT_ERROR();
    }
    if(USBD_IOTP_Get_Dir(&(test->tp)) != test->dir)
    {
        USBD_WARN(MAIN_APP_TEST_ERROR, "Invalid \"dir\" inside of TP / read by USBD_IOTP_Get_Dir");
        REPORT_ERROR();
    }
} /* check_result */

void iotp_test_init(USBD_Params_XT *usbd, USB_EP_Direction_ET dir, uint8_t ep_num)
{
    uint8_t ep_index = ep_num * ((USB_EP_DIRECTION_IN == dir) ? 1 : 2);
    test_params_T *test = &test_params[ep_index];

    ep_num = ep_num;
    test->dir = dir;

    prepare_test(usbd, ep_index, ep_num);
    perform_test(usbd, ep_index, ep_num);
    check_result(usbd, ep_index, ep_num);
} /* test_init */
