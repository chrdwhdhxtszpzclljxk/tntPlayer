//
// Created by Administrator on 2017-11-13.
//

#ifndef TNTPLAYER_GTMVREADER_H
#define TNTPLAYER_GTMVREADER_H


#include <mutex>
#include <list>
#include <atomic>
#include <queue>
#include "crossanyapp.h"
#include "protocol.h"

class gtmvreader {
private:
    gtmvreader(){};
    ~gtmvreader(){};
    bool init();
    void httpsdownloaderThread();

    std::list<fileinfo*> listdl;

    std::atomic<bool> r, breaknow;
public:
    static gtmvreader* me();
    void start();
    void push(std::string type,std::string tnow,std::string pubid,long);
    void setbreaknow(){ breaknow = true; };
    bool getbreaknow(){ return breaknow; };
    static bool YV12_to_RGB32(unsigned char* pRGB32, unsigned char* pYV12, uint32_t iWidth, uint32_t iHeight);
    static bool shit(uint32_t iWidth, uint32_t iHeight);
    static bool yv12torgb24(uint8_t* pRGB24, const uint8_t* pYV12, const int32_t& w, const int32_t& h, const int32_t& bottomup = 0, const int32_t& ibw = 0);
};

class gtmvrender{
public:
    typedef std::lock_guard<std::recursive_mutex> _lock;
    typedef std::queue<xiny120::GtmvData*> avqueue;
    std::atomic<int32_t> fps, frames, curframes, seekframes;
private:
    std::atomic<int32_t> w, h;

public:
    gtmvrender(); ~gtmvrender();
    static gtmvrender* me();

    static void destroy();
    void pushv(xiny120::GtmvData*);
    void setwh(int w,int h);
    void getwh(int& w, int&h);
    xiny120::GtmvData* popv();
    uint64_t touchv();
    void clear();
    void resettimeline(const uint64_t&);

private:
    std::recursive_mutex mtv; avqueue v;
};

#endif //TNTPLAYER_GTMVREADER_H
