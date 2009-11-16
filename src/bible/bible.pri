VPATH += $$PWD
DEPENDPATH += $$PWD

QT += network


# Input
HEADERS +=  \
	BibleModel.h \
	BibleConnector.h \
	BibleGatewayConnector.h
SOURCES += \
	BibleModel.cpp \
	BibleConnector.cpp \
	BibleGatewayConnector.cpp