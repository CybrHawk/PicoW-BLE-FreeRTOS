add_executable(${ProjectName}
    main.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

#Adding header file directories
include_directories(../lib/config)
include_directories(../lib/font)
include_directories(../lib/lcd)
include_directories(../lib/fatfs)
include_directories(../lib/sdcard)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    pico_cyw43_arch_none # For CYW4343W WiFi chip of the Wifi version of the Pico
    pico_multicore 
    FreeRTOS-Kernel-Heap4
    lcd
    font
    fatfs
    sdcard
    config
    hardware_spi
    )

pico_enable_stdio_usb(${ProjectName} 1) # Enable USB output
pico_enable_stdio_uart(${ProjectName} 0) #Disable UART output
pico_add_extra_outputs(${ProjectName})

# Standalone example that reads from the on board temperature sensor and sends notifications via BLE
# Flashes slowly each second to show it's running
add_executable(picow_ble_server
        server.c server_common.c
)
target_link_libraries(picow_ble_server
        pico_stdlib
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none
        hardware_adc
)
target_include_directories(picow_ble_server PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} # For btstack config
)
pico_btstack_make_gatt_header(picow_ble_server PRIVATE "${CMAKE_CURRENT_LIST_DIR}/server.gatt")

pico_add_extra_outputs(picow_ble_server)



# Standalone example that connects to picow_ble_server and reads the temperature
# Flahes once quickly each second when it's running but not connected to another device
# Flashes twice quickly each second when connected to another device and reading it's temperature
add_executable(picow_ble_client
        client.c
)
target_link_libraries(picow_ble_client
        FreeRTOS-Kernel-Heap4
        pico_multicore
        pico_stdlib
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none
        hardware_adc
        lcd
        font
        fatfs
        sdcard
        config
        hardware_spi
)
target_include_directories(picow_ble_client PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} # For btstack config
)
target_compile_definitions(picow_ble_client PRIVATE
        RUNNING_AS_CLIENT=1
)

# enable usb output, disable uart output
pico_enable_stdio_usb(picow_ble_client 1)
pico_enable_stdio_uart(picow_ble_client 0)

pico_add_extra_outputs(picow_ble_client)
