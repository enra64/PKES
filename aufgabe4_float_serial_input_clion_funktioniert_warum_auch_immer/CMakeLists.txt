cmake_minimum_required(VERSION 3.3)
project(aufgabe4_float_serial_input_clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/opt/arduino-1.6.6/hardware/arduino/avr/cores/arduino /opt/arduino-1.6.6/hardware/tools/avr/avr/include /opt/arduino-1.6.6/hardware/arduino/avr/variants/mega)
include_directories(/opt/arduino/hardware/arduino/avr/cores/arduino /opt/arduino/hardware/tools/avr/avr/include /opt/arduino/hardware/arduino/avr/variants/mega)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Os")

set(CMAKE_C_COMPILER   avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(SOURCE_FILES
    aufgabe4_float_serial_input_clion.ino
    average_ring.h
    general.h
    inputhandler.cpp
    inputhandler.h
    motor.cpp
    motor.h
    odometry.cpp
    odometry.h
    pidcontrol.cpp
    pidcontrol.h
    SSD.h)

add_executable(aufgabe4_float_serial_input_clion ${SOURCE_FILES})