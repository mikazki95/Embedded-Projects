################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-812291039: ../example.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.17.0/sysconfig_cli.bat" -s "C:/ti/mcu_plus_sdk_am64x_09_00_00_35/.metadata/product.json" --script "C:/Users/TONA/workspace_v12/task_switch_am64x-evm_r5fss0-0_freertos_gcc-armv7/example.syscfg" --context "r5fss0-0" -o "syscfg" --part Default --package ALV --compiler gcc
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_dpl_config.c: build-812291039 ../example.syscfg
syscfg/ti_dpl_config.h: build-812291039
syscfg/ti_drivers_config.c: build-812291039
syscfg/ti_drivers_config.h: build-812291039
syscfg/ti_drivers_open_close.c: build-812291039
syscfg/ti_drivers_open_close.h: build-812291039
syscfg/ti_pinmux_config.c: build-812291039
syscfg/ti_power_clock_config.c: build-812291039
syscfg/ti_board_config.c: build-812291039
syscfg/ti_board_config.h: build-812291039
syscfg/ti_board_open_close.c: build-812291039
syscfg/ti_board_open_close.h: build-812291039
syscfg/ti_enet_config.c: build-812291039
syscfg/ti_enet_config.h: build-812291039
syscfg/ti_enet_open_close.c: build-812291039
syscfg/ti_enet_open_close.h: build-812291039
syscfg/ti_enet_soc.c: build-812291039
syscfg/ti_enet_lwipif.c: build-812291039
syscfg/ti_enet_lwipif.h: build-812291039
syscfg/ti_pru_io_config.inc: build-812291039
syscfg/: build-812291039

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/bin/aarch64-none-elf-gcc-9.2.1.exe" -c -mcpu=cortex-r5 -marm -mfloat-abi=hard -mfpu=vfpv3-d16 -DSOC_AM64X -D_DEBUG_=1 -I"C:/Users/TONA/workspace_v12/task_switch_am64x-evm_r5fss0-0_freertos_gcc-armv7" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/FreeRTOS-Kernel/include" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/config/am64x/r5f" -I"C:/ti/aarch64-none-elf/include" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/task_switch_am64x-evm_r5fss0-0_freertos_gcc-armv7/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/bin/aarch64-none-elf-gcc-9.2.1.exe" -c -mcpu=cortex-r5 -marm -mfloat-abi=hard -mfpu=vfpv3-d16 -DSOC_AM64X -D_DEBUG_=1 -I"C:/Users/TONA/workspace_v12/task_switch_am64x-evm_r5fss0-0_freertos_gcc-armv7" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/FreeRTOS-Kernel/include" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F" -I"C:/ti/mcu_plus_sdk_am64x_09_00_00_35/source/kernel/freertos/config/am64x/r5f" -I"C:/ti/aarch64-none-elf/include" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Werror -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/task_switch_am64x-evm_r5fss0-0_freertos_gcc-armv7/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


