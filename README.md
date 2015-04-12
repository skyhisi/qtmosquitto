# QtMosquitto
Wrapper of the [Mosquitto](http://mosquitto.org/) library for [Qt](http://www.qt.io/) 5


# Building on Windows
	"c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
	set PATH=C:\Qt\5.4\msvc2013\bin;%PATH%
    mkdir build
    cd build
    cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE=Debug ..\source
    nmake
    gui\qtmosquitto-demo.exe
