################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Middlewares/Application_Driver/vishwaAdc/src/subdir.mk
-include Middlewares/Application_Driver/stateMachine/src/subdir.mk
-include Middlewares/Application_Driver/modbusrtu/src/rtu/subdir.mk
-include Middlewares/Application_Driver/modbusrtu/src/functions/subdir.mk
-include Middlewares/Application_Driver/modbusrtu/src/common/subdir.mk
-include Middlewares/Application_Driver/modbusrtu/src/subdir.mk
-include Middlewares/Application_Driver/keypress/src/subdir.mk
-include Middlewares/Application_Driver/eeprom/src/subdir.mk
-include Middlewares/Application_Driver/display_state/src/subdir.mk
-include Middlewares/Application_Driver/appl_diag/src/subdir.mk
-include Middlewares/Application_Driver/GraphicsLcd_12864/src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := QPS_0019_ThreePhaseSCR_RC
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
QPS_0019_ThreePhaseSCR_RC.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
QPS_0019_ThreePhaseSCR_RC.list \

OBJCOPY_BIN += \
QPS_0019_ThreePhaseSCR_RC.bin \


# All Target
all: main-build

# Main-build Target
main-build: QPS_0019_ThreePhaseSCR_RC.elf secondary-outputs

# Tool invocations
QPS_0019_ThreePhaseSCR_RC.elf: $(OBJS) $(USER_OBJS) E:\GitHub_2\QPS_00000019_ThreePhaseSCR\ProjectFolder\workspace\QPS_0019_ThreePhaseSCR_RC\STM32F446RCTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "QPS_0019_ThreePhaseSCR_RC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\GitHub_2\QPS_00000019_ThreePhaseSCR\ProjectFolder\workspace\QPS_0019_ThreePhaseSCR_RC\STM32F446RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="QPS_0019_ThreePhaseSCR_RC.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

QPS_0019_ThreePhaseSCR_RC.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "QPS_0019_ThreePhaseSCR_RC.list"
	@echo 'Finished building: $@'
	@echo ' '

QPS_0019_ThreePhaseSCR_RC.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "QPS_0019_ThreePhaseSCR_RC.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJDUMP_LIST)$(C_UPPER_DEPS)$(S_DEPS)$(C_DEPS)$(OBJCOPY_BIN)$(CC_DEPS)$(SIZE_OUTPUT)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(CXX_DEPS)$(S_UPPER_DEPS)$(CPP_DEPS) QPS_0019_ThreePhaseSCR_RC.elf
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
