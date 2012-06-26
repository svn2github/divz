VPATH += $$PWD
DEPENDPATH += $$PWD

QT += network
 
FORMS += HttpUserSetupDialog.ui
	
HEADERS += \
	HttpServer.h \
	SimpleTemplate.h \
	ControlServer.h \
	ViewServer.h \
	TabletServer.h \
	HttpUserUtil.h \
	HttpUserSetupDialog.h
	
SOURCES += HttpServer.cpp \
	SimpleTemplate.cpp \
	ControlServer.cpp \
	ViewServer.cpp \
	TabletServer.cpp \
	HttpUserUtil.cpp \
	HttpUserSetupDialog.cpp