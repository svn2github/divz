TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += ../

include(../audio/audio.pri)

# Input
HEADERS += recorder.h
SOURCES += audio_test.cpp \
           recorder.cpp