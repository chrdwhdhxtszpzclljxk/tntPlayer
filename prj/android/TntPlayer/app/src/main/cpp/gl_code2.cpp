//
// Created by Administrator on 2017-11-24.
//

#include "gl_code2.h"
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author:  mnorst@foxmail.com
 * created: 2014/10/20
 * purpose: 纹理的使用
 */

// OpenGL ES 1.x code

#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "native-lib.h"
#include "gtmvreader.h"
#include "AudioEngine.h"

/************************************************************************/
/*                             定义                                     */
/************************************************************************/

#define  LOG_TAG    "libgljni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//初始化纹理数组
//GLuint gTexture [10] = {0};
GLuint* gTexture = 0;
GLuint gTexture0[10] = {0};

// 定义π
const GLfloat PI = 3.1415f;

// 旋转角度
static GLfloat gAngle = 0.0f;

GLfloat w = 360;
GLfloat h = 288;
GLfloat z = 0.1f;

const GLfloat gVerticesSquare[] = {
        -w, -h, 0.0f, // 左下
        w, -h, 0.0f,  // 右下
        -w, h, 0.0f,  // 左上
        w, h, 0.0f    // 右上
};

//（0,1)（左上角）、(1,1)(右上角)、(1,0)(右下角)、（0，0）(左下角）
const GLfloat gTextureSquareCoord[] = {
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
};
/************************************************************************/
/*                             C++代码                                  */
/************************************************************************/

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

bool init() {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    // 启用阴影平滑
    glShadeModel(GL_SMOOTH);
    // 黑色背景
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // 设置深度缓存
    glClearDepthf(1.0f);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 所作深度测试的类型
    glDepthFunc(GL_LEQUAL);
    // 对透视进行修正
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glGenTextures(1, gTexture0); //Generate  the Y, U and V texture
    return true;
}

static void _gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{

    GLfloat top = zNear * ((GLfloat) tan(fovy * PI / 360.0));
    GLfloat bottom = -top;
    GLfloat left = bottom * aspect;
    GLfloat right = top * aspect;
    LOGI("%f,%f,%f,%f",top,bottom,left,right);
    //glFrustumf(left, right, bottom, top, zNear, zFar);
    glFrustumf(-aspect, aspect, -1, 1, zNear, zFar);
}

void resize(int width, int height){
    // 防止被零除
    if (height==0){
        height=1;
    }

    // 重置当前的视口
    glViewport(0, 0, width, height);
    // 选择投影矩阵
    glMatrixMode(GL_PROJECTION);
    // 重置投影矩阵
    glLoadIdentity();
    // 设置视口的大小
    _gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);
    // 选择模型观察矩阵
    glMatrixMode(GL_MODELVIEW);
    // 重置模型观察矩阵
    glLoadIdentity();
}

void renderFrame() {
    //LOGI("renderFrame");
    // 清除屏幕及深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置背景颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // 重置当前的模型观察矩阵
    glLoadIdentity();
    // 启用顶点数组
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // 纹理设置
    glEnable(GL_TEXTURE_2D);                                // 启用纹理映射
    glBindTexture(GL_TEXTURE_2D, gTexture0[0]);              // 选择纹理
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);            // 启用纹理坐标数组

    glTranslatef(0.0f,0.0f,-288.0f);                         // 设置矩形形位置
    glVertexPointer(3, GL_FLOAT, 0, gVerticesSquare);       // 指定顶点数组
    glTexCoordPointer(2, GL_FLOAT, 0, gTextureSquareCoord); // 设置纹理坐标
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                  // 绘制正方形

    // 关闭顶点数组
    glDisableClientState(GL_VERTEX_ARRAY);
    // 关闭纹理数组
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

}

int64_t getframes(){
    int64_t ret = 0;
    int64_t base = 1000000;
    int64_t curf = gtmvrender::me()->curframes;
    int64_t totalf = gtmvrender::me()->frames;
    //LOGI("draw (%d/%d)" , curf,totalf);
    ret = totalf * base + curf;
    return ret;
}

void genTexture(){
    //LOGI("genTexture");

    //glActiveTexture(name);
    xiny120::GtmvData* v;
    while (gtmvrender::me()->touchv() <= xiny120::AudioEngine::me()->mstart){
        if ((v = gtmvrender::me()->popv()) != NULL && v->len != 0){
            if(v->len > 1) {
                //z-= 5.0f;

                glBindTexture(GL_TEXTURE_2D, gTexture0[0]);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, v->width, v->height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, v->data);
            }

            delete[](char*) v;
            //lv->step();
        }else{
            if (v != NULL && v->len == 0) {

            }
            if (v != NULL) delete v;
            break;
        }
    }

}

/************************************************************************/
/*                          JNI代码                                     */
/************************************************************************/


JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_resize(JNIEnv * env, jobject obj,  jint width, jint height)
{
    resize(width, height);
}

JNIEXPORT jlong JNICALL Java_com_gwgz_tntplayer_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    genTexture();
    renderFrame();
    return getframes();
}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_init(JNIEnv * env, jobject obj)
{
    z = 1.0f;
    init();

}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setTexture(JNIEnv * env, jclass obj, jintArray tex)
{
    gTexture = (GLuint *)env->GetIntArrayElements(tex,0);
}