VPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += SongSearchOnlineDialog.ui

# Check for (and install) any missing perl modules required for the online lyrics search functionality
system(perl get_deps.pl)

HEADERS +=  \
	SongRecord.h \
	SongRecordListModel.h \
	SongSlideGroup.h \
	SongSlideGroupFactory.h \
	SongSlideGroupListModel.h \
	SongEditorWindow.h \
	SongBrowser.h \
	SongSearchOnlineDialog.h \
	ArrangementListModel.h \
	SongEditorHighlighter.h

    
SOURCES += \
	SongRecord.cpp \
	SongRecordListModel.cpp \
	SongSlideGroup.cpp \
	SongSlideGroupFactory.cpp \
	SongSlideGroupListModel.cpp \
	SongEditorWindow.cpp \
	SongBrowser.cpp \
	SongSearchOnlineDialog.cpp \
	ArrangementListModel.cpp \
	SongEditorHighlighter.cpp