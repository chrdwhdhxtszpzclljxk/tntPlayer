//
// Created by Administrator on 2017-11-13.
//
#include <cstdlib>
#include <thread>
#include <android/log.h>
#include <queue>
#include <sstream>
#include <assert.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "gtmvreader.h"
#include "protocol.h"
#include "speex_jni.h"
#include "AudioEngine.h"

float CCRANDOM_0_1(){
    return std::rand() / (float)RAND_MAX;
}

bool load192k(const int64_t& idx, unsigned char* buf, int64_t& len){
    bool ret = false;
    int lentmp; std::string file; std::stringstream ss;
    ss << "images/playctrl/" << idx - '0' << "_192k.png"; ss >> file; //"0_192k.png";
    len = 0;
    if (buf != 0){
        AAsset* asset = AAssetManager_open(crossanyapp::me()->getAm(), file.c_str(), AASSET_MODE_UNKNOWN);
        long size = AAsset_getLength(asset);//获取文件长度
        char* buffer = (char*) malloc (sizeof(char)*size+1);
        buffer[size]='\0';
        AAsset_read(asset,buffer,size);
        AAsset_close(asset);
        lentmp = size / 2;
        short* src, *dst;
        if (lentmp > 0){
            src = (short*)buffer;
            dst = (short*)buf;
            int j = 0;
            for (int i = 0; i < lentmp; i++){
                dst[j] = src[i] / 15;
                j++;
                dst[j] = src[i] / 15;
                j++;
            }
            len = j * 2;
            ret = true;
        }
        free(buffer);
    }
    return ret;
}

void asleep(int n){
    std::this_thread::sleep_for (std::chrono::milliseconds(n));
}

void gtmvreader::httpsdownloaderThread(){
    std::srand(time(NULL));
    int i = 0;
    unsigned char* a192k; int64_t a192klen,a192klenloaded;
    a192klen = 1024 * 1024 * 20;
    a192k = new unsigned char[a192klen];

    while(true){
        fileinfo *fi = nullptr;
        {
            std::lock_guard<std::recursive_mutex> locker(crossanyapp::me()->_lock);
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

        unsigned char* dst = a192k; int64_t len = a192klen; a192klenloaded = 0;
        std::string userid = crossanyapp::me()->getUserid();
        for (i = 0; i < userid.length(); i++){
            if (load192k(userid[i], dst, len)){
                a192klenloaded += len; dst += len; dst += 12000; a192klenloaded += 12000; len = a192klen - a192klenloaded;
            }
        }
        a192klenloaded -= 12000;
        unsigned char	key = atoi(pfileId);
        do {
            FILE *fp = fopen(path, "r");
            if (fp != 0) {
                if (((fread(&header, 1, sizeof(header), fp) == sizeof(header)) && (fread(&mt, 1, sizeof(mt), fp) == sizeof(mt)))){
                    __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","open success %s",path);
                    std::queue<xiny120::GtmvData*> mvq; xiny120::GtmvData* lastv = NULL; uint64_t refTS,refStartV = 0,totalframes = 0; bool first = true;
                    int8_t* pread,*pcur,*pbuf = NULL; int32_t markerspan[11],markerspanid = 0,markeridxlast = 0; short* amcur = (short*)a192k;
                    markerspan[0] = CCRANDOM_0_1() * (120 - 60) + 70;
                    for (i = 1; i < sizeof(markerspan) / sizeof(markerspan[0]); i++){ markerspan[i] = CCRANDOM_0_1() * (120 - 60) + 70; }
                    int32_t yv12len = mt.wid * mt.hei * 3 / 2, total = yv12len * 10, left = total, readed = 0, datalen = 0,acount = 0,curreadsize = 0;
                    uint8_t* yv12buf = new uint8_t[yv12len]; memset(yv12buf, 0, yv12len);
                    if (pbuf == NULL){ pbuf = new int8_t[total];  pread = pcur = pbuf; }
                    if (lastv == NULL){
                        lastv = (xiny120::GtmvData*)new char[sizeof(xiny120::GtmvData) + yv12len];
                        lastv->id = atoi(ppubid); lastv->type = xiny120::C_PU_VIDEO; lastv->len = yv12len; lastv->width = mt.wid; lastv->height = mt.hei;
                        lastv->now = atoi(pfileId); lastv->start = 0; lastv->end = 0; lastv->ticks = 0;
                    }
                    speex dec; refTS = xiny120::UNITS / mt.frm;
                    assert(curreadsize == 0);
                    fseek(fp, curreadsize, SEEK_CUR);  int64_t t1 = ftell(fp); fseek(fp, 0, SEEK_END); int64_t t2 = ftell(fp); fseek(fp, t1, SEEK_SET);
                    breaknow = false;
                    do{
                        if(fp == nullptr)
                            break;
                        if ((readed = fread(pread, 1, left, fp)) <= 0){
                            int retry01 = 100;
                            while ((curreadsize + sizeof(header) + sizeof(mt)) < fi->filelen && (!breaknow)){
                                if ((readed = fread(pread, 1, left, fp)) > 0){ break; } // when file downloading,we readed it infinite ,when readed any data, we keep going
                                //log("fread fail2 %d %d\r\n",ferror(fp),feof(fp));
                                asleep(100);
                                retry01--;
                                if(retry01 <= 0){
                                    retry01 = 100;
                                    fclose(fp);
                                    if((fp = fopen(path,"rb")) != NULL){
                                        fseek(fp, curreadsize + sizeof(header) + sizeof(mt), SEEK_SET);
                                    }else{
                                        breaknow = true;
                                        break;
                                    }
                                }
                            }
                            if ((curreadsize + sizeof(header) + sizeof(mt)) >= fi->filelen) break;
                        }
                        curreadsize += readed; pread += readed; left -= readed;
                        while (pcur < pread && (!breaknow)){
                            pnode = (xiny120::GTMVNODE*)pcur; pcur += pnode->len + sizeof(pnode->len) + sizeof(pnode->type);
                            if (pcur > pread){
                                pcur = (int8_t*)pnode;
                                break;
                            }; // ���Ȳ���һ��node,���Ŷ�ȡ��
                            if (pnode->type == xiny120::C_PU_VIDEO){// ��ǰ֡����Ƶ֡
                                xiny120::GtmvData* v = (xiny120::GtmvData*) new int8_t[yv12len + sizeof(*v)]; uint8_t* out = NULL;
                                assert(v != 0);
                                v->id = atoi(ppubid); v->type = xiny120::C_PU_VIDEO; v->len = yv12len; v->width = mt.wid; v->height = mt.hei;
                                v->now = atoi(pfileId); v->start = 0; v->end = 0; v->ticks = 0;

                                //v->len = ZipUtils::inflateMemoryWithHint(pnode->data, pnode->len, &out, yv12len);
                                if (v->len == 0) {
                                    //bkeepcheckfile = true;
                                    breaknow = true;
                                }
                                for (i = 0; i < yv12len; i++){ out[i] ^= key; if (out[i] != 0) yv12buf[i] = out[i]; } // ���ء�
                                free(out); memcpy(v->data, yv12buf, yv12len);
                                mvq.push(v); memmove(lastv, v, sizeof(xiny120::GtmvData) + yv12len);	//SaveToBmp(v->data, mt.wid, mt.hei, 0);
                                //log("mvq size:%d",mvq.size());
                            }
                            else if (pnode->type == xiny120::C_PU_AUDIO){ // ��ǰ֡����Ƶ֡
                                do{
                                    if (breaknow) break;
                                    int8_t* a = new int8_t[8000 * 2]; uint32_t len = 0;
                                    dec.decode((char*)pnode->data, pnode->len, (short*)a, len);
                                    int32_t addframes = mt.frm - mvq.size();
                                    while (addframes > 0){
                                        xiny120::GtmvData* add = (xiny120::GtmvData*) new int8_t[yv12len + sizeof(*add)];
                                        memmove(add, lastv, yv12len + sizeof(*add)); mvq.push(add); addframes--;
                                    };
                                    while (!mvq.empty()){
                                        xiny120::GtmvData* vprepare = mvq.front(); mvq.pop();
                                        if (vprepare != NULL){
                                            vprepare->start = refStartV; refStartV += refTS; vprepare->end = refStartV;
                                            if (first){
                                                gtmvrender::me()->h = mt.hei; gtmvrender::me()->w = mt.wid; gtmvrender::me()->fps = mt.frm;
                                                gtmvrender::me()->frames = header.a; gtmvrender::me()->curframes = 0; gtmvrender::me()->seekframes = 0;
                                                xiny120::GtmvData* vfirst = new xiny120::GtmvData; vfirst->id = atoi(ppubid); vfirst->type = xiny120::C_PU_VIDEO;
                                                vfirst->width = mt.wid; vfirst->height = mt.hei; vfirst->start = mt.frm; vfirst->now = atoi(pfileId);
                                                vfirst->len = vfirst->end = 1; gtmvrender::me()->pushv(vfirst); first = false;
                                            }
                                            gtmvrender::me()->pushv(vprepare);
                                        }
                                    }
                                    while (xiny120::AudioEngine::me()->isfull(len * 2) &&  (!breaknow)){ asleep(200); }
                                    if (markerspan[markerspanid] < totalframes){
                                        if (((unsigned char*)amcur - a192k) < a192klenloaded){
                                            short* dst = (short*)a; // ���ƻ���
                                            int32_t t1;
                                            for (i = 0; i < len; i++){
                                                //dst[i] = amcur[i];
                                                if (amcur[i] != 0){
                                                    t1 = dst[i] + amcur[i] - (dst[i] * amcur[i] >> 0x10);
                                                    if (t1 > 32767) t1 = 32767;
                                                    else if (t1 < -32768) t1 = -32768;
                                                    dst[i] = t1;
                                                }
                                            }
                                            amcur += len;
                                        }
                                        else { amcur = (short*)a192k; markerspanid++;  }
                                    }
                                    totalframes++;
                                    //std::string pcmfile = App::basePath + "/test002.pcm";
                                    //FILE* fp = fopen(pcmfile.c_str(),"ab+");
                                    //fseek(fp,0,SEEK_END);
                                    //fwrite(a,1,len * 2,fp);
                                    //fclose(fp);
                                    xiny120::AudioEngine::me()->push((int8_t*)a, len * 2); acount++; // ����������Ͷ�Ÿ�����������档
                                    delete[](int8_t*)a;
                                    gtmvrender::me()->curframes++; gtmvrender::me()->seekframes.store(gtmvrender::me()->curframes);
                                    if (acount == 2) xiny120::AudioEngine::me()->start(1);
                                } while (false);
                            }
                        }
                        datalen = pread - pcur; memmove(pbuf, pcur, datalen);
                        pcur = pbuf; pread = pbuf + datalen; left = total - datalen;
                    } while (!breaknow);
                    delete[] yv12buf;
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
    std::lock_guard<std::recursive_mutex> locker(crossanyapp::me()->_lock);
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
    return true;
}

void gtmvreader::start() {
    std::lock_guard<std::recursive_mutex> locker(crossanyapp::me()->_lock);
    static std::thread t1 (&gtmvreader::httpsdownloaderThread,this);

}


static gtmvrender* __gtmvrender = NULL;
gtmvrender::gtmvrender(){}
gtmvrender::~gtmvrender(){	clear(); }
gtmvrender* gtmvrender::me(){ if (__gtmvrender == NULL) __gtmvrender = new gtmvrender(); return __gtmvrender; }
void gtmvrender::destroy(){ delete __gtmvrender; }
void gtmvrender::pushv(xiny120::GtmvData* d){ _lock lock(mtv); v.push(d); /*log("v size:%d", v.size());*/ }
xiny120::GtmvData* gtmvrender::popv(){ xiny120::GtmvData* ret = NULL; _lock lock(mtv); if (!v.empty()){ ret = v.front(); v.pop(); } return ret; }
uint64_t gtmvrender::touchv(){ uint64_t ret = 0; _lock lock(mtv); if (!v.empty()){ ret = v.front()->start; } return ret; };
void gtmvrender::clear(){
    mtv.lock(); while (!v.empty()){ delete[](char*) v.front(); v.pop(); }; mtv.unlock();
};

void gtmvrender::resettimeline(const uint64_t& now){
    mtv.lock();
    avqueue temp;
    while (!v.empty()){
        if (v.front()->start < now){
            delete[](char*) v.front(); v.pop();
        }
        else{
            v.front()->start -= now;
            temp.push(v.front()); v.pop();
        }
    };
    v = temp;
    mtv.unlock();
}
