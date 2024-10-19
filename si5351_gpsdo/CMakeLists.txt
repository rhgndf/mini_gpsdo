#############################################################################################################################
# file:  CMakeLists.txt
# brief: Template "CMakeLists.txt" for building of executables and static libraries.
#
# usage: Edit "VARIABLES"-section to suit project requirements.
#        For debug build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Debug -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
#          make -C Debug VERBOSE=1 -j
#        For release build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Release -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#          make -C Release VERBOSE=1 -j
#############################################################################################################################
cmake_minimum_required(VERSION 3.20)

set (CMAKE_TOOLCHAIN_FILE "cubeide-gcc.cmake")

###################### CONSTANTS ######################################
set (PROJECT_TYPE_EXECUTABLE          "exe")
set (PROJECT_TYPE_STATIC_LIBRARY      "static-lib")
set (MCPU_CORTEX_M0				      "-mcpu=cortex-m0")
set (MCPU_CORTEX_M0PLUS				  "-mcpu=cortex-m0plus")
set (MCPU_CORTEX_M3				      "-mcpu=cortex-m3")
set (MCPU_CORTEX_M4				      "-mcpu=cortex-m4")
set (MCPU_CORTEX_M7				      "-mcpu=cortex-m7")
set (MCPU_CORTEX_M33				  "-mcpu=cortex-m33")
set (MCPU_CORTEX_M55				  "-mcpu=cortex-m55")
set (MCPU_CORTEX_M85				  "-mcpu=cortex-m85")
set (MFPU_FPV4_SP_D16                 "-mfpu=fpv4-sp-d16")
set (MFPU_FPV5_D16                    "-mfpu=fpv5-d16")
set (RUNTIME_LIBRARY_REDUCED_C        "--specs=nano.specs")
set (RUNTIME_LIBRARY_STD_C            "")
set (RUNTIME_LIBRARY_SYSCALLS_MINIMAL "--specs=nosys.specs")
set (RUNTIME_LIBRARY_SYSCALLS_NONE    "")
set (MFLOAT_ABI_SOFTWARE              "-mfloat-abi=soft")
set (MFLOAT_ABI_HARDWARE              "-mfloat-abi=hard")
set (MFLOAT_ABI_MIX                   "-mfloat-abi=softfp")
#######################################################################

###################### VARIABLES ######################################
set (PROJECT_NAME             "si5351_gpsdo")
set (PROJECT_TYPE             "exe")
set (LINKER_SCRIPT            "../STM32G431CBUX_FLASH.ld")
set (MCPU                     "-mcpu=Cortex-M4")
set (MFPU                 "-mfpu=fpv4-sp-d16")
set (MFLOAT_ABI               "")
set (RUNTIME_LIBRARY          "--specs=nano.specs")
set (RUNTIME_LIBRARY_SYSCALLS "--specs=nosys.specs")

execute_process(COMMAND git submodule update --init --recursive)

set (PROJECT_SOURCES
	# LIST SOURCE FILES HERE
	Startup/startup_stm32g431cbux.s
	Sources/main.c
	Sources/syscalls.c
	Sources/sysmem.c
	Sources/system_stm32g4xx.c
	Sources/stm32g4xx_it.c
	Sources/usb_descriptors.c
	Sources/usb.c
	Sources/ms5351.c
	Sources/gps.c
	
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_adc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_adc_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_comp.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_cordic.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_cortex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_crc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_crc_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_cryp.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_cryp_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_dac.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_dac_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_dma.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_dma_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_exti.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_fdcan.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_flash.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_flash_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_flash_ramfunc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_fmac.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_gpio.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_hrtim.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_i2c.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_i2c_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_i2s.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_irda.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_iwdg.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_lptim.c
	#Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_msp_template.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_nand.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_nor.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_opamp.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_opamp_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_pcd.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_pcd_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_pwr.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_pwr_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_qspi.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_rcc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_rcc_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_rng.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_rtc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_rtc_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_sai.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_sai_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_smartcard.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_smartcard_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_smbus.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_smbus_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_spi.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_spi_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_sram.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_tim.c
	#Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_timebase_tim_template.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_tim_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_uart.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_uart_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_usart.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_usart_ex.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_hal_wwdg.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_adc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_comp.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_cordic.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_crc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_crs.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_dac.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_dma.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_exti.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_fmac.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_fmc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_gpio.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_hrtim.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_i2c.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_lptim.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_lpuart.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_opamp.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_pwr.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_rcc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_rng.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_rtc.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_spi.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_tim.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_ucpd.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_usart.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_usb.c
	Drivers/stm32g4xx-hal-driver/Src/stm32g4xx_ll_utils.c

    Drivers/tinyusb/src/tusb.c
    Drivers/tinyusb/src/common/tusb_fifo.c
    # device
    Drivers/tinyusb/src/device/usbd.c
    Drivers/tinyusb/src/device/usbd_control.c
    Drivers/tinyusb/src/class/audio/audio_device.c
    Drivers/tinyusb/src/class/cdc/cdc_device.c
    Drivers/tinyusb/src/class/dfu/dfu_device.c
    Drivers/tinyusb/src/class/dfu/dfu_rt_device.c
    Drivers/tinyusb/src/class/hid/hid_device.c
    Drivers/tinyusb/src/class/midi/midi_device.c
    Drivers/tinyusb/src/class/msc/msc_device.c
    Drivers/tinyusb/src/class/net/ecm_rndis_device.c
    Drivers/tinyusb/src/class/net/ncm_device.c
    Drivers/tinyusb/src/class/usbtmc/usbtmc_device.c
    Drivers/tinyusb/src/class/vendor/vendor_device.c
    Drivers/tinyusb/src/class/video/video_device.c
    # host
    Drivers/tinyusb/src/host/usbh.c
    Drivers/tinyusb/src/host/hub.c
    Drivers/tinyusb/src/class/cdc/cdc_host.c
    Drivers/tinyusb/src/class/hid/hid_host.c
    Drivers/tinyusb/src/class/msc/msc_host.c
    Drivers/tinyusb/src/class/vendor/vendor_host.c

	Drivers/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
	)

set (PROJECT_DEFINES
	# LIST COMPILER DEFINITIONS HERE
	-DSTM32G431xx
	-DCFG_TUSB_MCU=OPT_MCU_STM32G4
    )

set (PROJECT_INCLUDES
	# LIST INCLUDE DIRECTORIES HERE
	Sources/
	Drivers/stm32g4xx-hal-driver/Inc
	Drivers/cmsis-device-g4/Include
	Drivers/CMSIS_6/CMSIS/Core/Include
    Drivers/tinyusb/src
    )

############ MODIFY ACCORDING TO REQUIREMENTS) ########################

#######################################################################

################## PROJECT SETUP ######################################
project(${PROJECT_NAME})
enable_language(ASM)

if (${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_EXECUTABLE})
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
  )
elseif (${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_STATIC_LIBRARY})
  add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

add_compile_definitions (${PROJECT_DEFINES})
include_directories (${PROJECT_INCLUDES})

set (CMAKE_EXECUTABLE_SUFFIX ".elf")
set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set (CMAKE_C_FLAGS "${MCPU} -std=gnu11 ${MFPU} ${MFLOAT_ABI} ${RUNTIME_LIBRARY} -g3 -O3 -flto -Wall -Werror")
set (CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -flto -Wl,-Map=test.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
