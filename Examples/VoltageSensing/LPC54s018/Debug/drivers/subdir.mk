################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/NAFESDK.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_ctimer.c \
../drivers/fsl_dma.c \
../drivers/fsl_emc.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_pint.c \
../drivers/fsl_reset.c \
../drivers/fsl_spi.c \
../drivers/fsl_usart.c 

C_DEPS += \
./drivers/NAFESDK.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_ctimer.d \
./drivers/fsl_dma.d \
./drivers/fsl_emc.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_pint.d \
./drivers/fsl_reset.d \
./drivers/fsl_spi.d \
./drivers/fsl_usart.d 

OBJS += \
./drivers/NAFESDK.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_ctimer.o \
./drivers/fsl_dma.o \
./drivers/fsl_emc.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_pint.o \
./drivers/fsl_reset.o \
./drivers/fsl_spi.o \
./drivers/fsl_usart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC54S018JET180 -DCPU_LPC54S018JET180_cm4 -DXIP_IMAGE -DW25Q128JVFM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\board" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\source" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\drivers" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\LPC54S018\drivers" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\device" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\CMSIS" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\component\serial_manager" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\utilities" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\component\uart" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\VoltageSensing\LPC54s018\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/NAFESDK.d ./drivers/NAFESDK.o ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_ctimer.d ./drivers/fsl_ctimer.o ./drivers/fsl_dma.d ./drivers/fsl_dma.o ./drivers/fsl_emc.d ./drivers/fsl_emc.o ./drivers/fsl_flexcomm.d ./drivers/fsl_flexcomm.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_inputmux.d ./drivers/fsl_inputmux.o ./drivers/fsl_pint.d ./drivers/fsl_pint.o ./drivers/fsl_reset.d ./drivers/fsl_reset.o ./drivers/fsl_spi.d ./drivers/fsl_spi.o ./drivers/fsl_usart.d ./drivers/fsl_usart.o

.PHONY: clean-drivers

