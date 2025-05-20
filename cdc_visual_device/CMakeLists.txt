# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(cdc_visual_device C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(cdc_visual_device main.c 
inc/ssd1306_i2c.c
helpers/display_oled/display_oled.c
helpers/setup/setup.c
helpers/leds/leds.c
helpers/buzzer/buzzer.c)

pico_set_program_name(cdc_visual_device "cdc_visual_device")
pico_set_program_version(cdc_visual_device "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(cdc_visual_device 0)
pico_enable_stdio_usb(cdc_visual_device 1)

# Add the standard library to the build
target_link_libraries(cdc_visual_device
        pico_stdlib
        hardware_uart
        hardware_pwm
        hardware_gpio
        hardware_i2c
        hardware_clocks 
)

# Add the standard include files to the build
target_include_directories(cdc_visual_device PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/helpers/
        ${CMAKE_CURRENT_LIST_DIR}/helpers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/helpers/setup
        ${CMAKE_CURRENT_LIST_DIR}/helpers/leds
        ${CMAKE_CURRENT_LIST_DIR}/helpers/buzzer
        ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Add any user requested libraries
target_link_libraries(cdc_visual_device 
        
        )

pico_add_extra_outputs(cdc_visual_device)

