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

GLfloat x = 1000.0f,y = 1000.0f,ii = 20;


//GLfloat w = 360;
//GLfloat h = 288;
GLfloat w = 320;
GLfloat h = 240;

GLfloat z = 0.1f;

const GLfloat gVerticesSquare[] = {
        -w, -h, 0.0f, // 左下
        w, -h, 0.0f,  // 右下
        -w, h, 0.0f,  // 左上
        w, h, 0.0f    // 右上
};

GLfloat wv = 60;
GLfloat hv = 60;
GLfloat zv = 0.1f;

const GLfloat gVerticesSquareV[] = {
        -wv, -hv, 0.0f, // 左下
        wv, -hv, 0.0f,  // 右下
        -wv, hv, 0.0f,  // 左上
        wv, hv, 0.0f    // 右上
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
    // 纹理设置h
    glEnable(GL_TEXTURE_2D);                                // 启用纹理映射
    glBindTexture(GL_TEXTURE_2D, gTexture0[0]);              // 选择纹理
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);            // 启用纹理坐标数组
    glTranslatef(0.0f,0.0f,-h);                         // 设置矩形形位置
    glVertexPointer(3, GL_FLOAT, 0, gVerticesSquare);       // 指定顶点数组
    glTexCoordPointer(2, GL_FLOAT, 0, gTextureSquareCoord); // 设置纹理坐标
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                  // 绘制正方形

    glTranslatef(x,y,0.0f);
    glBindTexture(GL_TEXTURE_2D, gTexture[0]);              // 选择纹理
    glVertexPointer(3, GL_FLOAT, 0, gVerticesSquareV);       // 指定顶点数组
    glTexCoordPointer(2, GL_FLOAT, 0, gTextureSquareCoord); // 设置纹理坐标
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);                  // 绘制正方形

    // 关闭顶点数组
    glDisableClientState(GL_VERTEX_ARRAY);
    // 关闭纹理数组
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
   // x=x+1;
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
        uint64_t mstart = xiny120::AudioEngine::me()->mstart;
        //LOGE("mstart %llu ",mstart);
        if ((v = gtmvrender::me()->popv()) != NULL && v->len != 0){
            if(v->len > 1) {
                //z-= 5.0f;
                glBindTexture(GL_TEXTURE_2D, gTexture0[0]);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                char path[2048] = {0};
                const char* ppath = crossanyapp::me()->getWritablePath().c_str();
                sprintf(path,"%s/data/%llu.bmp",ppath,mstart);
                //LOGE("image w: %d h: %d",v->width,v->height);
                //save_bmp(path,v->width,v->height,v->data,24);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, v->width, v->height, 0,GL_RGB, GL_UNSIGNED_BYTE, v->data);

                if(ii <= 0) {
                    x = v->width / 2 - ((float) v->width * (rand() / float(RAND_MAX + 1.0)));
                    y = v->height / 2 - ((float) v->height * (rand() / float(RAND_MAX + 1.0)));
                    if (abs(y) > (v->height / 2 - 60)) y = v->height / 2 - 60;
                    if(y > 0.0f) y = 0.0f - y;
                    //LOGI("rand %.2f , %.2f", x, y);
                    ii = 20;
                }
                if(ii <= 8){
                    x = 1000.0f;
                    y = 1000.0f;
                }
                ii--;
            }

            delete[](char*) v;
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
    //return 0;
    genTexture();
    renderFrame();
    return getframes();
}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_init(JNIEnv * env, jobject obj)
{
    srand(time(NULL));
    z = 1.0f;
    init();

}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setTexture(JNIEnv * env, jclass obj, jintArray tex)
{
    gTexture = (GLuint *)env->GetIntArrayElements(tex,0);
}

/********************************************************************
    created:    2012/02/07
    filename:   savebmp.c
    author:

    purpose:
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


//-------------------------------------------------------------------
/*
　　位图文件的组成
　　结构名称 符 号
　　位图文件头 (bitmap-file header) BITMAPFILEHEADER bmfh
　　位图信息头 (bitmap-information header) BITMAPINFOHEADER bmih
　　彩色表　(color table) RGBQUAD aColors[]
　　图象数据阵列字节 BYTE aBitmapBits[]
*/
typedef struct bmp_header
{
    short twobyte           ;//两个字节，用来保证下面成员紧凑排列，这两个字符不能写到文件中
    //14B
    char bfType[2]          ;//!文件的类型,该值必需是0x4D42，也就是字符'BM'
    unsigned int bfSize     ;//!说明文件的大小，用字节为单位
    unsigned int bfReserved1;//保留，必须设置为0
    unsigned int bfOffBits  ;//!说明从文件头开始到实际的图象数据之间的字节的偏移量，这里为14B+sizeof(BMPINFO)
}BMPHEADER;

typedef struct bmp_info
{
    //40B
    unsigned int biSize         ;//!BMPINFO结构所需要的字数
    int biWidth                 ;//!图象的宽度，以象素为单位
    int biHeight                ;//!图象的宽度，以象素为单位,如果该值是正数，说明图像是倒向的，如果该值是负数，则是正向的
    unsigned short biPlanes     ;//!目标设备说明位面数，其值将总是被设为1
    unsigned short biBitCount   ;//!比特数/象素，其值为1、4、8、16、24、或32
    unsigned int biCompression  ;//说明图象数据压缩的类型
#define BI_RGB        0L    //没有压缩
#define BI_RLE8       1L    //每个象素8比特的RLE压缩编码，压缩格式由2字节组成（重复象素计数和颜色索引）；
#define BI_RLE4       2L    //每个象素4比特的RLE压缩编码，压缩格式由2字节组成
#define BI_BITFIELDS  3L    //每个象素的比特由指定的掩码决定。
    unsigned int biSizeImage    ;//图象的大小，以字节为单位。当用BI_RGB格式时，可设置为0
    int biXPelsPerMeter         ;//水平分辨率，用象素/米表示
    int biYPelsPerMeter         ;//垂直分辨率，用象素/米表示
    unsigned int biClrUsed      ;//位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
    unsigned int biClrImportant ;//对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。
}BMPINFO;

typedef struct tagRGBQUAD {
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
    BMPINFO    bmiHeader;
    //RGBQUAD    bmiColors[1];
    unsigned int rgb[3];
} BITMAPINFO;

static int get_rgb888_header(int w, int h, BMPHEADER * head, BMPINFO * info)
{
    int size = 0;
    if (head && info) {
        size = w * h * 3;
        memset(head, 0, sizeof(* head));
        memset(info, 0, sizeof(* info));
        head->bfType[0] = 'B';
        head->bfType[1] = 'M';
        head->bfOffBits = 14 + sizeof(* info);
        head->bfSize = head->bfOffBits + size;
        head->bfSize = (head->bfSize + 3) & ~3;//windows要求文件大小必须是4的倍数
        size = head->bfSize - head->bfOffBits;

        info->biSize = sizeof(BMPINFO);
        info->biWidth = w;
        info->biHeight = -h;
        info->biPlanes = 1;
        info->biBitCount = 24;
        info->biCompression = BI_RGB;
        info->biSizeImage = size;

        printf("rgb888:%dbit,%d*%d,%d\n", info->biBitCount, w, h, head->bfSize);
    }
    return size;
}

static int get_rgb565_header(int w, int h, BMPHEADER * head, BITMAPINFO * info)
{
    int size = 0;
    if (head && info) {
        size = w * h * 2;
        memset(head, 0, sizeof(* head));
        memset(info, 0, sizeof(* info));
        head->bfType[0] = 'B';
        head->bfType[1] = 'M';
        head->bfOffBits = 14 + sizeof(* info);
        head->bfSize = head->bfOffBits + size;
        head->bfSize = (head->bfSize + 3) & ~3;
        size = head->bfSize - head->bfOffBits;

        info->bmiHeader.biSize = sizeof(info->bmiHeader);
        info->bmiHeader.biWidth = w;
        info->bmiHeader.biHeight = -h;
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 16;
        info->bmiHeader.biCompression = BI_BITFIELDS;
        info->bmiHeader.biSizeImage = size;

        info->rgb[0] = 0xF800;
        info->rgb[1] = 0x07E0;
        info->rgb[2] = 0x001F;

        printf("rgb565:%dbit,%d*%d,%d\n", info->bmiHeader.biBitCount, w, h, head->bfSize);
    }
    return size;
}

static int save_bmp_rgb565(FILE * hfile, int w, int h, void * pdata)
{
    int success = 0;
    int size = 0;
    BMPHEADER head;
    BITMAPINFO info;

    size = get_rgb565_header(w, h, &head, &info);
    if (size > 0) {
        fwrite(head.bfType, 1, 14, hfile);
        fwrite(&info, 1, sizeof(info), hfile);
        fwrite(pdata, 1, size, hfile);
        success = 1;
    }

    return success;
}

static int save_bmp_rgb888(FILE * hfile, int w, int h, void * pdata)
{
    int success = 0;
    int size = 0;
    BMPHEADER head;
    BMPINFO info;

    size = get_rgb888_header(w, h, &head, &info);
    if (size > 0) {
        fwrite(head.bfType, 1, 14, hfile);
        fwrite(&info, 1, sizeof(info), hfile);
        LOGE("w:%d h:%d size:%d",w,h,size);
        fwrite(pdata, 1, size, hfile);
        success = 1;
    }

    return success;
}

int save_bmp(const char * path, int w, int h, void * pdata, int bpp)
{
    int success = 0;
    FILE * hfile = NULL;

    do
    {
        if (path == NULL || w <= 0 || h <= 0 || pdata == NULL) {
            printf("if (path == NULL || w <= 0 || h <= 0 || pdata == NULL)\n");
            break;
        }

        remove(path);
        hfile = fopen(path, "wb");
        if (hfile == NULL) {
            printf("open(%s) failed!\n", path);
            break;
        }

        switch (bpp)
        {
            case 16:
                success = save_bmp_rgb565(hfile, w, h, pdata);
                break;
            case 24:
                success = save_bmp_rgb888(hfile, w, h, pdata);
                break;
            default:
                printf("error: not support format!\n");
                success = 0;
                break;
        }
    } while (0);

    if (hfile != NULL)
        fclose(hfile);

    return success;
}

//-------------------------------------------------------------------