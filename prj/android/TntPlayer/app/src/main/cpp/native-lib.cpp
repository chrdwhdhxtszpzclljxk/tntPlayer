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
#include "native-lib.h"

char* jstringTostring(JNIEnv* env, jstring jstr);
std::string jstring2string(JNIEnv* env,jstring jstr);


JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_GL2JNILib_initNdkApp(JNIEnv *env, jobject /* this */) {
    gtmvreader::me()->start();
    return 1;
}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setAssetManager(JNIEnv *env, jobject /* this */, jobject assetManager) {
    crossanyapp::me()->setAm(AAssetManager_fromJava( env, assetManager ));
}


JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_GL2JNILib_gtmvreaderPush(JNIEnv *env, jobject /* this */,jstring type,jstring tnow ,jstring pubid,jlong filelen) {
    char* ptype = jstringTostring(env,type);
    char* ptnow = jstringTostring(env,tnow);
    char* ppubid = jstringTostring(env,pubid);
    gtmvreader::me()->push(ptype,ptnow,ppubid,filelen);
    delete ptype;
    delete ptnow;
    delete ppubid;
    return 1;
}

JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideowh(JNIEnv *env, jobject /* this */) {
    char buf[1024] = {0};
    int w,h;
    gtmvrender::me()->getwh(w,h);
    sprintf(buf,"{\"w\":%d,\"h\":%d}",w,h);
    return env->NewStringUTF(buf);
}

bool go(uint32_t a, uint32_t b){
    return false;
}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideoFrame(JNIEnv *env, jobject /* this */,jobject bitmap) {
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


JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideoFrame1(JNIEnv *env, jobject /* this */,jobject buf,jint len) {
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


JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setVideoSurface(JNIEnv *env, jobject /* this */,jobject jsurface) {
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

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_audioEngineStop(JNIEnv *env, jobject /* this */) {
    gtmvreader::me()->setbreaknow();

}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setUnPwd(JNIEnv *env, jobject /* this */,jstring un,jstring pwd,jstring userid) {
    crossanyapp::me()->setun(jstring2string(env,un));
    crossanyapp::me()->setPwd(jstring2string(env,pwd));
    crossanyapp::me()->setUserid(jstring2string(env,userid));
}

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setWritablePath(JNIEnv *env, jobject /* this */,jstring path) {
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




JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    if(crossanyapp::me() == nullptr) return 0;
    if(gtmvreader::me() == nullptr) return 0;
    //crossanyapp::me()->setJvm(vm);
    // 返回jni的版本
    return JNI_VERSION_1_4;
}
