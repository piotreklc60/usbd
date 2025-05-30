# ---------------------------------------- LICENSE SECTION -------------------------------------------------------------------
#
# Copyright (c) 2024 Piotr Wojtowicz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# -----------------------------------------------------------------------------------------------------------------------------

APP_THIS_PATH = .
TEST_HELPERS_PATH = ./../../test_helpers/
USBD_PATH = ./../../../.
USBD_CORE_USBD_PATH = $(USBD_PATH)/core/core/usbd/
USBD_CORE_ADD_ONS_PATH = $(USBD_PATH)/core/core/add_ons/
USBD_CORE_ADD_ONS_CDP_PATH = $(USBD_CORE_ADD_ONS_PATH)/usb_cdp/
USBD_CORE_IOTP_EVENT_PATH = $(USBD_CORE_ADD_ONS_PATH)/usbd_iotp/event
USBD_CORE_USBD_ELEMENTS_PATH = $(USBD_PATH)/core/core/usbd_elements/
USBD_CORE_USBD_DEV_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbd_dev/
USBD_CORE_USBD_INIT_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbd_init/
USBD_CORE_USBD_REQUEST_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbd_request/
USBD_CORE_USBD_IO_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbd_io/
USBD_CORE_USBD_EVENT_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbd_event/
USBD_CORE_USBDC_PATH = $(USBD_CORE_USBD_ELEMENTS_PATH)/usbdc/
DEPEDENCIES_PATH = $(USBD_PATH)/../examples/depedencies/
IOCMD_PATH = $(DEPEDENCIES_PATH)/iocmd/iocmd/
BUFF_PATH = $(DEPEDENCIES_PATH)/buff/

UINCDIR =  -I $(APP_THIS_PATH)
UINCDIR += -I $(TEST_HELPERS_PATH)
UINCDIR += -I $(USBD_CORE_USBD_PATH)/api/         -I $(USBD_CORE_USBD_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBD_DEV_PATH)/api/     -I $(USBD_CORE_USBD_DEV_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBD_INIT_PATH)/api/    -I $(USBD_CORE_USBD_INIT_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBD_REQUEST_PATH)/api/ -I $(USBD_CORE_USBD_REQUEST_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBD_IO_PATH)/api/      -I $(USBD_CORE_USBD_IO_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBD_EVENT_PATH)/api/   -I $(USBD_CORE_USBD_EVENT_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_USBDC_PATH)/api/        -I $(USBD_CORE_USBDC_PATH)/imp/inc/
UINCDIR += -I $(USBD_CORE_ADD_ONS_CDP_PATH)/api/
UINCDIR += -I $(USBD_CORE_IOTP_EVENT_PATH)/api/   -I $(USBD_CORE_IOTP_EVENT_PATH)/imp/inc/
UINCDIR += -I $(IOCMD_PATH)/api/                  -I $(IOCMD_PATH)/imp/inc/
UINCDIR += -I $(BUFF_PATH)/api/                   -I $(BUFF_PATH)/imp/inc/

APPL_SRC =  $(APP_THIS_PATH)/main.c
APPL_SRC += $(BUFF_PATH)/imp/src/buff.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_out.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_out_utoa.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_out_parser.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_out_processor.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_log.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_in.c
APPL_SRC += $(IOCMD_PATH)/imp/src/iocmd_cmd.c
APPL_SRC += $(APP_THIS_PATH)/usbd_events_test_install.c
APPL_SRC += $(APP_THIS_PATH)/usbdc_events_test_install.c
APPL_SRC += $(APP_THIS_PATH)/events_test_call.c
APPL_SRC += $(APP_THIS_PATH)/events_test_states.c
APPL_SRC += $(TEST_HELPERS_PATH)/main_get_exe.c
USBD_SRC =  $(USBD_CORE_USBD_PATH)/imp/src/usbd.c
USBD_SRC += $(USBD_CORE_USBDC_PATH)/imp/src/usbdc.c
USBD_SRC += $(USBD_CORE_USBD_DEV_PATH)/imp/src/usbd_dev.c
USBD_SRC += $(USBD_CORE_USBD_EVENT_PATH)/imp/src/usbd_event.c
USBD_SRC += $(USBD_CORE_ADD_ONS_CDP_PATH)/imp/src/usb_cdp.c


SRC  = $(APPL_SRC)
SRC += $(USBD_SRC)

gcov:
	@echo " "
	@echo "USBD part coverage:"
	@echo " "
	gcov $(USBD_SRC)
	@echo " "
	@echo "Application part coverage:"
	@echo " "
	gcov $(APPL_SRC)

gcov_result:
	@echo " "
	@echo "USBD part coverage:"
	@echo " "
	gcov $(USBD_SRC) >> coverage_result_usbd.txt
	@echo " "
	@echo "Application part coverage:"
	@echo " "
	gcov $(APPL_SRC) >> coverage_result_appl.txt

all:
	@echo " "
	@echo "Build project:"
	@echo " "
	gcc $(UINCDIR) -Wa,-adhln -g > list.lst -fprofile-arcs -ftest-coverage $(SRC) -o test

E:
	@echo " "
	@echo "Precompiling project:"
	@echo " "
	gcc $(UINCDIR) -fprofile-arcs -ftest-coverage $(SRC) -E

E_result:
	@echo " "
	@echo "Precompiling project:"
	@echo " "
	gcc $(UINCDIR) -fprofile-arcs -ftest-coverage $(SRC) -E >> E_result.e

clean:
	rm -f ./*.lst
	rm -f ./*.e
	rm -f ./*.gcno
	rm -f ./test
	rm -f ./*.gcda
	rm -f ./*.gcov
	rm -f ./coverage_result*.txt
rm:
	rm -f ./*~
	make clean

test:
	make all
	@echo " "
	@echo "Test Application execution:"
	@echo " "
	./test
	@echo " "
	@echo "Test Application executed!"
	@echo " "
	make gcov
test_result:
	make all
	@echo " "
	@echo "Test Application execution:"
	@echo " "
	./test
	@echo " "
	@echo "Test Application executed!"
	@echo " "
	make gcov_result
