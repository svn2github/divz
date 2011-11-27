TEMPLATE = app
TARGET = miditcp
DEPENDPATH += .
INCLUDEPATH += .

QT += network

# Input
HEADERS += MidiReader.h \
	MidiServer.h \
	MidiServerThread.h
SOURCES += MidiReader.cpp \
	MidiServer.cpp \
	MidiServerThread.cpp \
	main.cpp 

rxtest: {
	SOURCES -= main.cpp
	SOURCES += MidiReceiver.cpp \
		main_rxtest.cpp
	HEADERS += MidiReceiver.h
	TARGET = rxtest 
}