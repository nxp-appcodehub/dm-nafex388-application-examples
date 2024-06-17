################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/lists/fsl_component_generic_list.c 

C_DEPS += \
./component/lists/fsl_component_generic_list.d 

OBJS += \
./component/lists/fsl_component_generic_list.o 


# Each subdirectory must supply rules for building sources it contributes
component/lists/%.o: ../component/lists/%.c component/lists/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC54S018JET180 -DCPU_LPC54S018JET180_cm4 -DXIP_IMAGE -DW25Q128JVFM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\board" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\source" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\drivers" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\LPC54S018\drivers" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\device" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\CMSIS" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\component\serial_manager" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\utilities" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\component\uart" -I"C:\Users\nxf99259\Documents\MCUXpressoIDE_11.7.0_9198\workspace\dm-nafexx388-application-examples\NAFE\WeightScale\LPC54s018\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-lists

clean-component-2f-lists:
	-$(RM) ./component/lists/fsl_component_generic_list.d ./component/lists/fsl_component_generic_list.o

.PHONY: clean-component-2f-lists

