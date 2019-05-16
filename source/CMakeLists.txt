cmake_minimum_required(VERSION 3.0)

if (QTDIR)
  list (APPEND CMAKE_PREFIX_PATH ${QTDIR})
endif (QTDIR)

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)

find_library(MOSQUITTO_LIB mosquitto
  HINTS "$ENV{MOSQUITTO_DIR}/devel"
)

find_path(MOSQUITTO_INC mosquitto.h
  HINTS "$ENV{MOSQUITTO_DIR}/devel"
)

include_directories("${MOSQUITTO_INC}")

add_library(qtmosquitto SHARED
  qtmosquitto.hpp
  qtmosquitto.cpp
)

qt5_use_modules(qtmosquitto Core)
target_link_libraries(qtmosquitto "${MOSQUITTO_LIB}")


add_subdirectory(demo)


