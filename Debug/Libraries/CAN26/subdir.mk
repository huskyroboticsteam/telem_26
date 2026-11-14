################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/CAN26/CANPacket.c 

OBJS += \
./Libraries/CAN26/CANPacket.o 

C_DEPS += \
./Libraries/CAN26/CANPacket.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/CAN26/%.o Libraries/CAN26/%.su Libraries/CAN26/%.cyclo: ../Libraries/CAN26/%.c Libraries/CAN26/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCHIP_TYPE=CHIP_TYPE_STM32_G4XX -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Libraries/CAN26/Packets -I../Libraries/CAN26/Ports -I../Libraries/CAN26 -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-CAN26

clean-Libraries-2f-CAN26:
	-$(RM) ./Libraries/CAN26/CANPacket.cyclo ./Libraries/CAN26/CANPacket.d ./Libraries/CAN26/CANPacket.o ./Libraries/CAN26/CANPacket.su

.PHONY: clean-Libraries-2f-CAN26

