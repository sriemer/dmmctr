# #####################################################################
# DMM_get_values
# #####################################################################
PROJECT = DMM_get_values
TEMPLATE = app
DEPENDPATH += .
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
HEADERS = mainwindow.h \
    dmmcontrol.h \
    settings.h \
    ledindicator.h \
    report.h \
    serialportctr.h \
    configxml.h
SOURCES = main.cpp \
    mainwindow.cpp \
    dmmcontrol.cpp \
    settings.cpp \
    ledindicator.cpp \
    report.cpp \
    serialportctr.cpp \
    configxml.cpp
TRANSLATIONS = de_DE.ts
RESOURCES += ledindicator.qrc \
    appicons.qrc
OTHER_FILES += appicon.rc
CONFIG += xml \
    extserialport
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    LIBS += -lQt5ExtSerialPort
} else {
    LIBS += -lqextserialport
}
QMAKE_RESOURCE_FLAGS += -no-compress
win32 { 
    LIBS += -lsetupapi \
        -lqaxcontainer
    CONFIG += qaxcontainer
    DEFINES += _TTY_WIN_
    RC_FILE = appicon.rc
}
unix:DEFINES += _TTY_POSIX_
linux:DEFINES += _TTY_POSIX_
