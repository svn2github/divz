
TEMPLATE = app
TARGET = dviz

INCLUDEPATH += .
DEPENDPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

QT += sql

BUILDNUM = $$system(perl buildcount.pl -v)
VERSTR = '\\"$${BUILDNUM}\\"'  # place quotes around the version string

unix {
	VERSION  = "0.8.2"
	VERSION = "$${VERSION} (Build $${BUILDNUM})"
	SVNREV   = $$system(svn info -r HEAD . | grep Changed\ Rev | cut -b 19-)
	
	!isEmpty(SVNREV) {
		VERSION = "$${VERSION} r$${SVNREV}"
	}
	VERSTR = '\\"$${VERSION}\\"'  # place quotes around the version string
}

DEFINES += VER=\"$${VERSTR}\" # create a VER macro containing the version string

#HEADERS   = videoplayer.h  videoitem.h QVideo.h QVideoBuffer.h QVideoDecoder.h QVideoEncoder.h QResizeDecorator.h QVideoTest.h
#SOURCES   = main.cpp videoplayer.cpp videoitem.cpp QVideo.cpp QVideoBuffer.cpp QVideoDecoder.cpp QVideoEncoder.cpp QResizeDecorator.cpp QVideoTest.cpp

CONFIG += debug

# use OpenGL where available
contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2) {
	QT += opengl
}

FORMS += mainwindow.ui \
	outputsetupdialog.ui \
	AppSettingsDialog.ui \
	DocumentSettingsDialog.ui \
	GridDialog.ui \
	SlideSettingsDialog.ui \
	SlideGroupSettingsDialog.ui \
	ImageImportDialog.ui
	
RESOURCES += dviz.qrc

HEADERS += \
	MainWindow.h \
	MyGraphicsScene.h \
	RenderOpts.h \
	SlideGroupListModel.h \
	SlideEditorWindow.h \
	DocumentListModel.h \
	SlideGroupViewer.h \
	OutputViewer.h \
	OutputSetupDialog.h \
	AppSettings.h \
	AppSettingsDialog.h \
	DocumentSettingsDialog.h \
	GridDialog.h \
	SlideSettingsDialog.h \
	SlideGroupSettingsDialog.h \
	ImageFilters.h \
	SlideItemListModel.h \
	MediaBrowser.h \
	MediaBrowserDialog.h \
	OutputInstance.h \
	OutputControl.h \
	JpegServer.h \
	DeepProgressIndicator.h \
	OutputServer.h \
	ImageImportDialog.h

	
SOURCES += \
	main.cpp \
	MainWindow.cpp \
	MyGraphicsScene.cpp \
	SlideGroupListModel.cpp \
	SlideEditorWindow.cpp \
	DocumentListModel.cpp \
	SlideGroupViewer.cpp \
	OutputViewer.cpp \
	OutputSetupDialog.cpp \
	AppSettings.cpp \
	AppSettingsDialog.cpp \
	DocumentSettingsDialog.cpp \
	GridDialog.cpp \
	SlideSettingsDialog.cpp \
	SlideGroupSettingsDialog.cpp \
	ImageFilters.cpp \
	SlideItemListModel.cpp \
	MediaBrowser.cpp \
	MediaBrowserDialog.cpp \
	OutputInstance.cpp \
	OutputControl.cpp \
	JpegServer.cpp \
	DeepProgressIndicator.cpp \
	OutputServer.cpp \
	ImageImportDialog.cpp
	

QT += core \
	gui \
	svg \
	network \
	xml

unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2
}

win32 {
	INCLUDEPATH += \
		./external/ffmpeg/include/msinttypes \
		./external/ffmpeg/include/libswscale \
		./external/ffmpeg/include/libavutil \
		./external/ffmpeg/include/libavdevice \
		./external/ffmpeg/include/libavformat \
		./external/ffmpeg/include/libavcodec \
		./external/ffmpeg/include
	
	LIBS += -L"./external/ffmpeg/lib" \
		-lavcodec-51 \
		-lavformat-52 \
		-lavutil-49 \
		-lavdevice-52 \
		-lswscale-0
}

include(frames/frames.pri)
include(items/items.pri)
include(model/model.pri)
include(songdb/songdb.pri)
include(itemlistfilters/itemlistfilters.pri)
include(3rdparty/richtextedit/richtextedit.pri)
include(3rdparty/qtgradienteditor/qtgradienteditor.pri)
include(3rdparty/videocapture/videocapture.pri)
include(3rdparty/analyzers/analyzers.pri)
include(qvideo/qvideo.pri)
include(qtcolorpicker/qtcolorpicker.pri)
include(qtmultimedia/audio/audio.pri)
include(3rdparty/rtmidi/rtmidi.pri)
include(3rdparty/md5/md5.pri)
include(3rdparty/qjson/qjson.pri)

# deployment on Linux
unix {
	target.path = /usr/bin
	icon.files = dviz.png
	icon.path = /usr/share/pixmaps
	dfile.files = dviz.desktop
	dfile.path = /usr/share/applications
	man.files = dviz.1
	man.path = /usr/share/man/man1
	INSTALLS += target \
		icon \
		dfile \
		man
}


# static builds
win32|macx {
	contains(CONFIG, static)|contains(CONFIG, qt_no_framework) {
		DEFINES  += STATIC_LINK
		QTPLUGIN += qgif \
			qjpeg \
			qsvg \
			qtiff
	}
}


