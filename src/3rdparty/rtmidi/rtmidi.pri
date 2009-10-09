
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += RtError.h RtMidi.h
SOURCES += RtMidi.cpp



win32 {
    DEFINES += __WINDOWS_MM__
    LIBS += -lwinmm
    
    linker : -mwindows
}

macx {
    DEFINES += __MACOSX_CORE__
    LIBS += -framework CoreMidi -framework CoreAudio -framework CoreFoundation
    
}

unix {
    DEFINES += __LINUX_ALSASEQ__ DAVOID_TIMESTAMPING
    INCLUDEPATH += /usr/include/alsa
    LIBS += -lasound
}