#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T10:46:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = floorplanner_gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Floorplans.cpp \
    Geometry.cpp \
    Module.cpp \
    SlicingStructure.cpp \
    GraphicsArea.cpp \
    InputOutputManager.cpp

HEADERS  += mainwindow.h \
    Floorplans.h \
    Geometry.h \
    Module.h \
    SlicingStructure.h \
    GraphicsArea.h \
    InputOutputManager.h

LIBS += \
       -lboost_system \
       -lboost_regex

FORMS    += mainwindow.ui
