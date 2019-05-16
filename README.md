# QtMosquitto
Wrapper of the [Mosquitto](http://mosquitto.org/) library for [Qt](http://www.qt.io/) 5

[![Build Status](https://travis-ci.org/marcopellin/qtmosquitto.svg?branch=master)](https://travis-ci.org/marcopellin/qtmosquitto)

# QtMosquitto on Linux

## Building with cmake
    cd build
    cmake -D CMAKE_BUILD_TYPE=Debug ../source
    make
    gui/qtmosquitto-demo

## Building with cmake and different QTDIR
    cd build
    cmake -D CMAKE_BUILD_TYPE=Debug -D QTDIR=/your_home/Qt/5.x/gcc_64/lib/cmake ../source
    make
    gui/qtmosquitto-demo

## Building with qmake
    qmake qtmosquitto.pro
    make

## Installing the libray
    cp build/lib/libqtmosquitto.so* /usr/lib/
    cp source/qtmosquitto.hpp /usr/include/

## Using the library in a Qt Project
    Include the header file:
        #include "qtmosquitto.hpp"

    Add the library to your .pro file:
        LIBS += -lqtmosquitto

# QtMosquitto on Windows

## Building with cmake
    "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
    set PATH=C:\Qt\5.4\msvc2013\bin;%PATH%
    cd build
    cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE=Debug ..\source
    nmake
    gui\qtmosquitto-demo.exe
