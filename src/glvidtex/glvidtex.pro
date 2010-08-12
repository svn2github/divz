
MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

TARGET = glvidtex



HEADERS       = GLWidget.h \
		../livemix/VideoSource.h \
		../livemix/VideoThread.h \
		../livemix/VideoFrame.h \
		../livemix/CameraThread.h
		
SOURCES       = GLWidget.cpp \
		main.cpp \
		../livemix/VideoSource.cpp \
		../livemix/VideoThread.cpp \
		../livemix/VideoFrame.cpp \
		../livemix/CameraThread.cpp \
		../livemix/SimpleV4L2.cpp

RESOURCES     = glvidtex.qrc
QT           += opengl multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS textures.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)


unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2 
}

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

