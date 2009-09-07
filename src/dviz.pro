contains(QT_CONFIG, opengl): QT += opengl

#HEADERS   = videoplayer.h  videoitem.h QVideo.h QVideoBuffer.h QVideoDecoder.h QVideoEncoder.h QResizeDecorator.h QVideoTest.h
#SOURCES   = main.cpp videoplayer.cpp videoitem.cpp QVideo.cpp QVideoBuffer.cpp QVideoDecoder.cpp QVideoEncoder.cpp QResizeDecorator.cpp QVideoTest.cpp
HEADERS += \
	dviz.h
	
SOURCES += \
	dviz.cpp \
	main.cpp


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


# # install
# target.path = $$[QT_INSTALL_EXAMPLES]/video/videographicsitem
# sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
# sources.path = $$[QT_INSTALL_EXAMPLES]/video/videographicsitem
# INSTALLS += target sources
