# This file is based on the work of:
#
# http://mjo.tc/atelier/2009/02/arduino-cli.html
# http://johanneshoff.com/arduino-command-line.html
# http://www.arduino.cc/playground/Code/CmakeBuild
# http://www.tmpsantos.com.br/en/2010/12/arduino-uno-ubuntu-cmake/
# The libarduino-1.0 version is based on the work of:
# http://playground.arduino.cc/Code/Kdevelop


set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# C only fine tunning
set(TUNNING_FLAGS "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums") 

set(CMAKE_CXX_FLAGS "-mmcu=${ARDUINO_MCU} -DF_CPU=${ARDUINO_FCPU} -Os")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${TUNNING_FLAGS} -Wall -Wstrict-prototypes -std=gnu99")

set(ARDUINO_CORE_DIR "/usr/share/arduino/hardware/arduino/cores/arduino/")
set(ARDUINO_PINS_DIR "/usr/share/arduino/hardware/arduino/variants/${ARDUINO_BOARD}")
include_directories(${ARDUINO_PINS_DIR})
include_directories(${ARDUINO_CORE_DIR})
set(ARDUINO_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/scripts/ldd_hack.cpp
	${ARDUINO_CORE_DIR}/HardwareSerial.cpp
	${ARDUINO_CORE_DIR}/Print.cpp
	${ARDUINO_CORE_DIR}/WInterrupts.c
	${ARDUINO_CORE_DIR}/wiring_analog.c
	${ARDUINO_CORE_DIR}/wiring.c
	${ARDUINO_CORE_DIR}/wiring_digital.c
	${ARDUINO_CORE_DIR}/wiring_pulse.c
	${ARDUINO_CORE_DIR}/wiring_shift.c
	${ARDUINO_CORE_DIR}/WMath.cpp
	${ARDUINO_CORE_DIR}/WString.cpp
)

set(PORT $ENV{ARDUINO_PORT})
if (NOT PORT)
    set(PORT ${ARDUINO_PORT})
endif()

find_program(AVROBJCOPY "avr-objcopy")
find_program(AVRDUDE "avrdude")

# FIXME: Forcing target name to be "firmware"
if(AVROBJCOPY AND AVRDUDE)
    add_custom_target(hex)
    add_dependencies(hex the_beginning_4)

    add_custom_command(TARGET hex POST_BUILD
        COMMAND ${AVROBJCOPY} -O ihex -R .eeprom ${CMAKE_CURRENT_BINARY_DIR}/the_beginning_4 the_beginning_4.hex
    )

    add_custom_target(flash)
    add_dependencies(flash hex)

    add_custom_command(TARGET flash POST_BUILD
        COMMAND ${AVRDUDE} -P ${PORT} -b ${ARDUINO_UPLOAD_SPEED} -c ${ARDUINO_PROTOCOL} -p ${ARDUINO_MCU} -V -F -U flash:w:the_beginning_4.hex:i
    )
endif()

add_custom_target(reset)
add_custom_command(TARGET reset POST_BUILD
    COMMAND echo 0 > ${PORT}
)
