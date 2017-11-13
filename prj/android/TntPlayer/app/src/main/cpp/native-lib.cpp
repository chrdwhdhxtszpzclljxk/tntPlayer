#include <jni.h>
#include <string>
#include "httpsdownloader.h"
#include "crossanyapp.h"
#include "gtmvreader.h"

char* jstringTostring(JNIEnv* env, jstring jstr);
std::string jstring2string(JNIEnv* env,jstring jstr);

extern "C" JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_MainActy_stringFromJNI(
        JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_MainActy_createHttpsdownloader(
        JNIEnv *env, jobject /* this */) {
    //if (httpsdownloader::me() == nullptr) return 0;
    if(crossanyapp::me() == nullptr) return 0;
    if(gtmvreader::me() == nullptr) return 0;
    return 1;
}

extern "C" JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_MainActy_httpsdownloaderPush(
        JNIEnv *env, jobject /* this */,jstring type,jstring tnow ,jstring pubid) {
    char* ptype = jstringTostring(env,type);
    char* ptnow = jstringTostring(env,tnow);
    char* ppubid = jstringTostring(env,pubid);
    gtmvreader::me()->push(ptype,ptnow,ppubid);
    delete ptype;
    delete ptnow;
    delete ppubid;
    return 1;
}

extern "C" JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_MainActy_setWritablePath(
        JNIEnv *env, jobject /* this */,jstring path) {
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