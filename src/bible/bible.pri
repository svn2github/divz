VPATH += $$PWD
DEPENDPATH += $$PWD

QT += network

# Generate the book name map file, BookNameMap.cpp
system(perl book-abbrv-to-qt-map.pl)

# Input
FORMS += ChooseGroupDialog.ui

HEADERS +=  \
	BibleModel.h \
	BibleConnector.h \
	BibleGatewayConnector.h \
	BibleBrowser.h \
	ChooseGroupDialog.h \
	LocalBibleManager.h
SOURCES += \
	BibleModel.cpp \
	BookNameMap.cpp \
	BibleConnector.cpp \
	BibleGatewayConnector.cpp \
	BibleBrowser.cpp \
	ChooseGroupDialog.cpp \
	LocalBibleManager.cpp
	

# Unit Tests
tests: {
	SOURCES += BibleTestClass.cpp
	HEADERS += BibleTestClass.h
}