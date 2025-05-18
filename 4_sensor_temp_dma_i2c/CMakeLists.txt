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

project(sensor_temp_dma_i2c C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(sensor_temp_dma_i2c sensor_temp_dma_i2c.c inc/ssd1306_i2c.c 
    helpers/display_oled/display_oled.c
    helpers/setup/setup.c
    helpers/temperature_dma_adc/temperature_dma_adc.c )

pico_set_program_name(sensor_temp_dma_i2c "sensor_temp_dma_i2c")
pico_set_program_version(sensor_temp_dma_i2c "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(sensor_temp_dma_i2c 0)
pico_enable_stdio_usb(sensor_temp_dma_i2c 1)

# Add the standard library to the build
target_link_libraries(sensor_temp_dma_i2c
        pico_stdlib
        hardware_i2c
        hardware_dma
        pico_stdlib
        hardware_adc)

# Add the standard include files to the build
target_include_directories(sensor_temp_dma_i2c PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/helpers
        ${CMAKE_CURRENT_LIST_DIR}/helpers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/helpers/setup
        ${CMAKE_CURRENT_LIST_DIR}/helpers/temperature_dma_adc
        ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Add any user requested libraries
target_link_libraries(sensor_temp_dma_i2c 
        
        )

pico_add_extra_outputs(sensor_temp_dma_i2c)

