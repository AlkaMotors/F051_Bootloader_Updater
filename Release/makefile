################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
F051_LL_BOOTLOADER_UPLOADER.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
F051_LL_BOOTLOADER_UPLOADER.list \

OBJCOPY_BIN += \
F051_LL_BOOTLOADER_UPLOADER.bin \


# All Target
all: F051_LL_BOOTLOADER_UPLOADER.elf secondary-outputs

# Tool invocations
F051_LL_BOOTLOADER_UPLOADER.elf: $(OBJS) $(USER_OBJS) C:\Users\Alka\STM32CubeIDE\workspace_1.0.2\F051_LL_BOOTLOADER_UPLOADER\STM32F051K6TX_FLASH.ld
	arm-none-eabi-gcc -o "F051_LL_BOOTLOADER_UPLOADER.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\Alka\STM32CubeIDE\workspace_1.0.2\F051_LL_BOOTLOADER_UPLOADER\STM32F051K6TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="F051_LL_BOOTLOADER_UPLOADER.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

F051_LL_BOOTLOADER_UPLOADER.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "F051_LL_BOOTLOADER_UPLOADER.list"
	@echo 'Finished building: $@'
	@echo ' '

F051_LL_BOOTLOADER_UPLOADER.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "F051_LL_BOOTLOADER_UPLOADER.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
