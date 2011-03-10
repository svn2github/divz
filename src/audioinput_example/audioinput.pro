HEADERS       = audioinput.h
SOURCES       = audioinput.cpp \
                main.cpp

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += ../

#CONFIG+=console

#include(../audio/audio.pri)
include(../qtmultimedia/audio/audio.pri)
include(../qtmultimedia/video/video.pri)
include(../3rdparty/analyzers/analyzers.pri)
#include(../qtmultimedia/multimedia.pri)



