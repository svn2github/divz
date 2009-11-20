
TARGET = dviz-viewer
TEMPLATE = app
DEPENDPATH += $$PWD ../
INCLUDEPATH += $$PWD ../
VPATH += ../

CONFIG += debug


win32 {
	INCLUDEPATH += \
		../external/ffmpeg/include/msinttypes \
		../external/ffmpeg/include/libswscale \
		../external/ffmpeg/include/libavutil \
		../external/ffmpeg/include/libavdevice \
		../external/ffmpeg/include/libavformat \
		../external/ffmpeg/include/libavcodec \
		../external/ffmpeg/include
	
	LIBS += -L"../external/ffmpeg/lib" \
		-lavcodec-51 \
		-lavformat-52 \
		-lavutil-49 \
		-lavdevice-52 \
		-lswscale-0
}


include(../frames/frames.pri)
include(../items/items.pri)
include(../model/model.pri)
include(../songdb/songdb.pri)
include(../itemlistfilters/itemlistfilters.pri)
include(../3rdparty/richtextedit/richtextedit.pri)
include(../3rdparty/qtgradienteditor/qtgradienteditor.pri)
include(../3rdparty/videocapture/videocapture.pri)
include(../3rdparty/analyzers/analyzers.pri)
include(../qvideo/qvideo.pri)
include(../qtcolorpicker/qtcolorpicker.pri)
include(../qtmultimedia/audio/audio.pri)
include(../3rdparty/rtmidi/rtmidi.pri)
include(../3rdparty/md5/md5.pri)
include(../3rdparty/qjson/qjson.pri)
include(../ppt/ppt.pri)
include(../groupplayer/groupplayer.pri)

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

# Input

FORMS += MainWindow.ui \
	outputsetupdialog.ui \
	SingleOutputSetupDialog.ui \
	AppSettingsDialog.ui \
	DocumentSettingsDialog.ui \
	GridDialog.ui \
	SlideSettingsDialog.ui \
	SlideGroupSettingsDialog.ui \
	ConnectDialog.ui
	
RESOURCES += dviz.qrc

HEADERS += \
	MainWindow.h \
	NetworkClient.h \
	MyGraphicsScene.h \
	RenderOpts.h \
	SlideGroupListModel.h \
	SlideEditorWindow.h \
	DocumentListModel.h \
	SlideGroupViewer.h \
	OutputSetupDialog.h \
	SingleOutputSetupDialog.h \
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
	ConnectDialog.h \
	OutputServer.h \
	DirectoryListModel.h
	
SOURCES += main.cpp \
	MainWindow.cpp \
	ConnectDialog.cpp \
	NetworkClient.cpp \
	MyGraphicsScene.cpp \
	SlideGroupListModel.cpp \
	SlideEditorWindow.cpp \
	DocumentListModel.cpp \
	SlideGroupViewer.cpp \
	OutputViewer.cpp \
	OutputSetupDialog.cpp \
	SingleOutputSetupDialog.cpp \
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
	DirectoryListModel.cpp
	

QT += core \
	gui \
	svg \
	network \
	xml \
	sql \
	opengl

unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2
}


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


