#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T12:12:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VuzdarChat
TEMPLATE = app


SOURCES += main.cpp\
        application.cpp \
    connection.cpp

HEADERS  += application.h \
    connection.h

FORMS    += application.ui
