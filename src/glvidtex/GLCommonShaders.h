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

static const char *qt_glsl_warpingVertexShaderProgram =
        "attribute highp vec4 vertexCoordArray;\n"
        "attribute highp vec2 textureCoordArray;\n"
        "uniform highp mat4 positionMatrix;\n"
        "uniform highp mat4 warpMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "   gl_Position = positionMatrix * warpMatrix * vertexCoordArray;\n"
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

//#define ENABLE_ATI_CONVOLUTION_KERNELS

// Paints an RGB32 frame with Levels adjustments
static const char *qt_glsl_xrgbLevelsShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "uniform mediump float blackLevel;\n"
        "uniform mediump float whiteLevel;\n"  /// TODO add midLevel adjustments
        "uniform mediump float gamma;\n"
        "varying highp vec2 textureCoord;\n"
        #ifdef ENABLE_ATI_CONVOLUTION_KERNELS
        "#define KERNEL_SIZE 9\n"
	"float kernel[KERNEL_SIZE];\n"
	"uniform float width;\n"
	"uniform float height;\n"
	"float step_w = 1.0/width;\n"
	"float step_h = 1.0/height;\n"
	"vec2 offset[KERNEL_SIZE];\n"
	#endif
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        #ifdef ENABLE_ATI_CONVOLUTION_KERNELS
        "	highp vec4 color = vec4(0.0);\n"
        "	int i = 0;\n"
	"	vec4 sum = vec4(0.0);\n"
	"	\n"
	"	offset[0] = vec2(-step_w, -step_h);\n"
	"	offset[1] = vec2(0.0, -step_h);\n"
	"	offset[2] = vec2(step_w, -step_h);\n"
	"	\n"
	"	offset[3] = vec2(-step_w, 0.0);\n"
	"	offset[4] = vec2(0.0, 0.0);\n"
	"	offset[5] = vec2(step_w, 0.0);\n"
	"	\n"
	"	offset[6] = vec2(-step_w, step_h);\n"
	"	offset[7] = vec2(0.0, step_h);\n"
	"	offset[8] = vec2(step_w, step_h);\n"
	"	\n"
	
	#define KERNEL_SHARP


	#ifdef KERNEL_BLUR
	"	kernel[0] = 1.0/16.0; 	kernel[1] = 2.0/16.0;	kernel[2] = 1.0/16.0;\n"
	"	kernel[3] = 2.0/16.0;	kernel[4] = 4.0/16.0;	kernel[5] = 2.0/16.0;\n"
	"	kernel[6] = 1.0/16.0;  	kernel[7] = 2.0/16.0;	kernel[8] = 1.0/16.0;\n"
	#endif
	#ifdef KERNEL_EMBOSS
	"	kernel[0] = 2.0/16.0; 	kernel[1] = 0.0/16.0;	kernel[2] = 0.0/16.0;\n"
	"	kernel[3] = 0.0/16.0;	kernel[4] = -1.0/16.0;	kernel[5] = 0.0/16.0;\n"
	"	kernel[6] = 0.0/16.0;  	kernel[7] = 0.0/16.0;	kernel[8] = -1.0/16.0;\n"
	#endif
	#ifdef KERNEL_BLOOM
	"	kernel[0] = 5.0/9.0; 	kernel[1] = 0.0/9.0;	kernel[2] = 10.0/9.0;\n"
	"	kernel[3] = 0.0/9.0;	kernel[4] = 10.0/9.0;	kernel[5] = 0.0/9.0;\n"
	"	kernel[6] = 10.0/9.0;  	kernel[7] = 0.0/9.0;	kernel[8] = 5.0/9.0;\n"
	#endif
	#ifdef KERNEL_MEAN
	"	kernel[0] = 1.0/9.0; 	kernel[1] = 1.0/9.0;	kernel[2] = 1.0/9.0;\n"
	"	kernel[3] = 1.0/9.0;	kernel[4] = 1.0/9.0;	kernel[5] = 1.0/9.0;\n"
	"	kernel[6] = 1.0/9.0;  	kernel[7] = 1.0/9.0;	kernel[8] = 1.0/9.0;\n"
	#endif
	#ifdef KERNEL_SHARP
	"	kernel[0] = 0.0; 	kernel[1] = -0.5;	kernel[2] = 0.0;\n"
	"	kernel[3] = -0.5;	kernel[4] =  3.0;	kernel[5] = -0.5;\n"
	"	kernel[6] = 0.0;  	kernel[7] = -0.5;	kernel[8] = 0.0;\n"
	#endif
	#ifdef KERNEL_EDGE
	"	kernel[0] = 0.0/16.0; 	kernel[1] = 1.0/16.0;	kernel[2] = 0.0/16.0;\n"
	"	kernel[3] = 1.0/16.0;	kernel[4] = -4.0/16.0;	kernel[5] = 1.0/16.0;\n"
	"	kernel[6] = 0.0/16.0;  	kernel[7] = 1.0/16.0;	kernel[8] = 0.0/16.0;\n"
	#endif
	"	\n"
	"	if(texPoint.s<0.595) {\n"
	"	for( i=0; i<KERNEL_SIZE; i++ )\n"
	"	{\n"
	//"		vec4 tmp = texture2D(texRgb, texPoint + offset[i]);\n"
	"		vec4 tmp = vec4(texture2D(texRgb, texPoint + offset[i]).bgr, 1.0);\n"
	"		sum += tmp * kernel[i];\n"
	"	}\n"
	"	color = clamp(sum,0.0,1.0);\n"
	"	} else if(texPoint.s>0.605) {\n"
	"		color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
	"	} else {\n"
	"		color = vec4(1.0, 0.0, 0.0, 1.0);\n" 
	"	}\n"
	#else
	"    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
        #endif

//         "    color.r = (pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.g = (pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.b = (pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
        "    color.r = clamp((pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
        "    color.g = clamp((pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
        "    color.b = clamp((pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
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

//
// Paints an ARGB frame with levels adjustment
static const char *qt_glsl_argbLevelsShaderProgram =
	"uniform sampler2D texRgb;\n"
	"uniform sampler2D alphaMask;\n"
	"uniform mediump mat4 colorMatrix;\n"
	"uniform mediump float alpha;\n"
	"uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
	"uniform mediump float blackLevel;\n"
        "uniform mediump float whiteLevel;\n"  /// TODO add midLevel adjustments
        "uniform mediump float gamma;\n"
        "varying highp vec2 textureCoord;\n"
	"void main(void)\n"
	"{\n"
	"    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
	"    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"

/// Works, no clamp
//         "    color.r = (pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.g = (pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.b = (pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"

/// Works, clamped
// 	"    color.r = clamp((pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
//         "    color.g = clamp((pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
//         "    color.b = clamp((pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
        
        //"    mediump float range = whiteLevel - blackLevel;
	"    color = colorMatrix * color;\n"
	"    color.r = clamp(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), 0.0, 1.0);\n"
        "    color.g = clamp(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), 0.0, 1.0);\n"
        "    color.b = clamp(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), 0.0, 1.0);\n"
        
	"    gl_FragColor = vec4(color.rgb, texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
	//"    gl_FragColor = vec4(color.rgb, 1);"//texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
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

// Paints an RGB(A) frame with levels adjustments
static const char *qt_glsl_rgbLevelsShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
	"uniform mediump float blackLevel;\n"
        "uniform mediump float whiteLevel;\n"  /// TODO add midLevel adjustments
        "uniform mediump float gamma;\n"
	"varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(texture2D(texRgb, texPoint).rgb, 1.0);\n"
//         "    color.r = (pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.g = (pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
//         "    color.b = (pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255) / 255.0;\n"
	"    color.r = clamp((pow(((color.r * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
        "    color.g = clamp((pow(((color.g * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
        "    color.b = clamp((pow(((color.b * 255.0) - blackLevel) / (whiteLevel - blackLevel), gamma) * 255.0) / 255.0, 0.0, 1.0);\n"
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

// TODO The following shaders have not been adapted to honor the 'alphaMask' property or 'texture offsets'

// Paints a YUV444 frame.
static const char *qt_glsl_xyuvShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "uniform mediump float alpha;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).gba, alpha);\n"
        "    gl_FragColor = colorMatrix * color;\n"
        "}\n";

// Paints a AYUV444 frame.
static const char *qt_glsl_ayuvShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "uniform mediump float alpha;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).gba, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).r * alpha);\n"
        "}\n";

// Paints a UYVY(YUV422) frame.
static const char *qt_glsl_uyvyShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texC;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "uniform mediump float alpha;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4( texture2D(texY, textureCoord.st).a, 0.0, 0.0, 1.0);\n"
        "    color.gb = texture2D(texC, textureCoord.st).rb;\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha);\n"
        "}\n";

// Paints a YUYV(YUY2) frame.
static const char *qt_glsl_yuyvShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texC;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "uniform mediump float alpha;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4( texture2D(texY, textureCoord.st).r, 0.0, 0.0, 1.0);\n"
        "    color.gb = texture2D(texC, textureCoord.st).ga;\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha);\n"
        "}\n";



#endif
