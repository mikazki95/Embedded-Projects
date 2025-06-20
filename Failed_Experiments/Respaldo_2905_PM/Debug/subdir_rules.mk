################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/gabriela/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang" -c -mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16 -mlittle-endian -mthumb -I"/home/gabriela/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/include/c" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/FreeRTOS-Kernel/include" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/config/am64x/r5f" -DSOC_AM64X -D_DEBUG_=1 -g -Wall -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/home/gabriela/Workspace_Nvo/test_ipc_uart/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1057410319: ../rproc_r5f0_0.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/home/gabriela/ti/ccs1220/ccs/utils/sysconfig_1.15.0/sysconfig_cli.sh" -s "/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/.metadata/product.json" --script "/home/gabriela/Workspace_Nvo/test_ipc_uart/rproc_r5f0_0.syscfg" --context "r5fss0-0" -o "syscfg" --part Default --package ALV --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_dpl_config.c: build-1057410319 ../rproc_r5f0_0.syscfg
syscfg/ti_dpl_config.h: build-1057410319
syscfg/ti_drivers_config.c: build-1057410319
syscfg/ti_drivers_config.h: build-1057410319
syscfg/ti_drivers_open_close.c: build-1057410319
syscfg/ti_drivers_open_close.h: build-1057410319
syscfg/ti_pinmux_config.c: build-1057410319
syscfg/ti_power_clock_config.c: build-1057410319
syscfg/ti_board_config.c: build-1057410319
syscfg/ti_board_config.h: build-1057410319
syscfg/ti_board_open_close.c: build-1057410319
syscfg/ti_board_open_close.h: build-1057410319
syscfg/ti_enet_config.c: build-1057410319
syscfg/ti_enet_config.h: build-1057410319
syscfg/ti_enet_open_close.c: build-1057410319
syscfg/ti_enet_open_close.h: build-1057410319
syscfg/ti_enet_soc.c: build-1057410319
syscfg/ti_enet_lwipif.c: build-1057410319
syscfg/ti_enet_lwipif.h: build-1057410319
syscfg/ti_pru_io_config.inc: build-1057410319
syscfg/: build-1057410319

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/gabriela/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang" -c -mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16 -mlittle-endian -mthumb -I"/home/gabriela/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/include/c" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/FreeRTOS-Kernel/include" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"/home/gabriela/ti/mcu_plus_sdk_am64x_08_05_00_24/source/kernel/freertos/config/am64x/r5f" -DSOC_AM64X -D_DEBUG_=1 -g -Wall -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"/home/gabriela/Workspace_Nvo/test_ipc_uart/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


