################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/CmdHandler/FPEC_interface.c \
../Core/Src/CmdHandler/aes.c \
../Core/Src/CmdHandler/aes_operation.c \
../Core/Src/CmdHandler/cmd_handling.c \
../Core/Src/CmdHandler/com_utils.c \
../Core/Src/CmdHandler/flash_operation.c \
../Core/Src/CmdHandler/hal_utils.c \
../Core/Src/CmdHandler/sram_operation.c \
../Core/Src/CmdHandler/utils.c 

OBJS += \
./Core/Src/CmdHandler/FPEC_interface.o \
./Core/Src/CmdHandler/aes.o \
./Core/Src/CmdHandler/aes_operation.o \
./Core/Src/CmdHandler/cmd_handling.o \
./Core/Src/CmdHandler/com_utils.o \
./Core/Src/CmdHandler/flash_operation.o \
./Core/Src/CmdHandler/hal_utils.o \
./Core/Src/CmdHandler/sram_operation.o \
./Core/Src/CmdHandler/utils.o 

C_DEPS += \
./Core/Src/CmdHandler/FPEC_interface.d \
./Core/Src/CmdHandler/aes.d \
./Core/Src/CmdHandler/aes_operation.d \
./Core/Src/CmdHandler/cmd_handling.d \
./Core/Src/CmdHandler/com_utils.d \
./Core/Src/CmdHandler/flash_operation.d \
./Core/Src/CmdHandler/hal_utils.d \
./Core/Src/CmdHandler/sram_operation.d \
./Core/Src/CmdHandler/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/CmdHandler/%.o Core/Src/CmdHandler/%.su Core/Src/CmdHandler/%.cyclo: ../Core/Src/CmdHandler/%.c Core/Src/CmdHandler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/hugo/.local/share/STMicroelectronics/STM32Cube/workspace_1.12.1/SPD_F401RBT_target/Core/Inc/CmdHandler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-CmdHandler

clean-Core-2f-Src-2f-CmdHandler:
	-$(RM) ./Core/Src/CmdHandler/FPEC_interface.cyclo ./Core/Src/CmdHandler/FPEC_interface.d ./Core/Src/CmdHandler/FPEC_interface.o ./Core/Src/CmdHandler/FPEC_interface.su ./Core/Src/CmdHandler/aes.cyclo ./Core/Src/CmdHandler/aes.d ./Core/Src/CmdHandler/aes.o ./Core/Src/CmdHandler/aes.su ./Core/Src/CmdHandler/aes_operation.cyclo ./Core/Src/CmdHandler/aes_operation.d ./Core/Src/CmdHandler/aes_operation.o ./Core/Src/CmdHandler/aes_operation.su ./Core/Src/CmdHandler/cmd_handling.cyclo ./Core/Src/CmdHandler/cmd_handling.d ./Core/Src/CmdHandler/cmd_handling.o ./Core/Src/CmdHandler/cmd_handling.su ./Core/Src/CmdHandler/com_utils.cyclo ./Core/Src/CmdHandler/com_utils.d ./Core/Src/CmdHandler/com_utils.o ./Core/Src/CmdHandler/com_utils.su ./Core/Src/CmdHandler/flash_operation.cyclo ./Core/Src/CmdHandler/flash_operation.d ./Core/Src/CmdHandler/flash_operation.o ./Core/Src/CmdHandler/flash_operation.su ./Core/Src/CmdHandler/hal_utils.cyclo ./Core/Src/CmdHandler/hal_utils.d ./Core/Src/CmdHandler/hal_utils.o ./Core/Src/CmdHandler/hal_utils.su ./Core/Src/CmdHandler/sram_operation.cyclo ./Core/Src/CmdHandler/sram_operation.d ./Core/Src/CmdHandler/sram_operation.o ./Core/Src/CmdHandler/sram_operation.su ./Core/Src/CmdHandler/utils.cyclo ./Core/Src/CmdHandler/utils.d ./Core/Src/CmdHandler/utils.o ./Core/Src/CmdHandler/utils.su

.PHONY: clean-Core-2f-Src-2f-CmdHandler

