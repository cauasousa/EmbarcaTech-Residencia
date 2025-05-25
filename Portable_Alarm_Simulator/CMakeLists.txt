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

project(Portable_Alarm_Simulator C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(portable_alarm_simulator_background Portable_Alarm_Simulator.c 
inc/ssd1306_i2c.c
helpers/display_oled/display_oled.c
helpers/setup/setup.c
helpers/leds/leds.c
helpers/buzzer/buzzer.c
dhcpserver/dhcpserver.c
dnsserver/dnsserver.c)

# pico_set_program_name(Portable_Alarm_Simulator "Portable_Alarm_Simulator")
# pico_set_program_version(Portable_Alarm_Simulator "0.1")


# Add the standard library to the build
target_link_libraries(portable_alarm_simulator_background
        pico_stdlib
        hardware_uart
        hardware_pwm
        hardware_gpio
        hardware_i2c
        hardware_clocks )

# Add the standard include files to the build
target_include_directories(portable_alarm_simulator_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/helpers/
        ${CMAKE_CURRENT_LIST_DIR}/helpers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/helpers/setup
        ${CMAKE_CURRENT_LIST_DIR}/helpers/leds
        ${CMAKE_CURRENT_LIST_DIR}/helpers/buzzer
        ${CMAKE_CURRENT_LIST_DIR}/inc
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
)

# Add any user requested libraries
target_link_libraries(portable_alarm_simulator_background pico_cyw43_arch_lwip_threadsafe_background hardware_pwm hardware_i2c hardware_clocks hardware_gpio)

# You can change the address below to change the address of the access point
pico_configure_ip4_address(portable_alarm_simulator_background PRIVATE
        CYW43_DEFAULT_IP_AP_ADDRESS 192.168.4.1
        )
pico_add_extra_outputs(portable_alarm_simulator_background)

#############################

add_executable(portable_alarm_simulator_poll
        Portable_Alarm_Simulator.c
        inc/ssd1306_i2c.c
helpers/display_oled/display_oled.c
helpers/setup/setup.c
helpers/leds/leds.c
helpers/buzzer/buzzer.c
dhcpserver/dhcpserver.c
dnsserver/dnsserver.c
        )
target_include_directories(portable_alarm_simulator_poll PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/helpers/
        ${CMAKE_CURRENT_LIST_DIR}/helpers/display_oled
        ${CMAKE_CURRENT_LIST_DIR}/helpers/setup
        ${CMAKE_CURRENT_LIST_DIR}/helpers/leds
        ${CMAKE_CURRENT_LIST_DIR}/helpers/buzzer
        ${CMAKE_CURRENT_LIST_DIR}/inc
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
        )
target_link_libraries(portable_alarm_simulator_poll
        pico_cyw43_arch_lwip_poll
        pico_stdlib hardware_pwm hardware_i2c hardware_clocks hardware_gpio
        )
# You can change the address below to change the address of the access point
pico_configure_ip4_address(portable_alarm_simulator_poll PRIVATE
        CYW43_DEFAULT_IP_AP_ADDRESS 192.168.4.1
        )
pico_add_extra_outputs(portable_alarm_simulator_poll)