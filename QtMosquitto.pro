########################################################################################################################
# libqtmosquitto : A C++ (Qt based) wrapper for mosquitto.						               #
########################################################################################################################
# Version .......: 1.0                                                                                                 #
# Authors .......: Silas Parker, Marco Pellin                                                                          #
# Changes .......: -                                                                                                   #
########################################################################################################################


# COMMON SETTINGS FOR ALL PLATFORMS ####################################################################################
TEMPLATE        = lib                           # Library.
QT             += core network                  # We use Qt's Core and Network modules, not the GUI module.
QT             -= gui
CONFIG         += warn_on \                     # Show all warnings.
                  debug_and_release build_all \ # Build debug and release version of the lib.
                  silent                        # Build silent.
                  c++11
LIBS           += -lmosquitto

# MACOSX SPECIFIC SETTINGS #############################################################################################
macx:CONFIG += lib_bundle                       # Create the GDF2 library as a framework for easy deployment.


# DEPENDENCIES AND INLCUDES ############################################################################################
INCLUDEPATH    += /usr/include                  # Our own includes.


# OUTPUT DESTINATIONS ##################################################################################################
MOC_DIR         = ./build/.moc                  # Qt's meta object compiler files.
CONFIG( debug , debug | release ) {
OBJECTS_DIR     = ./build/.obj/debug            # Object folder in debug mode.
TARGET          = qtmosquittod                      # Library name for the debug version.
} else {
OBJECTS_DIR     = ./build/.obj/release          # Object folder in release mode.
TARGET          = qtmosquitto                       # Library name for the release version.
}
DESTDIR         = ./build/lib                   # Library files output folder.


# FILES ################################################################################################################
HEADERS        +=   source/qtmosquitto.hpp

SOURCES        +=   source/qtmosquitto.cpp


# INSTALLATION #########################################################################################################
QMAKE_STRIP     =                                   # Only with this workaround we can actually install debug versions.
HEADERS_INSTALL.files   = ./include/*               # All header files can be found at this location.
LIB_INSTALL.files       = ./build/lib/*             # All release binary files can be found there.

# Linux install locations.
unix:!macx {
HEADERS_INSTALL.path    = /usr/include/qtmosquitto/
LIB_INSTALL.path        = /usr/lib/
}

# OSX install locations.
macx {
FRAMEWORK_HEADERS.files = include/
FRAMEWORK_HEADERS.path = Headers
QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
LIB_INSTALL.path = /Library/Frameworks/
}

# Windows install locations.
win32 {
HEADERS_INSTALL.path    = C:/windows/system32/include/QModbus
LIB_INSTALL.path        = C:/windows/system32}

INSTALLS = LIB_INSTALL
!macx:INSTALLS += HEADERS_INSTALL                   # Header install not needed on Mac, since the framework stuff do it.
