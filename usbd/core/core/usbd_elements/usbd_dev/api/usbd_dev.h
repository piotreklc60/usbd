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

#ifndef USBD_DEV_H_
#define USBD_DEV_H_

/** @defgroup USBD_DEV
 * @{
 */

#if(USBD_MAX_NUM_STRINGS > 0)

/**
 * Installs string in usbd. Each string which has to be visible for HOST
 * must be installed using this function.
 * iString must provide valid string number because this number will be used when HOST reads string.
 * Following iString numbers: 0,1,2,3 are reserved. These values are used for:
 * 0 for Language_String, 1 for Manufacturer_String, 2 for Product_String and 3 for Serial_Number_String.
 * These strings can be installed using separate functions: USBD_DEV_Install_Language_String,
 * USBD_DEV_Install_Manufacturer_String, USBD_DEV_Install_Product_String and USBD_DEV_Install_Serial_Number_String.
 *
 * \param usbd pointer to usb device
 * \param string pointer to string
 * \param string_size size of string in bytes
 * \param iString identifier of the string. This is the number which is used in some descriptor to identify this string.
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size,
      uint8_t iString);

/**
 * Installs "LANGID" string in usbd.
 * This string cannot be removed
 *
 * \param usbd pointer to usb device
 * \param string pointer to manufacturer string
 * \param string_size size of string in bytes
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_Language_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size);

/**
 * Installs "manufacturer" string in usbd.
 * This string can be removed using \see USBD_DEV_Remove_String.
 *
 * \param usbd pointer to usb device
 * \param string pointer to manufacturer string
 * \param string_size size of string in bytes
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_Manufacturer_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size);

/**
 * Installs "product" string in usbd.
 * This string can be removed using \see USBD_DEV_Remove_String
 *
 * \param usbd pointer to usb device
 * \param string pointer to manufacturer string
 * \param string_size size of string in bytes
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_Product_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size);

/**
 * Installs "serial number" string in usbd.
 * This string can be removed using \see USBD_DEV_Remove_String
 *
 * \param usbd pointer to usb device
 * \param string pointer to manufacturer string
 * \param string_size size of string in bytes
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_Serial_Number_String(
      USBD_Params_XT *usbd,
      const uint8_t *string,
      uint8_t string_size);

/**
 * Removes previously installed string from list of installed strings.
 *
 * \param usbd pointer to usb device
 * \param string pointer to string which shall be removed
 * \return USBD_TRUE if string removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Remove_String(
      USBD_Params_XT *usbd,
      const uint8_t *string);

/**
 * Removes all installed strings
 *
 * \param usbd pointer to usb device
 * \return USBD_TRUE if all strings removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Remove_All_Strings(
      USBD_Params_XT *usbd);

/**
 * Checks how many strings are installed
 *
 * \param usbd pointer to usb device
 * \return number of installed strings if usbd pointer is valid, 0 otherwise
 */
uint8_t USBD_DEV_Get_Num_Installed_Strings(
      USBD_Params_XT *usbd);

/**
 * Gets string by position in table of installed strings
 *
 * \param usbd pointer to usb device
 * \param index position in strings table to be read
 * \return pointer to string if succeeded, USBD_MAKE_INVALID_PTR(uint8_t) otherwise
 */
const uint8_t *USBD_DEV_Get_String(
      USBD_Params_XT *usbd,
      uint8_t index);

/**
 * Finds string position in table of installed strings by pointer to string.
 *
 * \param usbd pointer to usb device
 * \param string pointer to string which index shall be found
 * \return position of string in  @see string_tab
 * if found, 0 if usbd pointer is invalid,
 * value returned by @see USBD_DEV_Get_Num_Installed_Strings
 * otherwise
 */
uint8_t USBD_DEV_Find_String_Index_By_String_Ptr(
      USBD_Params_XT *usbd,
      const uint8_t *string);

#endif



/**
 * Gets Device Descriptor
 * Copies device descriptor to provided buffer
 *
 * \param usbd pointer to usb device
 * \param desc pointer to USB_Device_Desc_DT structure
 *      into which device descriptor will be copied
 * \return USBD_TRUE if device descriptor copied successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Get_Dev_Desc(
      USBD_Params_XT *usbd,
      USB_Device_Desc_DT *desc);

/**
 * Sets Device Descriptor
 * Copies context of provided buffer to device descriptor
 * Attention! following fields are not copied to device descriptor:
 * \see bLength, bDescriptorType, iManufacturer, iProduct, iSerialNumber, bNumConfigurations.
 * These fields are set by USBD automatically and cannot be configured manually.
 *
 * \param usbd pointer to usb device
 * \param desc pointer to USB_Device_Desc_DT structure
 *      which will be copied to device descriptor
 * \return USBD_TRUE if device descriptor set successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Set_Dev_Desc(
      USBD_Params_XT *usbd,
      USB_Device_Desc_DT *desc);



#if(USBD_FEATURE_PRESENT == USBD_DEV_SUPPORT_CONFIG_VALIDATION)
/**
 * Checks if configuration descriptor has correct structure.
 * This procedure also checks configuration descriptor regarding to port parameters
 * analyzing such parameters like maximum number of supported endpoints or endpoints memory size.
 * If "port" parameter is set to USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT) then descriptor
 * is checked only regarding to USB specification.
 * User should be aware, that if configuration is installed without comparing descriptor
 * by port parser then it can be implausible for port. In such case request from HOST
 * to set such configuration will be rejected.
 *
 * \param usbd pointer to usb device. For this function it is optional parameter and can be NULL,
 * for example when configuration has to be checked before installing.
 * \param port pointer to port parameters, can be set to USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT).
 * \param desc pointer to configuration descriptor (whole descriptor with interfaces,
 *      endpoints, etc.
 * \param desc_size total size of configuration descriptor
 * \return check result - @see USBD_DEV_Config_Desc_Check_Result_XT
 */
USBD_DEV_Config_Desc_Check_Result_XT USBD_DEV_Check_Config_Desc(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port,
      const uint8_t *desc,
      uint16_t desc_size);
#endif

/**
 * Installs configuration in usbd.
 * This function checks configuration descriptor before setting configuration. If descriptor is invalid
 * then configuration is not set and error code is returned. This function also can check descriptor
 * regarding to port parameters using function USBD_DEV_Check_Config_Desc. For more details
 * please check description of function @see USBD_DEV_Check_Config_Desc.
 *
 * \param usbd pointer to usb device
 * \param port pointer to port parameters, can be set to USBD_MAKE_INVALID_PTR(USBD_DEV_Port_Handler_XT).
 * \param usbdc pointer to configuration which shall be installed
 * \return installation result container - @see USBD_DEV_Installation_Result_XT
 */
USBD_DEV_Installation_Result_XT USBD_DEV_Install_Config(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port,
      USBDC_Params_XT *usbdc);

/**
 * Removes previously installed configuration from list of installed configurations.
 *
 * \param usbd pointer to usb device
 * \param usbdc pointer to configuration which shall be removed
 * \return USBD_TRUE if configuration removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Remove_Config(
      USBD_Params_XT *usbd,
      USBDC_Params_XT *usbdc);

/**
 * Removes all installed configurations
 *
 * \param usbd pointer to usb device
 * \return USBD_TRUE if all configurations removed successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Remove_All_Configs(
      USBD_Params_XT *usbd);

/**
 * Checks how many configurations are installed
 *
 * \param usbd pointer to usb device
 * \return number of installed connfigurations if usbd pointer is valid, 0 otherwise
 */
uint8_t USBD_DEV_Get_Num_Installed_Configs(
      USBD_Params_XT *usbd);

/**
 * Gets pointer to configuration descriptor
 *
 * \param usbd pointer to usb device
 * \param index position in table of configurations. It is equal
 * to number of configuration - 1 if configuration was installed
 * to be enumerated automatically.
 * \return pointer to configuration descriptor if found,
 * USBD_MAKE_INVALID_PTR(USB_Configuration_Desc_DT) otherwise
 */
const USB_Configuration_Desc_DT *USBD_DEV_Get_Config_Desc(
      USBD_Params_XT *usbd,
      uint8_t index);

/**
 * Gets pointer to "usbdc" structure with configuration parameters
 *
 * \param usbd pointer to usb device
 * \param index position in table of configurations. It is equal
 * to number of configuration - 1 if configuration was installed
 * to be enumerated automatically.
 * \return pointer to structure with configuration parameters if found,
 * USBD_MAKE_INVALID_PTR(void) otherwise
 */
USBDC_Params_XT *USBD_DEV_Get_Config(
      USBD_Params_XT *usbd,
      uint8_t index);

/**
 * Gets pointer to "usbdc" structure with configuration parameters
 *
 * \param usbd pointer to usb device
 * \return number of active configuration
 */
uint8_t USBD_DEV_Get_Active_Config_Num(
      USBD_Params_XT *usbd);

/**
 * Checks maximum number of interfaces
 * There are checked all configurations to find that one which has most interfaces.
 * This function returns number of interfaces which that configuration has.
 *
 * \param usbd pointer to usb device
 * \return number of installed connfigurations
 */
uint8_t USBD_DEV_Get_Max_Num_Interfaces_In_Installed_Configs(
      USBD_Params_XT *usbd);

/**
 * Checks number of interfaces in active configuration.
 * If there is no active configuration then returns 0.
 *
 * \param usbd pointer to usb device
 * \return number of installed connfigurations
 */
uint8_t USBD_DEV_Get_Num_Interfaces_In_Active_Config(
      USBD_Params_XT *usbd);

/**
 * Checks number of endpoints used by active configuration.
 * If there is no active configuration then returns 1 because default control pipe (EP0) is always in use.
 *
 * \param usbd pointer to usb device
 * \return number of installed connfigurations
 */
uint8_t USBD_DEV_Get_Num_Endpoints_In_Active_Config(
      USBD_Params_XT *usbd);



/**
 * Gets pointer to interface descriptor
 *
 * \param usbd pointer to usb device
 * \param interface_num number of interface which descriptor is requested
 */
const USB_Interface_Desc_DT *USBD_DEV_Get_Interface_Desc(
      USBD_Params_XT *usbd,
      uint8_t interface_num);

/**
 * Checks how many interfaces current configuration has
 *
 * \param usbd pointer to usb device
 * \return number of installed connfigurations
 */
uint8_t USBD_DEV_Get_Num_Interfaces_In_Active_Config(
      USBD_Params_XT *usbd);



/**
 * Gets state of specified endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return endpoint state
 */
USBD_DEV_EP_States_ET USBD_DEV_Get_EP_State(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

/**
 * Gets endpoint descriptor for specified endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return pointer to endpoint's descriptor or NULL when not configured
 */
const USB_Endpoint_Desc_DT *USBD_DEV_Get_EP_Desc(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);

#if(USBD_MAX_NUM_ENDPOINTS > 1)
/**
 * Gets number of interface which uses this endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return number of interface which uses this endpont.
 * If some endpoint (number&dir) is not used by any interface (EP not configured) then value 0xFF will be returned.
 */
uint8_t USBD_DEV_Get_EP_Interface_Num(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);
#else
#define USBD_DEV_Get_EP_Interface_Num(usbd, ep_num, dir)       0xFF
#endif

/**
 * Sets/resets HALT condition for specified endpoint and disables/enables it.
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \param state USBD_TRUE when HALT condition shall be set, USBD_FALSE otherwise.
 */
void USBD_DEV_Set_EP_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir,
      USBD_Bool_DT state);

/**
 * Checks if HALT conndition is set for specified endpoint
 *
 * \param usbd pointer to usb device
 * \param ep_num endpoint number
 * \param dir direction. For possible values please refer to @see USB_EP_Direction_ET
 * \return USBD_TRUE when halt condition is set, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Get_EP_Halt(
      USBD_Params_XT *usbd,
      uint8_t ep_num,
      USB_EP_Direction_ET dir);



/**
 * Activates device. Initializes usb device's hardware so it can be
 * detected by HOST.
 *
 * \param usbd pointer to usb device
 * \param port pointer to port parameters
 * \return USBD_TRUE if usbd activated successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT  USBD_DEV_Activate(
      USBD_Params_XT *usbd,
      const USBD_DEV_Port_Handler_XT *port);

/**
 * Deactivates device. If usb was running then this function disconnects
 * it from HOST and deinitializes hardware.
 *
 * \param usbd pointer to usb device
 * \return USBD_TRUE if usbd deactivated successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Deactivate(
      USBD_Params_XT *usbd);



/**
 * Gets state of usbd
 *
 * \param usbd pointer to usb device
 * \return usbd state \see USBD_DEV_States_ET
 */
USBD_DEV_States_ET USBD_DEV_Get_State(
      USBD_Params_XT *usbd);



/**
 * Gets frame number
 *
 * \param usbd pointer to usb device
 * \return frame number
 */
uint16_t USBD_DEV_Get_Frame_Num(
      USBD_Params_XT *usbd);



#if((USBD_DEV_LOW_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED) && (USBD_DEV_FULL_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED))
/**
 * Checks which USB speed types are supported by hardware
 *
 * \param usbd pointer to usb device
 * \return speed bit mask if port correctly bound to usbd,
 * otherwise @see USBD_DEV_SPEED_INVALID
 */
USBD_DEV_Speed_ET USBD_DEV_Get_Supported_Speed(
      USBD_Params_XT *usbd);
#else
#define USBD_DEV_Get_Supported_Speed(usbd)      USBD_DEV_SUPPORTED_SPEED
#endif

#if((USBD_DEV_LOW_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED) && (USBD_DEV_FULL_SPEED_SUPPORTED != USBD_DEV_SUPPORTED_SPEED))
/**
 * Checks which USB speed is currently used by hardware
 *
 * \param usbd pointer to usb device
 * \return speed bit mask if port correctly bound to usbd,
 * otherwise @see USBD_DEV_SPEED_INVALID
 */
USBD_DEV_Speed_ET USBD_DEV_Get_Current_Speed(
      USBD_Params_XT *usbd);
#else
#define USBD_DEV_Get_Current_Speed(usbd)        USBD_DEV_SUPPORTED_SPEED
#endif



/**
 * Sets device to attached/detached state.
 * Shall be called by port layer when hardware detects that device has been attached/detached.
 * If hardware doesn't support attached/detached detection then this function doesn't
 * need to be used.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param state USBD_TRUE if device has been attached, USBD_FALSE otherwise
 */
void USBD_DEV_Attached(
      USBD_Params_XT *usbd,
      USBD_Bool_DT state);

/**
 * Sets device to powered/not powered state.
 * Shall be called by port layer when hardware detects 5V on VBUS line
 * If hardware doesn't support powered/unpowered detection then this function
 * has to be called when hardware is activated.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 * \param state USBD_TRUE if device is powered USBD_FALSE otherwise
 */
void USBD_DEV_Powered(
      USBD_Params_XT *usbd,
      USBD_Bool_DT state);

/**
 * Sets device to reset state.
 * Shall be called by port layer when USB reset has been received.
 * USB reset detection is mandatory feature which has to be supported by HW.
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 */
void USBD_DEV_Reset(
      USBD_Params_XT *usbd);

/**
 * Sets device to addressed state.
 * Shall be called by USBD_REQ layer when USB receives request to set proper address.
 * When USBD_REQ layer is not used then it is mandatory to support detection
 * of SET_ADDRESS request and inform USBD_DEV layer by calling this function that address
 * has been received.
 *
 * Called only internally by USBD Request layer
 *
 * \param usbd pointer to usb device
 * \param address device adddress set by HOST
 */
void USBD_DEV_Addressed(
      USBD_Params_XT *usbd,
      uint8_t address);

/**
 * Sets device to configured state.
 * Shall be called by USBD_REQ layer when USB receives request to set proper configuration.
 * When USBD_REQ layer is not used then it is mandatory to support detection
 * of SET_CONFIGURATION request and inform USBD_DEV layer by calling this function that configuration
 * has been set.
 *
 * Called only internally by USBD Request layer
 *
 * \param usbd pointer to usb device
 * \param config_num number of configuration to be set
 * \param respond handler to function called to send STATUS ack if SET_CONFIGURATION request has been approved.
 * If provided as invalid handler then caller must send STATUS ack after function returns OSBD_TRUE.
 * \return USBD_TRUE if configuration set successfully, USBD_FALSE otherwise
 */
USBD_Bool_DT USBD_DEV_Configured(
      USBD_Params_XT *usbd,
      uint8_t config_num,
      USBD_DEV_Set_Configuration_Respond_HT respond);

#if(USBD_MAX_NUM_ALTERNATE_INTERFACE_SETTINGS > 0)
/**
 * It doesn't chande Device state. This function is used to set alternative version
 * of interface.
 *
 * Called only internally by USBD Request layer
 *
 * \param usbd pointer to usb device
 * \param interface_num number of interface to be reconfigured
 * \param alternative_setting number of alternative version of interface to be set
 * \return setting interface alternate setting result. @see USBD_DEV_Set_Interface_Result_ET
 */
USBD_DEV_Set_Interface_Result_ET USBD_DEV_Set_Interface(
      USBD_Params_XT *usbd,
      uint8_t interface_num,
      uint8_t alternative_setting);
#endif

/**
 * Sets device to suspended state.
 * Shall be called by port layer when low level software/hardware detects
 * no communication present more than 3 ms. There is mandatory to detect SUSPEND state.
 * It can be detected by software or hardware (if hardware supports it).
 *
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 */
void USBD_DEV_Suspended(
      USBD_Params_XT *usbd);

/**
 * Restores device from suspended state.
 * Shall be called by port layer when low level software/hardware detects
 * resume event.
 *
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 */
void USBD_DEV_Resumed(
      USBD_Params_XT *usbd);

/**
 * Informs USBD_DEV module that Start Of Frame (SOF) message has been received
 *
 *
 * Called only by low level layer (port)
 *
 * \param usbd pointer to usb device
 */
void USBD_DEV_SOF_Received(
      USBD_Params_XT *usbd);



/** @} */ /* USBD_DEV */


#endif
