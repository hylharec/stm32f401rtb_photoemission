################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f401rbtx.s 

OBJS += \
./Core/Startup/startup_stm32f401rbtx.o 

S_DEPS += \
./Core/Startup/startup_stm32f401rbtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"/home/hugo/.local/share/STMicroelectronics/STM32Cube/workspace_1.12.1/SPD_F401RBT_target/Core/Inc/CmdHandler" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f401rbtx.d ./Core/Startup/startup_stm32f401rbtx.o

.PHONY: clean-Core-2f-Startup

