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
#include "cmd_line.h"
#include "iocmd.h"
#include "usbd.h"
#include "main_usbd_init.h"
#include "iocmd_exe_usart.h"
#include "iocmd_exe_vcom.h"
#include "usbd_port_stm32_cat_a.h"

static void set_exe_vcom(IOCMD_Arg_DT *arg);
static void set_exe_uart(IOCMD_Arg_DT *arg);
static void usbd_dump(IOCMD_Arg_DT *arg);
static void buff_dump(IOCMD_Arg_DT *arg);
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
static void usbd_ring(IOCMD_Arg_DT *arg);
static void usbd_dsr(IOCMD_Arg_DT *arg);
static void usbd_dcd(IOCMD_Arg_DT *arg);
#endif

static const IOCMD_Command_Tree_XT vcom_cmds_tab[] =
{
   IOCMD_GROUP_START("exe"                                           , "sets logs output"),
   IOCMD_ELEM(          "vcom"               , set_exe_vcom          , "sets vcom as logs output"),
   IOCMD_ELEM(          "uart"               , set_exe_uart          , "sets uart as logs output"),
   IOCMD_GROUP_END(),
   IOCMD_ELEM(       "dump"                  , usbd_dump             , "USBD HW dump"),
   IOCMD_ELEM(       "buff"                  , buff_dump             , "buffers dump"),
#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)
   IOCMD_ELEM(       "ring"                  , usbd_ring             , "Toggles RING signal for vcom 0"),
   IOCMD_ELEM(       "dsr"                   , usbd_dsr              , "Toggles DSR signal for vcom 0"),
   IOCMD_ELEM(       "dcd"                   , usbd_dcd              , "Toggles DCD signal for vcom 0"),
#endif
};

static const IOCMD_Command_Tree_List_XT cmds_tree[] =
{
   {
      .branch = vcom_cmds_tab,
      .branch_num_elems = Num_Elems(vcom_cmds_tab)
   }
};
static IOCMD_Line_Collector_Params_XT line_collector =
{
   .cmds_tree = cmds_tree,
   .cmds_tree_num_elems = Num_Elems(cmds_tree),
   .line_pos = 0,
#ifdef IOCMD_USE_LOG
   .parse_also_lib_cmds = IOCMD_TRUE
#else
   .parse_also_lib_cmds = IOCMD_FALSE
#endif
};

extern void main_set_logs_exe(const IOCMD_Print_Exe_Params_XT *exe);

static void set_exe_vcom(IOCMD_Arg_DT *arg)
{
   IOCMD_UNUSED_PARAM(arg);

   main_set_logs_exe(vcom_get_exe());
}

static void set_exe_uart(IOCMD_Arg_DT *arg)
{
   IOCMD_UNUSED_PARAM(arg);

   main_set_logs_exe(usart_get_exe());
}

static void usbd_dump(IOCMD_Arg_DT *arg)
{
   IOCMD_UNUSED_PARAM(arg);
   USBD_Port_STM32_CAT_A_Print_HW_Dump("CMD", __LINE__);
}

static void dump_ring(const IOCMD_Print_Exe_Params_XT *arg_out, Buff_Ring_XT *buff)
{
#if(BUFF_RING_USE_EXTENSIONS)
   IOCMD_Oprintf(arg_out, "%-32s: %p\n\r", "extension", buff->extension);
   if(NULL != buff->extension)
   {
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_read_params",      buff->extension->on_read_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_write_params",     buff->extension->on_write_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_overwrite_params", buff->extension->on_overwrite_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_remove_params",    buff->extension->on_remove_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_read",             buff->extension->on_read);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_write",            buff->extension->on_write);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_overwrite",        buff->extension->on_overwrite);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_remove",           buff->extension->on_remove);
   }
#endif
#if(BUFF_RING_USE_PROTECTED_EXTENSIONS)
   IOCMD_Oprintf(arg_out, "%-32s: %p\n\r", "protected_extension", buff->protected_extension);
   if(NULL != buff->protected_extension)
   {
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_read_params",      buff->protected_extension->on_read_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_write_params",     buff->protected_extension->on_write_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_overwrite_params", buff->protected_extension->on_overwrite_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_remove_params",    buff->protected_extension->on_remove_params);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_read",             buff->protected_extension->on_read);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_write",            buff->protected_extension->on_write);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_overwrite",        buff->protected_extension->on_overwrite);
      IOCMD_Oprintf(arg_out, "   %-29s: %p\n\r", "on_remove",           buff->protected_extension->on_remove);
   }
#endif
   IOCMD_Oprintf(arg_out, "%-32s: %p\n\r", "memory", buff->memory);
#if(BUFF_RING_DATA_CHECK_IN_CHECK_OUT_ENABLED)
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\r", "original_size", buff->original_size);
#endif
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\r", "size", buff->size);
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\r", "busy_size", buff->busy_size);
#if(BUFF_RING_USE_BUSY_SIZE_MONITORING)
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\r", "max_busy_size", buff->max_busy_size);
#endif
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\r", "first_free", buff->first_free);
   IOCMD_Oprintf(arg_out, "%-32s: %d\n\n\r", "first_busy", buff->first_busy);
}

static void buff_dump(IOCMD_Arg_DT *arg)
{
   IOCMD_Oprintf(arg->arg_out, "vcom 0 IN:\n\r");
   dump_ring(arg->arg_out, CDC_Vcom_Get_In_Buf(&vcom_0));
   IOCMD_Oprintf(arg->arg_out, "vcom 0 OUT:\n\r");
   dump_ring(arg->arg_out, CDC_Vcom_Get_Out_Buf(&vcom_0));
   IOCMD_Oprintf(arg->arg_out, "vcom 1 IN:\n\r");
   dump_ring(arg->arg_out, CDC_Vcom_Get_In_Buf(&vcom_1));
   IOCMD_Oprintf(arg->arg_out, "vcom 1 OUT:\n\r");
   dump_ring(arg->arg_out, CDC_Vcom_Get_Out_Buf(&vcom_1));
   IOCMD_Oprintf(arg->arg_out, "vcom 2 IN-OUT:\n\r");
   dump_ring(arg->arg_out, CDC_Vcom_Get_Out_Buf(&vcom_2));


   IOCMD_Oprintf(arg->arg_out, "USART1->SR:   0X%04X\n\r", USART1->SR);
   IOCMD_Oprintf(arg->arg_out, "USART1->BRR:  0X%04X\n\r", USART1->BRR);
   IOCMD_Oprintf(arg->arg_out, "USART1->CR1:  0X%04X\n\r", USART1->CR1);
   IOCMD_Oprintf(arg->arg_out, "USART1->CR2:  0X%04X\n\r", USART1->CR2);
   IOCMD_Oprintf(arg->arg_out, "USART1->CR3:  0X%04X\n\r", USART1->CR3);
   IOCMD_Oprintf(arg->arg_out, "USART1->GTPR: 0X%04X\n\r", USART1->GTPR);

}

#if(CDC_VCOM_MODE_DATA_AND_SIGNALS == CDC_VCOM_MODE)

static void usbd_ring(IOCMD_Arg_DT *arg)
{
   static USBD_Bool_DT ring = USBD_FALSE;

   USBD_BOOL_TOGGLE(ring);

   IOCMD_Oprintf(arg->arg_out, "RING new value: %d", ring);

   CDC_VCOM_Set_Ring(&vcom_0, ring);
}

static void usbd_dsr(IOCMD_Arg_DT *arg)
{
   static USBD_Bool_DT dsr = USBD_FALSE;

   USBD_BOOL_TOGGLE(dsr);

   IOCMD_Oprintf(arg->arg_out, "DSR new value: %d", dsr);

   CDC_VCOM_Set_Dsr(&vcom_0, dsr);
}

static void usbd_dcd(IOCMD_Arg_DT *arg)
{
   static USBD_Bool_DT dcd = USBD_FALSE;

   USBD_BOOL_TOGGLE(dcd);

   IOCMD_Oprintf(arg->arg_out, "DCD new value: %d", dcd);

   CDC_VCOM_Set_Dcd(&vcom_0, dcd);
}

#endif

void Cmd_Parse_Bytes(const IOCMD_Print_Exe_Params_XT *exe, uint8_t *recv_bytes, size_t num_recv_bytes)
{
   IOCMD_Line_Collector_Parse_Bytes(&line_collector, exe, (char*)recv_bytes, num_recv_bytes);
}

const char *Cmd_Parser_Get_Current_Line(void)
{
   return line_collector.line;
}

