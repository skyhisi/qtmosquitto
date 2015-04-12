cmake_minimum_required(VERSION 3.0)

find_package(Qt5Core)
find_package(Qt5Widgets)


qt5_wrap_cpp(DEMO_MOC window.hpp)

add_executable(qtmosquitto-demo
  demo.cpp
  window.cpp
  window.hpp
  "${DEMO_MOC}"
)
qt5_use_modules(qtmosquitto-demo Core Widgets)
target_link_libraries(qtmosquitto-demo qtmosquitto)