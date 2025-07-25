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

project(project_micro_servo_education C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(project_micro_servo_education project_micro_servo_education.c 
        dhcpserver/dhcpserver.c
        dnsserver/dnsserver.c
        src/web_server/web_server.c
        src/wifi_manager/wifi_manager.c
        src/servo_control/servo_control.c)


pico_set_program_name(project_micro_servo_education "project_micro_servo_education")
pico_set_program_version(project_micro_servo_education "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(project_micro_servo_education 0)
pico_enable_stdio_usb(project_micro_servo_education 1)

# Add the standard library to the build
target_link_libraries(project_micro_servo_education
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        hardware_pwm
        pico_multicore
        )

# Add the standard include files to the build
target_include_directories(project_micro_servo_education PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
        ${CMAKE_CURRENT_SOURCE_DIR}/src/web_server
        ${CMAKE_CURRENT_SOURCE_DIR}/src/wifi_manager
        ${CMAKE_CURRENT_SOURCE_DIR}/src/servo_control
        ${CMAKE_CURRENT_SOURCE_DIR}/src 
)

# Add any user requested libraries
target_link_libraries(project_micro_servo_education 
        
        )

# You can change the address below to change the address of the access point
pico_configure_ip4_address(project_micro_servo_education PRIVATE
        CYW43_DEFAULT_IP_AP_ADDRESS 192.168.4.1
    )

pico_add_extra_outputs(project_micro_servo_education)

