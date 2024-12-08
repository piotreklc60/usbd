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

#include "dev_test_strings.h"
#ifndef USBD_DEV_INTERNAL_H_
#include "usbd_dev_internal.h"
#endif

#include "cfg.h"
#include "main_get_exe.h"

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state);
static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd);
static uint16_t test_get_dev_status(USBD_Params_XT *usbd);
static uint16_t test_get_frame_nr(USBD_Params_XT *usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd);
static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd);

static USBD_DEV_Installation_Result_XT usbdc_install_result_1;
static USB_Configuration_Desc_DT desc1;
static USBDC_Params_XT usbdcp1;
static USBDC_Params_XT *usbdc1 = &usbdcp1;

static USBD_DEV_Installation_Result_XT install_result_1;
static USBD_DEV_Installation_Result_XT install_result_2;
static USBD_DEV_Installation_Result_XT install_result_3;
static USBD_DEV_Installation_Result_XT install_result_4;
static USBD_DEV_Installation_Result_XT install_result_5;
static USBD_DEV_Installation_Result_XT install_result_6;

static bool remove_result;

static uint16_t string1[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string1")),
    's','t','r','i','n','g','1'
};
static uint16_t string2[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string2")),
    's','t','r','i','n','g','2'
};
static uint16_t string3[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string3")),
    's','t','r','i','n','g','3'
};
static uint16_t string4[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string4")),
    's','t','r','i','n','g','4'
};
static uint16_t string5[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string5")),
    's','t','r','i','n','g','5'
};
static uint16_t string6[] =
{
    USB_STRING_COMPOSE_LENGTH_AND_TYPE(sizeof("string6")),
    's','t','r','i','n','g','6'
};

static uint8_t activity = 0;
static USB_Endpoint_Desc_DT ep0_full =
{
   .bLength             = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType     = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress    = 0,
   .bmAttributes        = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L    = TEST_EP0_MPS,
   .wMaxPacketSize.H    = 0,
   .bInterval           = 0xFF,
};
static USB_Endpoint_Desc_DT ep0_high =
{
   .bLength             = USB_DESC_TYPE_ENDPOINT_SIZE,
   .bDescriptorType     = USB_DESC_TYPE_ENDPOINT,
   .bEndpointAddress    = 0,
   .bmAttributes        = USB_EP_DESC_TRANSFER_TYPE_CONTROL,
   .wMaxPacketSize.L    = TEST_EP0_MPS,
   .wMaxPacketSize.H    = 0,
   .bInterval           = 0xFF,
};
static USBD_DEV_Speed_ET speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t device_status = 0;
static uint16_t frame_nr = 0;
static USBD_DEV_Port_Handler_XT port =
{
   .handlers.activate               = test_activate_deactivate,
   .handlers.ep_parse               = test_parse_ep,
   .handlers.cfg_parse              = test_parse_cfg,
   .handlers.get_supported_speed    = test_get_supported_speed,
   .handlers.get_current_speed      = test_get_current_speed,
   .handlers.get_device_status      = test_get_dev_status,
   .handlers.get_frame_num          = test_get_frame_nr,
   .handlers.get_ep0_low_full_speed = test_get_ep0_low_full_speed,
   .handlers.get_ep0_high_speed     = test_get_ep0_high_speed
};

static void test_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    if(USBD_BOOL_IS_TRUE(state))
    {
        activity = 5;
    }
    else
    {
        activity = 11;
    }

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
}

static USBD_Bool_DT test_parse_ep(USBD_Params_XT* usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return USBD_TRUE;
}

static USBD_Bool_DT test_parse_cfg(USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return USBD_TRUE;
}

static USBD_DEV_Speed_ET test_get_supported_speed(USBD_Params_XT *usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return USBD_DEV_FULL_AND_HIGH_SPEED;
}

static USBD_DEV_Speed_ET test_get_current_speed(USBD_Params_XT *usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return speed;
}

static uint16_t test_get_dev_status(USBD_Params_XT *usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return device_status;
}

static uint16_t test_get_frame_nr(USBD_Params_XT *usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    frame_nr++;
    frame_nr &= 0x7FF;

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return frame_nr;
}

static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_low_full_speed(USBD_Params_XT* usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return (&ep0_full);
}

static USBD_DEV_PORT_Get_EP_Desc_Result_DT test_get_ep0_high_speed(USBD_Params_XT* usbd)
{
    USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

    USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

    return (&ep0_high);
}




static void report_error(void)
{
    char ec;

    printf("some error occured!\n\rpress any key to continue... ");
    ec = getc(stdin);
} /* report_error */

static uint8_t working_buf[IOCMD_WORKING_BUF_RECOMMENDED_SIZE];

#define REPORT_ERROR()\
    IOCMD_Proc_Buffered_Logs(true, main_get_exe(), working_buf, sizeof(working_buf));\
    printf("file: %s\nline: %d\n", __FILE__, __LINE__);\
    report_error()










static void set_dummy_cfg(USB_Configuration_Desc_DT *desc)
{
    desc->bLength             = USB_DESC_TYPE_CONFIGURATION_SIZE;
    desc->bDescriptorType     = USB_DESC_TYPE_CONFIGURATION;
    desc->wTotalLength.L      = USB_DESC_TYPE_CONFIGURATION_SIZE;
    desc->wTotalLength.H      = 0;
    desc->bNumInterfaces      = 0;
    desc->bConfigurationValue = 0;
    desc->iConfiguration      = 0;
    desc->bmAttributes        = 0x40;
    desc->bMaxPower           = 50;
}

static void test_compose_configs(USBD_Params_XT *usbd)
{
    USBD_ENTER_FUNC(MAIN_APP_TEST);

    if(0 != USBD_DEV_Get_Num_Installed_Configs(usbd))
    {
        printf("incorrect number of installed configs\n\r");
        REPORT_ERROR();
    }

    USBDC_Init(usbdc1);
    set_dummy_cfg(&desc1);
    USBDC_Bind_Config_Desc(usbdc1, (const uint8_t *)(&desc1));

    usbdc_install_result_1 = USBD_DEV_Install_Config(usbd, USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT), usbdc1);

    if(1 != USBD_DEV_Get_Num_Installed_Configs(usbd))
    {
        printf("incorrect number of installed configs\n\r");
        REPORT_ERROR();
    }

    if((USBD_DEV_Get_Config(usbd, 0) != usbdc1)
      || ((uint8_t*)USBD_DEV_Get_Config_Desc(usbd, 0) != (uint8_t*)(&desc1))
      || (0 != usbdc_install_result_1.index)
      || (USBD_DEV_INSTALLATION_OK != usbdc_install_result_1.result))
    {
        printf("config_table[0] is not same which was installed\n\r");
        REPORT_ERROR();
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}



void test_strings(USBD_Params_XT *usbd)
{
    USBD_DEV_String_Header_XT *string_table = USBD_DEV_GET_STRING_TAB_PTR(usbd);

    USBD_ENTER_FUNC(MAIN_APP_TEST);

    USBD_Init(usbd);

    if(1 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }

    activity = 0;
    USBD_DEV_Activate(usbd, &port);

    if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
    {
        printf("incorrect state after USBD_DEV_Activate(usbd)!\n\r");
        REPORT_ERROR();
    }

    if(activity != 5)
    {
        printf("port methid \"activate\" didn't called!\n\r");
        REPORT_ERROR();
    }

    activity = 0;

    install_result_1 = USBD_DEV_Install_Manufacturer_String(usbd, (uint8_t*)string1, sizeof(string1));

    if(2 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings; should be %d; returned: %d; instalation error: %u\n\r",
         2, USBD_DEV_Get_Num_Installed_Strings(usbd), install_result_1.result);
        REPORT_ERROR();
    }
    else
    {
        if((0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        || (1 != install_result_1.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    install_result_2 = USBD_DEV_Install_Product_String(usbd, (uint8_t*)string2, sizeof(string2));

    if(3 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings; should be %d; returned: %d; instalation error: %u\n\r",
         3, USBD_DEV_Get_Num_Installed_Strings(usbd), install_result_2.result);
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        || (2 != install_result_2.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    install_result_3 = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));

    if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings; should be %d; returned: %d; instalation error: %u\n\r",
         4, USBD_DEV_Get_Num_Installed_Strings(usbd), install_result_3.result);
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if(0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        || (3 != install_result_3.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_String(usbd, (uint8_t*)string2);

    if((3 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if(0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_String(usbd, (uint8_t*)string2);

    if((3 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if(0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_String(usbd, (uint8_t*)string1);

    if((2 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_String(usbd, (uint8_t*)string3);

    if((1 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }

    remove_result = USBD_DEV_Remove_String(usbd, (uint8_t*)string3);

    if((1 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }

    install_result_1 = USBD_DEV_Install_Manufacturer_String( usbd, (uint8_t*)string1, sizeof(string1));
    install_result_2 = USBD_DEV_Install_Product_String(      usbd, (uint8_t*)string2, sizeof(string2));
    install_result_3 = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));

    if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings; should be %d; returned: %d\n\r"
         "instalation1 error: %u\n\rinstalation2 error: %u\n\rinstalation3 error: %u\n\r",
         4, USBD_DEV_Get_Num_Installed_Strings(usbd), install_result_1.result, install_result_2.result, install_result_3.result);
        REPORT_ERROR();
    }
    else
    {
        if((0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        || (1 != install_result_1.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        || (2 != install_result_2.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        || (3 != install_result_3.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_All_Strings(usbd);

    if((1 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings: %d\n\r", USBD_DEV_Get_Num_Installed_Strings(usbd));
        REPORT_ERROR();
    }

    install_result_1 = USBD_DEV_Install_Manufacturer_String( usbd, (uint8_t*)string1, sizeof(string1));
    install_result_2 = USBD_DEV_Install_Product_String(      usbd, (uint8_t*)string2, sizeof(string2));
    install_result_3 = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));
    install_result_4 = USBD_DEV_Install_String(usbd, (uint8_t*)string4, sizeof(string4), 4);
    install_result_5 = USBD_DEV_Install_String(usbd, (uint8_t*)string5, sizeof(string5), 5);
    install_result_6 = USBD_DEV_Install_String(usbd, (uint8_t*)string6, sizeof(string6), 6);

    if(5 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings; should be %d; returned: %d\n\r"
         "instalation1 error: %u\n\rinstalation2 error: %u\n\rinstalation3 error: %u\n\r"
         "instalation4 error: %u\n\rinstalation5 error: %u\n\rinstalation6 error: %u\n\r",
         5, USBD_DEV_Get_Num_Installed_Strings(usbd),
         install_result_1.result, install_result_2.result, install_result_3.result,
         install_result_4.result, install_result_5.result, install_result_6.result);
        REPORT_ERROR();
    }
    else
    {
        if((0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        || (1 != install_result_1.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        || (2 != install_result_2.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        || (3 != install_result_3.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[4].string, string4, sizeof(string4)))
        || (4 != install_result_4.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_4.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != install_result_5.index)
        || (USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE != install_result_5.result))
        {
            printf("string_table[0] should not be installed\n\r");
            REPORT_ERROR();
        }

        if((0 != install_result_6.index)
        || (USBD_DEV_INSTALLATION_INDEX_OUT_OF_RANGE != install_result_6.result))
        {
            printf("string_table[0] should not be installed\n\r");
            REPORT_ERROR();
        }
    }

    remove_result = USBD_DEV_Remove_All_Strings(usbd);

    if((1 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }

    install_result_1 = USBD_DEV_Install_Manufacturer_String( usbd, (uint8_t*)string1, sizeof(string1));
    install_result_2 = USBD_DEV_Install_Product_String(      usbd, (uint8_t*)string2, sizeof(string2));
    install_result_3 = USBD_DEV_Install_Serial_Number_String(usbd, (uint8_t*)string3, sizeof(string3));

    if(4 != USBD_DEV_Get_Num_Installed_Strings(usbd))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }
    else
    {
        if((0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        || (1 != install_result_1.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_1.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        || (2 != install_result_2.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_2.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if((0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        || (3 != install_result_3.index)
        || (USBD_DEV_INSTALLATION_OK != install_result_3.result))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    test_compose_configs(usbd);
    USBD_DEV_Reset(usbd);

    remove_result = USBD_DEV_Remove_All_Strings(usbd);

    if((4 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }
    else
    {
        if(0 != memcmp(string_table[1].string, string1, sizeof(string1)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if(0 != memcmp(string_table[2].string, string2, sizeof(string2)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }

        if(0 != memcmp(string_table[3].string, string3, sizeof(string3)))
        {
            printf("string_table[0] is not same which was installed\n\r");
            REPORT_ERROR();
        }
    }

    USBD_DEV_Deactivate(usbd);

    if(USBD_DEV_STATE_OFF != USBD_DEV_Get_State(usbd))
    {
        printf("incorrect state after USBD_DEV_Deactivate(usbd)!\n\r");
        REPORT_ERROR();
    }

    if(activity != 11)
    {
        printf("port methid \"activate\" didn't called!\n\r");
        REPORT_ERROR();
    }
    activity = 0;

    remove_result = USBD_DEV_Remove_All_Strings(usbd);

    if((1 != USBD_DEV_Get_Num_Installed_Strings(usbd)) || (!remove_result))
    {
        printf("incorrect number of installed strings\n\r");
        REPORT_ERROR();
    }

    USBD_EXIT_FUNC(MAIN_APP_TEST);
}
