
TEMPLATE = app
TARGET = dviz

INCLUDEPATH += .
DEPENDPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build


#HEADERS   = videoplayer.h  videoitem.h QVideo.h QVideoBuffer.h QVideoDecoder.h QVideoEncoder.h QResizeDecorator.h QVideoTest.h
#SOURCES   = main.cpp videoplayer.cpp videoitem.cpp QVideo.cpp QVideoBuffer.cpp QVideoDecoder.cpp QVideoEncoder.cpp QResizeDecorator.cpp QVideoTest.cpp

# use OpenGL where available
contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2) {
    QT += opengl
}

RESOURCES += dviz.qrc

HEADERS += \
	MainWindow.h \
	MyGraphicsScene.h \
	RenderOpts.h 
	
SOURCES += \
	MainWindow.cpp \
	MyGraphicsScene.cpp \
	main.cpp

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
include(3rdparty/richtextedit/richtextedit.pri)
include(3rdparty/videocapture/videocapture.pri)
include(3rdparty/posterazor/posterazor.pri)

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
        DEFINES += STATIC_LINK
        QTPLUGIN += qgif \
            qjpeg \
            qsvg \
            qtiff
    }
}
