//
// Created by Administrator on 2017-11-10.
//
#include <thread>
#include <android/log.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "httpsdownloader.h"
#include "crossanyapp.h"

void httpsdownloader::httpsdownloaderThread(){
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

    }

}

void httpsdownloader::push(std::string type,std::string tnow,std::string pubid){
    fileinfo *fi = new fileinfo();
    fi->fileType = type;
    fi->fileId = tnow;
    fi->pubId = pubid;
    fi->errcount = 0;
    std::lock_guard<std::recursive_mutex> locker(_lock);
    listdl.push_back(fi);
}

httpsdownloader* httpsdownloader::me(){
    static httpsdownloader* pthis = nullptr;
    if(pthis == nullptr){
        pthis = new httpsdownloader();
        if(!pthis->init()) {
            delete pthis;
            pthis = nullptr;
        }
    }
    return pthis;
}

bool httpsdownloader::init(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    //listdl = new std::list<fileinfo>();
    static std::thread t1 (&httpsdownloader::httpsdownloaderThread,this);
    return true;
}


// 获取服务器上的文件大小
double getDownloadFileLength(std::string url){
    CURL *easy_handle = NULL;
    int ret = CURLE_OK;
    double size = -1;

    do{
        easy_handle = curl_easy_init();
        if (!easy_handle){
            printf("easy_handle init error\n");
            break;
        }

        // 仅获取HTTP头
        ret = curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
        ret |= curl_easy_setopt(easy_handle, CURLOPT_HEADER, 1L);
        ret |= curl_easy_setopt(easy_handle, CURLOPT_NOBODY, 1L);

        if (ret != CURLE_OK){
            break;
        }

        ret = curl_easy_perform(easy_handle);
        if (ret != CURLE_OK){
            break;
        }

        // 没有查询到的话size=-1
        ret = curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
        if (ret != CURLE_OK){
            break;
        }
    } while (0);

    curl_easy_cleanup(easy_handle);
    return size;
}