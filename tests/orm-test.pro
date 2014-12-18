#-------------------------------------------------
#
# Project created by QtCreator 2014-12-17T22:18:28
#
#-------------------------------------------------

QT       += core sql
QT       -= gui

TARGET = orm-test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../src

SOURCES += main.cpp\
    ../src/databaseobject.cpp \
    ../src/databasehelper.cpp \
    objb.cpp

HEADERS  += widget.h \
    ../src/databaseobject.h \
    ../src/databasehelper.h \
    objb.h

