################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../main.syscfg 

C_SRCS += \
../main.c \
./syscfg/ti_msp_dl_config.c \
/Users/lance/ti/mspm0_sdk_2_03_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c \
../startup_mspm0g350x_ticlang.c 

GEN_CMDS += \
./syscfg/device_linker.cmd 

GEN_FILES += \
./syscfg/device_linker.cmd \
./syscfg/device.opt \
./syscfg/ti_msp_dl_config.c 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./main.d \
./syscfg/ti_msp_dl_config.d \
./startup_mspm0g350x_ticlang.d \
./startup_mspm0g350x_ticlang.d 

GEN_OPTS += \
./syscfg/device.opt 

OBJS += \
./main.o \
./syscfg/ti_msp_dl_config.o \
./startup_mspm0g350x_ticlang.o 

GEN_MISC_FILES += \
./syscfg/device.cmd.genlibs \
./syscfg/ti_msp_dl_config.h \
./syscfg/Event.dot 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"main.o" \
"syscfg/ti_msp_dl_config.o" \
"startup_mspm0g350x_ticlang.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg/device.cmd.genlibs" \
"syscfg/ti_msp_dl_config.h" \
"syscfg/Event.dot" 

C_DEPS__QUOTED += \
"main.d" \
"syscfg/ti_msp_dl_config.d" \
"startup_mspm0g350x_ticlang.d" \
"startup_mspm0g350x_ticlang.d" 

GEN_FILES__QUOTED += \
"syscfg/device_linker.cmd" \
"syscfg/device.opt" \
"syscfg/ti_msp_dl_config.c" 

C_SRCS__QUOTED += \
"../main.c" \
"./syscfg/ti_msp_dl_config.c" \
"/Users/lance/ti/mspm0_sdk_2_03_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c" \
"../startup_mspm0g350x_ticlang.c" 


