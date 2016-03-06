# PKES
Meine Bearbeitungen der praktischen Aufgaben für PKES im Wintersemester 2015/2016. Ich hab einfach alles gepusht was ich im Verlauf des Semesters erstellt habe.
Abgenommene Versionen (glaube ich):
* Aufgabe **1** (Siebensegmentanzeige):
    * **fertig/PKES_PRAC_1/PKES_first/PKES_prac_1_atmel_1/**
* Aufgabe **2** (Sensordatenverarbeitung):
    * Wasserwaage:
        * **fertig/PKES_prac_2_level/**
    * Distanzsensor
        * **fertig/PKES_prac_2_distance_working_for_49/**
* Aufgabe **3**:
    * Kollisionsfreies Fahren:
        * **fertig/PKES_PRAC_3/collision_free_driving/**
    * Gyro-Drehung
        * **fertig/PKES_PRAC_3/serial_rotate_by_degree/**
* Aufgabe **4** und **5**:
    * **aufgabe4_float_serial_input_clion_funktioniert_warum_auch_immer/**
    
CMakeLists.txt die ich zum Entwickeln mit CLion und Compilen mit der Arduino-IDE genutzt habe:
```
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
```
