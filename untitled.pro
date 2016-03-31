#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T11:01:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stepper_motor.cpp \
    spectrom.cpp

HEADERS  += mainwindow.h \
    stepper_motor.h \
    spectrom.h \
    sm32_2.h

FORMS    += mainwindow.ui
