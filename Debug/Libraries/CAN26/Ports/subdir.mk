################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/CAN26/Ports/PortSTM32g4xx.c 

OBJS += \
./Libraries/CAN26/Ports/PortSTM32g4xx.o 

C_DEPS += \
./Libraries/CAN26/Ports/PortSTM32g4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/CAN26/Ports/%.o Libraries/CAN26/Ports/%.su Libraries/CAN26/Ports/%.cyclo: ../Libraries/CAN26/Ports/%.c Libraries/CAN26/Ports/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCHIP_TYPE=CHIP_TYPE_STM32_G4XX -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Libraries/CAN26/Packets -I../Libraries/CAN26/Ports -I../Libraries/CAN26 -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-CAN26-2f-Ports

clean-Libraries-2f-CAN26-2f-Ports:
	-$(RM) ./Libraries/CAN26/Ports/PortSTM32g4xx.cyclo ./Libraries/CAN26/Ports/PortSTM32g4xx.d ./Libraries/CAN26/Ports/PortSTM32g4xx.o ./Libraries/CAN26/Ports/PortSTM32g4xx.su

.PHONY: clean-Libraries-2f-CAN26-2f-Ports

