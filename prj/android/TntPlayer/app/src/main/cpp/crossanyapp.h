//
// Created by Administrator on 2017-11-10.
//

#ifndef TNTPLAYER_CROSSANYAPP_H
#define TNTPLAYER_CROSSANYAPP_H


#include <string>
#include <jni.h>
#include <android/asset_manager.h>
#include <mutex>

class fileinfo{
public:
    std::string fileType;
    std::string fileId;
    std::string pubId;
    long filelen;
    int64_t errcount;
};


class crossanyapp {
private:
    crossanyapp(){};
    ~crossanyapp(){};
    bool init();
    std::string writablePath;
    std::string un;
    std::string pwd;
    JavaVM* jvm;
    AAssetManager * am;
    std::string userid;
public:
    std::recursive_mutex _lock;
    static crossanyapp* me();
    JavaVM* setJvm(JavaVM* vm){return (jvm = vm);};
    JavaVM* getJvm(){return jvm;}
    AAssetManager* setAm(AAssetManager* _am){return (am = _am);};
    AAssetManager* getAm(){return am;};
    void setUserid(std::string _userid);//{userid=_userid;};
    std::string getUserid();//{return userid;};
    std::string getWritablePath();//{return writablePath;};
    std::string setWritablePath(std::string path);
    std::string getun();//{return un;};
    std::string setun(std::string un0);//{return (un = un0);};
    std::string getPwd();//{return pwd;};
    std::string setPwd(std::string pwd0);//{return (pwd = pwd0);};

};


#endif //TNTPLAYER_CROSSANYAPP_H
