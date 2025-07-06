add_executable(picow_freertos
    main.c
    ../helpers/botoes/botoes.c
    ../helpers/buzzer/buzzer.c
    ../helpers/joystick/joystick.c
    ../helpers/leds/leds.c
    ../helpers/micro/micro.c
    ../helpers/setup/setup.c
    ../helpers/tasks_logic/tasks_logic.c
)

set_target_properties(picow_freertos PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_sources(picow_freertos
    PRIVATE
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
)

target_include_directories(picow_freertos PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../config
    ${CMAKE_CURRENT_LIST_DIR}/../tasks
    ${CMAKE_CURRENT_LIST_DIR}/../drivers
    ${CMAKE_CURRENT_LIST_DIR}/../helpers

)

target_link_libraries(picow_freertos 
    pico_stdlib 
    hardware_adc
    hardware_dma
    hardware_timer
    hardware_clocks
    hardware_pwm
    FreeRTOS-Kernel
)


pico_add_extra_outputs(picow_freertos)
