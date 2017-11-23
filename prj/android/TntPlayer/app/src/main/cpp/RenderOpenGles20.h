//
// Created by Administrator on 2017-11-22.
//

#ifndef TNTPLAYER_RENDEROPENGLES20_H
#define TNTPLAYER_RENDEROPENGLES20_H
#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <assert.h>
#include <android/log.h>
#include <cstdlib>
#include <cstring>
#include <GLES2/gl2.h>

class RenderOpenGles20 {
public:
    static const char g_indices[];
    static const char g_vertextShader[];
    static const char g_fragmentShader[];
    RenderOpenGles20();
    ~RenderOpenGles20();
    int _id, _textureWidth,_textureHeight;
    GLfloat _vertices[100];
    GLuint _textureIds[100];
    GLuint _program;
    int32_t Setup(int32_t width, int32_t height);
    int32_t SetCoordinates(int32_t zOrder,
                                             const float left,
                                             const float top,
                                             const float right,
                                             const float bottom);
    GLuint loadShader(GLenum shaderType, const char* pSource);
    GLuint createProgram(const char* pVertexSource,
                                           const char* pFragmentSource);
    void printGLString(const char *name, GLenum s);
    void checkGlError(const char* op);
    int32_t Render(void * data, int32_t widht, int32_t height);
    void SetupTextures(int32_t width, int32_t height);
    void UpdateTextures(void* data, int32_t widht, int32_t height);


};


#endif //TNTPLAYER_RENDEROPENGLES20_H
