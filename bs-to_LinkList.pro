#-------------------------------------------------
#
# Project created by QtCreator 2015-04-19T12:35:46
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bs-to_LinkList
TEMPLATE = app

win32:CONFIG(debug, debug|release): LIBS += -lqwindowsd -lqtfreetyped -lQt5PlatformSupportd

SOURCES += main.cpp\
        mainwindow.cpp \
    httpdownloader.cpp \
    httprequestjob.cpp \
    bswidget.cpp

HEADERS  += mainwindow.h \
    httpdownloader.h \
    httprequestjob.h \
    bswidget.h

FORMS    += mainwindow.ui \
    bswidget.ui
