//
// Created by Administrator on 2017-11-23.
//

#ifndef TNTPLAYER_NATIVE_LIB_H
#define TNTPLAYER_NATIVE_LIB_H
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <assert.h>
#include <android/log.h>


extern "C" {

JNIEXPORT jshort JNICALL
Java_com_gwgz_tntplayer_GL2JNILib_initNdkApp(JNIEnv *env, jobject /* this */);

JNIEXPORT void JNICALL
Java_com_gwgz_tntplayer_GL2JNILib_setAssetManager(JNIEnv *env, jobject /* this */,
                                                 jobject assetManager);

JNIEXPORT jshort JNICALL Java_com_gwgz_tntplayer_GL2JNILib_gtmvreaderPush(JNIEnv *env, jobject /* this */,jstring type,jstring tnow ,jstring pubid,jlong filelen);

JNIEXPORT jstring JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideowh(JNIEnv *env, jobject /* this */);

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideoFrame(JNIEnv *env, jobject /* this */,jobject bitmap);

JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_GL2JNILib_getVideoFrame1(JNIEnv *env, jobject /* this */,jobject buf,jint len);

JNIEXPORT jboolean JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setVideoSurface(JNIEnv *env, jobject /* this */,jobject jsurface);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_audioEngineStop(JNIEnv *env, jobject /* this */);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_audioEnginePause(JNIEnv *env, jobject /* this */);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_audioEngineResume(JNIEnv *env, jobject /* this */);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setUnPwd(JNIEnv *env, jobject /* this */,jstring un,jstring pwd,jstring userid);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setWritablePath(JNIEnv *env, jobject /* this */,jstring path);
//JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
//JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_step(JNIEnv * env, jobject obj);

JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_resize(JNIEnv * env, jobject obj,  jint width, jint height);
JNIEXPORT jlong JNICALL Java_com_gwgz_tntplayer_GL2JNILib_step(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_init(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_gwgz_tntplayer_GL2JNILib_setTexture(JNIEnv * env, jclass obj, jintArray tex);

};
//




#endif //TNTPLAYER_NATIVE_LIB_H
