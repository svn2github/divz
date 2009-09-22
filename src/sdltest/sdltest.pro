TEMPLATE = app
TARGET = sdltest
DEPENDPATH += .
INCLUDEPATH += .
#QT += opengl

# Input
#HEADERS += sdltest.h
SOURCES += sdltest.cpp



win32 {
     INCLUDEPATH += \
	 ./external/include/SDL
         
     LIBS += -L"./external/lib" \
         -lSDL \
         -lSDLmain
}
