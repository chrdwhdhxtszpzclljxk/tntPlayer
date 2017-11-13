//
// Created by Administrator on 2017-11-13.
//

#include <thread>
#include <android/log.h>
#include "gtmvreader.h"


void gtmvreader::httpsdownloaderThread(){
    while(true){
        fileinfo *fi = nullptr;
        {
            std::lock_guard<std::recursive_mutex> locker(_lock);
            if(listdl.size() > 0) {
                fi = *listdl.begin();
                listdl.pop_front();
            }
        }
        if(fi == nullptr){
            std::this_thread::sleep_for (std::chrono::seconds(1));
            continue;
        }
        __android_log_print(ANDROID_LOG_INFO,"JNI/download","got one!");
        char path[2048] = {0};
        const char* ppath = crossanyapp::me()->getWritablePath().c_str();
        const char* ppubid = fi->pubId.c_str();
        const char* pfileId = fi->fileId.c_str();
        sprintf(path,"%s/data/%s/%s.gtmv",ppath,ppubid,pfileId);
        __android_log_print(ANDROID_LOG_INFO,"JNI/download","%s",path);
        FILE * fh = fopen(path,"r");
        if(fh != 0) {
            fclose(fh);
        }

    }

}

void gtmvreader::push(std::string type,std::string tnow,std::string pubid){
    fileinfo *fi = new fileinfo();
    fi->fileType = type;
    fi->fileId = tnow;
    fi->pubId = pubid;
    fi->errcount = 0;
    std::lock_guard<std::recursive_mutex> locker(_lock);
    listdl.push_back(fi);
}

gtmvreader* gtmvreader::me(){
    static gtmvreader* pthis = nullptr;
    if(pthis == nullptr){
        pthis = new gtmvreader();
        if(!pthis->init()) {
            delete pthis;
            pthis = nullptr;
        }
    }
    return pthis;
}

bool gtmvreader::init(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    //listdl = new std::list<fileinfo>();
    static std::thread t1 (&gtmvreader::httpsdownloaderThread,this);
    return true;
}