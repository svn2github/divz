#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QVideoSurfaceFormat>
#include <QGLShaderProgram>

class QGLShaderProgram;

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0, QGLWidget *shareWidget = 0);
	~GLWidget();
	
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void rotateBy(int xAngle, int yAngle, int zAngle);
	void setClearColor(const QColor &color);

signals:
	void clicked();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	
	void initYv12TextureInfo(const QSize &size);
	void initYuv420PTextureInfo(const QSize &size);
	void initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size);
        void updateColors(int brightness, int contrast, int hue, int saturation);

private:
	void makeObject();
	
	QColor clearColor;
	QPoint lastPos;
	int xRot;
	int yRot;
	int zRot;
	GLuint textures[6];
	QVector<QVector3D> vertices;
	QVector<QVector2D> texCoords;
	//#ifdef QT_OPENGL_ES_2
	QGLShaderProgram m_program;
	// #endif
	
	
	QList<QVideoFrame::PixelFormat> m_imagePixelFormats;
	QList<QVideoFrame::PixelFormat> m_glPixelFormats;
	QMatrix4x4 m_colorMatrix;
	QVideoFrame m_frame;
	
	//QGLContext *m_context;
	//QAbstractVideoBuffer::HandleType m_handleType;
	QVideoSurfaceFormat::Direction m_scanLineDirection;
	GLenum m_textureFormat;
	GLuint m_textureInternalFormat;
	GLenum m_textureType;
	
	int m_textureCount;
	
	GLuint m_textureIds[3];
	int m_textureWidths[3];
	int m_textureHeights[3];
	int m_textureOffsets[3];
	
	bool m_yuv;
	
	bool m_colorsDirty;
	
	int m_brightness;
	int m_contrast;
	int m_hue;
	int m_saturation;
	
	QImage m_sampleTexture;
	
	QSize m_frameSize;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
};

#endif
