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

#ifndef PORT_TEST_H_
#include "port_test.h"
#endif

#ifndef PORT_TEST_CONTROL_H_
#include "port_test_control.h"
#endif

typedef struct port_test_io_ep_eXtended_Tag
{
   struct
   {
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT   *size_done;
         USBD_IO_Inout_Data_Size_DT    size;
         uint32_t                      num_transactions_passed;
         USBD_Bool_DT                  set;
      }user_data;
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT    size;
         USBD_IO_Inout_Data_Size_DT    packet_size;
      }buf;
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT    size;
         USBD_IO_Inout_Data_Size_DT    total_size;
         Port_Test_DMA_Type_T          dma_type;
      }dma;
      USBD_Bool_DT                     dont_wait_for_ready;
      USBD_Bool_DT                     is_ep_processed;
      USBD_Bool_DT                     is_ep_waiting;
      uint8_t                          ep_type;
      USBD_IO_Inout_Data_Size_DT       mps;
   }out;
   struct
   {
      struct
      {
         void                         *data;
         USBD_IO_Inout_Data_Size_DT   *size_done;
         USBD_IO_Inout_Data_Size_DT    size;
         uint32_t                      num_transactions_passed;
      }user_data;
      /*USBD_IO_Inout_Data_Size_DT*/ uint16_t     mps;
      struct
      {
         /*USBD_IO_Inout_Data_Size_DT*/ uint16_t  size;
      }buf0;
      struct
      {
         /*USBD_IO_Inout_Data_Size_DT*/ uint16_t  size;
      }buf1;
      struct
      {
         const void                   *data;
         USBD_IO_Inout_Data_Size_DT    size;
      //    USBD_IO_Inout_Data_Size_DT  total_size;
         Port_Test_DMA_Type_T          dma_type;
      }dma;
      uint8_t                          num_used_bufs;
      uint8_t                          max_num_bufs;
      uint8_t                          ep_type;
      USBD_Bool_DT                     data_provided;
   }in;
} port_test_io_ep_XT;

/** ******************************************************************************************************************************
 * USBD DEV part
 ********************************************************************************************************************************/

static void                         port_test_dev_activate_deactivate   (USBD_Params_XT *usbd, USBD_Bool_DT state);
static void                         port_test_dev_set_config_ep_parse_start_stop(USBD_Params_XT* usbd, USBD_Bool_DT is_before_parsing);
static USBD_Bool_DT                 port_test_dev_parse_ep              (USBD_Params_XT *usbd, uint8_t ep_num, const USB_Interface_Desc_DT *if_desc, const USB_Endpoint_Desc_DT *ep_desc);
static USBD_Bool_DT                 port_test_dev_parse_cfg             (USBD_Params_XT* usbd, const uint8_t *desc, uint16_t desc_size, USBD_DEV_Config_Desc_Check_Result_XT *details);
static USBD_DEV_Speed_ET            port_test_dev_get_supported_speed   (USBD_Params_XT *usbd);
static USBD_DEV_Speed_ET            port_test_dev_get_current_speed     (USBD_Params_XT *usbd);
static uint16_t                     port_test_dev_get_dev_status        (USBD_Params_XT *usbd);
static uint16_t                     port_test_dev_get_frame_nr          (USBD_Params_XT *usbd);
static const USB_Endpoint_Desc_DT*  port_test_dev_get_ep0_low_full_speed(USBD_Params_XT *usbd);
static const USB_Endpoint_Desc_DT*  port_test_dev_get_ep0_high_speed    (USBD_Params_XT *usbd);

static USBD_Bool_DT                 port_test_dev_activity = USBD_FALSE;
static USB_Endpoint_Desc_DT         port_test_dev_ep0_full = USB_ENDPOINT_DESC_STRUCT_FILL(
      /* _ep_num */              0,
      /* _ep_dir */              USB_EP_DESC_DIR_INOUT_CONTROL,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      TEST_EP0_MPS,
      /* _bInterval */           0xFF);
static USB_Endpoint_Desc_DT   port_test_dev_ep0_high = USB_ENDPOINT_DESC_STRUCT_FILL(
      /* _ep_num */              0,
      /* _ep_dir */              USB_EP_DESC_DIR_INOUT_CONTROL,
      /* _ep_type */             USB_EP_DESC_TRANSFER_TYPE_CONTROL,
      /* _ep_sync - iso */       USB_EP_DESC_SYNC_TYPE_NOT_USED,
      /* _ep_usage - iso/irq3 */ USB_EP_DESC_USAGE_TYPE_NOT_USED,
      /* _wMaxPacketSize */      TEST_EP0_MPS,
      /* _bInterval */           0xFF);
static USBD_DEV_Speed_ET               port_test_dev_speed = USBD_DEV_FULL_AND_HIGH_SPEED;
static uint16_t                        port_test_dev_device_status = 0;
static uint16_t                        port_test_dev_frame_nr = 0;
static const USBD_DEV_Port_Handler_XT  port_test_dev =
{
   .handlers.activate               = port_test_dev_activate_deactivate,
#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
   .handlers.ep_parse               = port_test_dev_parse_ep,
   .handlers.cfg_parse              = port_test_dev_parse_cfg,
#endif
#if((USBD_DEV_LOW_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED) && (USBD_DEV_FULL_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED))
   .handlers.get_supported_speed    = port_test_dev_get_supported_speed,
   .handlers.get_current_speed      = port_test_dev_get_current_speed,
#endif
   .handlers.get_device_status      = port_test_dev_get_dev_status,
   .handlers.get_frame_num          = port_test_dev_get_frame_nr,
   .handlers.get_ep0_low_full_speed = port_test_dev_get_ep0_low_full_speed,
#if(USBD_DEV_SUPPORTED_SPEED >= USBD_DEV_HIGH_SPEED_SUPPORTED)
   .handlers.get_ep0_high_speed     = port_test_dev_get_ep0_high_speed,
#endif
   .data.ep_both_directions_must_have_same_type = USBD_TRUE
};
static USBD_Params_XT *port_test_usbd = USBD_MAKE_INVALID_PTR(USBD_Params_XT);

/** ******************************************************************************************************************************
 * USBD IO part
 ********************************************************************************************************************************/

#ifdef USBD_IO_PRESENT

static void                         port_test_process_data_in           (port_test_io_ep_XT *ep, const void* data, USBD_IO_Inout_Data_Size_DT size);
static USBD_IO_Inout_Data_Size_DT   port_test_io_data_in_process_bufs   (port_test_io_ep_XT *ep, const void* data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_test_process_data_out          (port_test_io_ep_XT *ep, void* data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_test_io_in_provide             (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_test_io_in_memcpy              (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_test_io_out_provide            (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_IO_Inout_Data_Size_DT   port_test_io_out_memcpy             (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part);
static USBD_Bool_DT                 port_test_io_trigger                (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait_for_ready);
static USBD_IO_Inout_Data_Size_DT   port_test_io_get_ep_out_waiting_size(USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT left_size);
static USBD_IO_Inout_Data_Size_DT   port_test_io_get_ep_in_buffered_size(USBD_Params_XT *usbd, uint8_t ep_num);
static void                         port_test_io_stall                  (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir);
static void                         port_test_io_abort                  (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs);
static void                         port_test_io_halt                   (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state);
static void                         port_test_io_configure              (USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state);

static port_test_io_ep_XT                    port_test_io_ep[USBD_MAX_NUM_ENDPOINTS];
static const USBD_IO_DOWN_Common_Handlers_XT port_test_io_common = USBD_IO_DOWN_FILL_COMMON_HANDLERS(
      port_test_io_trigger,
      port_test_io_get_ep_out_waiting_size,
      port_test_io_get_ep_in_buffered_size,
      port_test_io_abort,
      port_test_io_stall,
      port_test_io_halt,
      port_test_io_configure);

#endif

/** ******************************************************************************************************************************
 * USBD DEV part functions
 ********************************************************************************************************************************/

static void port_test_dev_activate_deactivate(USBD_Params_XT* usbd, USBD_Bool_DT state)
{
   port_test_io_ep_XT *ep;
   uint8_t ep_num;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_BOOL_IS_TRUE(state))
   {
      port_test_dev_activity = USBD_TRUE;

#ifdef USBD_IO_PRESENT
      USBD_IO_DOWN_Set_Common_Handlers(usbd, &port_test_io_common);
#endif

      for(ep_num = 0; ep_num < USBD_MAX_NUM_ENDPOINTS; ep_num++)
      {
         ep = &port_test_io_ep[ep_num];

         ep->out.mps                = 0;
         ep->out.user_data.data     = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->out.user_data.size     = 0;
         ep->out.is_ep_waiting      = USBD_FALSE;
         ep->out.buf.size           = (-1);
         ep->out.buf.packet_size    = (-1);
         ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->out.dma.size           = 0;
         ep->out.dma.total_size     = 0;

         ep->in.mps                 = 0;
         ep->in.user_data.data      = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->in.user_data.size      = 0;
         ep->in.max_num_bufs        = 1;
         ep->in.num_used_bufs       = 0;
         ep->in.data_provided       = USBD_FALSE;
         ep->in.dma.size            = 0;
   //      ep->in.dma.total_size      = 0;
         ep->in.buf0.size           = 0;
         ep->in.buf1.size           = 0;
      }

      port_test_usbd = usbd;
   }
   else
   {
      port_test_dev_activity = USBD_FALSE;
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_dev_activate_deactivate */

static void port_test_dev_set_config_ep_parse_start_stop(USBD_Params_XT* usbd, USBD_Bool_DT is_before_parsing)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_dev_set_config_ep_parse_start_stop */

static USBD_Bool_DT port_test_dev_parse_ep(
   USBD_Params_XT* usbd,
   uint8_t ep_num,
   const USB_Interface_Desc_DT *if_desc,
   const USB_Endpoint_Desc_DT *ep_desc)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   result = USBD_TRUE;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_test_dev_parse_ep */

USBD_Bool_DT port_test_dev_parse_cfg(
   USBD_Params_XT* usbd,
   const uint8_t *desc,
   uint16_t desc_size,
   USBD_DEV_Config_Desc_Check_Result_XT *details)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   result = USBD_TRUE;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_test_dev_parse_cfg */

static USBD_DEV_Speed_ET port_test_dev_get_supported_speed(USBD_Params_XT *usbd)
{
   USBD_DEV_Speed_ET result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   result = USBD_DEV_FULL_AND_HIGH_SPEED;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_test_dev_get_supported_speed */

static USBD_DEV_Speed_ET port_test_dev_get_current_speed(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return port_test_dev_speed;
} /* port_test_dev_get_current_speed */

static uint16_t port_test_dev_get_dev_status(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return port_test_dev_device_status;
} /* port_test_dev_get_dev_status */

static uint16_t port_test_dev_get_frame_nr(USBD_Params_XT *usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   port_test_dev_frame_nr ++;
   port_test_dev_frame_nr &= 0x7FF;

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return port_test_dev_frame_nr;
} /* port_test_dev_get_frame_nr */

static const USB_Endpoint_Desc_DT* port_test_dev_get_ep0_low_full_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&port_test_dev_ep0_full);
} /* port_test_dev_get_ep0_low_full_speed */

static const USB_Endpoint_Desc_DT* port_test_dev_get_ep0_high_speed(USBD_Params_XT* usbd)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return (&port_test_dev_ep0_high);
} /* port_test_dev_get_ep0_high_speed */

/**
 * PUBLIC INTERFACE
 */

const USBD_DEV_Port_Handler_XT *USBD_Port_Test_Get_Port(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return(&port_test_dev);
} /* USBD_port_test_get_port */

/**
 * ADDDITIONAL EXTERNAL INTERFACE USED FOR TESTING
 */

void port_test_attached(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Attached(port_test_usbd, USBD_TRUE);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_attached */

void port_test_detached(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Attached(port_test_usbd, USBD_FALSE);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_detached */

void port_test_powered(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Powered(port_test_usbd, USBD_TRUE);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_powered */

void port_test_unpowered(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Powered(port_test_usbd, USBD_FALSE);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_unpowered */

void port_test_reset(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Reset(port_test_usbd);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_reset */

void port_test_addressed(uint8_t addr)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Addressed(port_test_usbd, addr);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_addressed */

USBD_Bool_DT port_test_configured(uint8_t config_num)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      result = USBD_DEV_Configured(port_test_usbd, config_num, USBD_MAKE_INVALID_HANDLER(USBD_DEV_Set_Configuration_Respond_HT));
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);

   return result;
} /* port_test_configured */

void port_test_suspended(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_Suspended(port_test_usbd);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_suspended */

void port_test_sof_received(void)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_DEV);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd))
   {
      USBD_DEV_SOF_Received(port_test_usbd);
   }
   else
   {
      USBD_WARN_1(USBD_DBG_PORT_INVALID_PARAMS, "function invalid parameter! usbd: %p", port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_DEV);
} /* port_test_sof_received */

/** ******************************************************************************************************************************
 * USBD IO part functions
 ********************************************************************************************************************************/

#ifdef USBD_IO_PRESENT

static void port_test_print_internal_out_state(const char *desc, uint8_t ep_num)
{
#if 1
   port_test_io_ep_XT *ep;

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "%s; port params for EP OUT: %d", desc, ep_num);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO,
         "   user_data:: data: %p; size_done: %p; *size_done: %d; size: %d; num_transactions_passed: %d",
         ep->out.user_data.data,
         ep->out.user_data.size_done,
         (NULL != ep->out.user_data.size_done) ? *(ep->out.user_data.size_done) : 0,
         ep->out.user_data.size,
         ep->out.user_data.num_transactions_passed);
      USBD_DEBUG_LO_1(USBD_DBG_PORT_IO,
         "   user_data:: set: %s",
         ep->out.user_data.set ? "true" : "false");
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   buf:: data: %p; size: %d; packet_size: %d",
         ep->out.buf.data,
         ep->out.buf.size,
         ep->out.buf.packet_size);
      USBD_DEBUG_LO_4(USBD_DBG_PORT_IO, "   dma:: data: %p; size: %d; total_size: %d; dma_type: %d",
         ep->out.dma.data,
         ep->out.dma.size,
         ep->out.dma.total_size,
         ep->out.dma.dma_type);
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   is_ep_waiting: %s; ep_type: %d; mps: %d",
         ep->out.is_ep_waiting ? "true" : "false",
         ep->out.ep_type,
         ep->out.mps);
   }

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "%s; port params for EP IN: %d", desc, ep_num);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO,
         "   user_data:: data: %p; size_done: %p; *size_done: %d; size: %d; num_transactions_passed: %d",
         ep->in.user_data.data,
         ep->in.user_data.size_done,
         (NULL != ep->in.user_data.size_done) ? *(ep->in.user_data.size_done) : 0,
         ep->in.user_data.size,
         ep->in.user_data.num_transactions_passed);
      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "   buf0:: size: %d; buf1:: size: %d",
         ep->in.buf0.size,
         ep->in.buf1.size);
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   dma:: data: %p; size: %d; dma_type: %d",
         ep->in.dma.data,
         ep->in.dma.size,
         ep->in.dma.dma_type);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO, "   mps: %d; num_used_bufs: %d; max_num_bufs: %d; ep_type: %d; data_provided: %s",
         ep->in.mps,
         ep->in.num_used_bufs,
         ep->in.max_num_bufs,
         ep->in.ep_type,
         ep->in.data_provided ? "true" : "false");
   }
#endif
} /* port_test_print_internal_out_state */

static void port_test_print_internal_in_state(const char *desc, uint8_t ep_num)
{
#if 1
   port_test_io_ep_XT *ep;

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "%s; port params for EP OUT: %d", desc, ep_num);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO,
         "   user_data:: data: %p; size_done: %p; *size_done: %d; size: %d; num_transactions_passed: %d",
         ep->out.user_data.data,
         ep->out.user_data.size_done,
         (NULL != ep->out.user_data.size_done) ? *(ep->out.user_data.size_done) : 0,
         ep->out.user_data.size,
         ep->out.user_data.num_transactions_passed);
      USBD_DEBUG_LO_1(USBD_DBG_PORT_IO,
         "   user_data:: set: %s",
         ep->out.user_data.set ? "true" : "false");
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   buf:: data: %p; size: %d; packet_size: %d",
         ep->out.buf.data,
         ep->out.buf.size,
         ep->out.buf.packet_size);
      USBD_DEBUG_LO_4(USBD_DBG_PORT_IO, "   dma:: data: %p; size: %d; total_size: %d; dma_type: %d",
         ep->out.dma.data,
         ep->out.dma.size,
         ep->out.dma.total_size,
         ep->out.dma.dma_type);
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   is_ep_waiting: %s; ep_type: %d; mps: %d",
         ep->out.is_ep_waiting ? "true" : "false",
         ep->out.ep_type,
         ep->out.mps);
   }

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "%s; port params for EP IN: %d", desc, ep_num);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO,
         "   user_data:: data: %p; size_done: %p; *size_done: %d; size: %d; num_transactions_passed: %d",
         ep->in.user_data.data,
         ep->in.user_data.size_done,
         (NULL != ep->in.user_data.size_done) ? *(ep->in.user_data.size_done) : 0,
         ep->in.user_data.size,
         ep->in.user_data.num_transactions_passed);
      USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "   buf0:: size: %d; buf1:: size: %d",
         ep->in.buf0.size,
         ep->in.buf1.size);
      USBD_DEBUG_LO_3(USBD_DBG_PORT_IO, "   dma:: data: %p; size: %d; dma_type: %d",
         ep->in.dma.data,
         ep->in.dma.size,
         ep->in.dma.dma_type);
      USBD_DEBUG_LO_5(USBD_DBG_PORT_IO, "   mps: %d; num_used_bufs: %d; max_num_bufs: %d; ep_type: %d; data_provided: %s",
         ep->in.mps,
         ep->in.num_used_bufs,
         ep->in.max_num_bufs,
         ep->in.ep_type,
         ep->in.data_provided ? "true" : "false");
   }
#endif
} /* port_test_print_internal_in_state */



static void port_test_process_data_in(port_test_io_ep_XT *ep, const void *data, USBD_IO_Inout_Data_Size_DT size)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(void, ep->in.user_data.data))
   {
      USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size: %d; data: %p",
         (ep - port_test_io_ep) / sizeof(port_test_io_ep_XT), size, data);

      /* update user-defined counter of transferred data */
      if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, ep->in.user_data.size_done))
      {
         *(ep->in.user_data.size_done) += size;
      }

      /* buffer overflow protection for memcpy operation */
      if(ep->in.user_data.size < size)
      {
         size = ep->in.user_data.size;
      }

      /* copy data to user-defined buffer */
      memcpy(ep->in.user_data.data, data, size);

      /* update user-defined data pointer and data size */
      if(ep->in.user_data.size > size)
      {
         ep->in.user_data.data = &((uint8_t*)(ep->in.user_data.data))[size];
         ep->in.user_data.size -= size;
      }
      else
      {
         ep->in.user_data.size = 0;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
}/* port_test_process_data_in */

static USBD_IO_Inout_Data_Size_DT port_test_io_data_in_process_bufs(
   port_test_io_ep_XT *ep,
   const void* data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_Bool_DT is_last_part)
{
   USBD_IO_Inout_Data_Size_DT  part;
   USBD_IO_Inout_Data_Size_DT  result = 0;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   ep->in.data_provided = USBD_TRUE;

   if(2 == ep->in.max_num_bufs)
   {
      USBD_DEBUG_MID_1(USBD_DBG_PORT_IO, "2 bufs; used bufs: %d", ep->in.num_used_bufs);

      if(0 == ep->in.num_used_bufs)
      {
         /* init EP - copy data to EP buffer */
         part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size)) ?
            (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size) : size;

         port_test_process_data_in(ep, data, part);

         result            = part;
         ep->in.buf0.size += (uint16_t)part;

         if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))) )
         {
            ep->in.num_used_bufs = 1;

            /* increment counter of processed transactions */
            (ep->in.user_data.num_transactions_passed)++;
         }

         if(size > part)
         {
            size -= part;
            data  = &((uint8_t*)data)[part];

            /* init EP - copy data to EP buffer */
            part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size)) ?
               (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size) : size;

            port_test_process_data_in(ep, data, part);

            result            += part;
            ep->in.buf1.size  += (uint16_t)part;

            if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
            {
               ep->in.num_used_bufs = 2;

               /* increment counter of processed transactions */
               (ep->in.user_data.num_transactions_passed)++;
            }
         }
      }
      else if(1 == ep->in.num_used_bufs)
      {
         /* init EP - copy data to EP buffer */
         part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size)) ?
            (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf1.size) : size;

         port_test_process_data_in(ep, data, part);

         result            = part;
         ep->in.buf1.size += (uint16_t)part;

         if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
         {
            ep->in.num_used_bufs = 2;

            /* increment counter of processed transactions */
            (ep->in.user_data.num_transactions_passed)++;
         }
      }
   }
   else if(0 == ep->in.num_used_bufs)
   {
      USBD_DEBUG_MID_1(USBD_DBG_PORT_IO, "1 buf; used bufs: %d", ep->in.num_used_bufs);

      /* init EP - copy data to EP buffer */
      part = (size > (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size)) ?
         (USBD_IO_Inout_Data_Size_DT)(ep->in.mps - ep->in.buf0.size) : size;

      port_test_process_data_in(ep, data, part);

      result            = part;
      ep->in.buf0.size += (uint16_t)part;

      if(USBD_BOOL_IS_TRUE(is_last_part) || (size >= ((USBD_IO_Inout_Data_Size_DT)(ep->in.mps))))
      {
         ep->in.num_used_bufs = 1;

         /* increment counter of processed transactions */
         (ep->in.user_data.num_transactions_passed)++;
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_data_in_process_bufs */



USBD_Bool_DT port_test_simulate_out_packet(uint8_t ep_num)
{
   port_test_io_ep_XT *ep;
   USBD_Bool_DT result = USBD_FALSE;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   port_test_print_internal_out_state("state before simulate_out_packet", ep_num);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      /* these few lines emulate reading parameters of received data from HW registers */
      ep->out.buf.size        = (ep->out.user_data.size > ep->out.mps) ? ep->out.mps : ep->out.user_data.size;
      ep->out.buf.packet_size = ep->out.buf.size;
      ep->out.buf.data        = ep->out.user_data.data;

      /* update size and pointer to next packets which will be emulated */
      ep->out.user_data.size -= ep->out.buf.size;
      ep->out.user_data.data  = &((uint8_t*)(ep->out.user_data.data))[ep->out.buf.size];

      /* increment counter of processed transactions */
      (ep->out.user_data.num_transactions_passed)++;

      /* give result information which tells if next packet shall be simulated */
      if(ep->out.user_data.size > 0)
      {
         result = USBD_TRUE;
      }
      else
      {
         result = USBD_FALSE;
      }
   }

   if(USBD_BOOL_IS_TRUE(result))
   {
      port_test_print_internal_out_state("state after simulate_out_packet with result: true", ep_num);
   }
   else
   {
      port_test_print_internal_out_state("state after simulate_out_packet with result: false", ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_simulate_out_packet */

static USBD_IO_Inout_Data_Size_DT port_test_process_data_out(
   port_test_io_ep_XT *ep,
   void* data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_Bool_DT is_last_part)
{
   uint8_t ep_num = (ep - port_test_io_ep) / sizeof(port_test_io_ep_XT);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size: %d; data: %p", ep_num, size, data);

   port_test_print_internal_out_state("state before process_data_out", ep_num);

   /* protection and setting ep->out.buf.size to -1 in case when whole data is read */
   if(ep->out.buf.size <= size)
   {
      size = ep->out.buf.size;

      ep->out.buf.size -= 1;
   }

   ep->out.buf.size    -= size;

   if(USBD_CHECK_PTR(void, ep->out.buf.data))
   {
      memcpy(data, ep->out.buf.data, size);
      ep->out.buf.data = &((uint8_t*)(ep->out.buf.data))[size];
   }
   else
   {
      memset(data, 0, size);
      USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "EP: %d; buf data pointer invalid, memset %d bytes to 0", ep_num, size);
   }

   ep->out.is_ep_processed    = USBD_TRUE;

   if(USBD_BOOL_IS_TRUE(is_last_part))
   {
      ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
      ep->out.is_ep_waiting   = USBD_FALSE;
   }

   /* update user-defined counter of transferred data */
   if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, ep->out.user_data.size_done))
   {
      USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size_done: %d; size: %d", ep_num, *(ep->out.user_data.size_done), size);
      *(ep->out.user_data.size_done) += size;
      USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size_done: %d; size: %d", ep_num, *(ep->out.user_data.size_done), size);
   }

   port_test_print_internal_out_state("state after process_data_out", ep_num);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
}/* port_test_process_data_out */



static USBD_IO_Inout_Data_Size_DT port_test_io_in_provide(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   port_test_print_internal_in_state("state before port_test_io_in_provide", ep_num);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && USBD_CHECK_PTR(void, data))
   {
      ep = &port_test_io_ep[ep_num];

      if(0 == ep->in.dma.size)
      {
         size                /= ep->in.mps;
         size                *= ep->in.mps;
         ep->in.dma.data      = data;
         ep->in.dma.size      = size;
      //    ep->in.dma.total_size   = size + ep->in.buf0.size + ep->in.buf1.size;
         result               = size;

         USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "%d bytes taken by dma", result);

         if(PORT_TEST_TRUE_DMA == ep->in.dma.dma_type)
         {
            if(0 == ep->in.num_used_bufs)
            {
               /**
                * set DMA channel to send the data only if buffers are empty. this condition is an examply only because
                * it depends strongly to hadrware behavior.
                */
               port_test_process_data_in(ep, data, size);

               /* increment counter of processed transactions */
               (ep->in.user_data.num_transactions_passed)++;

               ep->in.dma.data = USBD_MAKE_INVALID_PTR(uint8_t);
            }
         }
         else if(PORT_TEST_HALF_DMA == ep->in.dma.dma_type)
         {
            if(ep->in.num_used_bufs < ep->in.max_num_bufs)
            {
               size = port_test_io_data_in_process_bufs(ep, data, size, USBD_TRUE);

               ep->in.dma.data  = &(((uint8_t*)(ep->in.dma.data))[size]);
               ep->in.dma.size -= size;
            }
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! data: %p, ep_num: %d", data, ep_num);
   }

   port_test_print_internal_in_state("state after port_test_io_in_provide", ep_num);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_in_provide */

static USBD_IO_Inout_Data_Size_DT port_test_io_in_memcpy(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "data: %p, size: %d", data, size);

   port_test_print_internal_in_state("state before port_test_io_in_memcpy", ep_num);

   size = port_test_io_data_in_process_bufs(&port_test_io_ep[ep_num], data, size, is_last_part);

   port_test_print_internal_in_state("state after port_test_io_in_memcpy", ep_num);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
} /* port_test_io_in_memcpy */

static USBD_IO_Inout_Data_Size_DT port_test_io_out_provide(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = 0;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if((ep_num < USBD_MAX_NUM_ENDPOINTS) && USBD_CHECK_PTR(void, data))
   {
      ep = &port_test_io_ep[ep_num];

      USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "%d bytes provided by TP", size);

      if(0 == ep->out.dma.size)
      {
         size                   /= ep->out.mps;
         size                   *= ep->out.mps;
         ep->out.dma.data        = data;
         ep->out.dma.size        = size;
         ep->out.dma.total_size  = size;
         result                  = size;

         USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "%d bytes taken by dma", result);

         if(PORT_TEST_TRUE_DMA == ep->out.dma.dma_type)
         {
            /**
             * set DMA channel to recv the data. this is an examply only because
             * it depends strongly to hadrware behavior.
             */
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! data: %p, ep_num: %d", data, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_out_provide */

static USBD_IO_Inout_Data_Size_DT port_test_io_out_memcpy(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, void *data, USBD_IO_Inout_Data_Size_DT size, USBD_Bool_DT is_last_part)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "data: %p, size: %d", data, size);

   size = port_test_process_data_out(&port_test_io_ep[ep_num], data, size, is_last_part);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return size;
} /* port_test_io_out_memcpy */

static USBD_Bool_DT port_test_io_trigger(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT dont_wait_for_ready)
{
   port_test_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      result = USBD_TRUE;
      ep = &port_test_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         port_test_print_internal_out_state("state before port_test_io_trigger", ep_num);

         transaction = USBD_IO_GET_OUT_TRANSACATION_PARAMS(usbd, ep_num);
         tp_params   = USBD_IO_GET_OUT_TP_PARAMS_PTR(usbd, ep_num);

         ep->out.dont_wait_for_ready = dont_wait_for_ready;
         ep->out.is_ep_processed    = USBD_TRUE;

         USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "tp_params: %p, transaction: %p", tp_params, transaction);

         if(PORT_TEST_TRUE_DMA == ep->out.dma.dma_type)
         {
            if((ep->out.buf.size > 0) && USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);

               USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                  usbd, tp_params, transaction, ep->out.buf.packet_size, ep->out.buf.size, port_test_io_out_memcpy);

               if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting) && (ep->out.buf.size <= 0))
               {
                  ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
                  ep->out.buf.size        = 0;
                  ep->out.is_ep_waiting   = USBD_FALSE;
               }
            }
            else
            {
               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.buf.size        = 0;
               ep->out.is_ep_waiting   = USBD_FALSE;
            }

            if(USBD_BOOL_IS_FALSE(ep->out.is_ep_waiting) && USBD_IO_CHECK_OUT_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "provide with size: %d", (-1));

               USBD_IO_CALL_OUT_PROVIDE_HANDLER(
                  usbd, tp_params, transaction, ep->out.buf.packet_size, (-1), port_test_io_out_provide);
            }
         }
         else if(PORT_TEST_HALF_DMA == ep->out.dma.dma_type)
         {
            if((ep->out.buf.size > 0) && USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);

               USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                  usbd, tp_params, transaction, ep->out.buf.packet_size, ep->out.buf.size, port_test_io_out_memcpy);

               if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting) && (ep->out.buf.size <= 0))
               {
                  ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
                  ep->out.buf.size        = 0;
                  ep->out.is_ep_waiting   = USBD_FALSE;
               }
            }
            else
            {
               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.buf.size        = 0;
               ep->out.is_ep_waiting   = USBD_FALSE;
            }

            if(USBD_BOOL_IS_FALSE(ep->out.is_ep_waiting) && USBD_IO_CHECK_OUT_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "provide with size: %d", (-1));

               USBD_IO_CALL_OUT_PROVIDE_HANDLER(
                  usbd, tp_params, transaction, ep->out.buf.packet_size, (-1), port_test_io_out_provide);
            }
         }
         else
         {
            if((ep->out.buf.size > 0) && USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);

               USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                  usbd, tp_params, transaction, ep->out.buf.packet_size, ep->out.buf.size, port_test_io_out_memcpy);

               if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting) && (ep->out.buf.size <= 0))
               {
                  ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
                  ep->out.buf.size        = 0;
                  ep->out.is_ep_waiting   = USBD_FALSE;
               }
            }
            else
            {
               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.buf.size        = 0;
               ep->out.is_ep_waiting   = USBD_FALSE;
            }
         }

         port_test_print_internal_out_state("state after port_test_io_trigger", ep_num);
      }
      else
      /* USB_EP_DIRECTION_IN */
      {
         port_test_print_internal_in_state("state before port_test_io_trigger", ep_num);

         transaction = USBD_IO_GET_IN_TRANSACATION_PARAMS(usbd, ep_num);
         tp_params   = USBD_IO_GET_IN_TP_PARAMS_PTR(usbd, ep_num);

         USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "tp_params: %p, transaction: %p", tp_params, transaction);

         if(PORT_TEST_TRUE_DMA == ep->in.dma.dma_type)
         {
            if(USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "provide in");
               USBD_IO_CALL_IN_PROVIDE_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_provide);
            }
            if((0 == ep->in.dma.size)
               && (ep->in.num_used_bufs < ep->in.max_num_bufs))
            {
               if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
               {
                  USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
                  USBD_IO_CALL_IN_MEMCPY_HANDLER(usbd, tp_params, transaction, port_test_io_in_memcpy);
               }
               else if( ((0 == ep->in.num_used_bufs) && (ep->in.buf0.size > 0))
                  || ((1 == ep->in.num_used_bufs) && (ep->in.buf1.size > 0)) )
               {
                  (void)port_test_io_data_in_process_bufs(ep, &ep_num, 0, USBD_TRUE);
               }
            }
         }
         else if(PORT_TEST_HALF_DMA == ep->in.dma.dma_type)
         {
            if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "provide in");
               USBD_IO_CALL_IN_PROVIDE_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_provide);
            }
            if(ep->in.num_used_bufs < ep->in.max_num_bufs)
            {
               if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
               {
                  USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
                  USBD_IO_CALL_IN_MEMCPY_HANDLER(usbd, tp_params, transaction, port_test_io_in_memcpy);
               }
               else if( ((0 == ep->in.num_used_bufs) && (ep->in.buf0.size > 0))
                  || ((1 == ep->in.num_used_bufs) && (ep->in.buf1.size > 0)) )
               {
                  (void)port_test_io_data_in_process_bufs(ep, &ep_num, 0, USBD_TRUE);
               }
            }
         }
         else
         {
            if(ep->in.num_used_bufs < ep->in.max_num_bufs)
            {
               if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
               {
                  USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
                  USBD_IO_CALL_IN_MEMCPY_HANDLER(usbd, tp_params, transaction, port_test_io_in_memcpy);
               }
               else if( ((0 == ep->in.num_used_bufs) && (ep->in.buf0.size > 0))
                  || ((1 == ep->in.num_used_bufs) && (ep->in.buf1.size > 0)) )
               {
                  (void)port_test_io_data_in_process_bufs(ep, &ep_num, 0, USBD_TRUE);
               }
            }
         }

         port_test_print_internal_in_state("state after port_test_io_trigger", ep_num);
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_trigger */

static USBD_IO_Inout_Data_Size_DT port_test_io_get_ep_out_waiting_size(USBD_Params_XT *usbd, uint8_t ep_num, USBD_Bool_DT left_size)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_test_io_ep[ep_num];

      if(USBD_BOOL_IS_TRUE(ep->out.is_ep_waiting))
      {
         if(USBD_BOOL_IS_TRUE(left_size))
         {
            result = ep->out.buf.size;
         }
         else
         {
            result = ep->out.buf.packet_size;
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! usbd: %p, ep_num: %d", usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_get_ep_out_waiting_size */

static USBD_IO_Inout_Data_Size_DT port_test_io_get_ep_in_buffered_size(USBD_Params_XT *usbd, uint8_t ep_num)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(USBD_Params_XT, usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_test_io_ep[ep_num];

      if((ep->in.dma.size > 0) || (ep->in.num_used_bufs > 0))
      {
         result =
            (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size) + ep->in.dma.size;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "invalid params! usbd: %p, ep_num: %d", usbd, ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_io_get_ep_in_buffered_size */

static void port_test_io_stall(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "EP: %d; dir: %s", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in");

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_io_stall */

static void port_test_io_abort(USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, USBD_Bool_DT flush_hw_bufs)
{
   port_test_io_ep_XT *ep;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_2(USBD_DBG_PORT_IO, "EP: %d; dir: %s", ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in");

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         ep->out.user_data.data     = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->out.user_data.size     = 0;
         ep->out.is_ep_waiting      = USBD_FALSE;
         ep->out.buf.size           = (-1);
         ep->out.buf.packet_size    = (-1);
         ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->out.dma.size           = 0;
         ep->out.dma.total_size     = 0;
      }
      else
      {
         ep->in.user_data.data      = USBD_MAKE_INVALID_PTR(uint8_t);
         ep->in.user_data.size      = 0;
         if(USBD_BOOL_IS_TRUE(flush_hw_bufs))
         {
            ep->in.num_used_bufs    = 0;
            ep->in.buf0.size        = 0;
            ep->in.buf1.size        = 0;
            ep->in.data_provided    = USBD_FALSE;
            ep->in.dma.size         = 0;
     //        ep->in.dma.total_size      = 0;
         }
      }
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_io_stall */

static void port_test_io_halt(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; state: %s",
         ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in", USBD_BOOL_IS_TRUE(state) ? "true" : "false");

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_io_halt */

static void port_test_io_configure(
   USBD_Params_XT *usbd, uint8_t ep_num, USB_EP_Direction_ET dir, const USB_Endpoint_Desc_DT *ep_desc, USBD_Bool_DT state)
{
   port_test_io_ep_XT *ep;
   uint8_t max_num_endpoints;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; state: %s",
         ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in", USBD_BOOL_IS_TRUE(state) ? "true" : "false");

   if(USB_EP_DIRECTION_OUT == dir)
   {
      port_test_print_internal_out_state("state before port_test_io_configure", ep_num);
   }
   else
   {
      port_test_print_internal_in_state("state before port_test_io_configure", ep_num);
   }

   max_num_endpoints = USBD_DEV_Get_Num_Endpoints_In_Active_Config(usbd);

   if(ep_num < max_num_endpoints)
   {
      ep = &port_test_io_ep[ep_num];

      if(USBD_BOOL_IS_TRUE(state))
      {
         if(USB_EP_DIRECTION_OUT == dir)
         {
            if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
            {
               ep->out.mps             = ((USBD_IO_Inout_Data_Size_DT)(ep_desc->wMaxPacketSize.H)) & 0x07;
               ep->out.mps            *= 256;
               ep->out.mps            += ep_desc->wMaxPacketSize.L;
               ep->out.ep_type         = ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK;
            }
            else
            {
               USBD_ALERT(USBD_DBG_PORT_IO, "EP descriptor invalid!");
            }
            ep->out.user_data.data     = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->out.user_data.size     = 0;
            ep->out.is_ep_waiting      = USBD_FALSE;
            ep->out.buf.size           = (-1);
            ep->out.buf.packet_size    = (-1);
            ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->out.dma.size           = 0;
            ep->out.dma.total_size     = 0;
         }
         else
         {
            if(USBD_CHECK_PTR(const USB_Endpoint_Desc_DT, ep_desc))
            {
               ep->in.mps              = ((uint16_t)(ep_desc->wMaxPacketSize.H)) & 0x07;
               ep->in.mps             *= 256;
               ep->in.mps             += ep_desc->wMaxPacketSize.L;
               ep->in.ep_type          = ep_desc->bmAttributes & USB_EP_DESC_TRANSFER_TYPE_MASK;
            }
            else
            {
               USBD_ALERT(USBD_DBG_PORT_IO, "EP descriptor invalid!");
            }
            ep->in.user_data.data      = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->in.user_data.size      = 0;
            ep->in.max_num_bufs        = 1;
            ep->in.num_used_bufs       = 0;
            ep->in.data_provided       = USBD_FALSE;
            ep->in.dma.size            = 0;
      //      ep->in.dma.total_size      = 0;
            ep->in.buf0.size           = 0;
            ep->in.buf1.size           = 0;
         }
      }
      else
      {
         if(USB_EP_DIRECTION_OUT == dir)
         {
            ep->out.mps                = 0;
            ep->out.user_data.data     = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->out.user_data.size     = 0;
            ep->out.is_ep_waiting      = USBD_FALSE;
            ep->out.buf.size           = (-1);
            ep->out.buf.packet_size    = (-1);
            ep->out.buf.data           = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->out.dma.size           = 0;
            ep->out.dma.total_size     = 0;
         }
         else
         {
            ep->in.mps                 = 0;
            ep->in.user_data.data      = USBD_MAKE_INVALID_PTR(uint8_t);
            ep->in.user_data.size      = 0;
            ep->in.max_num_bufs        = 1;
            ep->in.num_used_bufs       = 0;
            ep->in.data_provided       = USBD_FALSE;
            ep->in.dma.size            = 0;
      //      ep->in.dma.total_size      = 0;
            ep->in.buf0.size           = 0;
            ep->in.buf1.size           = 0;
         }
      }
   }

   if(USB_EP_DIRECTION_OUT == dir)
   {
      port_test_print_internal_out_state("state after port_test_io_configure", ep_num);
   }
   else
   {
      port_test_print_internal_in_state("state after port_test_io_configure", ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_io_configure */

/**
 * ADDDITIONAL EXTERNAL INTERFACE USED FOR TESTING
 */

void port_test_set_num_buffers(uint8_t ep_num, USB_EP_Direction_ET dir, uint8_t num_buffers)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         USBD_INFO_HI_2(USBD_DBG_PORT_IO, "EP: %d; dir: %s; set num bufs omitted for \"out\" endpoint!",
               ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in");
      }
      else
      {
         port_test_io_ep[ep_num].in.max_num_bufs = (num_buffers >= 2) ? 2 : num_buffers;

         USBD_INFO_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; num_bufs: %d",
               ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in", port_test_io_ep[ep_num].in.max_num_bufs);
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_set_num_buffers */

void port_test_set_dma_type(uint8_t ep_num, USB_EP_Direction_ET dir, Port_Test_DMA_Type_T dma_type)
{
   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      USBD_INFO_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; state: %s dma",
         ep_num,
         (USB_EP_DIRECTION_OUT == dir) ? "out" : "in",
         (PORT_TEST_NO_DMA != dma_type) ? (PORT_TEST_HALF_DMA != dma_type) ? "true" : "half" : "no");

      if(USB_EP_DIRECTION_OUT == dir)
      {
         port_test_io_ep[ep_num].out.dma.dma_type   = dma_type;
      }
      else
      {
         port_test_io_ep[ep_num].in.dma.dma_type    = dma_type;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_set_dma_type */

Port_Test_DMA_Type_T port_test_get_dma_type(uint8_t ep_num, USB_EP_Direction_ET dir)
{
   Port_Test_DMA_Type_T result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         result = port_test_io_ep[ep_num].out.dma.dma_type;
      }
      else
      {
         result = port_test_io_ep[ep_num].in.dma.dma_type;
      }

      USBD_INFO_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; state: %s dma",
         ep_num,
         (USB_EP_DIRECTION_OUT == dir) ? "out" : "in",
         (PORT_TEST_NO_DMA != result) ? (PORT_TEST_HALF_DMA != result) ? "true" : "half" : "no");
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);

      result = PORT_TEST_NO_DMA;
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_get_dma_type */

USBD_Bool_DT port_test_is_dma_active(uint8_t ep_num, USB_EP_Direction_ET dir)
{
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   result = USBD_FALSE;

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         result = (
                  (PORT_TEST_TRUE_DMA == port_test_io_ep[ep_num].out.dma.dma_type)
                  || (PORT_TEST_HALF_DMA == port_test_io_ep[ep_num].out.dma.dma_type)
                ) ? USBD_TRUE : USBD_FALSE;
      }
      else
      {
         result = (PORT_TEST_TRUE_DMA == port_test_io_ep[ep_num].in.dma.dma_type) ? USBD_TRUE : USBD_FALSE;
      }

      USBD_INFO_HI_3(USBD_DBG_PORT_IO, "EP: %d; dir: %s; state: %s",
            ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in", USBD_BOOL_IS_TRUE(result) ? "active" : "passive");
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_is_dma_active */

void port_test_set_data(
   uint8_t ep_num,
   USB_EP_Direction_ET dir,
   uint8_t *data,
   USBD_IO_Inout_Data_Size_DT size,
   USBD_IO_Inout_Data_Size_DT *size_done)
{
   port_test_io_ep_XT *ep;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   USBD_INFO_HI_4(USBD_DBG_PORT_IO, "EP = %d; dir: %s; size: %d; size_done: %d",
      ep_num, (USB_EP_DIRECTION_OUT == dir) ? "out" : "in", size, *size_done);

   if(USB_EP_DIRECTION_OUT == dir)
   {
      port_test_print_internal_out_state("state before set_data", ep_num);
   }
   else
   {
      port_test_print_internal_in_state("state before set_data", ep_num);
   }

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         ep->out.user_data.data                    = data;
         ep->out.user_data.size                    = size;
         ep->out.user_data.size_done               = size_done;
         ep->out.user_data.num_transactions_passed = 0;
         ep->out.user_data.set                     = USBD_TRUE;
      }
      else
      {
         ep->in.user_data.data                     = data;
         ep->in.user_data.size                     = size;
         ep->in.user_data.size_done                = size_done;
         ep->in.user_data.num_transactions_passed  = 0;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   if(USB_EP_DIRECTION_OUT == dir)
   {
      port_test_print_internal_out_state("state after set_data", ep_num);
   }
   else
   {
      port_test_print_internal_in_state("state after set_data", ep_num);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_set_data */

USBD_IO_Inout_Data_Size_DT port_test_get_waiting_user_data_size(
   uint8_t ep_num,
   USB_EP_Direction_ET dir)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      if(USB_EP_DIRECTION_OUT == dir)
      {
         result = ep->out.user_data.size;
      }
      else
      {
         result = ep->in.user_data.size;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   return result;
}

uint32_t port_test_get_num_transactions_passed(uint8_t ep_num, USB_EP_Direction_ET dir)
{
   uint32_t result = 0;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      if(USB_EP_DIRECTION_OUT == dir)
      {
         result = port_test_io_ep[ep_num].out.user_data.num_transactions_passed;
      }
      else
      {
         result = port_test_io_ep[ep_num].in.user_data.num_transactions_passed;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_INFO_HI_1(USBD_DBG_PORT_IO, "port get num transactions: %d", result);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_get_num_transactions_passed */

uint8_t port_test_get_num_used_bufs(uint8_t ep_num, USB_EP_Direction_ET dir)
{
   port_test_io_ep_XT *ep;
   uint8_t result = 0xFF;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      if(USB_EP_DIRECTION_IN == dir)
      {
         if(PORT_TEST_TRUE_DMA == ep->in.dma.dma_type)
         {
            result = ep->in.num_used_bufs + ((ep->in.dma.size > 0) ? 1 : 0);
         }
         else
         {
            result = ep->in.num_used_bufs;
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_INFO_HI_1(USBD_DBG_PORT_IO, "port get num used bufs: %d", result);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_get_num_used_bufs */

USBD_IO_Inout_Data_Size_DT port_test_get_out_ep_buf_size(uint8_t ep_num)
{
   port_test_io_ep_XT *ep;
   USBD_IO_Inout_Data_Size_DT result = (-1);

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(ep_num < USBD_MAX_NUM_ENDPOINTS)
   {
      ep = &port_test_io_ep[ep_num];

      result = ep->out.buf.size;
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "EP: %d bigger than max number of endpoints = %d!",
         ep_num, USBD_MAX_NUM_ENDPOINTS);
   }

   USBD_INFO_HI_1(USBD_DBG_PORT_IO, "port get ep out buffer size: %d", result);

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
}

void port_test_process_in(uint8_t ep_num)
{
   port_test_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   USBD_IO_Inout_Data_Size_DT size;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_test_io_ep[ep_num];
      transaction = USBD_IO_GET_IN_TRANSACATION_PARAMS(port_test_usbd, ep_num);
      tp_params   = USBD_IO_GET_IN_TP_PARAMS_PTR(port_test_usbd, ep_num);

      ep->in.data_provided = USBD_FALSE;

      if(ep->in.num_used_bufs > 0)
      {
         (ep->in.num_used_bufs)--;
      }
      ep->in.buf0.size      = ep->in.buf1.size;
      ep->in.buf1.size      = 0;

      if(PORT_TEST_TRUE_DMA == ep->in.dma.dma_type)
      {
         USBD_INFO_HI_3(USBD_DBG_PORT_IO, "irq: in: true_dma; num_used_bufs: %d; dma size: %d; dma data: %p",
            ep->in.num_used_bufs, ep->in.dma.size, ep->in.dma.data);

         if(ep->in.dma.size > 0)
         {
            if(0 == ep->in.num_used_bufs)
            {
               if(USBD_CHECK_PTR(uint8_t, ep->in.dma.data))
               {
                  /**
                  * set DMA channel to send the data only if buffers are empty. this condition is an example only because
                  * it depends strongly to hadrware behavior.
                  */
                  port_test_process_data_in(ep, ep->in.dma.data, ep->in.dma.size);

                  /* increment counter of processed transactions */
                  (ep->in.user_data.num_transactions_passed)++;

                  ep->in.dma.data = USBD_MAKE_INVALID_PTR(uint8_t);
               }
               else
               {
                  ep->in.dma.size = 0;
               }
            }
         }

         if(0 == ep->in.dma.size)
         {
            if(USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "provide in");
               USBD_IO_CALL_IN_PROVIDE_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_provide);
            }
            if((0 == ep->in.dma.size)
               && (ep->in.num_used_bufs < ep->in.max_num_bufs)
               && USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
               USBD_IO_CALL_IN_MEMCPY_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_memcpy);
            }
            if(USBD_BOOL_IS_FALSE(ep->in.data_provided) && (0 == ep->in.dma.size))
            {
               if(ep->in.num_used_bufs > 0)
               {
                  size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
               }
               else
               {
                  size = (-1);
               }
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", size);
               USBD_IO_DOWN_Process_IN_Data_Event(port_test_usbd, ep_num, size, port_test_io_in_memcpy);
            }
         }
      }
      else if(PORT_TEST_HALF_DMA == ep->in.dma.dma_type)
      {
         USBD_INFO_HI_2(USBD_DBG_PORT_IO, "irq: in: half_dma; num_used_bufs: %d; dma_size: %d", ep->in.num_used_bufs, ep->in.dma.size);

         if(ep->in.dma.size > 0)
         {
            size = port_test_io_data_in_process_bufs(ep, ep->in.dma.data, ep->in.dma.size, USBD_TRUE);

            ep->in.dma.data  = &(((uint8_t*)(ep->in.dma.data))[size]);
            ep->in.dma.size -= size;
         }

         if(0 == ep->in.dma.size)
         {
            if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_PROVIDE_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "provide in");
               USBD_IO_CALL_IN_PROVIDE_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_provide);
            }
            if((ep->in.num_used_bufs < ep->in.max_num_bufs) && USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
               USBD_IO_CALL_IN_MEMCPY_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_memcpy);
            }
            if(USBD_BOOL_IS_FALSE(ep->in.data_provided))
            {
               if(ep->in.num_used_bufs > 0)
               {
                  size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
               }
               else
               {
                  size = (-1);
               }
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", size);
               USBD_IO_DOWN_Process_IN_Data_Event(port_test_usbd, ep_num, size, port_test_io_in_memcpy);
            }
         }
      }
      else
      {
         if(USBD_IO_CHECK_IN_MEMCPY_HANDLER(transaction))
         {
            USBD_DEBUG_LO(USBD_DBG_PORT_IO, "memcpy in");
            USBD_IO_CALL_IN_MEMCPY_HANDLER(port_test_usbd, tp_params, transaction, port_test_io_in_memcpy);
         }
         if(USBD_BOOL_IS_FALSE(ep->in.data_provided))
         {
            if(ep->in.num_used_bufs > 0)
            {
               size = (USBD_IO_Inout_Data_Size_DT)(ep->in.buf0.size) + (USBD_IO_Inout_Data_Size_DT)(ep->in.buf1.size);
            }
            else
            {
               size = (-1);
            }
            USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", size);
            USBD_IO_DOWN_Process_IN_Data_Event(port_test_usbd, ep_num, size, port_test_io_in_memcpy);
         }
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "port test proc in - params invalid! ep_num = %d, port_test_usbd = %p",
         ep_num, port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);
} /* port_test_process_in */

USBD_Bool_DT port_test_process_out(uint8_t ep_num)
{
   port_test_io_ep_XT *ep;
   USBD_IO_UP_DOWN_Transaction_Params_XT *transaction;
   void *tp_params;
   uint8_t *data;
   USBD_IO_Inout_Data_Size_DT size;
   USBD_IO_Inout_Data_Size_DT buf_size;
   USBD_IO_Inout_Data_Size_DT done_size;
   USBD_Bool_DT result;

   USBD_ENTER_FUNC(USBD_DBG_PORT_IO);

   result = USBD_FALSE;

   if(USBD_CHECK_PTR(USBD_Params_XT, port_test_usbd) && (ep_num < USBD_MAX_NUM_ENDPOINTS))
   {
      ep = &port_test_io_ep[ep_num];
      transaction = USBD_IO_GET_OUT_TRANSACATION_PARAMS(port_test_usbd, ep_num);
      tp_params   = USBD_IO_GET_OUT_TP_PARAMS_PTR(port_test_usbd, ep_num);

      if((USB_EP_DESC_TRANSFER_TYPE_CONTROL == ep->in.ep_type) && ((ep->in.num_used_bufs > 0) || (ep->in.dma.size > 0)))
      {
         USBD_IO_Abort(port_test_usbd, ep_num, USB_EP_DIRECTION_IN, USBD_TRUE);
      }
      else if(USBD_BOOL_IS_FALSE(ep->out.is_ep_waiting))
      {
         if(PORT_TEST_TRUE_DMA == ep->out.dma.dma_type)
         {
            ep->out.user_data.set = USBD_FALSE;

            if(ep->out.dma.size > 0)
            {
               port_test_print_internal_out_state("true dma mode before", ep_num);

               done_size = 0;

               data = ep->out.user_data.data;

               /* for test port only - normally done by DMA channel HW */
               while(done_size < ep->out.dma.size)
               {
                  result = port_test_simulate_out_packet(ep_num);

                  buf_size   = ep->out.buf.size;
                  done_size += buf_size;

                  if(USBD_BOOL_IS_FALSE(result))
                  {
                     break;
                  }
               }

               ep->out.dma.size -= done_size;

               /* for test port only - normally done by DMA channel HW */
               memcpy(ep->out.dma.data, data, done_size);

               /* update user-defined counter of transferred data */
               if(USBD_CHECK_PTR(USBD_IO_Inout_Data_Size_DT, ep->out.user_data.size_done))
               {
                  USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size_done: %d; size: %d", ep_num, *(ep->out.user_data.size_done), done_size);
                  *(ep->out.user_data.size_done) += done_size;
                  USBD_DEBUG_HI_3(USBD_DBG_PORT_IO, "EP: %d; size_done: %d; size: %d", ep_num, *(ep->out.user_data.size_done), done_size);
               }

               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "%d bytes processed by DMA", done_size);

               /*
                * this condition is for test only - normally,
                * when received size is smaller than programmed DMA size
                * and received size is equal to multiplication of MPS
                * then DMA transaction is still in progress - DMA doesn't know that HOST will not send any more data */
               if((ep->out.dma.size > 0)
                  && (0 == (done_size % ep->out.mps))
                  && (0 != done_size))
               {
                  /* in this situation transfer still occurs */
               }
               else
               {
                  /* these few lines emulate reading parameters of received data from HW registers */
                  ep->out.buf.size           = (-1);
                  ep->out.buf.packet_size    = done_size;
                  ep->out.buf.data           = ep->out.user_data.data;

                  /* DMA channel HW must support blocking EP for waiting if size recived was smaller than programmed. */
                  if((0 != ep->out.dma.size) && USBD_BOOL_IS_FALSE(ep->out.dont_wait_for_ready))
                  {
                     ep->out.is_ep_waiting   = USBD_TRUE;
                  }
                  else
                  {
                     ep->out.is_ep_waiting   = USBD_FALSE;
                  }
                  ep->out.is_ep_processed    = USBD_TRUE;

                  ep->out.dma.total_size     = 0;
                  ep->out.dma.size           = 0;

                  USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, done_size);

                  USBD_DEBUG_LO_2(USBD_DBG_PORT_IO, "data_event with size: %d; transferred_size: %d",
                     0, USBD_IO_GET_OUT_TRANSFERRED_SIZE(transaction));

                  /*
                   * call with last parameter USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT)
                   * instead of port_test_io_out_memcpy because DMA transfer has been finished but nothing more - just
                   * declared space was filled by data. there is no data waiting in EP buffer so upper layer
                   * cannot perform memcpy operation.
                   */
                  USBD_IO_DOWN_Process_OUT_Data_Event(
                     port_test_usbd, ep_num, (-1), USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT));

                  port_test_print_internal_out_state("true dma mode after", ep_num);
               }
            }
            else
            {
               /*
               * this function is valid for test only - simulates received packet basing on "user_data" structure
               * and reads parameters of that packet to "buf" structure. In real port it should be replaced
               * by piece of code which will read parameters of received data from HW registers.
               */
               result = port_test_simulate_out_packet(ep_num);

               /*
               * set two markers:
               * - is_ep_processed is used to recognize if any action was performed on received data
               *   (if not then packet is terminated by this function)
               * - is_ep_waiting is used to recognize if buffer has been released by upper layer
               *   of if port must wait for further actions before allowing for next packets reception
               */
               ep->out.is_ep_processed = USBD_FALSE;
               ep->out.is_ep_waiting   = USBD_TRUE;

               USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, ep->out.dma.total_size);

               ep->out.dma.total_size  = 0;

               if(USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
               {
                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);
                  USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                     port_test_usbd,
                     tp_params,
                     transaction,
                     ep->out.buf.packet_size,
                     ep->out.buf.size,
                     port_test_io_out_memcpy);
               }
               else
               {
                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", ep->out.buf.size);
                  USBD_IO_DOWN_Process_OUT_Data_Event(port_test_usbd, ep_num, ep->out.buf.size, port_test_io_out_memcpy);
               }

               if(USBD_BOOL_IS_FALSE(ep->out.is_ep_processed))
               {
                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "abort OUT packet on EP %d", ep_num);

                  ep->out.is_ep_processed = USBD_TRUE;

                  ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
                  ep->out.is_ep_waiting   = USBD_FALSE;
               }
            }
         }
         else if(PORT_TEST_HALF_DMA == ep->out.dma.dma_type)
         {
            ep->out.user_data.set   = USBD_FALSE;

            /*
             * this function is valid for test only - simulates received packet basing on "user_data" structure
             * and reads parameters of that packet to "buf" structure. In real port it should be replaced
             * by piece of code which will read parameters of received data from HW registers.
             */
            result = port_test_simulate_out_packet(ep_num);

            /*
             * set two markers:
             * - is_ep_processed is used to recognize if any action was performed on received data
             *   (if not then packet is terminated by this function)
             * - is_ep_waiting is used to recognize if buffer has been released by upper layer
             *   of if port must wait for further actions before allowing for next packets reception
             */
            ep->out.is_ep_processed = USBD_FALSE;
            ep->out.is_ep_waiting   = USBD_TRUE;

            if(ep->out.dma.size > 0)
            {
               if(ep->out.buf.size >= ep->out.mps)
               {
                  size = port_test_process_data_out(ep, ep->out.dma.data, ep->out.dma.size, USBD_TRUE);

                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "size processed: %d", size);

                  ep->out.dma.size -= size;
                  ep->out.dma.data  = &((uint8_t*)(ep->out.dma.data))[size];
               }
               else
               {
                  size = port_test_process_data_out(ep, ep->out.dma.data, ep->out.dma.size, ep->out.dont_wait_for_ready);

                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "size processed: %d", size);

                  ep->out.dma.size       -= size;

                  ep->out.dma.total_size -= ep->out.dma.size;
                  ep->out.dma.size        = 0;
               }

               if(0 == ep->out.dma.size)
               {
                  USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, ep->out.dma.total_size);

                  ep->out.dma.total_size  = 0;

                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", 0);

                  /*
                   * call with last parameter USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT)
                   * instead of port_test_io_out_memcpy because DMA transfer has been finished but nothing more - just
                   * declared space was filled by data. there is no data waiting in EP buffer so upper layer
                   * cannot perform memcpy operation.
                   */
                  USBD_IO_DOWN_Process_OUT_Data_Event(
                     port_test_usbd, ep_num, (-1), USBD_MAKE_INVALID_HANDLER(USBD_IO_OUT_Data_Method_Port_HT));
               }
            }
            else
            {
               ep->out.dma.total_size  = 0;
               ep->out.dma.size        = 0;

               if(USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
               {
                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);
                  USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                     port_test_usbd,
                     tp_params,
                     transaction,
                     ep->out.buf.packet_size,
                     ep->out.buf.size,
                     port_test_io_out_memcpy);
               }
               else
               {
                  USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", ep->out.buf.size);
                  USBD_IO_DOWN_Process_OUT_Data_Event(port_test_usbd, ep_num, ep->out.buf.size, port_test_io_out_memcpy);
               }
            }

            if(USBD_BOOL_IS_FALSE(ep->out.is_ep_processed))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "abort OUT packet on EP %d", ep_num);

               ep->out.is_ep_processed = USBD_TRUE;

               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.is_ep_waiting   = USBD_FALSE;
            }
         }
         else
         {
            ep->out.user_data.set      = USBD_FALSE;

            /*
             * this function is valid for test only - simulates received packet basing on "user_data" structure
             * and reads parameters of that packet to "buf" structure. In real port it should be replaced
             * by piece of code which will read parameters of received data from HW registers.
             */
            result = port_test_simulate_out_packet(ep_num);

            /*
             * set two markers:
             * - is_ep_processed is used to recognize if any action was performed on received data
             *   (if not then packet is terminated by this function)
             * - is_ep_waiting is used to recognize if buffer has been released by upper layer
             *   of if port must wait for further actions before allowing for next packets reception
             */
            ep->out.is_ep_processed = USBD_FALSE;
            ep->out.is_ep_waiting   = USBD_TRUE;

            /* update size transferred by provide method (to by used by upper layer) */
            USBD_IO_SET_OUT_TRANSFERRED_SIZE(transaction, 0);

            if(USBD_IO_CHECK_OUT_MEMCPY_HANDLER(transaction))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "memcpy with size: %d", ep->out.buf.size);
               USBD_IO_CALL_OUT_MEMCPY_HANDLER(
                  port_test_usbd,
                  tp_params,
                  transaction,
                  ep->out.buf.packet_size,
                  ep->out.buf.size,
                  port_test_io_out_memcpy);
            }
            else
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "data_event with size: %d", ep->out.buf.size);
               USBD_IO_DOWN_Process_OUT_Data_Event(port_test_usbd, ep_num, ep->out.buf.size, port_test_io_out_memcpy);
            }

            if(USBD_BOOL_IS_FALSE(ep->out.is_ep_processed))
            {
               USBD_DEBUG_LO_1(USBD_DBG_PORT_IO, "abort OUT packet on EP %d", ep_num);

               ep->out.is_ep_processed = USBD_TRUE;

               ep->out.buf.data        = USBD_MAKE_INVALID_PTR(void);
               ep->out.is_ep_waiting   = USBD_FALSE;
            }
         }

         if(USBD_BOOL_IS_TRUE(ep->out.user_data.set))
         {
            result = USBD_TRUE;
         }

         ep->out.user_data.set = USBD_FALSE;
      }
   }
   else
   {
      USBD_WARN_2(USBD_DBG_PORT_INVALID_PARAMS, "port test proc out - params invalid! ep_num = %d, port_test_usbd = %p",
         ep_num, port_test_usbd);
   }

   USBD_EXIT_FUNC(USBD_DBG_PORT_IO);

   return result;
} /* port_test_process_out */

#endif
