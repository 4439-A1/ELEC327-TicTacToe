################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang" -c -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -Og -I"/Users/lance/workspace_v12/finalproj1" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source/third_party/CMSIS/Core/Include" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source" -I"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/include" -D__MSPM0G3507__ -DSystemCoreClock=32000000 -D__FPU_PRESENT=0 -g -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/lance/workspace_v12/finalproj1/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-497456463: ../main.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Applications/ti/ccs1281/ccs/utils/sysconfig_1.21.0/sysconfig_cli.sh" --script "/Users/lance/workspace_v12/finalproj1/main.syscfg" -o "syscfg" -s "/Users/lance/ti/mspm0_sdk_2_03_00_07/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/device_linker.cmd: build-497456463 ../main.syscfg
syscfg/device.opt: build-497456463
syscfg/device.cmd.genlibs: build-497456463
syscfg/ti_msp_dl_config.c: build-497456463
syscfg/ti_msp_dl_config.h: build-497456463
syscfg/Event.dot: build-497456463
syscfg: build-497456463

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang" -c -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -Og -I"/Users/lance/workspace_v12/finalproj1" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source/third_party/CMSIS/Core/Include" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source" -I"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/include" -D__MSPM0G3507__ -DSystemCoreClock=32000000 -D__FPU_PRESENT=0 -g -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/lance/workspace_v12/finalproj1/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: /Users/lance/ti/mspm0_sdk_2_03_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang" -c -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -Og -I"/Users/lance/workspace_v12/finalproj1" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source/third_party/CMSIS/Core/Include" -I"/Users/lance/ti/mspm0_sdk_2_03_00_07/source" -I"/Applications/ti/ccs1281/ccs/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/include" -D__MSPM0G3507__ -DSystemCoreClock=32000000 -D__FPU_PRESENT=0 -g -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/lance/workspace_v12/finalproj1/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


