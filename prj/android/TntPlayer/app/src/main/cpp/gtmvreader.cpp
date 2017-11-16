//
// Created by Administrator on 2017-11-13.
//

#include <thread>
#include <android/log.h>
#include "gtmvreader.h"
#include "protocol.h"

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
        __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","got one!");
        char path[2048] = {0};
        const char* ppath = crossanyapp::me()->getWritablePath().c_str();
        const char* ppubid = fi->pubId.c_str();
        const char* pfileId = fi->fileId.c_str();
        sprintf(path,"%s/data/%s/%s.gtmv",ppath,ppubid,pfileId);
        __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","%s",path);
        int retry = 10;
        int retryspan = 2;
        bool ok = false;
        xiny120::GTMVHEADER header; xiny120::VAPUSHMT mt; xiny120::GTMVNODE* pnode;
        do {
            FILE *fp = fopen(path, "r");
            if (fp != 0) {
                if (((fread(&header, 1, sizeof(header), fp) == sizeof(header)) && (fread(&mt, 1, sizeof(mt), fp) == sizeof(mt)))){
                    __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","open success %s",path);

                    ok = true;
                }

                fclose(fp);
            }

            if(retry <= 1) break;
            retry--;
            std::this_thread::sleep_for (std::chrono::seconds(retryspan));

        }while(true && (!ok));

    }

}

void gtmvreader::push(std::string type,std::string tnow,std::string pubid,long filelen){
    fileinfo *fi = new fileinfo();
    fi->fileType = type;
    fi->fileId = tnow;
    fi->pubId = pubid;
    fi->errcount = 0;
    fi->filelen = filelen;
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