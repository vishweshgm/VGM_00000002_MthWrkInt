################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Application_Driver/modbusrtu/src/mbm.c \
../Middlewares/Application_Driver/modbusrtu/src/mbportevent.c \
../Middlewares/Application_Driver/modbusrtu/src/mbportother.c \
../Middlewares/Application_Driver/modbusrtu/src/mbportserial.c \
../Middlewares/Application_Driver/modbusrtu/src/mbporttimer.c \
../Middlewares/Application_Driver/modbusrtu/src/vishwaModbus.c 

C_DEPS += \
./Middlewares/Application_Driver/modbusrtu/src/mbm.d \
./Middlewares/Application_Driver/modbusrtu/src/mbportevent.d \
./Middlewares/Application_Driver/modbusrtu/src/mbportother.d \
./Middlewares/Application_Driver/modbusrtu/src/mbportserial.d \
./Middlewares/Application_Driver/modbusrtu/src/mbporttimer.d \
./Middlewares/Application_Driver/modbusrtu/src/vishwaModbus.d 

OBJS += \
./Middlewares/Application_Driver/modbusrtu/src/mbm.o \
./Middlewares/Application_Driver/modbusrtu/src/mbportevent.o \
./Middlewares/Application_Driver/modbusrtu/src/mbportother.o \
./Middlewares/Application_Driver/modbusrtu/src/mbportserial.o \
./Middlewares/Application_Driver/modbusrtu/src/mbporttimer.o \
./Middlewares/Application_Driver/modbusrtu/src/vishwaModbus.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Application_Driver/modbusrtu/src/%.o: ../Middlewares/Application_Driver/modbusrtu/src/%.c Middlewares/Application_Driver/modbusrtu/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/appl_diag/inc" -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/display_state/inc" -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/eeprom/inc" -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/modbusrtu/inc" -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/stateMachine/inc" -I"E:/GitHub_2/QPS_00000038_ThreePhaseBatteryCharger/ProjectFolder/workspace/QPS_00000038_ThreePhaseBatteryCharger/Middlewares/Application_Driver/vishwaAdc/inc" -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
