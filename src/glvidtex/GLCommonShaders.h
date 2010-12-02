#ifndef GL_COMMON_SHADERS_H
#define GL_COMMON_SHADERS_H

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
	
	#ifndef Q_WS_MAC
	# ifndef APIENTRYP
	#   ifdef APIENTRY
	#     define APIENTRYP APIENTRY *
	#   else
	#     define APIENTRY
	#     define APIENTRYP *
	#   endif
	# endif
	#else
	# define APIENTRY
	# define APIENTRYP *
	#endif
	
	#ifndef GL_TEXTURE0
	#  define GL_TEXTURE0    0x84C0
	#  define GL_TEXTURE1    0x84C1
	#  define GL_TEXTURE2    0x84C2
	#  define GL_TEXTURE3    0x84C3
	#endif
	#ifndef GL_PROGRAM_ERROR_STRING_ARB
	#  define GL_PROGRAM_ERROR_STRING_ARB       0x8874
	#endif
	
	#ifndef GL_UNSIGNED_SHORT_5_6_5
	#  define GL_UNSIGNED_SHORT_5_6_5 33635
	#endif

#endif

static const char *qt_glsl_vertexShaderProgram =
        "attribute highp vec4 vertexCoordArray;\n"
        "attribute highp vec2 textureCoordArray;\n"
        "uniform highp mat4 positionMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "   gl_Position = positionMatrix * vertexCoordArray;\n"
        "   textureCoord = textureCoordArray;\n"
        "}\n";

// Paints an RGB32 frame
static const char *qt_glsl_xrgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";

//
// Paints an ARGB frame.
static const char *qt_glsl_argbShaderProgram =
	"uniform sampler2D texRgb;\n"
	"uniform sampler2D alphaMask;\n"
	"uniform mediump mat4 colorMatrix;\n"
	"uniform mediump float alpha;\n"
	"uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
	"varying highp vec2 textureCoord;\n"
	"void main(void)\n"
	"{\n"
	"    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
	"    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
	"    color = colorMatrix * color;\n"
	"    gl_FragColor = vec4(color.rgb, texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
	"}\n";

// Paints an RGB(A) frame.
static const char *qt_glsl_rgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(texture2D(texRgb, texPoint).rgb, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";

// Paints a YUV420P or YV12 frame.
static const char *qt_glsl_yuvPlanarShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texU;\n"
        "uniform sampler2D texV;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(\n"
        "           texture2D(texY, texPoint).r,\n"
        "           texture2D(texU, texPoint).r,\n"
        "           texture2D(texV, texPoint).r,\n"
        "           1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";


#endif
