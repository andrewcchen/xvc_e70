
################################################################################
# Automatically-generated file. Do not edit!
################################################################################

ifdef SystemRoot
	SHELL = cmd.exe
	MK_DIR = mkdir
else
	ifeq ($(shell uname), Linux)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), CYGWIN)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), MINGW32)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), MINGW64)
		MK_DIR = mkdir -p
	endif
endif

# List the subdirectories for creating object files
SUB_DIRS +=  \
 \
usb/class/cdc/device \
usb/device \
hpl/pmc \
hpl/usart \
hal/src \
hpl/usbhs \
hpl/xdmac \
same70b/armcc/Device/Source \
hal/utils/src \
examples \
same70b/armcc/Device/Source/ARM \
hpl/core \
usb

# List the object files
OBJS +=  \
hal/src/hal_io.o \
hpl/xdmac/hpl_xdmac.o \
hpl/usart/hpl_usart.o \
usb/class/cdc/device/cdcdf_acm.o \
same70b/armcc/Device/Source/system_same70q21b.o \
hal/src/hal_delay.o \
hpl/usbhs/hpl_usbhs.o \
hpl/core/hpl_init.o \
hal/utils/src/utils_list.o \
hal/utils/src/utils_assert.o \
usb_start.o \
atmel_start.o \
hal/src/hal_usart_sync.o \
hpl/pmc/hpl_pmc.o \
usb/usb_protocol.o \
hal/src/hal_init.o \
hal/src/hal_usb_device.o \
main.o \
usb/device/usbdc.o \
examples/driver_examples.o \
driver_init.o \
hal/utils/src/utils_ringbuffer.o \
hal/utils/src/utils_event.o \
hal/src/hal_gpio.o \
hal/src/hal_sleep.o \
hpl/pmc/hpl_sleep.o \
hpl/core/hpl_core_m7_base.o \
hal/src/hal_atomic.o \
same70b/armcc/Device/Source/ARM/startup_SAME70.o

OBJS_AS_ARGS +=  \
"hal/src/hal_io.o" \
"hpl/xdmac/hpl_xdmac.o" \
"hpl/usart/hpl_usart.o" \
"usb/class/cdc/device/cdcdf_acm.o" \
"same70b/armcc/Device/Source/system_same70q21b.o" \
"hal/src/hal_delay.o" \
"hpl/usbhs/hpl_usbhs.o" \
"hpl/core/hpl_init.o" \
"hal/utils/src/utils_list.o" \
"hal/utils/src/utils_assert.o" \
"usb_start.o" \
"atmel_start.o" \
"hal/src/hal_usart_sync.o" \
"hpl/pmc/hpl_pmc.o" \
"usb/usb_protocol.o" \
"hal/src/hal_init.o" \
"hal/src/hal_usb_device.o" \
"main.o" \
"usb/device/usbdc.o" \
"examples/driver_examples.o" \
"driver_init.o" \
"hal/utils/src/utils_ringbuffer.o" \
"hal/utils/src/utils_event.o" \
"hal/src/hal_gpio.o" \
"hal/src/hal_sleep.o" \
"hpl/pmc/hpl_sleep.o" \
"hpl/core/hpl_core_m7_base.o" \
"hal/src/hal_atomic.o" \
"same70b/armcc/Device/Source/ARM/startup_SAME70.o"

# List the dependency files
DEPS := $(OBJS:%.o=%.d)

DEPS_AS_ARGS +=  \
"hal/src/hal_gpio.d" \
"hal/src/hal_io.d" \
"hpl/usart/hpl_usart.d" \
"usb/class/cdc/device/cdcdf_acm.d" \
"hpl/xdmac/hpl_xdmac.d" \
"same70b/armcc/Device/Source/system_same70q21b.d" \
"hal/utils/src/utils_list.d" \
"usb_start.d" \
"hal/utils/src/utils_assert.d" \
"hpl/usbhs/hpl_usbhs.d" \
"hal/src/hal_delay.d" \
"hpl/core/hpl_init.d" \
"usb/usb_protocol.d" \
"hal/src/hal_usb_device.d" \
"hal/src/hal_init.d" \
"hal/src/hal_usart_sync.d" \
"hpl/pmc/hpl_pmc.d" \
"driver_init.d" \
"main.d" \
"examples/driver_examples.d" \
"hal/src/hal_sleep.d" \
"hal/utils/src/utils_ringbuffer.d" \
"usb/device/usbdc.d" \
"hal/utils/src/utils_event.d" \
"hpl/pmc/hpl_sleep.d" \
"hal/src/hal_atomic.d" \
"hpl/core/hpl_core_m7_base.d" \
"same70b/armcc/Device/Source/ARM/startup_SAME70.d" \
"atmel_start.d"

OUTPUT_FILE_NAME :=AtmelStart
QUOTE := "
OUTPUT_FILE_PATH +=$(OUTPUT_FILE_NAME).elf
OUTPUT_FILE_PATH_AS_ARGS +=$(OUTPUT_FILE_NAME).elf

vpath %.c ../
vpath %.s ../
vpath %.S ../

# All Target
all: $(SUB_DIRS) $(OUTPUT_FILE_PATH)

# Linker target

$(OUTPUT_FILE_PATH): $(OBJS)
	@echo Building target: $@
	@echo Invoking: ARMCC Linker
	$(QUOTE)armlink$(QUOTE) --ro-base 0x00000000 --entry 0x00000000 --rw-base 0x20000000 --entry Reset_Handler --first __Vectors \
--strict --summary_stderr --info summarysizes --map --xref --callgraph --symbols \
--info sizes --info totals --info unused --info veneers --list $(OUTPUT_FILE_NAME).map \
-o $(OUTPUT_FILE_NAME).elf --cpu Cortex-M7.fp.dp \
$(OBJS_AS_ARGS)

	@echo Finished building target: $@

# Compiler target(s)




%.o: %.c
	@echo Building file: $<
	@echo ARMCC Compiler
	$(QUOTE)armcc$(QUOTE) --c99 -c -DDEBUG -O1 -g --apcs=interwork --split_sections --cpu Cortex-M7.fp.dp -D__SAME70Q21B__ \
-I"../" -I"../config" -I"../examples" -I"../hal/include" -I"../hal/utils/include" -I"../hpl/core" -I"../hpl/pio" -I"../hpl/pmc" -I"../hpl/usart" -I"../hpl/usbhs" -I"../hpl/xdmac" -I"../hri" -I"../" -I"../config" -I"../usb" -I"../usb/class/cdc" -I"../usb/class/cdc/device" -I"../usb/device" -I"../" -I"../CMSIS/Core/Include" -I"../same70b/include"  \
--depend "$@"  -o "$@" "$<"

	@echo Finished building: $<

%.o: %.s
	@echo Building file: $<
	@echo ARMCC Assembler
	$(QUOTE)armasm$(QUOTE) -g --apcs=interwork --cpu Cortex-M7.fp.dp --pd "D__SAME70Q21B__ SETA 1" \
-I"../" -I"../config" -I"../examples" -I"../hal/include" -I"../hal/utils/include" -I"../hpl/core" -I"../hpl/pio" -I"../hpl/pmc" -I"../hpl/usart" -I"../hpl/usbhs" -I"../hpl/xdmac" -I"../hri" -I"../" -I"../config" -I"../usb" -I"../usb/class/cdc" -I"../usb/class/cdc/device" -I"../usb/device" -I"../" -I"../CMSIS/Core/Include" -I"../same70b/include"  \
--depend "$(@:%.o=%.d)" -o "$@" "$<"

	@echo Finished building: $<

%.o: %.S
	@echo Building file: $<
	@echo ARMCC Preprocessing Assembler
	$(QUOTE)armcc$(QUOTE) --c99 -c -DDEBUG -O1 -g --apcs=interwork --split_sections --cpu Cortex-M7.fp.dp -D__SAME70Q21B__ \
-I"../" -I"../config" -I"../examples" -I"../hal/include" -I"../hal/utils/include" -I"../hpl/core" -I"../hpl/pio" -I"../hpl/pmc" -I"../hpl/usart" -I"../hpl/usbhs" -I"../hpl/xdmac" -I"../hri" -I"../" -I"../config" -I"../usb" -I"../usb/class/cdc" -I"../usb/class/cdc/device" -I"../usb/device" -I"../" -I"../CMSIS/Core/Include" -I"../same70b/include"  \
--depend "$@"  -o "$@" "$<"

	@echo Finished building: $<

# Detect changes in the dependent files and recompile the respective object files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(SUB_DIRS):
	$(MK_DIR) "$@"

clean:
	rm -f $(OBJS_AS_ARGS)
	rm -f $(OUTPUT_FILE_PATH)
	rm -f $(DEPS_AS_ARGS)
	rm -f $(OUTPUT_FILE_NAME).map $(OUTPUT_FILE_NAME).elf
