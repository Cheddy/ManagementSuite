#-------------------------------------------------
#
# Project created by QtCreator 2014-11-14T07:28:01
#
#-------------------------------------------------

QT += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ManagementSuite
TEMPLATE = app
CONFIG += c++14

RC_FILE = app.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    trainingoverview.cpp \
    protocoloverview.cpp

HEADERS  += mainwindow.h \
    trainingoverview.h \
    protocoloverview.h

FORMS    += mainwindow.ui \
    trainingoverview.ui \
    protocoloverview.ui
