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
    int64_t errcount;
};


class crossanyapp {
private:
    crossanyapp(){};
    ~crossanyapp(){};
    bool init();
    std::string writablePath;
public:
    static crossanyapp* me();
    std::string getWritablePath(){return writablePath;};
    std::string setWritablePath(std::string path);

};


#endif //TNTPLAYER_CROSSANYAPP_H
