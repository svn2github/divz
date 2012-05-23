TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR     = .build
OBJECTS_DIR = .build
RCC_DIR     = .build
UI_DIR      = .build

QT += network

include(../../../livepro/gfxengine/gfxengine.pri)

include(../../../livepro/varnet/varnet.pri)


# Input
HEADERS += \
	MainWindow.h
	
SOURCES += main.cpp \
	MainWindow.cpp
