cmake_minimum_required(VERSION 3.0)

find_package(Qt5 5.0 COMPONENTS Core)

find_library(MOSQUITTO_LIB mosquitto
  HINTS "$ENV{MOSQUITTO_DIR}/devel"
)

find_path(MOSQUITTO_INC mosquitto.h
  HINTS "$ENV{MOSQUITTO_DIR}/devel"
)

include_directories("${MOSQUITTO_INC}")

qt5_wrap_cpp(QTMOSQUITTO_MOC qtmosquitto.hpp)

add_library(qtmosquitto SHARED
  qtmosquitto.hpp
  qtmosquitto.cpp
  "${QTMOSQUITTO_MOC}"
)

target_link_libraries(qtmosquitto Qt5::Core "${MOSQUITTO_LIB}")


add_subdirectory(demo)


