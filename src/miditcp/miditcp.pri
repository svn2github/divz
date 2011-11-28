VPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

FORMS += \
	MidiInputSettingsDialog.ui
	
SOURCES += \
	MidiReceiver.cpp \
	MidiInputAdapter.cpp \
	MidiInputSettingsDialog.cpp

HEADERS += \
	MidiReceiver.h \
	MidiInputAdapter.h \
	MidiInputSettingsDialog.h