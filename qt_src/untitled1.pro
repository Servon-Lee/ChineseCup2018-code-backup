#-------------------------------------------------
#
# Project created by QtCreator 2018-06-06T20:06:01
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videoanalysis.cpp \
    realtimeanalysis.cpp \
    database.cpp \
    playvideo.cpp \
    initthread.cpp \
    connthread.cpp

HEADERS  += mainwindow.h \
    videoanalysis.h \
    realtimeanalysis.h \
    connection.h \
    database.h \
    playvideo.h \
    detector.h \
    initthread.h \
    connthread.h

FORMS    += mainwindow.ui \
    videoanalysis.ui \
    realtimeanalysis.ui \
    database.ui \
    playvideo.ui

RC_FILE += logo.rc

LIBS += yolo_cpp_dll.lib

INCLUDEPATH += include \
include/opencv \
include/opencv2

LIBS += vc15/lib/*.lib
LIBS += -L/vc15/bin/*.dll

RESOURCES += \
    src.qrc
