#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include "httpsdownloader.h"
#include "crossanyapp.h"
#include "gtmvreader.h"

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