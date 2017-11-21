#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <assert.h>
#include "httpsdownloader.h"
#include "crossanyapp.h"
#include "gtmvreader.h"
#include "AudioEngine.h"

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

extern "C" JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_MainActy_getVideoFrame(JNIEnv *env, jobject /* this */,jobject bitmap) {
    jbyteArray  ar = (*env).NewByteArray(1);
    uint8_t* rgbbuf = new uint8_t[1280 * 2048 * 4];
    xiny120::GtmvData* v;

    while (gtmvrender::me()->touchv() <= xiny120::AudioEngine::me()->mstart){
        if ((v = gtmvrender::me()->popv()) != NULL && v->len != 0){
            gtmvreader::yv12torgb24(rgbbuf,v->data,v->width,v->height);

            // Lock the bitmap to get the buffer
            void * pixels = NULL;
            int res = AndroidBitmap_lockPixels(env, bitmap, &pixels);

            AndroidBitmap_unlockPixels(env, bitmap);

            //tex->initWithData(rgbbuf, v->len * 2, Texture2D::PixelFormat::RGB888, v->width, v->height, cocos2d::Size(v->width, v->height));
            //cocos2d::Rect rc; cocos2d::Size s1 = Director::getInstance()->getWinSize(); cocos2d::Size s = tex->getContentSize();
            //rc.setRect(0, 0, s.width, s.height);
            //SpriteFrame* frame = SpriteFrame::createWithTexture(tex, rc);
            //if (frame != NULL) spritev->setSpriteFrame(frame);
            delete[](char*) v;
            //lv->step();
        }else{
            if (v != NULL && v->len == 0) {

            }
            if (v != NULL) delete v;
            break;
        }
    }
    delete [] rgbbuf;

    return env->NewStringUTF("");
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

