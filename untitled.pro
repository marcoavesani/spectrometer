#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T11:01:41
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11 `root-config --cflags --glibs`

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app
INCLUDEPATH += /usr/include/root

        LIBS += $$system(root-config --glibs)

        INCLUDEPATH += $(ROOTSYS)/include


SOURCES += main.cpp\
        mainwindow.cpp \
    stepper_motor.cpp \
    counting.cpp \
    spectrometer.cpp


HEADERS  += mainwindow.h \
    stepper_motor.h \
    sm32_2.h \
    counting.h \
    dt304.h \
    spectrometer.h


FORMS    += mainwindow.ui
