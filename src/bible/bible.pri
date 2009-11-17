VPATH += $$PWD
DEPENDPATH += $$PWD

QT += network

# Generate the book name map
$$system(perl book-abbrv-to-qt-map.pl > BookNameMap.cpp)


# Input
HEADERS +=  \
	BibleModel.h \
	BibleConnector.h \
	BibleGatewayConnector.h \
	BibleBrowser.h
SOURCES += \
	BibleModel.cpp \
	BookNameMap.cpp \
	BibleConnector.cpp \
	BibleGatewayConnector.cpp \
	BibleBrowser.cpp