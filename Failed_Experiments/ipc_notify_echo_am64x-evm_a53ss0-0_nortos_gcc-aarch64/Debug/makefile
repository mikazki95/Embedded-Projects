################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./syscfg/ti_dpl_config.o" \
"./syscfg/ti_drivers_config.o" \
"./syscfg/ti_drivers_open_close.o" \
"./syscfg/ti_pinmux_config.o" \
"./syscfg/ti_power_clock_config.o" \
"./syscfg/ti_board_config.o" \
"./syscfg/ti_board_open_close.o" \
"./syscfg/ti_enet_config.o" \
"./syscfg/ti_enet_open_close.o" \
"./syscfg/ti_enet_soc.o" \
"./syscfg/ti_enet_lwipif.o" \
"./ipc_notify_echo.o" \
"./main.o" \
-Wl,-T"../linker.cmd" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lstdc++ \
-lgcc \
-lm \
-lrdimon \
-l:nortos.am64x.a53.gcc-aarch64.debug.lib \
-l:drivers.am64x.a53.gcc-aarch64.debug.lib \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64.out \

EXE_OUTPUTS__QUOTED += \
"ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64.out" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64.out"

# Tool invocations
ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"C:/ti/bin/aarch64-none-elf-gcc-9.2.1.exe" -DSOC_AM64X -D_DEBUG_=1 -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Werror -mcmodel=large -mstrict-align -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -mcpu=cortex-a53+fp+simd -Wl,-Map,"ipc_notify_echo.Debug.map" -static -Wl,--gc-sections -L"C:/ti/mcu_plus_sdk_am64x_08_06_00_43/source/kernel/nortos/lib" -L"C:/ti/mcu_plus_sdk_am64x_08_06_00_43/source/drivers/lib" -L"C:/ti/mcu_plus_sdk_am64x_08_06_00_43/source/board/lib" -L"$(CGT_GCC_AARCH64_PATH)/lib" -Wl,--build-id=none -o"ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "syscfg\ti_dpl_config.o" "syscfg\ti_drivers_config.o" "syscfg\ti_drivers_open_close.o" "syscfg\ti_pinmux_config.o" "syscfg\ti_power_clock_config.o" "syscfg\ti_board_config.o" "syscfg\ti_board_open_close.o" "syscfg\ti_enet_config.o" "syscfg\ti_enet_open_close.o" "syscfg\ti_enet_soc.o" "syscfg\ti_enet_lwipif.o" "ipc_notify_echo.o" "main.o" 
	-$(RM) "syscfg\ti_dpl_config.d" "syscfg\ti_drivers_config.d" "syscfg\ti_drivers_open_close.d" "syscfg\ti_pinmux_config.d" "syscfg\ti_power_clock_config.d" "syscfg\ti_board_config.d" "syscfg\ti_board_open_close.d" "syscfg\ti_enet_config.d" "syscfg\ti_enet_open_close.d" "syscfg\ti_enet_soc.d" "syscfg\ti_enet_lwipif.d" "ipc_notify_echo.d" "main.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-$(MAKE) -C C:\Users\TONA\workspace_v12\ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64 -f makefile_ccs_bootimage_gen OUTNAME=ipc_notify_echo_am64x-evm_a53ss0-0_nortos_gcc-aarch64 PROFILE=Debug MCU_PLUS_SDK_PATH=C:/ti/mcu_plus_sdk_am64x_08_06_00_43 CG_TOOL_ROOT=C:/ti CCS_INSTALL_DIR=C:\ti\ccs1220\ccs CCS_IDE_MODE=desktop DEVICE=am64x
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

