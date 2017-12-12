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
#include "cryptlib.h"
#include "osrng.h"
#include "base64.h"
#include "default.h"
#include "gzip.h"
#include "zlib.h"

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

void ProcessBuffTrans(/*CryptoPP::BufferedTransformation& buf,*/ const byte* input, int inputlen,byte** output, int* outputlen)
{
    CryptoPP::ZlibDecompressor buf;
    buf.Put(input, inputlen);
    buf.MessageEnd();
    int len = buf.MaxRetrievable();
    if(len > *outputlen) {
        delete [] *output;
        *output = new byte[len];
    }
    buf.Get(*output, len);
    *outputlen = len;
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
        int retry = 40;
        int retryspan = 1;
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
                    gtmvrender::me()->clear();
                    std::queue<xiny120::GtmvData*> mvq; xiny120::GtmvData* lastv = NULL; uint64_t refTS,refStartV = 0,totalframes = 0; bool first = true;
                    int8_t* pread,*pcur,*pbuf = NULL; int32_t markerspan[11],markerspanid = 0,markeridxlast = 0; short* amcur = (short*)a192k;
                    markerspan[0] = CCRANDOM_0_1() * (120 - 60) + 70;
                    for (i = 1; i < sizeof(markerspan) / sizeof(markerspan[0]); i++){ markerspan[i] = CCRANDOM_0_1() * (120 - 60) + 70; }
                    int32_t rgb32len = mt.wid * mt.hei * 3;
                    int32_t yv12len = mt.wid * mt.hei * 3 / 2, total = yv12len * 10, left = total, readed = 0, datalen = 0,acount = 0,curreadsize = 0;
                    //rgb32len = yv12len;
                    uint8_t* yv12buf = new uint8_t[yv12len]; memset(yv12buf, 0, yv12len);
                    uint8_t* out = new uint8_t[yv12len]; memset(out, 0, yv12len);
                    if (pbuf == NULL){ pbuf = new int8_t[total];  pread = pcur = pbuf; }
                    if (lastv == NULL){
                        lastv = (xiny120::GtmvData*)new char[sizeof(xiny120::GtmvData) + rgb32len];
                        lastv->id = atoi(ppubid); lastv->type = xiny120::C_PU_VIDEO; lastv->len = rgb32len; lastv->width = mt.wid; lastv->height = mt.hei;
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
                                xiny120::GtmvData* v = (xiny120::GtmvData*) new int8_t[rgb32len + sizeof(*v)];
                                assert(v != 0);
                                v->id = atoi(ppubid); v->type = xiny120::C_PU_VIDEO; v->len = rgb32len; v->width = mt.wid; v->height = mt.hei;
                                v->now = atoi(pfileId); v->start = 0; v->end = 0; v->ticks = 0;

                                //v->len = ZipUtils::inflateMemoryWithHint(pnode->data, pnode->len, &out, yv12len);
                                int zliblen = yv12len;
                                ProcessBuffTrans(pnode->data,pnode->len,&out,&zliblen);
                                v->len = rgb32len;
                                //__android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","解压数据：%d - yv12len(%d)",zliblen,yv12len);
                                if (zliblen == 0) {
                                    //bkeepcheckfile = true;
                                    breaknow = true;
                                }
                                for (i = 0; i < yv12len; i++){ out[i] ^= key; if (out[i] != 0) yv12buf[i] = out[i]; } // ���ء�
                                //delete [] out; //memcpy(v->data, yv12buf, yv12len);
                                {
                                    //uint8_t * rgb = new uint8_t[v->width * v->height * 4];
                                    gtmvreader::yv12torgb24(v->data,yv12buf,v->width,v->height);
                                    //delete [] rgb;
                                }


                                mvq.push(v); memmove(lastv, v, sizeof(xiny120::GtmvData) + rgb32len);	//SaveToBmp(v->data, mt.wid, mt.hei, 0);
                                //log("mvq size:%d",mvq.size());
                            }
                            else if (pnode->type == xiny120::C_PU_AUDIO){ // ��ǰ֡����Ƶ֡
                                do{
                                    if (breaknow) break;
                                    int8_t* a = new int8_t[8000 * 2]; uint32_t len = 0;
                                    dec.decode((char*)pnode->data, pnode->len, (short*)a, len);
                                    int32_t addframes = mt.frm - mvq.size();
                                    while (addframes > 0){
                                        xiny120::GtmvData* add = (xiny120::GtmvData*) new int8_t[rgb32len + sizeof(*add)];
                                        memmove(add, lastv, rgb32len + sizeof(*add)); mvq.push(add); addframes--;
                                    };
                                    while (!mvq.empty()){
                                        xiny120::GtmvData* vprepare = mvq.front(); mvq.pop();
                                        if (vprepare != NULL){
                                            vprepare->start = refStartV; refStartV += refTS; vprepare->end = refStartV;
                                            if (first){
                                                gtmvrender::me()->setwh(mt.wid,mt.hei);
                                                /*gtmvrender::me()->h = mt.hei; gtmvrender::me()->w = mt.wid;*/ gtmvrender::me()->fps = mt.frm;
                                                gtmvrender::me()->frames = header.a; gtmvrender::me()->curframes = 0; gtmvrender::me()->seekframes = 0;
                                                xiny120::GtmvData* vfirst = new xiny120::GtmvData; vfirst->id = atoi(ppubid); vfirst->type = xiny120::C_PU_VIDEO;
                                                vfirst->width = mt.wid; vfirst->height = mt.hei; vfirst->start = mt.frm; vfirst->now = atoi(pfileId);
                                                vfirst->len = vfirst->end = 1; gtmvrender::me()->pushv(vfirst); first = false;
                                            }
                                            gtmvrender::me()->pushv(vprepare);
                                        }
                                    }
                                    while (xiny120::AudioEngine::me()->isfull(len * 2) &&  (!breaknow)){
                                        //__android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","queue full!");
                                        asleep(100);
                                    }
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

                    delete[](char*) lastv; delete[] pbuf;
                    while (!mvq.empty()){
                        xiny120::GtmvData* vprepare = mvq.front(); mvq.pop();
                        if (vprepare != NULL){
                            vprepare->start = refStartV; refStartV += refTS; vprepare->end = refStartV;
                            gtmvrender::me()->pushv(vprepare);
                        }
                    }
                    if (breaknow){ gtmvrender::me()->clear(); xiny120::AudioEngine::me()->clear(); }
                    xiny120::GtmvData* vend = new xiny120::GtmvData;
                    vend->id = atoi(ppubid); vend->type = xiny120::C_PU_AUDIO; vend->len = 0; vend->width = 0; vend->height = 0;
                    vend->now = atoi(pfileId); vend->start = 0; vend->end = 0; vend->ticks = 0;
                    gtmvrender::me()->pushv(vend); xiny120::AudioEngine::me()->setbreaknow();
                    while (!xiny120::AudioEngine::me()->empty() && r && (!breaknow)){ asleep(1);}
                    xiny120::AudioEngine::me()->stop();

                }
                fclose(fp);


            }
            else{
                __android_log_print(ANDROID_LOG_INFO,"JNI/gtmvreader","打开文件失败！%s",path);

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

bool gtmvreader::shit(uint32_t iWidth, uint32_t iHeight){
    uint32_t  a = iWidth / 2;
    uint32_t b = iHeight / 2;
    a = a+ b;
    return false;
}

bool gtmvreader::YV12_to_RGB32(unsigned char* pRGB32,unsigned char* pYV12,  uint32_t iWidth, uint32_t iHeight)
{
    if (!pYV12 || !pRGB32)
        return false;

    const long nYLen = long(iHeight * iWidth);
    const int32_t nHfWidth = (iWidth >> 1);

    if (nYLen < 1 || nHfWidth < 1)
        return false;

    unsigned char* yData = pYV12;
    unsigned char* vData = pYV12 + iWidth*iHeight + (iHeight / 2)*(iWidth / 2);//&vData[nYLen >> 2];
    unsigned char* uData = pYV12 + iWidth*iHeight;// &yData[nYLen];
    if (!uData || !vData)
        return false;

    int32_t rgb[4];
    int32_t jCol, iRow;
    for (iRow = 0; iRow < iHeight; iRow++)
    {
        for (jCol = 0; jCol < iWidth; jCol++)
        {
            rgb[3] = 1;

            int32_t Y = yData[iRow*iWidth + jCol];
            int32_t U = uData[(iRow / 2)*(iWidth / 2) + (jCol / 2)];
            int32_t V = vData[(iRow / 2)*(iWidth / 2) + (jCol / 2)];
            int32_t R = Y + (U - 128) + (((U - 128) * 103) >> 8);
            int32_t G = Y - (((V - 128) * 88) >> 8) - (((U - 128) * 183) >> 8);
            int32_t B = Y + (V - 128) + (((V - 128) * 198) >> 8);

            // r分量值
            R = R<0 ? 0 : R;
            rgb[2] = R > 255 ? 255 : R;
            // g分量值
            G = G<0 ? 0 : G;
            rgb[1] = G>255 ? 255 : G;
            // b分量值
            B = B<0 ? 0 : B;
            rgb[0] = B>255 ? 255 : B;
            pRGB32[4 * (iRow*iWidth + jCol) + 0] = rgb[0];
            pRGB32[4 * (iRow*iWidth + jCol) + 1] = rgb[1];
            pRGB32[4 * (iRow*iWidth + jCol) + 2] = rgb[2];
            pRGB32[4 * (iRow*iWidth + jCol) + 3] = rgb[3];
        }
    }

    return true;
}


const int32_t Table_fv1[256] = { -180, -179, -177, -176, -174, -173, -172, -170, -169, -167, -166, -165, -163, -162, -160, -159, -158, -156, -155, -153, -152, -151, -149, -148, -146, -145, -144, -142, -141, -139, -138, -137, -135, -134, -132, -131, -130, -128, -127, -125, -124, -123, -121, -120, -118, -117, -115, -114, -113, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96, -94, -93, -92, -90, -89, -87, -86, -85, -83, -82, -80, -79, -78, -76, -75, -73, -72, -71, -69, -68, -66, -65, -64, -62, -61, -59, -58, -57, -55, -54, -52, -51, -50, -48, -47, -45, -44, -43, -41, -40, -38, -37, -36, -34, -33, -31, -30, -29, -27, -26, -24, -23, -22, -20, -19, -17, -16, -15, -13, -12, -10, -9, -8, -6, -5, -3, -2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 36, 37, 39, 40, 42, 43, 44, 46, 47, 49, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 138, 140, 141, 143, 144, 145, 147, 148, 150, 151, 152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178 };
const int32_t Table_fv2[256] = { -92, -91, -91, -90, -89, -88, -88, -87, -86, -86, -85, -84, -83, -83, -82, -81, -81, -80, -79, -78, -78, -77, -76, -76, -75, -74, -73, -73, -72, -71, -71, -70, -69, -68, -68, -67, -66, -66, -65, -64, -63, -63, -62, -61, -61, -60, -59, -58, -58, -57, -56, -56, -55, -54, -53, -53, -52, -51, -51, -50, -49, -48, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -41, -40, -39, -38, -38, -37, -36, -36, -35, -34, -33, -33, -32, -31, -31, -30, -29, -28, -28, -27, -26, -26, -25, -24, -23, -23, -22, -21, -21, -20, -19, -18, -18, -17, -16, -16, -15, -14, -13, -13, -12, -11, -11, -10, -9, -8, -8, -7, -6, -6, -5, -4, -3, -3, -2, -1, 0, 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90 };
const int32_t Table_fu1[256] = { -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39, -39, -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43 };
const int32_t Table_fu2[256] = { -227, -226, -224, -222, -220, -219, -217, -215, -213, -212, -210, -208, -206, -204, -203, -201, -199, -197, -196, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173, -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -149, -148, -146, -144, -142, -141, -139, -137, -135, -134, -132, -130, -128, -126, -125, -123, -121, -119, -118, -116, -114, -112, -110, -109, -107, -105, -103, -102, -100, -98, -96, -94, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73, -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -47, -45, -43, -41, -40, -38, -36, -34, -32, -31, -29, -27, -25, -24, -22, -20, -18, -16, -15, -13, -11, -9, -8, -6, -4, -2, 0, 1, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 37, 39, 40, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 76, 78, 79, 81, 83, 85, 86, 88, 90, 92, 93, 95, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 141, 143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 170, 172, 173, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 219, 221, 223, 225 };

bool gtmvreader::yv12torgb24(uint8_t* pRGB24, const uint8_t* pYV12, const int32_t& w, const int32_t& h, const int32_t& bottomup, const int32_t& ibw){
    bool ret = true; if (!pYV12 || !pRGB24) return false;
    const uint32_t	nYLen = w * h, nHfWidth = (w >> 1);
    if (nYLen < 1 || nHfWidth < 1) return false;
    const uint8_t* yData = pYV12, *vData = &yData[nYLen], *uData = &vData[nYLen >> 2]; // y,v,u;
    if (!uData || !vData) return false;
    int rgb[3], i, j, m, n, x, y, py, rdif, invgdif, bdif;
    m = 0 - w; n = 0 - nHfWidth;
    bool addhalf = true;
    for (y = 0; y < h; y++){ // ��
        m += w;
        if (addhalf){ n += nHfWidth; addhalf = false; }
        else addhalf = true;
        for (x = 0; x < w; x++){ // ��
            i = m + x; j = n + (x >> 1); // i��ǰ�еĵڼ���
            py = yData[i];
            int v111 = vData[j], u111 = uData[j];				// search tables to get rdif invgdif and bidif
            rdif = Table_fv1[v111];    // fv1
            invgdif = Table_fu1[u111] + Table_fv2[v111]; // fu1+fv2
            bdif = Table_fu2[u111]; // fu2
            if (ibw == 1){ rdif = 0; invgdif = 0; bdif = 0; py = 255 - py; }
            //�˴�Ӧ���޸�Ϊ
            //====================
            rgb[0] = py + rdif;    // R
            rgb[1] = py - invgdif; // G
            rgb[2] = py + bdif;    // B
            //====================
            if (bottomup) i = (m + x) * 3;
            else{ j = nYLen - w - m + x; i = (j << 1) + j; }
            for (j = 0; j < 3; j++){	// copy this pixel to rgb data
                if (rgb[j] >= 0 && rgb[j] <= 255) pRGB24[i + j] = rgb[j];
                else pRGB24[i + j] = (rgb[j] < 0) ? 0 : 255;
            }
        }
    }
    return true;
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

void gtmvrender::setwh(int _w,int _h){
    _lock lock(mtv);
    w=_w;h=_h;
}

void gtmvrender::getwh(int& _w, int&_h){
    _lock lock(mtv);
    _w=w;_h=h;
}

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
