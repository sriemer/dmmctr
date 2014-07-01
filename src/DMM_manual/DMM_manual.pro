# #####################################################################
# DMM_manual
# #####################################################################
PROJECT = DMM_manual
TEMPLATE = app
DEPENDPATH += .
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
HEADERS = mainwindow.h \
    dmmcontrol.h \
    ../settings.h \
    ../ledindicator.h \
    ../serialportctr.h
SOURCES = ../main.cpp \
    mainwindow.cpp \
    dmmcontrol.cpp \
    ../settings.cpp \
    ../ledindicator.cpp \
    ../serialportctr.cpp
CONFIG += extserialport
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    LIBS += -lQt5ExtSerialPort
} else {
    LIBS += -lqextserialport
}
win32:LIBS += -lsetupapi
win32:CONFIG += console
unix:DEFINES += _TTY_POSIX_
linux:DEFINES += _TTY_POSIX_
win32:DEFINES += _TTY_WIN_
RESOURCES += ../ledindicator.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
