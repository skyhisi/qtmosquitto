cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Widgets)

add_executable(qtmosquitto-demo
  demo.cpp
  window.cpp
  window.hpp
)
qt5_use_modules(qtmosquitto-demo Core Widgets)
target_link_libraries(qtmosquitto-demo qtmosquitto)