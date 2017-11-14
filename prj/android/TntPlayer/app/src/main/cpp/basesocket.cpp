//
// Created by Administrator on 2017-11-14.
//
#include <thread>

#include "basesocket.h"


void basesocket::socketThread(){
    while(true){


    }

}



basesocket* basesocket::me(){
    static basesocket* pthis = nullptr;
    if(pthis == nullptr){
        pthis = new basesocket();
        if(!pthis->init()) {
            delete pthis;
            pthis = nullptr;
        }
    }
    return pthis;
}

bool basesocket::init(){
    //listdl = new std::list<fileinfo>();
    static std::thread t1 (&basesocket::socketThread,this);
    return true;
}