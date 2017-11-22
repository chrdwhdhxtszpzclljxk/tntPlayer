#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <assert.h>
#include <android/log.h>
#include <cstdlib>
#include <cstring>
#include "httpsdownloader.h"
#include "crossanyapp.h"
#include "gtmvreader.h"
#include "AudioEngine.h"
#include "RenderOpenGles20.h"

char* jstringTostring(JNIEnv* env, jstring jstr);
std::string jstring2string(JNIEnv* env,jstring jstr);

extern "C" JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_MainActy_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_MainActy_initNdkApp(JNIEnv *env, jobject /* this */) {
    gtmvreader::me()->start();
    return 1;
}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_setAssetManager(JNIEnv *env, jobject /* this */, jobject assetManager) {
    crossanyapp::me()->setAm(AAssetManager_fromJava( env, assetManager ));
}


extern "C" JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_MainActy_gtmvreaderPush(JNIEnv *env, jobject /* this */,jstring type,jstring tnow ,jstring pubid,jlong filelen) {
    char* ptype = jstringTostring(env,type);
    char* ptnow = jstringTostring(env,tnow);
    char* ppubid = jstringTostring(env,pubid);
    gtmvreader::me()->push(ptype,ptnow,ppubid,filelen);
    delete ptype;
    delete ptnow;
    delete ppubid;
    return 1;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_MainActy_getVideowh(JNIEnv *env, jobject /* this */) {
    char buf[1024] = {0};
    int w,h;
    gtmvrender::me()->getwh(w,h);
    sprintf(buf,"{\"w\":%d,\"h\":%d}",w,h);
    return env->NewStringUTF(buf);
}

bool go(uint32_t a, uint32_t b){
    return false;
}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_getVideoFrame(JNIEnv *env, jobject /* this */,jobject bitmap) {
    xiny120::GtmvData* v;

    AndroidBitmapInfo info;
    void *pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        return;
    }

    while (gtmvrender::me()->touchv() <= xiny120::AudioEngine::me()->mstart){
        if ((v = gtmvrender::me()->popv()) != NULL && v->len != 0){
            void * pixels = NULL;
            int res = AndroidBitmap_lockPixels(env, bitmap, &pixels);
            memmove(pixels,v->data,v->width * v->height * 4);
            AndroidBitmap_unlockPixels(env, bitmap);
            delete[](char*) v;
            //lv->step();
        }else{
            if (v != NULL && v->len == 0) {

            }
            if (v != NULL) delete v;
            break;
        }
    }
    //return env->NewStringUTF("");
}


extern "C" JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_MainActy_getVideoFrame1(JNIEnv *env, jobject /* this */,jobject buf,jint len) {
    xiny120::GtmvData* v;
    bool ret = false;
    while (gtmvrender::me()->touchv() <= xiny120::AudioEngine::me()->mstart){
        if ((v = gtmvrender::me()->popv()) != NULL && v->len != 0){
            unsigned char * pbuf = (unsigned char* )env->GetDirectBufferAddress(buf);
            if(pbuf != nullptr) {
                __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","memmove %d %d",len,v->len);
                gtmvreader::YV12_to_RGB32(pbuf,v->data,v->width,v->height);
                //memmove(pbuf, v->data, len);
            }
            delete[](char*) v;
            ret = true;
            //lv->step();
        }else{
            if (v != NULL && v->len == 0) {

            }
            if (v != NULL) delete v;
            break;
        }
    }
    return ret;
    //return env->NewStringUTF("");
}


extern "C" JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_MainActy_setVideoSurface(JNIEnv *env, jobject /* this */,jobject jsurface) {
    /*
    surface = android_view_Surface_getSurface(env, jsurface);
    if(android::Surface::isValid(surface)){
        ALOGE("surface is valid ");
    }else {
        ALOGE("surface is invalid ");
        return false;
    }
    ALOGE("[%s][%d]\n",__FILE__,__LINE__);*/
    return true;

}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_audioEngineStop(JNIEnv *env, jobject /* this */) {
    gtmvreader::me()->setbreaknow();

}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_setUnPwd(JNIEnv *env, jobject /* this */,jstring un,jstring pwd,jstring userid) {
    crossanyapp::me()->setun(jstring2string(env,un));
    crossanyapp::me()->setPwd(jstring2string(env,pwd));
    crossanyapp::me()->setUserid(jstring2string(env,userid));
}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_setWritablePath(JNIEnv *env, jobject /* this */,jstring path) {
    crossanyapp::me()->setWritablePath(jstring2string(env,path));
}

std::string jstring2string(JNIEnv* env,jstring jstr) {
    if (jstr == nullptr) {
        return "";
    }
    if (!env) {
        return nullptr;
    }

    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);

    return ret;
}

//jstring to char*
char* jstringTostring(JNIEnv* env, jstring jstr){
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = new char[alen + 1];
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


class AndroidNativeOpenGl2Channel
{
public:
    AndroidNativeOpenGl2Channel(JavaVM* jvm,
                                void* window)
    {
        _jvm = jvm;
        _ptrWindow = window;
        _buffer = (uint8_t*)malloc(1024000);
    }

    ~AndroidNativeOpenGl2Channel()
    {
        if (_jvm)
        {
            bool isAttached = false;
            JNIEnv* env = NULL;
            if (_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
                // try to attach the thread and get the env
                // Attach this thread to JVM
                jint res = _jvm->AttachCurrentThread(&env, NULL);

                // Get the JNI env for this thread
                if ((res < 0) || !env) {
                  //  WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
                 //                "%s: Could not attach thread to JVM (%d, %p)",
                   //              __FUNCTION__, res, env);
                    env = NULL;
                } else {
                    isAttached = true;
                }
            }

            if (env && _deRegisterNativeCID) {
                env->CallVoidMethod(_javaRenderObj, _deRegisterNativeCID);
            }

            env->DeleteGlobalRef(_javaRenderObj);
            env->DeleteGlobalRef(_javaRenderClass);

            if (isAttached) {
                if (_jvm->DetachCurrentThread() < 0) {
                    //WEBRTC_TRACE(kTraceWarning, kTraceVideoRenderer, _id,
                     //            "%s: Could not detach thread from JVM",
                     //            __FUNCTION__);
                }
            }
        }

        free(_buffer);
    }

    int32_t Init()
    {
        if (!_ptrWindow)
        {
         //   WEBRTC_TRACE(kTraceWarning, kTraceVideoRenderer, _id,
          //               "(%s): No window have been provided.", __FUNCTION__);
            return -1;
        }

        if (!_jvm)
        {
          //  WEBRTC_TRACE(kTraceWarning, kTraceVideoRenderer, _id,
          //               "(%s): No JavaVM have been provided.", __FUNCTION__);
            return -1;
        }

        // get the JNI env for this thread
        bool isAttached = false;
        JNIEnv* env = NULL;
        if (_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
            // try to attach the thread and get the env
            // Attach this thread to JVM
            jint res = _jvm->AttachCurrentThread(&env, NULL);

            // Get the JNI env for this thread
            if ((res < 0) || !env) {
             //   WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
              //               "%s: Could not attach thread to JVM (%d, %p)",
               //              __FUNCTION__, res, env);
                return -1;
            }
            isAttached = true;
        }

        // get the ViEAndroidGLES20 class
        jclass javaRenderClassLocal = reinterpret_cast<jclass> (env->FindClass("hzcw/opengl/ViEAndroidGLES20"));
        if (!javaRenderClassLocal) {
          //  WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
          //               "%s: could not find ViEAndroidGLES20", __FUNCTION__);
            return -1;
        }

        _javaRenderClass = reinterpret_cast<jclass> (env->NewGlobalRef(javaRenderClassLocal));
        if (!_javaRenderClass) {
           // WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
           //              "%s: could not create Java SurfaceHolder class reference",
           //              __FUNCTION__);
            return -1;
        }

        // Delete local class ref, we only use the global ref
        env->DeleteLocalRef(javaRenderClassLocal);
        jmethodID cidUseOpenGL = env->GetStaticMethodID(_javaRenderClass,
                                                        "UseOpenGL2",
                                                        "(Ljava/lang/Object;)Z");
        if (cidUseOpenGL == NULL) {
           // WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, -1,
           //              "%s: could not get UseOpenGL ID", __FUNCTION__);
            return false;
        }
        jboolean res = env->CallStaticBooleanMethod(_javaRenderClass,
                                                    cidUseOpenGL, (jobject) _ptrWindow);

        // create a reference to the object (to tell JNI that we are referencing it
        // after this function has returned)
        _javaRenderObj = reinterpret_cast<jobject> (env->NewGlobalRef((jobject)_ptrWindow));
        if (!_javaRenderObj)
        {
          //  WEBRTC_TRACE(
          //          kTraceError,
          //          kTraceVideoRenderer,
          //          _id,
           //         "%s: could not create Java SurfaceRender object reference",
            //        __FUNCTION__);
            return -1;
        }

        // get the method ID for the ReDraw function
        _redrawCid = env->GetMethodID(_javaRenderClass, "ReDraw", "()V");
        if (_redrawCid == NULL) {
         //   WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
         //                "%s: could not get ReDraw ID", __FUNCTION__);
            return -1;
        }

        _registerNativeCID = env->GetMethodID(_javaRenderClass,
                                              "RegisterNativeObject", "(J)V");
        if (_registerNativeCID == NULL) {
           // WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
            //             "%s: could not get RegisterNativeObject ID", __FUNCTION__);
            return -1;
        }

        _deRegisterNativeCID = env->GetMethodID(_javaRenderClass,
                                                "DeRegisterNativeObject", "()V");
        if (_deRegisterNativeCID == NULL) {
           // WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
           //              "%s: could not get DeRegisterNativeObject ID",
           //              __FUNCTION__);
            return -1;
        }

        JNINativeMethod nativeFunctions[2] = {
                { "DrawNative",
                        "(J)V",
                        (void*) &AndroidNativeOpenGl2Channel::DrawNativeStatic, },
                { "CreateOpenGLNative",
                        "(JII)I",
                        (void*) &AndroidNativeOpenGl2Channel::CreateOpenGLNativeStatic },
        };
        if (env->RegisterNatives(_javaRenderClass, nativeFunctions, 2) == 0) {
          //  WEBRTC_TRACE(kTraceDebug, kTraceVideoRenderer, -1,
           //              "%s: Registered native functions", __FUNCTION__);
        }
        else {
           // WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, -1,
           //              "%s: Failed to register native functions", __FUNCTION__);
            return -1;
        }

        env->CallVoidMethod(_javaRenderObj, _registerNativeCID, (jlong) this);

        if (isAttached) {
            if (_jvm->DetachCurrentThread() < 0) {
               // WEBRTC_TRACE(kTraceWarning, kTraceVideoRenderer, _id,
                //             "%s: Could not detach thread from JVM", __FUNCTION__);
            }
        }

        //WEBRTC_TRACE(kTraceDebug, kTraceVideoRenderer, _id, "%s done",
        //             __FUNCTION__);

//        if (_openGLRenderer.SetCoordinates(zOrder, left, top, right, bottom) != 0) {
//            return -1;
//        }

        return 0;
    }

    void DeliverFrame(int32_t widht, int32_t height)
    {
        if (_jvm)
        {
            bool isAttached = false;
            JNIEnv* env = NULL;
            if (_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
                // try to attach the thread and get the env
                // Attach this thread to JVM
                jint res = _jvm->AttachCurrentThread(&env, NULL);

                // Get the JNI env for this thread
                if ((res < 0) || !env) {
                    //WEBRTC_TRACE(kTraceError, kTraceVideoRenderer, _id,
                    //             "%s: Could not attach thread to JVM (%d, %p)",
                    //             __FUNCTION__, res, env);
                    env = NULL;
                } else {
                    isAttached = true;
                }
            }

            if (env && _redrawCid)
            {
                _widht = widht;
                _height = height;

                env->CallVoidMethod(_javaRenderObj, _redrawCid);
            }

            if (isAttached) {
                if (_jvm->DetachCurrentThread() < 0) {
                    //WEBRTC_TRACE(kTraceWarning, kTraceVideoRenderer, _id,
                    //             "%s: Could not detach thread from JVM",
                     //            __FUNCTION__);
                }
            }
        }
    }

    void GetDataBuf(uint8_t*& pbuf, int32_t& isize)
    {
        pbuf = _buffer;
        isize = 1024000;
    }

    static jint CreateOpenGLNativeStatic(JNIEnv * env,
                                         jobject,
                                         jlong context,
                                         jint width,
                                         jint height)
    {
        AndroidNativeOpenGl2Channel* renderChannel =
                reinterpret_cast<AndroidNativeOpenGl2Channel*> (context);

        //WEBRTC_TRACE(kTraceInfo, kTraceVideoRenderer, -1, "%s:", __FUNCTION__);

        return renderChannel->CreateOpenGLNative(width, height);
    }

    static void DrawNativeStatic(JNIEnv * env,jobject, jlong context)
    {
        AndroidNativeOpenGl2Channel* renderChannel =
                reinterpret_cast<AndroidNativeOpenGl2Channel*>(context);
        renderChannel->DrawNative();
    }

    jint CreateOpenGLNative(int width, int height)
    {
        return _openGLRenderer.Setup(width, height);
    }

    void DrawNative()
    {
        _openGLRenderer.Render(_buffer, _widht, _height);
    }

private:
    JavaVM*     _jvm;
    void* _ptrWindow;

    jobject _javaRenderObj;
    jclass _javaRenderClass;
    JNIEnv* _javaRenderJniEnv;

    jmethodID      _redrawCid;
    jmethodID      _registerNativeCID;
    jmethodID      _deRegisterNativeCID;

    RenderOpenGles20 _openGLRenderer;

    uint8_t* _buffer;
    int32_t _widht;
    int32_t _height;
};

static JavaVM* g_jvm = NULL;
static AndroidNativeOpenGl2Channel* p_opengl_channel = NULL;



JNIEXPORT void JNICALL Java_com_example_filltriangle_GL2JNILib_init
        (JNIEnv *env, jclass oclass, jobject glSurface)
{
    if (p_opengl_channel)
    {
        //WEBRTC_TRACE(kTraceInfo, kTraceVideoRenderer, -1, "初期化失败[%d].", __LINE__);
        return;
    }

    p_opengl_channel = new AndroidNativeOpenGl2Channel(g_jvm, glSurface);
    if (p_opengl_channel->Init() != 0)
    {
        //WEBRTC_TRACE(kTraceInfo, kTraceVideoRenderer, -1, "初期化失败[%d].", __LINE__);
        return;
    }
}



JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    g_jvm = vm;
    if(crossanyapp::me() == nullptr) return 0;
    if(gtmvreader::me() == nullptr) return 0;
    //crossanyapp::me()->setJvm(vm);
    // 返回jni的版本
    return JNI_VERSION_1_4;
}
