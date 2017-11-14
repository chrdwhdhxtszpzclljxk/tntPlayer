//
// Created by Administrator on 2017-11-14.
//

#ifndef TNTPLAYER_BASESOCKET_H
#define TNTPLAYER_BASESOCKET_H


class basesocket {
private:
    basesocket(){};
    ~basesocket(){};
    bool init();
    void socketThread();
public:
    static basesocket* me();
};


#endif //TNTPLAYER_BASESOCKET_H
