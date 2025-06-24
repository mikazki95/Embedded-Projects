################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1395070481: ../example.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.17.0/sysconfig_cli.bat" -s "C:/ti/mcu_plus_sdk_am64x_09_00_00_35/.metadata/product.json" --script "C:/Users/TONA/workspace_v12/UART_example/example.syscfg" --context "r5fss0-0" -o "syscfg" --part Default --package ALV --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_dpl_config.c: build-1395070481 ../example.syscfg
syscfg/ti_dpl_config.h: build-1395070481
syscfg/ti_drivers_config.c: build-1395070481
syscfg/ti_drivers_config.h: build-1395070481
syscfg/ti_drivers_open_close.c: build-1395070481
syscfg/ti_drivers_open_close.h: build-1395070481
syscfg/ti_pinmux_config.c: build-1395070481
syscfg/ti_power_clock_config.c: build-1395070481
syscfg/ti_board_config.c: build-1395070481
syscfg/ti_board_config.h: build-1395070481
syscfg/ti_board_open_close.c: build-1395070481
syscfg/ti_board_open_close.h: build-1395070481
syscfg/ti_enet_config.c: build-1395070481
syscfg/ti_enet_config.h: build-1395070481
syscfg/ti_enet_open_close.c: build-1395070481
syscfg/ti_enet_open_close.h: build-1395070481
syscfg/ti_enet_soc.c: build-1395070481
syscfg/ti_enet_lwipif.c: build-1395070481
syscfg/ti_enet_lwipif.h: build-1395070481
syscfg/ti_pru_io_config.inc: build-1395070481
syscfg/: build-1395070481

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16 -mlittle-endian -mthumb -I"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/include/c" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/FreeRTOS-Kernel/include" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/config/am64x/r5f" -DSOC_AM64X -D_DEBUG_=1 -g -Wall -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/UART_example/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16 -mlittle-endian -mthumb -I"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/include/c" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/FreeRTOS-Kernel/include" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/config/am64x/r5f" -DSOC_AM64X -D_DEBUG_=1 -g -Wall -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/UART_example/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


