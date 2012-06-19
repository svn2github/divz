VPATH += $$PWD
DEPENDPATH += $$PWD

QT += network 
	
HEADERS += \
	HttpServer.h \
	SimpleTemplate.h \
	ControlServer.h \
	ViewServer.h \
	TabletServer.h
SOURCES += HttpServer.cpp \
	SimpleTemplate.cpp \
	ControlServer.cpp \
	ViewServer.cpp \
	TabletServer.cpp