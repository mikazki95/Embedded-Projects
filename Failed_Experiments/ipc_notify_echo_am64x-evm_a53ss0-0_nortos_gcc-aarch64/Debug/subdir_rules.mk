################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-634964463: ../example.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1220/ccs/utils/sysconfig_1.15.0/sysconfig_cli.bat" -s "C:/ti/mcu_plus_sdk_am64x_08_06_00_43/.metadata/product.json" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_r5fss0-0_freertos_ti-arm-clang/example.syscfg" --context "r5fss0-0" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_r5fss0-1_nortos_ti-arm-clang/example.syscfg" --context "r5fss0-1" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_r5fss1-0_nortos_ti-arm-clang/example.syscfg" --context "r5fss1-0" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_r5fss1-1_nortos_ti-arm-clang/example.syscfg" --context "r5fss1-1" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_m4fss0-0_nortos_ti-arm-clang/example.syscfg" --context "m4fss0-0" --script "C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64/example.syscfg" --context "a53ss0-0" -o "syscfg" --part Default --package ALV --compiler gcc
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_dpl_config.c: build-634964463 ../example.syscfg
syscfg/ti_dpl_config.h: build-634964463
syscfg/ti_drivers_config.c: build-634964463
syscfg/ti_drivers_config.h: build-634964463
syscfg/ti_drivers_open_close.c: build-634964463
syscfg/ti_drivers_open_close.h: build-634964463
syscfg/ti_pinmux_config.c: build-634964463
syscfg/ti_power_clock_config.c: build-634964463
syscfg/ti_board_config.c: build-634964463
syscfg/ti_board_config.h: build-634964463
syscfg/ti_board_open_close.c: build-634964463
syscfg/ti_board_open_close.h: build-634964463
syscfg/ti_enet_config.c: build-634964463
syscfg/ti_enet_config.h: build-634964463
syscfg/ti_enet_open_close.c: build-634964463
syscfg/ti_enet_open_close.h: build-634964463
syscfg/ti_enet_soc.c: build-634964463
syscfg/ti_enet_lwipif.c: build-634964463
syscfg/ti_enet_lwipif.h: build-634964463
syscfg/ti_pru_io_config.inc: build-634964463
syscfg/: build-634964463

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/bin/aarch64-none-elf-gcc-9.2.1.exe" -c -mcpu=cortex-a53+fp+simd -DSOC_AM64X -D_DEBUG_=1 -I"C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64" -I"C:/ti/mcu_plus_sdk_am64x_08_06_00_43/source" -I"C:/ti/aarch64-none-elf/include" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Werror -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/bin/aarch64-none-elf-gcc-9.2.1.exe" -c -mcpu=cortex-a53+fp+simd -DSOC_AM64X -D_DEBUG_=1 -I"C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64" -I"C:/ti/mcu_plus_sdk_am64x_08_06_00_43/source" -I"C:/ti/aarch64-none-elf/include" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Werror -mabi=lp64 -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/TONA/workspace_v12/ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


