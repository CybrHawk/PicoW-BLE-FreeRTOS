add_executable(${ProjectName}
    main.c
    debug.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)


#Adding header file directories
include_directories(../lib/Config)
include_directories(../lib/GUI)
include_directories(../lib/OLED)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    pico_cyw43_arch_none # For CYW4343W WiFi chip of the Wifi version of the Pico
    pico_multicore 
    FreeRTOS-Kernel-Heap4
    OLED
    GUI
    Fonts
    Config
    hardware_spi
    )

pico_enable_stdio_usb(${ProjectName} 1) # Enable USB output

pico_enable_stdio_uart(${ProjectName} 0) #Disable UART output

pico_add_extra_outputs(${ProjectName})