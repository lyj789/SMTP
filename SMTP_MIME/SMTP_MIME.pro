#-------------------------------------------------
#
# Project created by QtCreator 2018-11-09T14:27:57
#
#-------------------------------------------------

QT       += core gui
QT      +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SMTP_MIME
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    base64.cpp \
    quoted.cpp \
    mymainwindow.cpp

HEADERS  += mainwindow.h \
    base64.h \
    quoted.h \
    mymainwindow.h

FORMS    += mainwindow.ui \
    mymainwindow.ui
