QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imei_gen
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH = /home/ibroheem/usr/include
QMAKE_LIBDIR = /home/ibroheem/usr/lib

LIBS += -lpugixml-arm

SOURCES += main.cpp

HEADERS  += mainwindow.h \
    generic_menu.h \
    luhn.h \
    mainwindow_droid.h

FORMS    += mainwindow.ui

OTHER_FILES +=
