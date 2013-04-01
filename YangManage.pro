#-------------------------------------------------
#
# Project created by QtCreator 2013-03-30T03:13:54
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YangManage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    coredata.cpp \
    addpersondialog.cpp

HEADERS  += mainwindow.h \
    coredata.h \
    addpersondialog.h

FORMS    += mainwindow.ui \
    addpersondialog.ui
