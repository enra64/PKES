project(the_beginning_4)

cmake_minimum_required(VERSION 2.6)

include(${CMAKE_SOURCE_DIR}/scripts/arduino_uno.cmake)

set(MYPROJECT_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/the_beginning_4.ino
    ${ARDUINO_SOURCE_FILES}
)

add_executable(the_beginning_4 ${MYPROJECT_SOURCE_FILES})
