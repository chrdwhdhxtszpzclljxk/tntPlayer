//
// Created by Administrator on 2017-11-13.
//

#ifndef TNTPLAYER_GTMVREADER_H
#define TNTPLAYER_GTMVREADER_H


#include <mutex>
#include <list>
#include "crossanyapp.h"

class gtmvreader {
private:
    gtmvreader(){};
    ~gtmvreader(){};
    bool init();
    void httpsdownloaderThread();
    std::list<fileinfo*> listdl;
    std::recursive_mutex _lock;
public:
    static gtmvreader* me();
    void push(std::string type,std::string tnow,std::string pubid,long);
};


#endif //TNTPLAYER_GTMVREADER_H
