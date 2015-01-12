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
    connection.cpp \
    ../VuzdarCommon/vuzdarpacket.cpp \
    newgroupwindow.cpp \
    conversationwindow.cpp \
    conversation.cpp \
    adminwindow.cpp

HEADERS  += application.h \
    connection.h \
    ../VuzdarCommon/vuzdarpacket.h \
    newgroupwindow.h \
    conversationwindow.h \
    conversation.h \
    adminwindow.h

FORMS    += application.ui \
    newgroupwindow.ui \
    conversationwindow.ui \
    adminwindow.ui
