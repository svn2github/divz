TEMPLATE = app
TARGET = sdltest
DEPENDPATH += .
INCLUDEPATH += .
#QT += opengl

# Input
#HEADERS += sdltest.h
SOURCES += sdltest.cpp

unix {
    LIBS += -lSDL -lSDLmain
}


win32 {
     INCLUDEPATH += \
	 ./external/include
         
     LIBS += -L"./external/lib" \
         -lSDL \
         -lSDLmain
}
