//
// Created by Administrator on 2017-11-10.
//

#include "crossanyapp.h"


crossanyapp* crossanyapp::me(){
    static crossanyapp* pthis = nullptr;
    if(pthis == nullptr){
        pthis = new crossanyapp();
        if(!pthis->init()) {
            delete pthis;
            pthis = nullptr;
        }
    }
    return pthis;
}

bool crossanyapp::init(){

    return true;
}

std::string crossanyapp::setWritablePath(std::string path){
    writablePath = path;
    return writablePath;
}