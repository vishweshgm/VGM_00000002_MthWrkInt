################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f446rctx.s 

S_DEPS += \
./Core/Startup/startup_stm32f446rctx.d 

OBJS += \
./Core/Startup/startup_stm32f446rctx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/appl_diag/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/display_state/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/eeprom/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/GraphicsLcd_12864/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/keypress/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/modbusrtu/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/stateMachine/inc" -I"E:/GitHub_2/QPS_00000019_ThreePhaseSCR/ProjectFolder/workspace/QPS_0019_ThreePhaseSCR_RC/Middlewares/Application_Driver/vishwaAdc/inc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

