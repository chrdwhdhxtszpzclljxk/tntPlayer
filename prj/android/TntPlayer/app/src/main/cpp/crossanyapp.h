//
// Created by Administrator on 2017-11-10.
//

#ifndef TNTPLAYER_CROSSANYAPP_H
#define TNTPLAYER_CROSSANYAPP_H


#include <string>

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
public:
    static crossanyapp* me();
    std::string getWritablePath(){return writablePath;};
    std::string setWritablePath(std::string path);
    std::string getun(){return un;};
    std::string setun(std::string un0){return (un = un0);};
    std::string getPwd(){return pwd;};
    std::string setPwd(std::string pwd0){return (pwd = pwd0);};

};


#endif //TNTPLAYER_CROSSANYAPP_H
