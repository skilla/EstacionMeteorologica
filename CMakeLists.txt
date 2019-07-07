cmake_minimum_required(VERSION 3.5)

set(CMAKE_FIND_FRAMEWORK FIRST)
set(CMAKE_FIND_APPBUNDLE FIRST)
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
include(/Users/sergio.zambrano/esp/esp-idf/tools/cmake/project.cmake)

set(PROJECT_NAME EstacionMeteorologica)
set(LIBRARY_BASE_PATH /Users/sergio.zambrano/Documents/Arduino/libraries)
project(${PROJECT_NAME})

register_hardware_platform(/Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2)
#register_hardware_platform(/Applications/Arduino.app/Contents/Java/hardware/arduino/avr)

include_directories(
        ${LIBRARY_BASE_PATH}/Adafruit_BMP280_Library
        ${LIBRARY_BASE_PATH}/Adafruit_BusIO
        ${LIBRARY_BASE_PATH}/Adafruit_Circuit_Playground
        ${LIBRARY_BASE_PATH}/Adafruit_Unified_Sensor
        ${LIBRARY_BASE_PATH}/Adafruit_VEML6075_Library
        ${LIBRARY_BASE_PATH}/DFRobot_SHT20-master
        /Users/sergio.zambrano/Documents/Arduino/libraries/RTClib
        ${LIBRARY_BASE_PATH}/Servo
        ${LIBRARY_BASE_PATH}/VEML6075
)
link_directories(
        ${LIBRARY_BASE_PATH}/Adafruit_BMP280_Library
        ${LIBRARY_BASE_PATH}/Adafruit_BusIO
        ${LIBRARY_BASE_PATH}/Adafruit_Circuit_Playground
        ${LIBRARY_BASE_PATH}/Adafruit_Unified_Sensor
        ${LIBRARY_BASE_PATH}/Adafruit_VEML6075_Library
        ${LIBRARY_BASE_PATH}/DFRobot_SHT20-master
        ${LIBRARY_BASE_PATH}/RTClib
        ${LIBRARY_BASE_PATH}/Servo
        ${LIBRARY_BASE_PATH}/VEML6075
)

set(${CMAKE_PROJECT_NAME}_SKETCH ${PROJECT_NAME}.ino)
set(${CMAKE_PROJECT_NAME}_BOARD esp32doit-devkit-v1)
set(${CMAKE_PROJECT_NAME}_PORT /dev/cu.SLAB_USBtoUART)
set(mega.build.mcu esp32)
set(mega.upload.protocol wiring)
set(mega.upload.speed 921600)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})



