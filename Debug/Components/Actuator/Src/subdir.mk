################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/Actuator/Src/actuator.c \
../Components/Actuator/Src/actuator_ram.c 

OBJS += \
./Components/Actuator/Src/actuator.o \
./Components/Actuator/Src/actuator_ram.o 

C_DEPS += \
./Components/Actuator/Src/actuator.d \
./Components/Actuator/Src/actuator_ram.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Actuator/Src/%.o Components/Actuator/Src/%.su Components/Actuator/Src/%.cyclo: ../Components/Actuator/Src/%.c Components/Actuator/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mustafa.acar/STM32CubeIDE/workspace_1.19.0/Actuator_Homing_FW/Components/Actuator/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-Actuator-2f-Src

clean-Components-2f-Actuator-2f-Src:
	-$(RM) ./Components/Actuator/Src/actuator.cyclo ./Components/Actuator/Src/actuator.d ./Components/Actuator/Src/actuator.o ./Components/Actuator/Src/actuator.su ./Components/Actuator/Src/actuator_ram.cyclo ./Components/Actuator/Src/actuator_ram.d ./Components/Actuator/Src/actuator_ram.o ./Components/Actuator/Src/actuator_ram.su

.PHONY: clean-Components-2f-Actuator-2f-Src

