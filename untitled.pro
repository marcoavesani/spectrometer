#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T11:01:41
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stepper_motor.cpp \
    spectrom.cpp \
    counting.cpp \
    calibration.cpp
    calibrate.cpp


HEADERS  += mainwindow.h \
    stepper_motor.h \
    spectrom.h \
    sm32_2.h \
    counting.h \
    dt304.h \
    calibrate.h


FORMS    += mainwindow.ui
