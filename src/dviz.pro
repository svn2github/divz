
TEMPLATE = app
TARGET = dviz

INCLUDEPATH += .
DEPENDPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

QT += sql

# Disable till I find a way to test QT Version here
#if(QT_VERSION >= 0x040600)
#{
#	QT += phonon
#	DEFINES += PHONON_ENABLED
#}

BUILDNUM = $$system(perl buildcount.pl -v)
VERSTR = '\\"$${BUILDNUM}\\"'  # place quotes around the version string

unix {
	VERSION  = "0.9.5"
	VERSION = "$${VERSION}-b$${BUILDNUM}"
	SVNREV   = $$system(svn info -r HEAD . | grep Changed\ Rev | cut -b 19-)
	
	!isEmpty(SVNREV) {
		VERSION = "$${VERSION}-r$${SVNREV}"
	}
	VERSTR = '\\"$${VERSION}\\"'  # place quotes around the version string

	# On Centos 5.2 and up, it seems the FC version has an incompatible symbol with Qt. 
	# Following the instructions at the following URL puts an updated FC version in
	# /opt/fontconfig... - here we include the lib dir just incase. This should not affect
	# anything on other Unix versions.
	# http://theitdepartment.wordpress.com/2009/03/15/centos-qt-fcfreetypequeryface/
	LIBS += -L/opt/fontconfig-2.4.2/lib/
}

DEFINES += VER=\"$${VERSTR}\" # create a VER macro containing the version string

#HEADERS   = videoplayer.h  videoitem.h QVideo.h QVideoBuffer.h QVideoDecoder.h QVideoEncoder.h QResizeDecorator.h QVideoTest.h
#SOURCES   = main.cpp videoplayer.cpp videoitem.cpp QVideo.cpp QVideoBuffer.cpp QVideoDecoder.cpp QVideoEncoder.cpp QResizeDecorator.cpp QVideoTest.cpp

#CONFIG += debug

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
	ImageImportDialog.ui \
	ImportGroupDialog.ui
	
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
	camera_test/CameraServer.h \
	DeepProgressIndicator.h \
	OutputServer.h \
	ImageImportDialog.h \
	DirectoryListModel.h \
	ImportGroupDialog.h \
	MimeTypes.h \
	SharedMemoryImageWriter.h

	
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
	camera_test/CameraServer.cpp \
	DeepProgressIndicator.cpp \
	OutputServer.cpp \
	ImageImportDialog.cpp \
	DirectoryListModel.cpp \
	ImportGroupDialog.cpp \
	MimeTypes.cpp \
	SharedMemoryImageWriter.cpp
	

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
		$$PWD/external/ffmpeg/include/msinttypes \
		$$PWD/external/ffmpeg/include/libswscale \
		$$PWD/external/ffmpeg/include/libavutil \
		$$PWD/external/ffmpeg/include/libavdevice \
		$$PWD/external/ffmpeg/include/libavformat \
		$$PWD/external/ffmpeg/include/libavcodec \
		$$PWD/external/ffmpeg/include
	
	LIBS += -L"$$PWD/external/ffmpeg/lib" \
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
#include(qtmultimedia/audio/audio.pri)
include(camera/camera.pri)
include(3rdparty/rtmidi/rtmidi.pri)
include(3rdparty/md5/md5.pri)
include(3rdparty/qjson/qjson.pri)
include(ppt/ppt.pri)
include(phonon/phonon.pri)
include(http/http.pri)
include(groupplayer/groupplayer.pri)
include(bible/bible.pri)
include(viewer/client.pri)
include(imgtool/exiv2-0.18.2-qtbuild/qt_build_root.pri)
include(webgroup/webgroup.pri)


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
	include(3rdparty/qtdotnetstyle-2.3_1/qtdotnetstyle.pri)
	contains(CONFIG, static)|contains(CONFIG, qt_no_framework) {
		DEFINES  += STATIC_LINK
		QTPLUGIN += qgif \
			qjpeg \
			qsvg \
			qtiff
	}
}



