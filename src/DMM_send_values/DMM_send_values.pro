# #####################################################################
# DMM_send_values
# #####################################################################
PROJECT = DMM_send_values
TEMPLATE = app
DEPENDPATH += .
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
HEADERS = mainwindow.h \
    dmmcontrol.h \
    ../config.h \
    ../settings.h \
    ../ledindicator.h \
    ../serialportctr.h
SOURCES = ../main.cpp \
    mainwindow.cpp \
    dmmcontrol.cpp \
    ../config.cpp \
    ../settings.cpp \
    ../ledindicator.cpp \
    ../serialportctr.cpp
CONFIG += extserialport
LIBS += -lqextserialport
win32:LIBS += -lsetupapi
win32:CONFIG += console
unix:DEFINES += _TTY_POSIX_
linux:DEFINES += _TTY_POSIX_
win32:DEFINES += _TTY_WIN_
RESOURCES += ../ledindicator.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
