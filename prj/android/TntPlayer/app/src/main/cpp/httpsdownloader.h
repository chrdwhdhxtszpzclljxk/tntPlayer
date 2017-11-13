//
// Created by Administrator on 2017-11-10.
//

#ifndef TNTPLAYER_HTTPSDOWNLOADER_H
#define TNTPLAYER_HTTPSDOWNLOADER_H

#include <stdint.h>
#include <list>
#include <string>
#include <mutex>
#include "crossanyapp.h"


class httpsdownloader {
private:
    httpsdownloader(){};
    ~httpsdownloader(){};
    bool init();
    void httpsdownloaderThread();
    std::list<fileinfo*> listdl;
    std::recursive_mutex _lock;
public:
    static httpsdownloader* me();
    void push(std::string type,std::string tnow,std::string pubid);

};


#endif //TNTPLAYER_HTTPSDOWNLOADER_H
