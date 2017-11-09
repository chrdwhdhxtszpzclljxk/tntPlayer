#ifndef __UPLOADFILE_H_
#define __UPLOADFILE_H_

#include "cocos2d.h"
#include "dll.h"

class uploadFile : public cocos2d::Object{
private:
	uploadFile();
	~uploadFile();
	std::string cookiefile;
	std::string url_fileup,url_login;
	//std::string aesiv, aeskey;
	char aesiv[1024], aeskey[1024];
	int aeskeylen,aesivlen;
	CryptoPP::AutoSeededRandomPool rng;
	CryptoPP::InvertibleRSAFunction key;
	bool cmdinit();
	bool cmdlogin();
	bool cmdsendFile(const std::string& path);
public:
	static std::string hex2str(const std::string str);
	static std::string hex2str(const char*, int);
	static int str2hex(std::string str,char*);
	static uploadFile* me();
	bool login();
	bool sendFile(const std::string& path);
	bool getCurLessions();
	static size_t read_local_send_to_server(uint8_t *dataBack, size_t size, size_t nmemb, void *userp);
	static size_t read_from_server(uint8_t *dataBack, size_t size, size_t nmemb, void *userp);

};

#endif