#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T12:21:45
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VuzdarServer
TEMPLATE = app


SOURCES += main.cpp\
        application.cpp \
    server.cpp \
    client.cpp \
    vuzdarpacket.cpp

HEADERS  += application.h \
    server.h \
    client.h \
    vuzdarpacket.h

FORMS    += application.ui
