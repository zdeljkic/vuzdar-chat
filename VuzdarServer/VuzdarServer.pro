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
    ../VuzdarCommon/vuzdarpacket.cpp \
    group.cpp \
    admininfo.cpp

HEADERS  += application.h \
    server.h \
    client.h \
    ../VuzdarCommon/vuzdarpacket.h \
    group.h \
    admininfo.h

FORMS    += application.ui
