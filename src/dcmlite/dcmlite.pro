#-------------------------------------------------
#
# Project created by QtCreator 2017-06-20T16:48:40
#
#-------------------------------------------------

QT       -= core gui

TARGET = dcmlite
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += data_dictionary.cpp \
    data_set.cpp \
    dicom_reader.cpp \
    read_handler.cpp \
    tag_table.cpp \
    vr.cpp \
    data_element.cpp \
    defs.cpp \
    file.cpp \
    tag.cpp \
    visitor.cpp \
    util.cpp

HEADERS += data_dictionary.h \
    data_set.h \
    dcmlite.h \
    dicom_reader.h \
    read_handler.h \
    tag_table.h \
    vr.h \
    data_element.h \
    defs.h \
    file.h \
    tag.h \
    visitor.h \
    util.h

# For include statement like "dcmlite/tag.h".
INCLUDEPATH += "$${PWD}/.."

win32:INCLUDEPATH += "C:\Boost\include\boost-1_61"

unix {
    target.path = /usr/lib
    INSTALLS += target
}
