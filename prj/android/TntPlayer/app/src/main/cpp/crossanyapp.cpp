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
    std::lock_guard<std::recursive_mutex> locker(_lock);
    writablePath = path;
    return writablePath;
}

void crossanyapp::setUserid(std::string _userid){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    userid=_userid;
}

std::string crossanyapp::getUserid(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return userid;
}

std::string crossanyapp::getWritablePath(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return writablePath;
}

std::string crossanyapp::getun(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return un;
}

std::string crossanyapp::setun(std::string un0){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return (un = un0);
}

std::string crossanyapp::getPwd(){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return pwd;
}

std::string crossanyapp::setPwd(std::string pwd0){
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return (pwd = pwd0);
}