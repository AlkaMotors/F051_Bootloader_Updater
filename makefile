CC = $(ARM_SDK_PREFIX)gcc
CP = $(ARM_SDK_PREFIX)objcopy
MCU := -mcpu=cortex-m0 -mthumb
LDSCRIPT := STM32F051K6TX_FLASH.ld
LDSCRIPT_COMBINED := STM32F051K6TX_FLASH_combined.ld
LIBS := -lc -lm -lnosys 
LDFLAGS := -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,--gc-sections -Wl,--print-memory-usage
MAIN_SRC_DIR := Src
SRC_DIR := Core/Startup \
	Core/Src \
	Drivers/STM32F0xx_HAL_Driver/Src
SRC := $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.[cs])) bootloader_include.S
OBJ := $(SRC:%.[cs]=%.o)
INCLUDES :=  \
	-ICore/Inc \
	-IDrivers/STM32F0xx_HAL_Driver/Inc \
	-IDrivers/CMSIS/Include \
	-IDrivers/CMSIS/Device/ST/STM32F0xx/Include 
VALUES :=  \
	-DUSE_MAKE \
	-DHSE_VALUE=8000000 \
	-DSTM32F051x8 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DDATA_CACHE_ENABLE=0 \
	-DINSTRUCTION_CACHE_ENABLE=0 \
	-DVDD_VALUE=3300 \
	-DLSI_VALUE=40000 \
	-DHSI_VALUE=8000000 \
	-DUSE_FULL_LL_DRIVER \
	-DPREFETCH_ENABLE=1
CFLAGS = $(MCU) $(VALUES) $(INCLUDES) -O2 -Wall -fdata-sections -ffunction-sections
CFLAGS += -D$(TARGET)
CFLAGS += -MMD -MP -MF $(@:%.bin=%.d)

TARGETS := PA2 PB4

CFLAGS += -D__bootloader_path__='"bootloader/BOOTLOADER_$(TARGET).bin"'
TARGET_PREFIX := BOOTLOADER_UPDATER_

# Working directories
ROOT := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Build tools, so we all share the same versions
# import macros common to all supported build systems
include $(ROOT)/make/system-id.mk

# configure some directories that are relative to wherever ROOT_DIR is located
TOOLS_DIR ?= $(ROOT)/tools
DL_DIR := $(ROOT)/downloads

.PHONY : clean all
all : $(TARGETS)

clean :
	rm -f Src/*.o

$(TARGETS) :
	$(MAKE) TARGET=$@ $(TARGET_PREFIX)$@.bin

$(TARGETS:%=$(TARGET_PREFIX)%.bin) : clean build_bootloader $(OBJ)
	echo $(CC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_PREFIX)$(TARGET).elf $(OBJ)
	$(CP) -O binary $(TARGET_PREFIX)$(TARGET).elf $(TARGET_PREFIX)$(TARGET).bin
	$(CP) $(TARGET_PREFIX)$(TARGET).elf -O ihex  $(TARGET_PREFIX)$(TARGET).hex

build_bootloader :
	cd bootloader && ARM_SDK_PREFIX=../$(ARM_SDK_PREFIX) $(MAKE) $(TARGET)

# mkdirs
$(DL_DIR):
	mkdir -p $@

$(TOOLS_DIR):
	mkdir -p $@

# include the tools makefile
include $(ROOT)/make/tools.mk
